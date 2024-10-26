#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>

#include "Packet.hpp"





class Server {
public:
    Server(const char* ip, uint16_t port);
    ~Server();
    void start();

private:
    int server_socket;
    sockaddr_in server_addr;
    std::mutex cout_mutex;
    std::vector<std::thread> threads;

    void handle_client(int client_socket);
    void sendTo(int client_socket,const char* message);
};
#endif