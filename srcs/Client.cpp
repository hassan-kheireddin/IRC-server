#include "../includes/Client.hpp"
using namespace std;

Client::Client(int socketFd, const string& ipAddr) : _socketFd(socketFd), _ipAddr(ipAddr), _isRegistered(false), _isAuthenticated(false), _hasSentPass(false), _hasSentNick(false), _hasSentUser(false) {}

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

const string& Client::getRname() const
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

void Client::setNname(const string& nickname)
{
    _nickname = nickname;
}

void Client::setUname(const string& username)
{
    _username = username;
}

void Client::setRname(const string& realname)
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