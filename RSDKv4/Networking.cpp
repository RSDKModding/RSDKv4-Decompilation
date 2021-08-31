#include "RetroEngine.hpp"
#if RETRO_USE_NETWORKING

#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <asio.hpp>

char networkHost[64];
char networkGame[16] = "SONIC2";
int networkPort      = 50;

int vsGameLength = 4;
int vsItemMode   = 1;
int vsPlayerID   = 0;

using asio::ip::tcp;

typedef std::deque<CodedData> data_queue;

class NetworkSession
{
public:
    bool running = false;
    NetworkSession(asio::io_context &io_context, const tcp::resolver::results_type &endpoints) : io_context_(io_context), socket_(io_context)
    {
        do_connect(endpoints);
    }

    void write(const CodedData &msg, int forceroom = 0)
    {
        CodedData sent(msg);
        sent.code     = code;
        sent.roomcode = forceroom ? forceroom : roomcode;
        write_msgs_.push_back(sent);
        do_write();
    }

    void start() { running = true; }

    void close()
    {
        if (running) {
            running = false;
            asio::post(io_context_, [this]() { socket_.close(); });
        }
    }

    NetworkSession &operator=(const NetworkSession &s) { close(); }

    uint64_t code = 0;
    bool wait     = false;
    int roomcode  = 0;

private:
    void do_connect(const tcp::resolver::results_type &endpoints)
    {
        asio::async_connect(socket_, endpoints, [this](std::error_code ec, tcp::endpoint) {
            if (!ec) {
                running = true;
                do_read();
            }
            else
                Engine.onlineActive = false;
        });
    }

    void do_read()
    {
        asio::async_read(socket_, asio::buffer(&read_msg_, sizeof(CodedData)), [this](std::error_code ec, std::size_t /*length*/) {
            if (!ec) {
                if (read_msg_.roomcode == roomcode || read_msg_.header == 0x01) {
                    switch (read_msg_.header) {
                        case 0x02: vsPlayerID = 1;
                        case 0x01: { // codes
                            code     = read_msg_.code;
                            roomcode = read_msg_.roomcode;
                            // prepare for takeoff :trollsmile:
                            wait = true;
                            CodedData send;
                            send.header   = 0x01;
                            send.roomcode = roomcode;
                            write(send);
                            break;
                        }
                        case 0x10: { // data
                            wait = false;
                            receive2PVSData(&read_msg_.data.multiData);
                            break;
                        }
                        case 0x20: { // send this entity back
                            SendEntity(&read_msg_.data.multiData.data[0], &read_msg_.data.multiData.data[1]);
                            break;
                        }
                        case 0x21: { // send value back
                            SendValue(&read_msg_.data.multiData.data[0], &read_msg_.data.multiData.data[1]);
                            break;
                        }
                        case 0x81: {
                            // error handle
                            break;
                        }
                        case 0x80: {
                            if (wait)
                                receive2PVSMatchCode(roomcode);
                            wait = false;
                            break;
                        }
                    }
                }
                if (!write_msgs_.empty())
                    do_write();
                do_read();
            }
        });
    }

    void do_write()
    {
        if (writing || write_msgs_.empty())
            return;
        writing = true;
        asio::error_code ec;
        socket_.write_some(asio::buffer(&write_msgs_.front(), sizeof(CodedData)), ec);
        if (!ec && !write_msgs_.empty()) {
            write_msgs_.pop_front();
            writing = false;
            do_write();
        }
        else if (ec) {
            socket_.close();
        }
        writing = false;
    }

    asio::io_context &io_context_;
    tcp::socket socket_;
    CodedData read_msg_;
    data_queue write_msgs_;

    bool writing = false;

    int attempts;
};

std::shared_ptr<NetworkSession> session;
asio::io_context io_context;

void initNetwork()
{
    try {
        tcp::resolver resolver(io_context);
        asio::error_code ec;
        auto endpoints = resolver.resolve(networkHost, std::to_string(networkPort), ec);
        session.reset();
        auto newsession = std::make_shared<NetworkSession>(io_context, endpoints);
        session.swap(newsession);
        Engine.onlineActive = true;
    } catch (std::exception &e) {
        Engine.onlineActive = false;
        printLog("Failed to initialize networking: %s", e.what());
    }
}

std::thread t;

void networkLoop()
{
    try {
        std::thread t([&]() { io_context.run(); });

        session->start();
        while (session->running)
            ;

        session->close();
        io_context.stop();
        t.join();
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}

void runNetwork()
{
    if (t.joinable()) {
        disconnectNetwork();
        initNetwork();
    }
    t = std::thread(networkLoop);
}

void sendData()
{
    CodedData send;
    send.header         = 0x10;
    send.data.multiData = multiplayerDataOUT;
    session->write(send);
}

void disconnectNetwork()
{
    CodedData send;
    send.header = 0xFF;
    session->write(send);
    session->running = false;
    if (t.joinable()) {
        t.join();
    }
    // Engine.devMenu = vsPlayerID;
}

void sendCodedData(CodedData &send) { session->write(send); }
int getRoomCode() { return session->roomcode; }
void setRoomCode(int code) { session->roomcode = code; }

void SetNetworkGameName(int *a1, const char *name) { StrCopy(networkGame, name); }
#endif