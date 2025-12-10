#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include <set>
#include <memory>
#include "Client.hpp"

class Channel {
    private:
    std::string _name;
    std::string _topic;
    std::set<Client*> _clients;
    std::set<Client*> _operators;
    std::set<Client*> _invited;
    std::set<char> _modes;
    std::string _key;
    size_t _userLimit;
        
    public:
        Channel(const std::string& name);
        ~Channel();

        const std::string& getName() const;
        const std::string& getTopic() const;
        void setTopic(const std::string& topic);

        void addClient(Client* client);
        void removeClient(const std::string& nickname);
        bool hasClient(const std::string& nickname) const;

        void setOperator(const std::string& nickname);
        const std::string& getOperator() const;
        bool isOperator(Client* client) const;

        void removeOperator(Client* client);

        bool isInviteOnly() const;
        void addInvitation(Client* client);
        void removeInvitation(Client* client);
        bool isInvited(Client* client) const;

        const std::set<Client*>& getClients() const;


        void addMode(char mode);
        void removeMode(char mode);
        bool hasMode(char mode) const;
        std::string getModes() const;
        bool isOperator(const std::string& nickname) const;
        void setKey(const std::string& key);
        const std::string& getKey() const;
        void removeKey();
        bool hasKey() const;

        void setClientLimit(size_t limit);
        size_t getClientLimit() const;
        void removeClientLimit();
        bool hasClientLimit() const;
        bool isFull() const;
    };

#endif