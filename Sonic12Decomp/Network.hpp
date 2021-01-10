#ifndef NETWORK_H
#define NETWORK_H
#include <string>

std::string generateCode(ushort port, int gameLength, int itemMode);

/*  returns 1 or more on error
    this also ensures the user is connected to the internet */
int getIP(uint* ip);

#endif