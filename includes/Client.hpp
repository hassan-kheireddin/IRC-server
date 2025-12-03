#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client
{
    private:
        int _socketFd; // Client socket file descriptor
        std::string _ipAddr;
        std::string _nickname;
        std::string _username;
        std::string _realname;
        std::string _buffer; // Buffer to store incoming data
        bool _hasSentPass;
        bool _hasSentNick;
        bool _hasSentUser;
        bool _isRegistered;
        bool _isAuthenticated;

    public:
        Client(int socketFd, const std::string& ipAddr);
        ~Client();

        int getSocketFd() const;
        int getFd() const;
        const std::string& getNickname() const;
        const std::string& getUsername() const;
        const std::string& getRealname() const;
        bool isReg() const;
        bool isAuth() const;
        bool hasSentPass() const;
        bool hasSentNick() const;
        bool hasSentUser() const;

        void setRealname(const std::string& realname);
        void setNickname(const std::string& nickname);
        void setUsername(const std::string& username);
        void HasSentPass(bool status);
        void HasSentNick(bool status);
        void HasSentUser(bool status);
        void setAuth(bool status);
        void setReg(bool status);
        std::string& getBuffer();
        void appendToBuffer(const std::string& data);
        void clearBuffer();
};

#endif