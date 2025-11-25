#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include "Command.hpp"
using namespace std;

class Client
{
    private:
        int _socketFd; // Client socket file descriptor
        string _ipAddr;
        string _nickname;
        string _username;
        string _realname;
        string _buffer; // Buffer to store incoming data
        bool _hasSentPass;
        bool _hasSentNick;
        bool _hasSentUser;
        bool _isRegistered;
        bool _isAuthenticated;

    public:
        Client(int socketFd, const string& ipAddr);
        ~Client();

        int getSocketFd() const;
        const string& getNname() const;
        const string& getUname() const;
        const string& getRname() const;
        bool isReg() const;
        bool isAuth() const;

        void setRname(const string& realname);
        void setNname(const string& nickname);
        void setUname(const string& username);
        void HasSentPass(bool status);
        void HasSentNick(bool status);
        void HasSentUser(bool status);
        void setAuth(bool status);
        void setReg(bool status);
        const string& getBuffer() const;
        void appendToBuffer(const string& data);
        void clearBuffer();
};

#endif