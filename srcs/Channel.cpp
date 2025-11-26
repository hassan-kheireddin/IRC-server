#include "../includes/Command.hpp"

using namespace std;

Channel::Channel(const std::string& name) : name(name), topic(""), operatorNickname(""), key(""), clientLimit(0) {}

Channel::~Channel() {}

const string& Channel::getName() const {
    return _name;
}

const string& Channel::getTopic() const {
    return _topic;
}

void Channel::setTopic(const string& topic) {
    _topic = topic;
}

void Channel::addClient(Client* client) {
    _clients.insert(client);
}

void Channel::removeClient(Client* client) {
    if (isOperator(client)) {
        removeOperator(client);
    }
    _clients.erase(client);
}

bool Channel::hasClient(Client* client) const {
    return _clients.find(client) != _clients.end();
}

void Channel::setOperator(Client* client) {
    _operators.insert(client);
}

bool Channel::isOperator(Client* client) const {
    return _operators.find(client) != _operators.end();
}

void Channel::removeOperator(Client* client) {
    _operators.erase(client);
}

void Channel::inviteClient(Client* client) {
    _invited.insert(client);
}

bool Channel::isInviteOnly() const {
    return hasMode('i');
}

void Channel::addInvitation(Client* client) {
    _invited.insert(client);
}


\void Channel::removeInvitation(Client* client) {
    _invited.erase(client);
}

bool Channel::isInvited(Client* client) const {
    return _invited.find(client) != _invited.end();
}

const set<Client*>& Channel::getClients() const {
    return _clients;
}

void Channel::addMode(char mode) {
    _modes.insert(mode);
}

void Channel::removeMode(char mode) {
    _modes.erase(mode);
}   

bool Channel::hasMode(char mode) const {
    return _modes.find(mode) != _modes.end();
}

string Channel::getModes() const {
    string modes;
    for (set<char>::const_iterator it = _modes.begin(); it != _modes.end(); ++it) {
        modes += *it;
    }
    return modes;
}


void Channel::setKey(const string& key) {
    _key = key;
}

const string& Channel::getKey() const {
    return _key;
}

void Channel::removeKey() {
    _key.clear();
}

bool Channel::hasKey() const {
    return !_key.empty();
}

void Channel::setClientLimit(size_t limit) {
    _userLimit = limit;
}

size_t Channel::getClientLimit() const {
    return _userLimit;
}

void Channel::removeClientLimit() {
    _userLimit = 0;
}

bool Channel::hasClientLimit() const {
    return _userLimit > 0;
}

bool Channel::isFull() const {
    return hasClientLimit() && _clients.size() >= _userLimit;
}



