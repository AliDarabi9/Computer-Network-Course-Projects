#include "Client.hpp"



Client::Client(const char* ip, uint16_t port){
    id_ = ++globalId_;
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &server_addr.sin_addr);
}


void Client::sendTo(int client_socket,const char* message){
    const char* packetData = Packet::encode(Packet(id_,message));;
    send(client_socket, packetData, strlen(packetData), 0);
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
    
    sendTo(client_socket,SYN);
    std::cout << "Client: sent " << SYN << std::endl;

    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    recv(client_socket, buffer, sizeof(buffer), 0);
    std::cout << "Client: received " << buffer << std::endl;
    const char* data = Packet::decode((char *)buffer).getData();

    if (strcmp(data, SYN_ACK) == 0) {
        sendTo(client_socket,ACK);
        std::cout << "Client: sent " << ACK << std::endl;
    }
}

void Client::send_data() {
    sendTo(client_socket,DEFAULT_MESSAGE);
    std::cout << "Client: sent " << DEFAULT_MESSAGE << std::endl;

    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    recv(client_socket, buffer, sizeof(buffer), 0);
    std::cout << "Client: received " << buffer << std::endl;

    Packet::decode((char*)buffer).show();


}

