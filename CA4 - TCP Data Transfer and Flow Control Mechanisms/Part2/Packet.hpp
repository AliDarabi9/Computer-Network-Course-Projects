#ifndef PACKET_HPP
#define PACKET_HPP

#include <string>


#include "Constants.hpp"

class Packet
{
private:
    int id_;
    const char* data_;

public:
    Packet(int id, const char* data);

    static const char* encode(const Packet& packet);
    static Packet decode(const char* str);

    void show() const;

    int getId();
    const char* getData();
};
#endif
