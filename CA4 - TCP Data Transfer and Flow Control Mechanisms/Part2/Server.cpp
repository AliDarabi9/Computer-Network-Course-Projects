#include "Server.hpp"

Server::Server(const char* ip, uint16_t port) {
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) == -1) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
}

Server::~Server() {
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }
    close(server_socket);
}


void Server::sendTo(int client_socket,int packet_id,const char* message){
    std::cout << "pack id : " << packet_id << std::endl;
    const char* packetData = Packet::encode(Packet(packet_id,message));;
    send(client_socket, packetData, strlen(packetData), 0);
}


void Server::start() {
    std::cout << "Server: waiting for connections..." << std::endl;

    while (true) {
        int client_socket = accept(server_socket, nullptr, nullptr);
        std::cout << "connect to client : "<< client_socket << std::endl;
        if (client_socket == -1) {
            perror("Accept failed");
            continue;
        }
        std::cout << "Server: connection accepted" << std::endl;
        threads.emplace_back(&Server::handle_client, this, client_socket);
    }
}

void Server::handle_client(int client_socket) {
    char buffer[1024];
    bool flag7 = true;
    bool flag8 = true;

    memset(buffer, 0, sizeof(buffer));
    recv(client_socket, buffer, sizeof(buffer), 0);
    std::lock_guard<std::mutex> lock(cout_mutex);
    std::cout << "Server: received " << buffer << std::endl;
    const char* data = Packet::decode((char*)buffer).getData();
    
    if (strcmp(data, SYN) == 0) {

        sendTo(client_socket,0,SYN_ACK);
        std::cout << "Server: sent " << SYN_ACK << std::endl;

        memset(buffer, 0, sizeof(buffer));
        recv(client_socket, buffer, sizeof(buffer), 0);
        std::cout << "Server: received " << buffer << std::endl;

        data = Packet::decode((char*)buffer).getData();

        if (strcmp(data, ACK) == 0) {
            for(int i=0 ; i < 101 ; i++){

                memset(buffer, 0, sizeof(buffer));
                recv(client_socket, buffer, sizeof(buffer), 0);
                std::cout << "Server: received " << buffer << std::endl;

                Packet packet = Packet::decode((char*)buffer);

                if (strcmp(packet.getData(), DEFAULT_MESSAGE) == 0) {
                    if(lastPacketId + 1 == packet.getId()){
                        if(packet.getId() == 10 && flag7){  
                                flag7 = false;
                        }
                        else if(packet.getId() == 23 && flag8){
                                flag8 = false;
                        }
                        else{
                            lastPacketId = packet.getId();
                        }
                    }
                    sendTo(client_socket,lastPacketId,ACK);
                    std::cout << "Server: sent " << ACK << "of packet (" << lastPacketId << ")"<< std::endl;
                }
            }

        }
    }
    close(client_socket);
}