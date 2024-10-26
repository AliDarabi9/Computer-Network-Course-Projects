#include "Server.hpp"
#include "Client.hpp"


int main() {
    char side = ' ';
    std::cin >> side ;
    if(side == 's'){
        Server server("127.0.0.1", 5500);
        server.start();
    }
    else{
        Client client1("127.0.0.1", 5500);
        client1.start();
    }
    return 0;
}