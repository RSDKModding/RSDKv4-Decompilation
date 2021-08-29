#if RETRO_USE_NETWORKING
#ifndef NETWORKING_H
#define NETWORKING_H
#include <thread>

extern char networkHost[64];
extern int networkPort;

struct MultiplayerData {
    int type;
    int data[0x1FF];
};

struct CodedData {
    byte header;
    int code;
    int roomcode;
    MultiplayerData multiData;
};

class NetworkSession;

extern std::shared_ptr<NetworkSession> session;

void initNetwork();
void runNetwork();
void sendData();
void disconnectNetwork();
void waitForData(int type, int id, int slot);

#endif
#endif