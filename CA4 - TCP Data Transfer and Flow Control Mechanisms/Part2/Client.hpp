#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <cstdint>
#include <mutex>
#include <vector>
#include <unordered_map>

#include "CongestionControl.hpp"
#include "Packet.hpp"

class Client {
public:
    Client(const char* ip, uint16_t port);
    void start();

private:
    int client_socket;
    sockaddr_in server_addr;
    CongestionControl congestionControl;
    std::unordered_map<int, std::string> packetBuffer;
    int startWindowIndex = 0;
    inline static int packetId = 0;
    inline static int lastAckId = -1;

    void sendTo(int client_socket, const char* message);
    void perform_handshake();
    void send_data();
    bool isDuplicateAck(const char* ack);
    void retransmit(int packetId);
};

#endif 

