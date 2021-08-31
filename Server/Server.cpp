#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <utility>
#include "asio.hpp"

#define DATA_MAX 0x1000

typedef unsigned int uint;

using asio::ip::tcp;

struct CodedData {
    unsigned char header;
    uint64_t code;
    uint roomcode;
    union {
        unsigned char bytes[DATA_MAX];
        int ints[DATA_MAX / sizeof(int)];
    } data;
};

//----------------------------------------------------------------------

typedef std::deque<CodedData> data_queue;

//----------------------------------------------------------------------

class player
{
public:
    virtual ~player() {}
    virtual void deliver(const CodedData &msg) = 0;
    uint64_t code                              = 0;
    uint roomcode                              = 0;
    bool hasPartner                            = true; // to prevent ==0 joins
};

typedef std::shared_ptr<player> player_ptr;
std::set<uint64_t> codes;

//----------------------------------------------------------------------

bool debug   = false;
bool verbose = false;

inline uint randomint()
{
    uint res = 0;
    for (int i = 0; i < 4; i++) {
        res |= (rand() & 0xFF) << (i * 8);
    }
    if (!res)
        res++;
    return res;
}

class game_room
{
public:
    game_room(){};
    game_room(std::string &name) : game_name(name), set(true){};
    void join(player_ptr participant)
    {
        participants.insert(participant);
        if (debug)
            std::cout << "[" + game_name + "] "
                      << "New player " << std::hex << participant->code << std::endl;
    }

    void leave(player_ptr participant)
    {
        participants.erase(participant);
        rooms.erase(participant->roomcode);
        codes.erase(participant->code);
        if (debug)
            std::cout << "[" + game_name + "] "
                      << "Player " << std::hex << participant->code << " in room " << participant->roomcode << " left" << std::endl;
    }

    void deliver(const CodedData &msg, player_ptr sender)
    {
        for (auto &participant : participants) {
            if (participant->code != sender->code && participant->roomcode == msg.roomcode) {
                CodedData s(msg);
                s.code = participant->code;
                participant->deliver(s);
            }
        }
    }

    bool meet(const CodedData msg, player_ptr searcher)
    {
        if (searcher->hasPartner)
            return true; // YOU HAVE ONE ALREADY
        for (auto &participant : participants) {
            if (participant->code != searcher->code && participant->roomcode == msg.roomcode && !participant->hasPartner) {
                participant->hasPartner = true;
                searcher->hasPartner    = true;
                CodedData send;
                send.code     = participant->code;
                send.header   = 0x80;
                send.roomcode = msg.roomcode;
                participant->deliver(send);
                return true;
            }
        }
        return false;
    }

    std::set<uint> rooms;
    std::set<player_ptr> participants;
    std::string game_name = "(NO GAME)";
    bool set              = false;
};

game_room blank_room;
std::vector<game_room> rooms;

//----------------------------------------------------------------------

class player_connection : public player, public std::enable_shared_from_this<player_connection>
{
public:
    player_connection(tcp::socket socket) : socket_(std::move(socket))
    {
        do {
            code = randomint() | ((uint64_t)randomint() << 32);
        } while (codes.find(code) != codes.end());
        codes.insert(code);
    }

    void start()
    {
        // don't join any room YET
        if (debug)
            std::cout << "New game-less player " << std::hex << code << std::endl;
        do_read();
    }

    void deliver(const CodedData &msg)
    {
        if (msg.code != code)
            return;
        write_msgs_.push_back(msg);
        do_write();
    }

private:
    inline void send_to_self(const CodedData &msg = CodedData())
    {
        auto self(shared_from_this());
        CodedData send(msg);
        send.code     = self->code;
        send.roomcode = self->roomcode;
        deliver(send);
    }
    inline void send_error(unsigned char code = 0)
    {
        auto self(shared_from_this());
        CodedData send;
        send.header        = 0x81;
        send.data.bytes[0] = code;
        send_to_self(send);
    }

    game_room &get_room()
    {
        auto self(shared_from_this());
        if (self->roomID == -1)
            return blank_room;
        return rooms[self->roomID];
    }

