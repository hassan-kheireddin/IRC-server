#include "../includes/Channel.hpp"

using namespace std;

Channel::Channel(const std::string& name) : _name(name), _topic(""), _key(""), _hasKey(false), _userLimit(0), _hasUserLimit(false) {}

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

void Channel::removeClient(const string& nickname) {
    for (set<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if ((*it)->getNickname() == nickname) {
            if (isOperator(*it)) {
                removeOperator(*it);
            }
            _clients.erase(it);
            return;
        }
    }
}

bool Channel::hasClient(Client* client) const {
    return _clients.find(client) != _clients.end();
}

bool Channel::hasClient(const string& nickname) const {
    for (set<Client*>::const_iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if ((*it)->getNickname() == nickname)
            return true;
    }
    return false;
}

void Channel::setOperator(Client* client) {
    _operators.insert(client);
}

void Channel::setOperator(const string& nickname) {
    for (set<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if ((*it)->getNickname() == nickname) {
            _operators.insert(*it);
            return;
        }
    }
}

const string& Channel::getOperator() const {
    static string empty = "";
    if (_operators.empty())
        return empty;
    return (*_operators.begin())->getNickname();
}

bool Channel::isOperator(Client* client) const {
    return _operators.find(client) != _operators.end();
}

bool Channel::isOperator(const string& nickname) const {
    for (set<Client*>::const_iterator it = _operators.begin(); it != _operators.end(); ++it) {
        if ((*it)->getNickname() == nickname)
            return true;
    }
    return false;
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

void Channel::addInvitation(const string& nickname) {
    for (set<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if ((*it)->getNickname() == nickname) {
            _invited.insert(*it);
            return;
        }
    }
}


void Channel::removeInvitation(Client* client) {
    _invited.erase(client);
}

void Channel::removeInvitation(const string& nickname) {
    for (set<Client*>::iterator it = _invited.begin(); it != _invited.end(); ++it) {
        if ((*it)->getNickname() == nickname) {
            _invited.erase(it);
            return;
        }
    }
}

bool Channel::isInvited(Client* client) const {
    return _invited.find(client) != _invited.end();
}

bool Channel::isInvited(const string& nickname) const {
    for (set<Client*>::const_iterator it = _invited.begin(); it != _invited.end(); ++it) {
        if ((*it)->getNickname() == nickname)
            return true;
    }
    return false;
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



