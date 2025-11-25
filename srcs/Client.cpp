#include "../includes/Client.hpp"

Client::Client(int socketFd, const string& ipAddr) : _socketFd(socketFd), _ipAddr(ipAddr), _isRegistered(false), _isAuthenticated(false){}

Client::~Client() {}

int Client::getSocketFd() const
{
    return _socketFd;
}

const string& Client::getNname() const
{
    return _nickname;
}

const string& Client::getUname() const
{
    return _username;
}

bool Client::isReg() const
{
    return _isRegistered;
}

bool Client::isAuth() const
{
    return _isAuthenticated;
}

void Client::setNname(const string& nickname)
{
    _nickname = nickname;
}

void Client::setUname(const string& username)
{
    _username = username;
}

void Client::setReg(bool status)
{
    _isRegistered = status;
}

void Client::setAuth(bool status)
{
    _isAuthenticated = status;
}

const string& Client::getBuffer() const
{
    return _buffer;
}

void Client::appendToBuffer(const string& data)
{
    _buffer += data;
}

void Client::clearBuffer()
{
    _buffer.clear();
}