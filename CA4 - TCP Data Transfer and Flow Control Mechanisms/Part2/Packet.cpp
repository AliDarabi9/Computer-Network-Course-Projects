#include "Packet.hpp"

#include <iostream>
#include <sstream>
#include <cstring>

Packet::Packet(int id, const char* data)
    : id_(id), data_(data)
{
}

const char* Packet::encode(const Packet& packet) {

    std::stringstream ss;
    ss << packet.id_ << ',' << packet.data_;
    
    char* encoded = new char[ss.str().length() + 1];
    std::strcpy(encoded, ss.str().c_str());
    return encoded;
}

Packet Packet::decode(const char* str) {

    std::stringstream ss(str);
    std::string token;
    std::getline(ss, token, ',');
    int id = std::stoi(token);
    std::getline(ss, token);
    const char* data = token.c_str();
    return Packet(id, data);
}

void Packet::show() const {

    std::cout << "Packet ID: " << id_ << std::endl;
    std::cout << "Packet Data: " << data_ << std::endl;
}

int Packet::getId(){
    return id_;
}

const char* Packet::getData(){
    return data_;
}
