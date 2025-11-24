#include "../includes/Messages.hpp"

using namespace std;

Server::Server(int port, const string& password) : _port(port), _password(password), _serverSocket(-1)
{
    setupServerSocket();
}

Server::~Server()
{
    close(_serverSocket);
}

void Server::setupServerSocket()
{
    _serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverSocket < 0)
        throw std::runtime_error("Failed to create socket");

    int opt = 1;
    if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)// Allows reusing the same port quickly after restart
        throw std::runtime_error("setsockopt failed");

    fcntl(_serverSocket, F_SETFL, O_NONBLOCK);// Sets socket to non-blocking mode

    sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(_port);

    if (bind(_serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) // Binds socket to the specified port on the local machine
        throw std::runtime_error("Bind failed");

    if (listen(_serverSocket, 10) < 0) // Prepares it to listen for incoming connections
        throw std::runtime_error("Listen failed");

    pollfd pfd;
    pfd.fd = _serverSocket;
    pfd.events = POLLIN;
    _pollfds.push_back(pfd);

    std::cout << "Server is listening on port " << _port << "\n";
}


Channel* Server::createOrGetChannel(const string& channelName)
{
    if (_channels.find(channelName) == _channels.end())
        _channels[channelName] = new Channel(channelName);
    return _channels[channelName];
}

const string& Server::getPassword() const
{
    return _password;
}

const map<int, Users*>& Server::getUser() const
{
    return _users;
}

Users* Server::getUserByNickname(const string& nickname) const
{
    for (map<int, Users*>::const_iterator it = _users.begin(); it != _users.end(); ++it)
    {
        if (it->second->getNickname() == nickname)
            return it->second;
    }
    return nullptr;
}

