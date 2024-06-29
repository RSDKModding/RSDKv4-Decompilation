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

uint64_t lastTime = 0;

using asio::ip::udp;

typedef std::deque<ServerPacket> DataQueue;

class NetworkSession
{
public:
    bool running = false;
    NetworkSession(asio::io_context &io_context, const udp::endpoint &endpoint)
        : io_context(io_context), socket(io_context), endpoint(endpoint), timer(io_context)
    {
        socket.open(udp::v4());
    }

    ~NetworkSession() {}

    void write(const ServerPacket &msg, bool repeat = false)
    {
        ServerPacket sent(msg);
        sent.player = code;
        sent.room   = room;
        write_msgs_.push_back(sent);
        if (repeat) {
            this->repeat = sent;
        }
    }

    void start()
    {
        repeat.header = 0x80;
        running       = true;
        ServerPacket sent;
        sent.header = CL_REQUEST_CODE;
        sent.room   = 0x1F2F3F4F;
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

    uint code    = 0;
    uint partner = 0;
    uint room    = 0;

    bool awaitingReceive = false;
    bool verifyReceived  = false;

    bool retried = true;
    uint retries = 0;

    ServerPacket repeat;

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
        if (!awaitingReceive)
            do_read();
        if (repeat.header != 0x80 && retried) {
            retried = false;
            timer.expires_from_now(asio::chrono::seconds(1));
            timer.async_wait([&](const asio::error_code &) {
                retried     = true;
                repeat.room = room;
                StrCopy(repeat.game, networkGame);
                if (retries++ == 10)
                    socket.send_to(asio::buffer(&repeat, sizeof(ServerPacket)), endpoint);
            });
        }
        else if (repeat.header == 0x80)
            retries = 0;
        if (retries > 10) {
            switch (repeat.header) {
                case 0x01: {
                    dcError          = 4;
                    vsPlaying        = false;
                    session->running = false;
                    break;
                }
                case 0x00: {
                    if (!room) {
                        dcError          = 4;
                        vsPlaying        = false;
                        session->running = false;
                    }
                    break;
                }
            }
        }

        io_context.poll();
        io_context.restart();
    }

    void leave()
    {
        ServerPacket send;
        send.header = CL_LEAVE;
        vsPlaying   = false;
        write(send);
    }

private:
    void do_read()
    {
        if (awaitingReceive)
            return;
        awaitingReceive = true;
        socket.async_receive(asio::buffer(&read_msg_, sizeof(ServerPacket)), [&](const asio::error_code &ec, size_t bytes) {
            awaitingReceive = false; // async, not threaded. this is safe
            if (ec || !session->running)
                return;
            // it's ok to use preformace counter; we're in a different thread and slowdown is safe
            lastPing       = ((SDL_GetPerformanceCounter() - lastTime) * 1000.0 / SDL_GetPerformanceFrequency());
            lastTime       = SDL_GetPerformanceCounter();
            waitingForPing = false;
            if (!code) {
                if (read_msg_.header == SV_CODES && read_msg_.player) {
                    code = read_msg_.player;
                }
                return;
            }

            switch (read_msg_.header) {
                case SV_CODES: {
                    if (vsPlaying)
                        return;
                    room = read_msg_.room;
                    if (read_msg_.data.multiData.type > 2) {
                        dcError = 3;
                        leave();
                        return;
                    }

                    if (read_msg_.data.multiData.type - 1) {
                        partner = *read_msg_.data.multiData.data;
                        Receive2PVSMatchCode(room);
                        repeat.header = 0x80;
                        return;
                    }
                    break;
                }
                case SV_NEW_PLAYER: {
                    if (partner)
                        return;
                    repeat.header = 0x80;
                    vsPlayerID    = 0;
                    partner       = read_msg_.player;
                    Receive2PVSMatchCode(room);
                    return;
                }
                case SV_DATA_VERIFIED:
                // fallthrough
                case SV_DATA: {
                    Receive2PVSData(&read_msg_.data.multiData);
                    return;
                }
                case SV_RECEIVED: {
                    if (repeat.header == CL_DATA_VERIFIED)
                        repeat.header = CL_QUERY_VERIFICATION;
                    return;
                }
                case SV_VERIFY_CLEAR: {
                    repeat.header = 0x80;
                    return;
                }
                case SV_NO_ROOM: {
                    leave();
                    dcError = 5;
                    return;
                }
                case SV_LEAVE: {
                    if (read_msg_.player != partner)
                        return;
                    leave();
                    dcError = 1;
                    return;
                }
            }
        });
    }

    asio::io_context &io_context;
    asio::steady_timer timer;
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

void InitNetwork()
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
        PrintLog("Failed to initialize networking: %s", e.what());
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

void RunNetwork()
{
    if (loopThread.joinable()) {
        DisconnectNetwork();
        InitNetwork();
    }
    loopThread = std::thread(networkLoop);
}

void SendData(bool verify)
{
    ServerPacket send;
    send.header         = CL_DATA + verify;
    send.data.multiData = multiplayerDataOUT;
    session->write(send, verify);
}

void DisconnectNetwork(bool finalClose)
{
    if (session->running)
        session->leave();
    if (loopThread.joinable())
        loopThread.join();

    if (finalClose) {
        if (session)
            session.reset();
    }
}

void SendServerPacket(ServerPacket &send, bool repeat) { session->write(send, repeat); }
int GetRoomCode() { return session->room; }
void SetRoomCode(int code) { session->room = code; }

void SetNetworkGameName(int *a1, const char *name) { StrCopy(networkGame, name); }
#endif