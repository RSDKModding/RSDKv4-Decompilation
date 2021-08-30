#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include "asio.hpp"

using asio::ip::tcp;

struct MultiplayerData {
    int type;
    int data[0x1FF];
};

struct CodedData {
    unsigned char header;
    int code;
    int roomcode;
    MultiplayerData multiData;
};

//----------------------------------------------------------------------

typedef std::deque<CodedData> data_queue;

//----------------------------------------------------------------------

class player
{
public:
    virtual ~player() {}
    virtual void deliver(const CodedData &msg) = 0;
    int code                                   = 0;
    int roomcode                               = 0;
    bool hasPartner                            = true; // to prevent ==0 joins
};

typedef std::shared_ptr<player> player_ptr;
std::vector<int> codes;
std::vector<int> rooms;

//----------------------------------------------------------------------

bool print = false;

class server_session
{
public:
    void join(player_ptr participant)
    {
        participants.insert(participant);
        if (print)
            std::cout << "New player " << std::hex << participant->code << std::endl;
    }

    void leave(player_ptr participant)
    {
        participants.erase(participant);
        if (print)
            std::cout << "Player " << std::hex << participant->code << " in room " << participant->roomcode << " left" << std::endl;
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

    std::set<player_ptr> participants;
};

//----------------------------------------------------------------------

class player_connection : public player, public std::enable_shared_from_this<player_connection>
{
public:
    player_connection(tcp::socket socket, server_session &room) : socket_(std::move(socket)), room_(room)
    {
        do {
            code = rand() + 1;
        } while (std::count(codes.begin(), codes.end(), code));
        codes.push_back(code);
    }

    void start()
    {
        room_.join(shared_from_this());
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

    void do_read()
    {
        auto self(shared_from_this());
        asio::async_read(socket_, asio::buffer(&read_msg_, sizeof(CodedData)), [this, self](std::error_code ec, std::size_t /*length*/) {
            if (ec)
                return;
            if (print)
                std::cout << "Reading data with header " << std::hex << (int)read_msg_.header << " from player " << std::hex << read_msg_.code
                          << " in room " << std::hex << read_msg_.roomcode << " to player " << self->code << std::endl;
            CodedData send;

            switch (read_msg_.header) {
                case 0x00: { // give me my codes
                    if (std::count(rooms.begin(), rooms.end(), read_msg_.roomcode)) {
                        // joining
                        read_msg_.header         = 0x01;
                        read_msg_.multiData.type = 1; // You can now play as Luigi!
                        self->roomcode           = read_msg_.roomcode;
                        send_to_self(read_msg_);
                    }
                    else {
                        int roomcodeBase = (read_msg_.multiData.data[0] << 4) | (read_msg_.multiData.data[1] << 8);
                        int tries        = 0;
                        do {
                            self->roomcode = (rand() % 0x10) | roomcodeBase | (rand() << 12);
                            if (++tries == 256) {
                                // uhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh
                            }
                        } while (std::count(rooms.begin(), rooms.end(), self->roomcode));

                        send.roomcode       = self->roomcode;
                        send.header         = 0x01; // this is code
                        send.multiData.type = 0;    // You are playing as Mario.
                        send_to_self(send);
                    }
                    self->hasPartner = false;
                    rooms.push_back(self->roomcode);
                    if (print)
                        std::cout << "Player " << std::hex << self->code << " is now in room " << std::hex << self->roomcode << std::endl;
                    break;
                }
                case 0x01: { // try to join player with code
                    if (!room_.meet(read_msg_, self)) {
                        // we couldn't join; player doesn't exist
                        send.header = 0x81;
                        send_to_self(send);
                    }
                    else {
                        send.header = 0x80;
                        send_to_self(send);
                    }
                    break;
                }
                case 0x10: { // data
                    room_.deliver(read_msg_, self);
                    break;
                }
                case 0x11:
                case 0x12: {                 // request entity/value
                    read_msg_.header += 0xF; // 0x20, 0x21
                    room_.deliver(read_msg_, self);
                }
                case 0xFF: { // leave/quit, sent to both players
                    room_.leave(self);
                    room_.deliver(read_msg_, self);
                    break;
                }
            }
            // let's check rooms and get rid of unused ones while we're here
            /*std::vector<int> codes;
            for (auto &p : room_.participants) {
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
                if (print)
                    std::cout << "Sending data with header " << std::hex << (int)msg.header << " to player " << std::hex << msg.code << " in room "
                              << std::hex << msg.roomcode << " from player " << self->code << std::endl;
                write_msgs_.pop_front();
                self->writing = false;
                do_write();
            }
            else if (ec) {
                room_.leave(shared_from_this());
            }
            self->writing = false;
        });
    }

    tcp::socket socket_;
    server_session &room_;
    CodedData read_msg_;
    data_queue write_msgs_;
    bool writing = false;
};

//----------------------------------------------------------------------

class chat_server
{
public:
    chat_server(asio::io_context &io_context, const tcp::endpoint &endpoint) : acceptor_(io_context, endpoint) { do_accept(); }

private:
    void do_accept()
    {
        acceptor_.async_accept([this](std::error_code ec, tcp::socket socket) {
            if (!ec) {
                std::make_shared<player_connection>(std::move(socket), room_)->start();
            }

            do_accept();
        });
    }

    tcp::acceptor acceptor_;
    server_session room_;
};

//----------------------------------------------------------------------

int main(int argc, char *argv[])
{
    try {
        if (argc < 2) {
            std::cerr << "Pass ports as arguments. (Server [port] [port] [port]...)";
            return 1;
        }

        asio::io_context io_context;

        std::list<chat_server> servers;
        for (int i = 1; i < argc; ++i) {
            if (std::string(argv[i]) == "debug") {
                print = true;
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