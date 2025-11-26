#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include <memory>
#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"

using namespace std;

class Channel {
    private:
    std::string _name;
    std::string _topic;
    std::set<Client*> _clients;
    std::set<Client*> _operators;
    std::set<Client*> _invited;
    std::set<char> _modes;
    std::string _key;
    bool _hasKey;
    size_t _userLimit;
    bool _hasUserLimit;
        
    public:
        Channel(const std::string& name);
        ~Channel();

        const string& getName() const;
        const string& getTopic() const;
        void setTopic(const string& topic);

        void addClient(Client* client);
        void removeClient(Client* client);
        bool hasClient(Client* client) const;

        void setOperator(Client* client);
        const string& getOperator() const;
        bool isOperator(Client* client) const;
        void removeOperator(Client* client);

        void inviteClient(Client* client);
        bool isInviteOnly() const;
        void addInvitation(Client* client);
        void removeInvitation(Client* client);
        bool isInvited(Client* client) const;

        const set<Client*>& getClients() const;


        void addMode(char mode);
        void removeMode(char mode);
        bool hasMode(char mode) const;
        string getModes() const;

        void setKey(const string& key);
        const string& getKey() const;
        void removeKey();
        bool hasKey() const;

        void setClientLimit(size_t limit);
        size_t getClientLimit() const;
        void removeClientLimit();
        bool hasClientLimit() const;
        bool isFull() const;
    };

#endif