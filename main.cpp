#include <iostream>
#include "Server.hpp"
#include "Client.hpp"
#include "Command.hpp"
#include "Messages.hpp"
#include <string>
#include <vector>

using namespace std;



int check_password(const char *pass)
{
    while(*pass)
    {
        if(*pass == ' ' || *pass == '\t')
            return 0;
        else
            pass++;
    }
    return 1;
}

int main(int argc, char* argv[]) {
    cout << argv[2] << endl;
    
    if (argc != 3) {
        cerr << "Usage: ./ircserv <port> <password>\n";
        return 1;
    }

    int port = atoi(argv[1]);
    
    if (port <= 0 || port > 65535) {
        cerr << "Invalid port number.\n";
        return 1;
    }

    if(!check_password(argv[2]))
    {
        cerr << "Wrong password format!\n";
        return 1;
    }
    
    string password = argv[2];
    
    try {
        Server server(port, password);
        server.run();
    } catch (const exception &e) {
        cerr << "Server error: " << e.what() << "\n";
    }
    
    return 0;
}