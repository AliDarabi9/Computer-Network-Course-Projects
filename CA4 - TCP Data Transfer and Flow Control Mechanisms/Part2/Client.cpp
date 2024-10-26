#include "Client.hpp"
#include "Packet.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>

Client::Client(const char* ip, uint16_t port)
    : congestionControl() {
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &server_addr.sin_addr);
}

void Client::sendTo(int client_socket, const char* message) {
    const char* packetData = Packet::encode(Packet(packetId, message));
    packetBuffer[packetId] = packetData;
    send(client_socket, packetData, strlen(packetData), 0);
    packetId++;
}

void Client::retransmit(int packetId) {
    if (packetBuffer.find(packetId) != packetBuffer.end()) {
        const char* packetData = packetBuffer[packetId].c_str();
        send(client_socket, packetData, strlen(packetData), 0);
        std::cout << "Client: retransmitted packet " << packetId << std::endl;
    }
}

void Client::start() {
    if (connect(client_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connect failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    perform_handshake();
    send_data();

    close(client_socket);
}

void Client::perform_handshake() {
    sendTo(client_socket, SYN);
    std::cout << "Client: sent " << SYN << std::endl;

    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    recv(client_socket, buffer, sizeof(buffer), 0);
    std::cout << "Client: received " << buffer << std::endl;
    const char* data = Packet::decode(buffer).getData();

    if (strcmp(data, SYN_ACK) == 0) {
        sendTo(client_socket, ACK);
        std::cout << "Client: sent " << ACK << std::endl;
    }
}

void Client::send_data() {
    int packetsToSend = 40; 
    int numberOfLossPacket = 0; 
    for (int i = 1; i <= packetsToSend; i++) {
        std::cout << "Client: sent " << DEFAULT_MESSAGE << "("<< packetId <<")"<< " with cwnd = " << congestionControl.getCwnd() << std::endl;
        sendTo(client_socket, DEFAULT_MESSAGE);
        

        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        if (recv(client_socket, buffer, sizeof(buffer), 0) > 0) {
            std::cout << "Client: received " << buffer << std::endl;
            bool isDupAck = isDuplicateAck(buffer);
            if(isDupAck){
                numberOfLossPacket++;
            }else if(numberOfLossPacket > 0){
                numberOfLossPacket--;
            }
            congestionControl.onPacketAcked(isDupAck,numberOfLossPacket);
            if (isDupAck && congestionControl.getState() == State::FAST_RECOVERY) {
                i = lastAckId+1; 
                packetId = i ;
                std::cout << "retransmit packet " << packetId << std::endl;
            }
        } else {
            congestionControl.onTimeout();
        }
    }
}



bool Client::isDuplicateAck(const char* ack) {

    
    int currentAckId = Packet::decode(ack).getId();
    if (currentAckId == lastAckId) {
        return true;
    }
    lastAckId = currentAckId;
    return false;
}
