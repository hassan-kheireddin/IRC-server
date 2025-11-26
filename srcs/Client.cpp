#include "../includes/Client.hpp"
using namespace std;

Client::Client(int socketFd, const string& ipAddr) : _socketFd(socketFd), _ipAddr(ipAddr), _nickname(""), _username(""), _realname(""), _buffer(""), _hasSentPass(false), _hasSentNick(false), _hasSentUser(false), _isRegistered(false), _isAuthenticated(false) {}

Client::~Client() {}

int Client::getSocketFd() const
{
    return _socketFd;
}

int Client::getFd() const
{
    return _socketFd;
}

const string& Client::getNickname() const
{
    return _nickname;
}

const string& Client::getUsername() const
{
    return _username;
}

const string& Client::getRealname() const
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

void Client::setNickname(const string& nickname)
{
    _nickname = nickname;
}

void Client::setUsername(const string& username)
{
    _username = username;
}

void Client::setRealname(const string& realname)
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

string& Client::getBuffer()
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