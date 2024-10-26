#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <chrono>

const int BUFFER_SIZE = 1536; 
const int N = 4;  

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

class HostA : public Node {
public:
    HostA(int port, int id, int RouterPort, int HostBPort) : Node(port, id), RouterPort(RouterPort), HostBPort(HostBPort) {}

    void run() override {
        struct sockaddr_in RouterAddr, HostBAddr;
        memset(&RouterAddr, 0, sizeof(RouterAddr));
        RouterAddr.sin_family = AF_INET;
        RouterAddr.sin_port = htons(RouterPort);
        inet_pton(AF_INET, "127.0.0.1", &RouterAddr.sin_addr);

        memset(&HostBAddr, 0, sizeof(HostBAddr));
        HostBAddr.sin_family = AF_INET;
        HostBAddr.sin_port = htons(HostBPort);
        inet_pton(AF_INET, "127.0.0.1", &HostBAddr.sin_addr);

        std::thread sendThread(&HostA::sendMessages, this, RouterAddr, HostBAddr);
        std::thread receiveThread(&HostA::receiveMessages, this);

        sendThread.join();
        receiveThread.join();
    }

private:
    int RouterPort;
    int HostBPort;
    int send_base;   
    int nextseqnum;  

    void sendMessages(struct sockaddr_in RouterAddr, struct sockaddr_in HostBAddr) {
        Packet packet;
        packet.sourceId = id;

        send_base = 0;
        nextseqnum = 0;

        std::ifstream file("data.txt", std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Failed to open the file" << std::endl;
            return;
        }

        std::vector<char> fileBuffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();

        int totalPackets = fileBuffer.size() / sizeof(packet.message) + (fileBuffer.size() % sizeof(packet.message) != 0);

        while (true) {
            while (nextseqnum < send_base + N && nextseqnum < totalPackets) {
                std::this_thread::sleep_for(std::chrono::milliseconds(2));
                packet.seq_num = nextseqnum;
                int startPos = nextseqnum * sizeof(packet.message);
                int endPos = std::min(startPos + (int)sizeof(packet.message), static_cast<int>(fileBuffer.size()));
                std::copy(fileBuffer.begin() + startPos, fileBuffer.begin() + endPos, packet.message);

                serializePacket(packet, buffer);
                sendto(sockfd, buffer, sizeof(buffer), 0, (const struct sockaddr *) &RouterAddr, sizeof(RouterAddr));
                std::cout << "HostA sent packet " << packet.seq_num << std::endl;
                nextseqnum++;
            }

        }
    }

    void receiveMessages() {
        struct sockaddr_in senderAddr;
        socklen_t len = sizeof(senderAddr);
        Packet packet;

        while (true) {
            int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, nullptr, nullptr);
            if (n > 0) {
                Packet ack_packet;
                deserializePacket(buffer, ack_packet);
                if(ack_packet.sourceId == 1) {
                    continue;
                }
                send_base++;
                std::cout << "ACK " << ack_packet.seq_num << std::endl;
            }
        }
    }
};

int main() {
    int HostAPort = 5018;
    int RouterPort = 5019;
    int HostBPort = 5020;

    HostA hostA(HostAPort, 1, RouterPort, HostBPort);

    hostA.run();

    return 0;
}
