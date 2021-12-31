#if RETRO_USE_NETWORKING
#ifndef NETWORKING_H
#define NETWORKING_H
#include <thread>
#include <memory>

extern char networkHost[64];
extern char networkGame[16];
extern int networkPort;

extern float lastPing;
extern int dcError;
extern bool waitingForPing;

struct MultiplayerData {
    int type;
    int data[0x1FF];
};

struct ServerPacket {
    byte header;
    uint64_t code;
    uint roomcode;
    union {
        unsigned char bytes[0x1000];
        MultiplayerData multiData;
    } data;
};

class NetworkSession;

extern std::shared_ptr<NetworkSession> session;

void initNetwork();
void runNetwork();
void sendData();
void disconnectNetwork(bool finalClose = false);
void sendServerPacket(ServerPacket &send);
int getRoomCode();
void setRoomCode(int code);

void SetNetworkGameName(int *a1, const char *name);

#endif
#endif
