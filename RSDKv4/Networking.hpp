#if RETRO_USE_NETWORKING
#ifndef NETWORKING_H
#define NETWORKING_H
#include <thread>
#include <memory>

#define PACKET_SIZE 0x1000

extern char networkHost[64];
extern char networkGame[7];
extern int networkPort;

extern float lastPing;
extern int dcError;
extern bool waitingForPing;
extern bool waitForRecieve;

struct MultiplayerData {
    int type;
    int data[(PACKET_SIZE - 16) / sizeof(int) - 1];
};

struct alignas(8) ServerPacket {
    byte header;
    char game[7];
    uint64_t code;
    uint room;

    union {
        unsigned char bytes[PACKET_SIZE - 16];
        MultiplayerData multiData;
    } data;
};

class NetworkSession;

extern std::shared_ptr<NetworkSession> session;

void initNetwork();
void runNetwork();
void sendData(bool verify = false);
void disconnectNetwork(bool finalClose = false);
void sendServerPacket(ServerPacket &send);
int getRoomCode();
void setRoomCode(int code);

void SetNetworkGameName(int *a1, const char *name);

#endif
#endif
