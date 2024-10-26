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

class Router : public Node {
public:
    Router(int port, int id, int HostAPort, int HostBPort) : Node(port, id), HostAPort(HostAPort), HostBPort(HostBPort) {}

    void run() override {
        struct sockaddr_in HostAAddr, HostBAddr;
        memset(&HostAAddr, 0, sizeof(HostAAddr));
        HostAAddr.sin_family = AF_INET;
        HostAAddr.sin_port = htons(HostAPort);
        inet_pton(AF_INET, "127.0.0.1", &HostAAddr.sin_addr);

        memset(&HostBAddr, 0, sizeof(HostBAddr));
        HostBAddr.sin_family = AF_INET;
        HostBAddr.sin_port = htons(HostBPort);
        inet_pton(AF_INET, "127.0.0.1", &HostBAddr.sin_addr);

        std::thread sendThread(&Router::sendMessages, this, HostAAddr, HostBAddr);
        std::thread receiveThread(&Router::receiveMessages, this);

        sendThread.join();
        receiveThread.join();
    }

private:
    int HostAPort;
    int HostBPort;

    std::deque<Packet*> buffer_message;

    void sendMessages(struct sockaddr_in HostAAddr, struct sockaddr_in HostBAddr) {
        Packet *packet;
        std::string message;

        while (true) {
            if(!buffer_message.empty()){
                packet = buffer_message.front();
                buffer_message.pop_front(); 
                if(packet->sourceId == 1){   
                    packet->sourceId = 2;
                    serializePacket(*packet, buffer);
                    sendto(sockfd, buffer, sizeof(buffer), 0, (const struct sockaddr *) &HostBAddr, sizeof(HostBAddr));
                }
                else if(packet->sourceId == 3){
                    packet->sourceId = 2;
                    serializePacket(*packet, buffer);
                    sendto(sockfd, buffer, sizeof(buffer), 0, (const struct sockaddr *) &HostAAddr, sizeof(HostAAddr));
                }
                delete packet;
            }
        }
    }

    void receiveMessages() {
        struct sockaddr_in senderAddr;
        socklen_t len = sizeof(senderAddr);
        while (true) {
            Packet *packet = new Packet();
            int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *) &senderAddr, &len);
            deserializePacket(buffer, *packet);
             if(packet->sourceId == 2){
                continue;
            }
            
           
            buffer_message.push_back(packet);
            
        }
    }
};

int main() {
    int HostAPort = 5018;
    int RouterPort = 5019;
    int HostBPort = 5020;

    Router router(RouterPort, 2, HostAPort, HostBPort);

    router.run();

    return 0;
}
