#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
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
        int getFd() const;
        const string& getNickname() const;
        const string& getUsername() const;
        const string& getRealname() const;
        bool isReg() const;
        bool isAuth() const;
        bool hasSentPass() const;
        bool hasSentNick() const;
        bool hasSentUser() const;

        void setRealname(const string& realname);
        void setNickname(const string& nickname);
        void setUsername(const string& username);
        void HasSentPass(bool status);
        void HasSentNick(bool status);
        void HasSentUser(bool status);
        void setAuth(bool status);
        void setReg(bool status);
        string& getBuffer();
        void appendToBuffer(const string& data);
        void clearBuffer();
};

#endif