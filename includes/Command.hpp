#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
#include <vector>
#include "Client.hpp"
#include "Server.hpp"
#include <sys/socket.h> //for send() : it is used for sending messages to clients 

class Command {
    private:
        static void AUTHENTICATE(Client& client, Server& server);
        static void PASS(const std::vector<std::string>& params, Client& client, Server& server);
        static void NICK(const std::vector<std::string>& params, Client& client, Server& server);
        static bool isValidNickname(const std::string& nickname);
        static void USER(const std::vector<std::string>& params, Client& client, Server& server);

        static void JOIN(const std::vector<std::string>& params, Client& client, Server& server);
        static void KICK(const std::vector<std::string>& params, Client& client, Server& server);
        static void INVITE(const std::vector<std::string>& params, Client& client, Server& server);
        static void TOPIC(const std::vector<std::string>& params, Client& client, Server& server);
        static void MODE(const std::vector<std::string>& params, Client& client, Server& server);
        static void PRIVMSG(const std::vector<std::string>& params, Client& client, Server& server);
        static void NOTICE(const std::vector<std::string>& params, Client& client, Server& server);

    public:
        static void executeCommand(const std::string& commandLine, Client& client, Server& server);
};

#endif