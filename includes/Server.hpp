#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <map>
#include <poll.h> // for poll()
#include <cstring> // for memset
#include <unistd.h> // for close()
#include <fcntl.h> // File control definitions
#include <netinet/in.h> // Internet address family
#include <sys/socket.h> // Socket definitions
#include <arpa/inet.h> // Internet operations definitions
#include "Client.hpp"
#include "Channel.hpp"

enum NicknameOperation {
    CHECK,
    REGISTER,
    UNREGISTER
};

class Command;

class Server
{
    private:
        int _port; // Server listening port
        std::string _password; // Server password
        int _serverSocket; // Server socket file descriptor
        std::vector<pollfd> _pollFds; // Poll file descriptors || This vector tracks ALL file descriptors the server needs to monitor (server socket + all client sockets)
        std::map<std::string, Channel*> _channels;// channel name → Channel object
        std::map<int, Client*> _clients; // socket FD → Client object
        std::map<std::string, Client*> _registeredNicknames; // nickname → Client object

        void setupServerSocket(); 
        void acceptNewConnection();
        void handleClientData(int ClientSocket);

    public:
        Server(int port, const std::string& password);
        ~Server();
        
        void run();

        bool manageNickname(const std::string &nickname, Client* client, NicknameOperation op);

        const std::string& getPassword() const;
        Channel* createOrGetChannel(const std::string& channelName);
        Channel* getChannel(const std::string& channelName);
        const std::map<int, Client*>& getClients() const;
        Client* getClientByNickname(const std::string& nickname) const;
        
};

#endif