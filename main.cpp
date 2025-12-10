#include <iostream>
#include <cstdlib>
#include "includes/Server.hpp"
#include "includes/Client.hpp"
#include "includes/Command.hpp"
#include <string>
#include <vector>
#include <csignal>

Server* g_server = NULL;

void signalHandler(int signal) {
    (void)signal;
    std::cout << "\nShutting down server...\n";
    if (g_server) {
        delete g_server;
        g_server = NULL;
    }
    exit(0);
}

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
    if (argc != 3) {
        std::cerr << "Usage: ./ircserv <port> <password>\n";
        return 1;
    }

    int port = atoi(argv[1]);
    
    if (port <= 0 || port > 65535) {
        std::cerr << "Invalid port number.\n";
        return 1;
    }

    if(!check_password(argv[2]))
    {
        std::cerr << "Wrong password format!\n";
        return 1;
    }
    
    std::string password = argv[2];
    
    // Register signal handler for Ctrl+C
    signal(SIGINT, signalHandler);
    
    try {
        g_server = new Server(port, password);
        g_server->run();
    } catch (const std::exception &e) {
        std::cerr << "Server error: " << e.what() << "\n";
        if (g_server) {
            delete g_server;
            g_server = NULL;
        }
    }
    
    return 0;
}