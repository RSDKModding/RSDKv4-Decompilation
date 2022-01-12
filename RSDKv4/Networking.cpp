#include "RetroEngine.hpp"
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
char networkGame[7] = "SONIC2";
int networkPort     = 50;
int dcError         = 0;
float lastPing      = 0;

bool waitingForPing = false;
bool waitForRecieve = false;

uint64_t lastTime = 0;

using asio::ip::udp;

typedef std::deque<ServerPacket> DataQueue;

class NetworkSession
{
public:
    bool running = false;
    NetworkSession(asio::io_context &io_context, const udp::endpoint &endpoint) : io_context_(io_context), socket(io_context), endpoint(endpoint)
    {
        socket.open(udp::v4());
    }

    ~NetworkSession() {}

    void write(const ServerPacket &msg, int forceroom = 0)
    {
        ServerPacket sent(msg);
        sent.code = code;
        sent.room = forceroom ? forceroom : room;
        write_msgs_.push_back(sent);
    }

    void start()
    {
        running = true;
        ServerPacket sent;
        sent.room = 0x1F2F3F4F;
        write_msgs_.push_back(sent);
    }

    void close()
    {
        if (running)
            running = false;
        if (socket.is_open())
            socket.close();
    }

    NetworkSession &operator=(const NetworkSession &s)
    {
        close();
        memcpy(this, &s, sizeof(NetworkSession));
        return *this;
    }

    uint64_t code    = 0;
    uint64_t partner = 0;
    bool wait        = false;
    int room         = 0;

    void run()
    {
        // do we write anything
        while (!write_msgs_.empty()) {
            ServerPacket *send = &write_msgs_.front();
            StrCopy(send->game, networkGame);
            socket.send_to(asio::buffer(send, sizeof(ServerPacket)), endpoint);
            write_msgs_.pop_front();
            if (send->header == 0xFF)
                session->running = false;
        }
        // listen in
        if (!wait)
            do_read();
    }

private:
    void do_read()
    {
        if (wait)
            return;
        wait = true;
        socket.async_receive(asio::buffer(&read_msg_, sizeof(ServerPacket)), [&](const asio::error_code &ec, size_t bytes) {
            wait = false; // async, not threaded. this is safe
            if (ec || !session->running)
                return;
            if (!code) {
                if (read_msg_.header == 0x00 && read_msg_.code) {
                    code = read_msg_.code;
                }
                return;
            }

            switch (read_msg_.header) {
                case 0x00: {
                    room = read_msg_.room;
                    if (read_msg_.data.multiData.type > 2) {
                        ServerPacket send;
                        send.header = 0xFF;
                        dcError     = 3;
                        vsPlaying   = false;
                        write(send);
                        return;
                    }

                    if (read_msg_.data.multiData.type - 1) {
                        partner = *(uint64_t *)read_msg_.data.multiData.data;
                        receive2PVSMatchCode(room);
                        return;
                    }
                }
                case 0x01: {
                    if (partner)
                        return;
                    partner = read_msg_.code;
                    return;
                }
                case 0x11: waitForRecieve = true;
                // fallthrough
                case 0x10: {
                    receive2PVSData(&read_msg_.data.multiData);
                    return;
                }
                case 0x21: {
                    waitForRecieve = false;
                    return;
                }
                case 0xFF: {
                    if (read_msg_.code != partner)
                        return;
                    dcError          = 1;
                    vsPlaying        = false;
                    session->running = false;
                    return;
                }
            }
        });
    }

    asio::io_context &io_context_;
    udp::socket socket;
    udp::endpoint endpoint;
    ServerPacket read_msg_;
    DataQueue write_msgs_;

    bool writing = false;

    int attempts;
};

std::shared_ptr<NetworkSession> session;
asio::io_context io_context;
std::thread loopThread;

void initNetwork()
{
    try {
        udp::resolver resolver(io_context);
        asio::error_code ec;
        auto endpoint = *resolver.resolve(udp::v4(), networkHost, std::to_string(networkPort), ec).begin();
        session.reset();
        auto newsession = std::make_shared<NetworkSession>(io_context, endpoint);
        session.swap(newsession);
    } catch (std::exception &e) {
        Engine.onlineActive = false;
        printLog("Failed to initialize networking: %s", e.what());
    }
}

void networkLoop()
{
    try {
        session->start();
        while (session->running) session->run();
        session->close();
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

void sendData(bool verify)
{
    ServerPacket send;
    send.header         = 0x10 + verify;
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

    if (finalClose) {
        if (session)
            session.reset();
    }
}

void sendServerPacket(ServerPacket &send) { session->write(send); }
int getRoomCode() { return session->room; }
void setRoomCode(int code) { session->room = code; }

void SetNetworkGameName(int *a1, const char *name) { StrCopy(networkGame, name); }
#endif