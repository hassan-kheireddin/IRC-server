#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <poll.h>
#include "Messages.hpp"
#include "Clients.hpp"
#include "Channel.hpp"
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "Command.hpp"

using namespace std;

class Server
{
    private:
        int _port;
        string _password;
        int _serverSocket;
        vector<pollfd> _pollFds;
        map<int, Clients*> _Clients;
        map<string, Channel*> _channels;
        Messages _messages;
        map<string, Clients*> _registeredNicknames;

        void setupServerSocket();
        void acceptNewConnection();
        void handleClientData(int ClientSocket);

    public:
        Server(int port, const string& password);
        ~Server();
        
        void run();

        bool manageNickname(const std::string &nickname, Client* client, NicknameOperation op);

        const string& getPassword() const;
        Channel* createOrGetChannel(const string& channelName);
        const map<string, Channel*>& getClient() const;
        Clients* getClientByNickname(const string& nickname) const;
        
};

    

#endif