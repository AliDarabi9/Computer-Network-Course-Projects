#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>

#include "Packet.hpp"

class Client {
public:
    Client(const char* ip, uint16_t port);
    void start();

private:
    inline static int globalId_ = 0;
    int id_;
    int client_socket;
    sockaddr_in server_addr;

    void perform_handshake();
    void send_data();
    void sendTo(int client_socket, const char* message);
};

#endif 

