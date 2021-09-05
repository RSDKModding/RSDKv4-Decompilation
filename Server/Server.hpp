#include <stdint.h>

#define SV_DATA_MAX 0x800

enum ServerCodes { SV_START = 0x01, SV_JOIN = 0x02, SV_LEAVE = 0xFF };

enum ClientCodes { CL_CONNECT = 0x00, CL_DATA = 0x10, CL_LEAVE = 0xFF };

struct ServerPacket {
    unsigned char header;
    uint64_t code;
    union {
        unsigned char bytes[SV_DATA_MAX];
        uint32_t ints[SV_DATA_MAX / sizeof(uint32_t)];
    } data;
};