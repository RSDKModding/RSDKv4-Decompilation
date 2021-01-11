#include "RetroEngine.hpp"

#if RETRO_PLATFORM == RETRO_WIN && _MSC_VER
#include <SDKDDKVer.h> //gives it a macro it wants, will make a note otherwise
#endif

#include <asio.hpp>
#include <iostream>
#include <thread>
#include <memory>

inline uint64_t GETBIT(uint64_t i, int n) { return (i & (1LL << n)) >> n; };

using namespace asio;
using ip::tcp;
using ip::udp;
using std::string;
using std::cout;
using std::endl;

const char *pattern = "AABAABC";
#define PLEN (7)

char index[36];
void buildNetworkIndex() {
    for (byte i = 0; i < 26; ++i) index[i] = i + 'A';
    for (byte i = 0; i < 10; ++i) index[i + 26] = i + '0';
}

udp::socket* udpSocket = nullptr;
udp::endpoint otherEndpoint;
CodeData datas[2]; //0 is my end
std::thread runner;

string connectedIP;

string generateCode(ushort port, int gameLength, int itemMode)
{
    uint64_t code = 0;
    uint ip;
    if (getIP(&ip))
        return string();
    datas[0].ip = ip;
    datas[0].port = port;
    datas[0].gameLength = gameLength;
    datas[0].itemMode = itemMode;
    datas[0].player = playerListPos;
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
    datas[0].codeString = out.str().c_str();
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
    //ret.ip = 127 | 1 << 24;
    std::ostringstream ip;
    ip << (ret.ip & 0xFF) << '.'
            << ((ret.ip >> 8) & 0xFF) << '.'
            << ((ret.ip >> 16) & 0xFF) << '.'
            << ((ret.ip >> 24) & 0xFF);
    ret.ipString = ip.str();
    ret.codeString = code;
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

void runServer(ushort);
void runClient(CodeData);

void initServer(ushort port) {
    runner = std::thread(runServer, port);
}
void initClient(CodeData data) {
    runner = std::thread(runClient, data);
}

void runServer(ushort port) {
    io_context io_context;
    MEM_ZERO(datas[1]);
    udpSocket = new udp::socket(io_context, udp::endpoint(udp::v4(), port));
    while (udpSocket->is_open()) {
        udp::endpoint remote_endpoint;
        error_code ignored;
        char recv_buf[0x1000]; 
        MEM_ZEROP(recv_buf);
        cout << "waiting for bytes" << endl;
        udpSocket->receive_from(buffer(recv_buf), remote_endpoint, 0, ignored);
        cout << "recieved " << +recv_buf[0] << " from " << remote_endpoint.address().to_v4().to_string() << ":" << remote_endpoint.port() << endl;
        if (datas[1].ip && datas[1].ipString.compare(remote_endpoint.address().to_v4().to_string())) {
            sendData(0x80, 0, nullptr, &remote_endpoint); //you're not who we're looking for
            continue;
        }
        char flag = recv_buf[0] & 0x7F, error = recv_buf[0] & 0x80;
        ushort datalen = *(short*)(recv_buf + 1);
        if (error) {
            udpSocket->close(ignored);
            receive2PVSMatchCode(1);
            break; //if, at ANY point, it no longer succeeds, break the whole loop
        }
        switch (flag) {
            default: continue;
            case 0x00: { //game code, this aint our business at this point
                receive2PVSData((MultiplayerData*)(recv_buf + 3));
                break;
            }
            case 0x01: { //first response, "introduce ourselves", make sure the code's correct
                string code((char*)(recv_buf + 3), datalen);
                if (code.compare(datas[0].codeString)) {
                    sendData(0x81, 0, nullptr, &remote_endpoint); //incorrect code
                    continue;
                }
                otherEndpoint = remote_endpoint; //friend!!!!!
                sendData(0x41, 0, nullptr);
                break;
            }
            case 0x02: { //after first client response, this has the code. after this one server starts
                datas[1] = parseCode(string((char*)(recv_buf + 3), datalen));
                receive2PVSMatchCode((datas[1].gameLength << 4) + (datas[1].itemMode << 8) + (datas[1].player << 12));
                break;
            }
        }
    }
}
void runClient(CodeData data) {
    io_context io_context;
    datas[1] = data;
    udp::resolver resolver(io_context);
    otherEndpoint =
      *resolver.resolve(udp::v4(), data.ipString, std::to_string(data.port)).begin();
    udpSocket = new udp::socket(io_context, otherEndpoint);
    udpSocket->connect(otherEndpoint);
    sendData(0x01, datas[1].codeString.length(), (void*)datas[1].codeString.c_str());
    while (udpSocket->is_open()) {
        udp::endpoint remote_endpoint;
        error_code ignored;
        char recv_buf[0x1000]; 
        MEM_ZEROP(recv_buf);
        cout << "waiting for bytes" << endl;
        udpSocket->receive_from(buffer(recv_buf), remote_endpoint, 0, ignored);
        cout << "recieved " << +recv_buf[0] << " from " << remote_endpoint.address().to_v4().to_string() << ":" << remote_endpoint.port() << endl;
        if (datas[1].port != remote_endpoint.port() || datas[1].ipString.compare(remote_endpoint.address().to_v4().to_string())) {
            sendData(0x80, 0, nullptr, &remote_endpoint); //you're not who we're looking for
            continue;
        }
        char flag = recv_buf[0] & 0x7F, error = recv_buf[0] & 0x80;
        ushort datalen = *(short*)(recv_buf + 1);
        if (error) {
            udpSocket->close(ignored);
            receive2PVSMatchCode(1);
            break; //if, at ANY point, it no longer succeeds, break the whole loop
        }
        switch (flag) {
            case 0x00: { //game code, this aint our business at this point
                receive2PVSData((MultiplayerData*)(recv_buf + 3));
                break;
            }
            case 0x41: { //after first response from the server, "send me your code." we start after waiting a little bit
                sendData(0x02, datas[0].codeString.length(), (void*)datas[0].codeString.c_str());
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                receive2PVSMatchCode((datas[1].gameLength << 4) + (datas[1].itemMode << 8) + (datas[1].player << 12));
                break;
            }
        }
    }
}

int sendData(byte flag, ushort datalen, void* data, void *endpoint) {
    if (!udpSocket || !udpSocket->is_open()) return 1;
    if (!endpoint) endpoint = &otherEndpoint;
    error_code ec;
    char sendbuf[0x1000];
    MEM_ZEROP(sendbuf);
    *sendbuf = flag;
    *(sendbuf + 1) = datalen;
    memcpy(sendbuf + 3, data, datalen);
    udpSocket->send_to(buffer(sendbuf), *(udp::endpoint*)endpoint, 0, ec);
    cout << "sent " << +flag << " to " << (*(udp::endpoint*)endpoint).address().to_v4().to_string() << endl;
    if (ec) return ec.value();
    return 0;
}