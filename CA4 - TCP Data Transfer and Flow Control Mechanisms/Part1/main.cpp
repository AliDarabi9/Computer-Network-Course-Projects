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
        Client client2("127.0.0.1", 5500);
        Client client3("127.0.0.1", 5500);
        client1.start();
        client2.start();
        client3.start();
    }
    return 0;
}