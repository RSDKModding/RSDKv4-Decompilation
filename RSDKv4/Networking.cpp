#include "RetroEngine.hpp"

char networkGame[16] = "SONIC2";

#if RETRO_USE_NETWORKING

#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <chrono>
#if RETRO_PLATFORM == RETRO_ANDROID
#define ASIO_NO_TYPEID
#endif
#include <asio.hpp>

char networkHost[64];
int networkPort      = 50;
int dcError          = 0;
float lastPing       = 0;

bool waitingForPing = false;

uint64_t lastTime = 0;

using asio::ip::tcp;

typedef std::deque<ServerPacket> DataQueue;

class NetworkSession
{
public:
    bool running = false;
    NetworkSession(asio::io_context &io_context, const tcp::resolver::results_type &endpoints) : io_context_(io_context), socket_(io_context)
    {
        do_connect(endpoints);
    }

    ~NetworkSession() {}

    void write(const ServerPacket &msg, int forceroom = 0)
    {
        ServerPacket sent(msg);
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

    NetworkSession &operator=(const NetworkSession &s)
    {
        close();
        memcpy(this, &s, sizeof(NetworkSession));
        return *this;
    }

    uint64_t code = 0;
    bool wait     = false;
    int roomcode  = 0;

private:
    void do_connect(const tcp::resolver::results_type &endpoints)
    {
        asio::async_connect(socket_, endpoints, [this](std::error_code ec, tcp::endpoint) {
            if (!ec) {
                Engine.onlineActive = true;
                running             = true;
                do_read();
            }
            else
                Engine.onlineActive = false;
        });
    }

    void do_read()
    {
        asio::async_read(socket_, asio::buffer(&read_msg_, sizeof(ServerPacket)), [this](std::error_code ec, std::size_t /*length*/) {
            if (ec)
                return do_read();
            lastPing       = ((SDL_GetPerformanceCounter() - lastTime) * 1000.0 / SDL_GetPerformanceFrequency());
            lastTime       = SDL_GetPerformanceCounter();
            waitingForPing = false;
            if (read_msg_.roomcode == roomcode || read_msg_.header == 0x01) {
                switch (read_msg_.header) {
                    case 0x02: vsPlayerID = 1;
                    case 0x01: { // codes
                        code     = read_msg_.code;
                        roomcode = read_msg_.roomcode;
                        // prepare for takeoff :trollsmile:
                        wait = true;
                        ServerPacket send;
                        send.header   = 0x01;
                        send.roomcode = roomcode;
                        write(send);
                        break;
                    }
                    case 0x10: { // data
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
                    case 0xFF: {
                        // other end disconnected
                        dcError          = 1;
                        vsPlaying        = false;
                        session->running = false;
                    }
                }
            }
            if (!write_msgs_.empty())
                do_write();
            do_read();
        });
    }

    void do_write()
    {
        if (writing || write_msgs_.empty())
            return;
        writing = true;
        asio::error_code ec;
        socket_.write_some(asio::buffer(&write_msgs_.front(), sizeof(ServerPacket)), ec);
        if (!ec && !write_msgs_.empty()) {
            lastTime = SDL_GetPerformanceCounter();
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
    ServerPacket read_msg_;
    DataQueue write_msgs_;

    bool writing = false;

    int attempts;
};

std::shared_ptr<NetworkSession> session;
asio::io_context io_context;
std::thread loopThread, ioThread;

void initNetwork()
{
    try {
        tcp::resolver resolver(io_context);
        asio::error_code ec;
        auto endpoints = resolver.resolve(networkHost, std::to_string(networkPort), ec);
        session.reset();
        auto newsession = std::make_shared<NetworkSession>(io_context, endpoints);
        session.swap(newsession);
        if (ioThread.joinable()) {
            io_context.stop();
            ioThread.join();
        }
        ioThread = std::thread([&]() { io_context.run(); });
    } catch (std::exception &e) {
        Engine.onlineActive = false;
        printLog("Failed to initialize networking: %s", e.what());
    }
}

void networkLoop()
{
    try {
        session->start();
        while (session->running)
            ;

        session->close();
        io_context.stop();
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}

void runNetwork()
{
    if (loopThread.joinable()) {
        disconnectNetwork();
        initNetwork();
    }
    loopThread = std::thread(networkLoop);
}

void sendData()
{
    ServerPacket send;
    send.header         = 0x10;
    send.data.multiData = multiplayerDataOUT;
    session->write(send);
}

void disconnectNetwork(bool finalClose)
{
    if (session->running) {
        ServerPacket send;
        send.header = 0xFF;
        session->write(send);
        session->running = false;
    }
    if (loopThread.joinable())
        loopThread.join();
    if (ioThread.joinable()) {
        if (!io_context.stopped())
            io_context.stop();
        ioThread.join();
    }

    if (finalClose) {
        if (session)
            session.reset();
    }
}

void sendServerPacket(ServerPacket &send) { session->write(send); }
int getRoomCode() { return session->roomcode; }
void setRoomCode(int code) { session->roomcode = code; }
#endif

void SetNetworkGameName(int *a1, const char *name) { StrCopy(networkGame, name); }