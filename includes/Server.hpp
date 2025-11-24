#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <poll.h>
#include "Messages.hpp"
#include "Users.hpp"
#include "Channel.hpp"

using namespace std;

class Server
{
    private:
        int _port;
        string _password;
        int _serverSocket;
        vector<pollfd> _pollFds;
        map<int, Users*> _users;
        map<string, Channel*> _channels;
        Messages _messages;
        map<string, Users*> _registeredNicknames;

        void setupServerSocket();
        void acceptNewConnection();
        void handleUserData(int userSocket);

    public:
        Server(int port, const string& password);
        ~Server();
        
        void run();

        bool isNicknameRegistered(const string& nickname) const;
        void registerNickname(const string& nickname, Users* user);
        void unregisterNickname(const string& nickname);

        const string& getPassword() const;
        Channel* createOrGetChannel(const string& channelName);
        const map<string, Channel*>& getUser() const;
        Users* getUserByNickname(const string& nickname) const;
        
};

    

#endif