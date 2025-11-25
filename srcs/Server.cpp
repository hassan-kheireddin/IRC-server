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

void Server::setupServerSocket() // handles the creation and configuration of the server socket. This includes socket creation, binding, and listening.
{
    _serverSocket = socket(AF_INET, SOCK_STREAM, 0); // socket():creates a new socket| AF_INET:use IPv4| SOCK_STREAM: Socket type (TCP) | 0: Default protocol
    
    if (_serverSocket < 0)
        throw runtime_error("Failed to create socket");

    int opt = 1;
    if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)// Allows reusing the same port quickly after restart
        throw runtime_error("setsockopt failed");

    fcntl(_serverSocket, F_SETFL, O_NONBLOCK);// Sets socket to non-blocking mode
    
    // IPv4 Structure for server address
    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0 - listens(bind) on all available interfaces
    serverAddr.sin_port = htons(_port); // Converts port number to network byte order

    if (bind(_serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) // Binds socket to the specified port on the local machine
        throw runtime_error("Bind failed");

    if (listen(_serverSocket, 10) < 0) // Prepares it to listen for incoming connections
        throw runtime_error("Listen failed");

    pollfd pfd;
    pfd.fd = _serverSocket;
    pfd.events = POLLIN;
    _pollfds.push_back(pfd);

    Messages::RPL_CREATED(" Server is up and running on port " + to_string(_port));
}

void Server::run() {
    while (true) {
        int ret = poll(&_pollfds[0], _pollfds.size(), -1); // Waits for events on the monitored file descriptors
        if (ret < 0)
            throw runtime_error("Poll failed");

        for (size_t i = 0; i < _pollfds.size(); ++i) // Iterates through all monitored file descriptors
        {
            if (_pollfds[i].revents & POLLIN) // Checks if there's data to read
            {
                if (_pollfds[i].fd == _serverSocket) // New incoming connection
                {
                    acceptNewConnection();
                }
                else // Data from existing client
                {
                    handleClientData(_pollfds[i].fd);
                }
            }
        }
    }
}

void Server::acceptNewConnection() //accepting new client connections
{
    sockaddr_in clientAddr;
    socklen_t len = sizeof(clientAddr);
    int clientFd = accept(_serverSocket, (struct sockaddr *)&clientAddr, &len);
    if (clientFd < 0)
        return;

    fcntl(clientFd, F_SETFL, O_NONBLOCK);

    pollfd clientPoll;
    clientPoll.fd = clientFd;
    clientPoll.events = POLLIN;
    _pollfds.push_back(clientPoll);

    string ip = inet_ntoa(clientAddr.sin_addr); // converts the client's IP address to a human-readable string
    _clients[clientFd] = new Client(clientFd, ip);

    cout << "New client connected: " << clientFd << " (IP: " << ip << ")\n";
}

void Server::handleClientData(int clientFd) //processes data received from a client
{
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    ssize_t bytes = recv(clientFd, buffer, sizeof(buffer), 0); // Receives data from the client

    if (bytes <= 0) {
        cout << "Client disconnected: " << clientFd << "\n";
    
        // Cleanup nickname if set
        Client* client = _clients[clientFd];
        string nick = client->getNickname();
        if (!nick.empty())
            ManageNickname(nick, nullptr, UNREGISTER);
    
        // Close socket
        close(clientFd);
    
        // Remove from poll list
        for (size_t i = 0; i < _pollfds.size(); ++i) {
            if (_pollfds[i].fd == clientFd) {
                _pollfds.erase(_pollfds.begin() + i);
                break;
            }
        }
    
        // Delete client
        delete client;
        _clients.erase(clientFd);
        return;
    }
    

    Client* client = _clients[clientFd];
    client->appendToBuffer(string(buffer, bytes)); // Append received data to client's buffer

    string& fullBuffer = client->getBuffer(); // Reference to client's buffer
    size_t pos;
    while ((pos = fullBuffer.find("\n")) != string::npos) // Process complete commands (terminated by newline)
    {
        string commandLine = fullBuffer.substr(0, pos);
        if (!commandLine.empty() && commandLine[commandLine.length() - 1] == '\r')
            commandLine = commandLine.substr(0, commandLine.length() - 1);
        fullBuffer.erase(0, pos + 1);
    
        string displayName = client->getNickname().empty() ? "(unknown)" : client->getNickname();
        cout << "Parsing command from client " << clientFd << " (" << displayName << "): " << commandLine << "\n";
        
        Command::executeCommand(commandLine, client, *this);
    }
    
}

bool Server::manageNickname(const string &nickname, Client* client, NicknameOperation op) {
    switch (op) {
        case CHECK:
            return _nickToClient.find(nickname) != _nickToClient.end();
        case REGISTER:
            _nickToClient[nickname] = client;
            return true;
        case UNREGISTER:
            _nickToClient.erase(nickname);
            return true;
        default:
            return false;
    }
}

// Usage:
// if (server.manageNickname("nick", nullptr, CHECK)) { ... }
// server.manageNickname("nick", client, REGISTER);
// server.manageNickname("nick", nullptr, UNREGISTER);


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

const map<int, Clients*>& Server::getClients() const
{
    return _clients;
}

Clients* Server::getClientByNickname(const string& nickname) const
{
    for (map<int, Clients*>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (it->second->getNickname() == nickname)
            return it->second;
    }
    return nullptr;
}

/*

Client Decides to Connect
 What happens:
 1. Client creates socket
 2. DNS lookup: irc.myserver.com → IP address
 3. TCP 3-way handshake:
    Client → SYN → Server
    Client ← SYN-ACK ← Server  
    Client → ACK → Server
 4. Connection established!

Server Accepts Connection
     Creates new socket specifically for this client
     Adds to poll() monitoring list
     Creates Client object to track this user
*/