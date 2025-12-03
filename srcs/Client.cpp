#include "../includes/Client.hpp"

Client::Client(int socketFd, const std::string& ipAddr) : _socketFd(socketFd), _ipAddr(ipAddr), _nickname(""), _username(""), _realname(""), _buffer(""), _hasSentPass(false), _hasSentNick(false), _hasSentUser(false), _isRegistered(false), _isAuthenticated(false) {}

Client::~Client() {}

int Client::getSocketFd() const
{
    return _socketFd;
}

int Client::getFd() const
{
    return _socketFd;
}

const std::string& Client::getNickname() const
{
    return _nickname;
}

const std::string& Client::getUsername() const
{
    return _username;
}

const std::string& Client::getRealname() const
{
    return _realname;
}

bool Client::isReg() const
{
    return _isRegistered;
}

bool Client::isAuth() const
{
    return _isAuthenticated;
}

bool Client::hasSentPass() const
{
    return _hasSentPass;
}

bool Client::hasSentNick() const
{
    return _hasSentNick;
}

bool Client::hasSentUser() const
{
    return _hasSentUser;
}

void Client::HasSentPass(bool status)
{
    _hasSentPass = status;
}

void Client::HasSentNick(bool status)
{
    _hasSentNick = status;
}

void Client::HasSentUser(bool status)
{
    _hasSentUser = status;
}

void Client::setNickname(const std::string& nickname)
{
    _nickname = nickname;
}

void Client::setUsername(const std::string& username)
{
    _username = username;
}

void Client::setRealname(const std::string& realname)
{
    _realname = realname;
}

void Client::setReg(bool status)
{
    _isRegistered = status;
}

void Client::setAuth(bool status)
{
    _isAuthenticated = status;
}

std::string& Client::getBuffer()
{
    return _buffer;
}

void Client::appendToBuffer(const std::string& data)
{
    _buffer += data;
}

void Client::clearBuffer()
{
    _buffer.clear();
}