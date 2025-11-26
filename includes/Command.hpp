#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
#include <vector>
#include "Client.hpp"
#include "Server.hpp"
#include <sys/socket.h> //for send() : it is used for sending messages to clients 

using namespace std;

class Command {
    private:
        static void AUTHENTICATE(Client& client, Server& server);
        static void PASS(const vector<string>& params, Client& client, Server& server);
        static void NICK(const vector<string>& params, Client& client, Server& server);
        static bool isValidNickname(const string& nickname);
        static void USER(const vector<string>& params, Client& client, Server& server);

        static void JOIN(const vector<string>& params, Client& client, Server& server);
        static void KICK(const vector<string>& params, Client& client, Server& server);
        static void INVITE(const vector<string>& params, Client& client, Server& server);
        static void TOPIC(const vector<string>& params, Client& client, Server& server);
        static void MODE(const vector<string>& params, Client& client, Server& server);
        static void PRIVMSG(const vector<string>& params, Client& client, Server& server);

    public:
        static void executeCommand(const string& commandLine, Client& client, Server& server);
};

#endif