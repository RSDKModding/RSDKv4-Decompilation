#ifndef NETWORK_H
#define NETWORK_H
#include <string>

struct CodeData {
    uint ip;
    ushort port;
    int gameLength;
    int itemMode;
    int player;
    std::string ipString;
    std::string codeString;
};
void buildNetworkIndex();

/*  generates the code based on IP, port, gameLength, itemMode, and playerListPos.
    the other user inputs this code to connect */
std::string generateCode(ushort port, int gameLength, int itemMode);

/*  parses the code given and returns a CodeData.  
    we can use this to get the IP and port and set our other variables later on */
CodeData parseCode(const std::string code);

/*  returns 1 or more on error
    this also ensures the user is connected to the internet */
int getIP(uint* ip);

void initServer(ushort port);
void initClient(CodeData data);
int sendData(byte flag, ushort datalen, void* data, void* endpoint = nullptr);

#endif