    void do_read()
    {
        auto self(shared_from_this());
        asio::async_read(socket_, asio::buffer(&read_msg_, sizeof(CodedData)), [this, self](std::error_code ec, std::size_t /*length*/) {
            if (ec)
                return do_read();
            if (debug && verbose)
                std::cout << "[" + self->get_room().game_name + "] "
                          << "Reading data with header " << std::uppercase << std::hex << (int)read_msg_.header << " from player " << std::hex
                          << read_msg_.code << " in room " << std::hex << read_msg_.roomcode << " to player " << self->code << std::endl;
            CodedData send;

            switch (read_msg_.header) {
                case 0x00: { // give me my codes, set the room
                    if (self->get_room().set) {
                        self->send_error(1); // already in a room
                        break;
                    }
                    std::string name((char *)&read_msg_.data.bytes[12], read_msg_.data.ints[2]);
                    for (int i = 0; i < rooms.size(); i++)
                        if (rooms[i].game_name == name)
                            self->roomID = i;
                    if (!self->get_room().set) {
                        auto newroom = game_room(name);
                        rooms.push_back(newroom);
                        if (debug)
                            std::cout << "Game " << name << " created" << std::endl;
                        self->roomID = rooms.size() - 1;
                    }
                    self->get_room().join(self);

                    if (self->get_room().rooms.find(read_msg_.roomcode) != self->get_room().rooms.end()) {
                        // joining
                        read_msg_.header = 0x02; // You can now play as Luigi!
                        self->roomcode   = read_msg_.roomcode;
                        send_to_self(read_msg_);
                    }
                    else if (read_msg_.roomcode) {
                        send_error(0); // roomcode doesn't exist
                    }
                    else {
                        int roomcodeMask = read_msg_.data.ints[0];
                        int roomcodeBase = read_msg_.data.ints[1];
                        int tries        = 0;
                        do {
                            self->socket_.async_wait(asio::socket_base::wait_read, [](asio::error_code) {});
                            self->roomcode = (randomint() & ~roomcodeMask) | roomcodeBase;
                        } while (self->get_room().rooms.find(self->roomcode) != self->get_room().rooms.end());

                        send.roomcode = self->roomcode;
                        send.header   = 0x01; // this is code and you are MARIO. NOT LUIGI. YOU **SUCK**
                        send_to_self(send);
                    }
                    self->hasPartner = false;
                    self->get_room().rooms.insert(self->roomcode);
                    if (debug)
                        std::cout << "[" + self->get_room().game_name + "] "
                                  << "Player " << std::hex << self->code << " is now in room " << std::hex << self->roomcode << std::endl;
                    break;
                }
                case 0x01: { // try to join player with code
                    if (get_room().meet(read_msg_, self)) {
                        send.header = 0x80;
                        send_to_self(send);
                    }
                    break;
                }
                case 0x10: { // data
                    get_room().deliver(read_msg_, self);
                    break;
                }
                case 0x11:
                case 0x12: {                 // request entity/value
                    read_msg_.header += 0xF; // 0x20, 0x21
                    get_room().deliver(read_msg_, self);
                }
                case 0xFF: { // leave/quit, sent to both players
                    get_room().leave(self);
                    get_room().deliver(read_msg_, self);
                    break;
                }
            }
            // let's check rooms and get rid of unused ones while we're here
            /*std::set<int> codes;
            for (auto &p : get_room().participants) {
                codes.push_back(p->roomcode);
            }
            for (auto &c : rooms) {
                if (!std::count(codes.begin(), codes.end(), c)) {
                    rooms.erase(std::find(rooms.begin(), rooms.end(), c));
                }
            }//*/
            if (!write_msgs_.empty()) {
                socket_.async_wait(asio::socket_base::wait_write, [](std::error_code) {});
                do_write();
            }
            do_read();
        });
    }

    void do_write()
    {
        auto self(shared_from_this());
        if (self->writing || write_msgs_.empty())
            return;
        asio::async_write(socket_, asio::buffer(&write_msgs_.front(), sizeof(CodedData)), [this, self](std::error_code ec, std::size_t /*length*/) {
            self->writing = true;
            if (!ec && !write_msgs_.empty()) {
                auto msg = write_msgs_.front();
                if (debug && verbose)
                    std::cout << "[" + self->get_room().game_name + "] "
                              << "Sending data with header " << std::uppercase << std::hex << (int)msg.header << " to player " << std::hex << msg.code
                              << " in room " << std::hex << msg.roomcode << " from player " << self->code << std::endl;
                write_msgs_.pop_front();
                self->writing = false;
                do_write();
            }
            else if (ec) {
                get_room().leave(shared_from_this());
            }
            self->writing = false;
        });
    }

    tcp::socket socket_;
    uint roomID = -1;
    CodedData read_msg_;
    data_queue write_msgs_;
    bool writing = false;
};

//----------------------------------------------------------------------

class server
{
public:
    server(asio::io_context &io_context, const tcp::endpoint &endpoint) : acceptor_(io_context, endpoint) { do_accept(); }

private:
    void do_accept()
    {
        acceptor_.async_accept([this](std::error_code ec, tcp::socket socket) {
            if (!ec) {
                std::make_shared<player_connection>(std::move(socket))->start();
            }

            do_accept();
        });
    }

    tcp::acceptor acceptor_;
};

//----------------------------------------------------------------------

int main(int argc, char *argv[])
{
    srand((uint)time(0));
    try {
        if (argc < 2) {
            std::cerr << "Pass ports as arguments. (Server [port] [port] [port]...)";
            return 1;
        }

        asio::io_context io_context;

        std::list<server> servers;
        for (int i = 1; i < argc; ++i) {
            if (std::string(argv[i]) == "debug") {
                debug = true;
                continue;
            }
            if (std::string(argv[i]) == "verbose") {
                debug   = true;
                verbose = true;
                continue;
            }
            tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[i]));
            servers.emplace_back(io_context, endpoint);
            std::cout << "Port " << argv[i] << " ready" << std::endl;
        }

        std::cout << "Starting..." << std::endl;
        io_context.run();
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}