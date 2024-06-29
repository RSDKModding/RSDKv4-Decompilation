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

struct MultiplayerData {
    int type;
    int data[(PACKET_SIZE - 16) / sizeof(int) - 1];
};

struct ServerPacket {
    byte header  = 0;
    char game[7] = { 0, 0, 0, 0, 0, 0, 0 };
    uint player  = 0;
    uint room    = 0;

    union {
        unsigned char bytes[PACKET_SIZE - 16];
        MultiplayerData multiData;
    } data;
};

enum ClientHeaders {
    CL_REQUEST_CODE = 0x00,
    CL_JOIN         = 0x01,

    CL_DATA          = 0x10,
    CL_DATA_VERIFIED = 0x11,

    CL_QUERY_VERIFICATION = 0x20,

    CL_LEAVE = 0xFF
};
enum ServerHeaders {
    SV_CODES      = 0x00,
    SV_NEW_PLAYER = 0x01,

    SV_DATA          = 0x10,
    SV_DATA_VERIFIED = 0x11,

    SV_RECEIVED     = 0x20,
    SV_VERIFY_CLEAR = 0x21,

    SV_INVALID_HEADER = 0x80,
    SV_NO_ROOM        = 0x81,
    SV_UNKNOWN_PLAYER = 0x82,

    SV_LEAVE = 0xFF
};

class NetworkSession;

extern std::shared_ptr<NetworkSession> session;

void InitNetwork();
void RunNetwork();
void SendData(bool verify = false);
void DisconnectNetwork(bool finalClose = false);
void SendServerPacket(ServerPacket &send, bool verify = false);
int GetRoomCode();
void SetRoomCode(int code);

void SetNetworkGameName(int *unused, const char *name);

#endif
#endif
