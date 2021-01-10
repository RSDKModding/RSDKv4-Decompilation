#include "RetroEngine.hpp"

#if RETRO_PLATFORM == RETRO_WIN && _MSC_VER
#include <SDKDDKVer.h> //gives it a macro it wants, will make a note otherwise
#endif

#include <asio.hpp>
#include <iostream>

inline uint64_t GETBIT(uint64_t i, int n) { return (i & (1LL << n)) >> n; };

using namespace asio;
using ip::tcp;
using ip::udp;
using std::string;

const char *pattern = "AABAABC";
#define PLEN (7)

char index[36];
void buildNetworkIndex() {
    for (byte i = 0; i < 26; ++i) index[i] = i + 'A';
    for (byte i = 0; i < 10; ++i) index[i + 26] = i + '0';
}

string generateCode(ushort port, int gameLength, int itemMode)
{
    uint64_t code = 0;
    uint ip;
    if (getIP(&ip))
        return string();
    short gamecode = gameLength | (itemMode << 4) | (playerListPos << 6);
    int ipc = 0, pc = 0, gc = 0;
    for (uint64_t i = 0; i < PLEN * 8; i++) {
        std::cout << code << ' ' << i << std::endl;
        char c = pattern[i % PLEN];
        switch (c) {
            case 'A': code |= GETBIT(ip, ipc++) << i; break;
            case 'B': code |= GETBIT(port, pc++) << i; break;
            case 'C': code |= GETBIT(gamecode, gc++) << i; break;
        }
    }
    int i = 0;
    while (code >= (uint64_t)pow(36, i + 1)) i++;
    std::ostringstream out;
    while (code) {
        out << index[code / (uint64_t)pow(36, i)];
        code %= pow(36, i--);
    }
    return out.str();
}

CodeData parseCode(const string code) 
{
    uint64_t out = 0;
    int i = 0;
    for (auto rit = code.rbegin(); rit != code.rend(); ++rit) {
        char c = *rit, mult;
        if (c <= '9')
            mult = c - '0' + 26;
        else mult = c - 'A';
        out += mult * (uint64_t)pow(36, i++);
    }
    int bitlen = floor(log2(out) + 1);
    CodeData ret;
    MEM_ZERO(ret);
    short gamecode = 0;
    int ipc = 0, pc = 0, gc = 0;
    for (int i = 0; i < bitlen; ++i) {
        char c = pattern[i % PLEN];
        auto bit = GETBIT(out, i);
        switch (c) {
            case 'A': ret.ip |= bit << ipc++; break;
            case 'B': ret.port |= bit << pc++; break;
            case 'C': gamecode |= bit << gc++; break;
        }
    }
    ret.gameLength = gamecode & 0b1111;
    ret.itemMode = (gamecode >> 4) & 0b11;
    ret.player = (gamecode >> 6) & 0b11;
    std::cout << (ret.ip & 0xFF) << '.'
            << ((ret.ip >> 8) & 0xFF) << '.'
            << ((ret.ip >> 16) & 0xFF) << '.'
            << ((ret.ip >> 24) & 0xFF) << std::endl;
    return ret;
}

int getIP(uint *ip)
{
    string ips;
    try {
        io_service io_service;
        tcp::resolver resolver(io_service);
        tcp::resolver::query query("api.ipify.org", "http");
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query), end;
        tcp::socket socket(io_service);

        error_code ec = error::not_connected;
        while (ec && endpoint_iterator != end) {
            socket.close();
            socket.connect(*endpoint_iterator++, ec);
        }
        if (ec)
            return ec.value();
        // form the HTTP request
        streambuf request;
        std::ostream request_stream(&request);
        request_stream << "GET / HTTP/1.0\r\n";
        request_stream << "Host: api.ipify.org\r\n\r\n";
        // send it
        write(socket, request, ec);
        if (ec)
            return ec.value();
        // read the status code and HTTP code
        streambuf response;
        read_until(socket, response, "\r\n", ec);
        if (ec)
            return ec.value();
        // is our response valid?
        std::istream response_stream(&response);
        string http_version;
        response_stream >> http_version;
        unsigned int status_code;
        response_stream >> status_code;
        string status_message;
        std::getline(response_stream, status_message);
        if (!response_stream || http_version.substr(0, 5) != "HTTP/")
            return 1; // invalid response
        if (status_code != 200)
            return status_code; // not OK
        // read the header
        std::string header;
        while (std::getline(response_stream, header) && header != "\r") {
        }
        // finally, read the IP address
        std::ostringstream output;
        if (response.size() > 0)
            output << &response;
        while (read(socket, response, transfer_at_least(1), ec)) output << &response;
        // output is now our ip address.
        // we can now exit the clause, but make sure we know our IP!
        ips = output.str();
    } catch (std::exception &e) {
        return 1; // any exceptions we just retun 1
    }
    std::istringstream ipstream(ips);
    char buf;
    *ip = 0;
    // fill the bytes in properly.
    for (int i = 0; i < 4; i++) {
        int read;
        ipstream >> read >> buf;
        *ip |= (read << (i * 8));
    }
    return 0;
}