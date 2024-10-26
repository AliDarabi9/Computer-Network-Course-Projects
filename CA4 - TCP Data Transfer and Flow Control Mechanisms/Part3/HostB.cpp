#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <deque>

const int BUFFER_SIZE = 1536;

struct Packet {
    int sourceId;
    int seq_num;
    char message[BUFFER_SIZE];
};

void serializePacket(const Packet& packet, char* buffer) {
    memcpy(buffer, &packet.sourceId, sizeof(packet.sourceId));
    memcpy(buffer + sizeof(packet.sourceId), &packet.seq_num, sizeof(packet.seq_num));
    memcpy(buffer + sizeof(packet.sourceId) + sizeof(packet.seq_num), packet.message, sizeof(packet.message));
}

void deserializePacket(const char* buffer, Packet& packet) {
    memcpy(&packet.sourceId, buffer, sizeof(packet.sourceId));
    memcpy(&packet.seq_num, buffer + sizeof(packet.sourceId), sizeof(packet.seq_num));
    memcpy(packet.message, buffer + sizeof(packet.sourceId) + sizeof(packet.seq_num), sizeof(packet.message));
}

class Node {
public:
    Node(int port, int id) : port(port), id(id) {
        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            std::cerr << "Socket creation failed" << std::endl;
            exit(EXIT_FAILURE);
        }

        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);

        if (bind(sockfd, (const struct sockaddr *)&addr, sizeof(addr)) < 0) {
            std::cerr << "Bind failed" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    virtual ~Node() {
        close(sockfd);
    }

    virtual void run() = 0;

protected:
    int sockfd;
    int port;
    int id;
    struct sockaddr_in addr;
    char buffer[BUFFER_SIZE];
};

class HostB : public Node {
public:
    HostB(int port, int id, int HostAPort, int RouterPort) : Node(port, id), HostAPort(HostAPort), RouterPort(RouterPort) {}

    void run() override {
        struct sockaddr_in HostAAddr, RouterAddr;
        memset(&HostAAddr, 0, sizeof(HostAAddr));
        HostAAddr.sin_family = AF_INET;
        HostAAddr.sin_port = htons(HostAPort);
        inet_pton(AF_INET, "127.0.0.1", &HostAAddr.sin_addr);

        memset(&RouterAddr, 0, sizeof(RouterAddr));
        RouterAddr.sin_family = AF_INET;
        RouterAddr.sin_port = htons(RouterPort);
        inet_pton(AF_INET, "127.0.0.1", &RouterAddr.sin_addr);

        std::thread sendThread(&HostB::sendMessages, this, HostAAddr, RouterAddr);
        std::thread receiveThread(&HostB::receiveMessages, this);

        sendThread.join();
        receiveThread.join();
    }

private:
    int HostAPort;
    int RouterPort;

    std::deque<Packet*> buffer_message;

    void sendMessages(struct sockaddr_in HostAAddr, struct sockaddr_in RouterAddr) {
        Packet* packet;
        std::string message;

        while (true) {
            if(!buffer_message.empty()){
                std::this_thread::sleep_for(std::chrono::milliseconds(2));
                packet = buffer_message.front();
                buffer_message.pop_front();
                serializePacket(*packet, buffer);
                sendto(sockfd, buffer, sizeof(buffer), 0, (const struct sockaddr *) &RouterAddr, sizeof(RouterAddr));
                std::cout << packet->seq_num << "," << packet->message << std::endl;
                delete packet;
            }
        }
    }

    void receiveMessages() {
        struct sockaddr_in senderAddr;
        socklen_t len = sizeof(senderAddr);
        Packet packet;

        while (true) {
            int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *) &senderAddr, &len);
            deserializePacket(buffer, packet);
            if(packet.sourceId != 3){
            Packet *response_packet = new Packet();
            response_packet->seq_num = packet.seq_num;
            response_packet->sourceId = id;
            std::string ack = "ack";
            strncpy(response_packet->message, ack.c_str(), sizeof(response_packet->message));
            buffer_message.push_back(response_packet);
            if(packet.seq_num == 999){
                std::cout << "HostB received from Node " << packet.sourceId << " seqnum "<< packet.seq_num << " :  " << packet.message << std::endl;
            }
            }
        }
    }
};

int main() {
    int HostAPort = 5018;
    int RouterPort = 5019;
    int HostBPort = 5020;

    HostB hostB(HostBPort, 3, HostAPort, RouterPort);

    hostB.run();

    return 0;
}
