#include "../includes/Command.hpp"
#include <iostream>

void Command::executeCommand(const std::string& commandLine, Client& client, Server& server) {

    std::vector<std::string> tokens;
    size_t pos = 0, found;
    while((found = commandLine.find_first_of(' ', pos)) != std::string::npos) {
        tokens.push_back(commandLine.substr(pos, found - pos));
        pos = found + 1;
    }
    tokens.push_back(commandLine.substr(pos));

    if (tokens.empty()) return;

    std::string command = tokens[0];
    std::vector<std::string> params(tokens.begin() + 1, tokens.end());

    if (command == "PASS") {
        PASS(params, client, server);
    } else if (command == "NICK") {
        NICK(params, client, server);
    } else if (command == "USER") {
        USER(params, client, server);
    } else if (command == "AUTHENTICATE") {
        AUTHENTICATE(client, server);
    } else if (command == "JOIN") {
        JOIN(params, client, server);
    } else if (command == "KICK") {
        KICK(params, client, server);
    } else if (command == "INVITE") {
        INVITE(params, client, server);
    } else if (command == "TOPIC") {
        TOPIC(params, client, server);
    } else if (command == "MODE") {
        MODE(params, client, server);
    } else if (command == "PRIVMSG") {
        PRIVMSG(params, client, server);
    }
    else {
        std::cout << "Error: Unknown command " << command << "." << std::endl;
    }
}

void Command::AUTHENTICATE(Client& client, Server& server) {
        (void)server;
        if (client.isAuth()) {
            std::cout << "Error: client " << client.getNickname() << " is already authenticated." << std::endl;
            return;
        }
        if (client.hasSentPass() && client.hasSentNick() && client.hasSentUser()) {
            client.setAuth(true);
            std::cout << client.getNickname() << ", You have been successfully authenticated!" << std::endl;
        } else {
            std::cout << "Error: client " << client.getNickname() << " is not authenticated. You Should provide Password for the Server, Nickname and Username." << std::endl;
        }
}

void Command::PASS(const std::vector<std::string>& params, Client& client, Server& server) {
    if (params.size() < 1)
    {
        std::cout << "Error: Not enough parameters for command PASS." << std::endl;
        return;
    }

    if (client.isReg()) {
        std::cout << "Error: client " << client.getNickname() << " is already registered." << std::endl;
        return;
    }

    if (params[0] == server.getPassword()) {
        client.setReg(true);
        client.HasSentPass(true);
        std::cout << client.getNickname() << ", Welcome to the server! If you want to Join Channels you must be authenticated." << std::endl;
    } else {
        std::cout << "Error: Password mismatch for client." << std::endl;
    }
}

void Command::NICK(const std::vector<std::string>& params, Client& client, Server& server) {
    if (params.size() < 1)
    {
        std::string error = "Error: Not enough parameters for command NICK\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        std::cout << "Error: Not enough parameters for command NICK." << std::endl;
        return;
    }

    std::string nickname = params[0];

    if (nickname.empty()) {
        std::string error = "Error: No nickname given\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        std::cout << "Error: No nickname given." << std::endl;
        return;
    }

    if (!isValidNickname(nickname)) {
        std::string error = "Error: Erroneous nickname " + nickname + ".\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        std::cout << "Error: Erroneous nickname " << nickname << "." << std::endl;
        return;
    }

    if (server.manageNickname(nickname, NULL, CHECK)) {
        std::string error = "Error: Nickname " + nickname + " is already in use.\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        std::cout << "Error: Nickname " << nickname << " is already in use." << std::endl;
        return;
    }

    std::string oldNickname = client.getNickname();
    if (!oldNickname.empty()) {
        server.manageNickname(oldNickname, NULL, UNREGISTER);
    }

    server.manageNickname(nickname, &client, REGISTER);
    client.setNickname(nickname);
    client.HasSentNick(true);
}

bool Command::isValidNickname(const std::string& nickname) {
    if (nickname.empty() || nickname.length() > 9)
        return false;

    if (nickname[0] == ':' || nickname[0] == '$' || nickname[0] == '#' || nickname[0] == '&' || nickname[0] == '+' || nickname[0] == '~' || nickname[0] == '@' || nickname[0] == '%' || isdigit(nickname[0])) {
        return false;
    }

    for (size_t i = 0; i < nickname.length(); ++i) {
        char c = nickname[i];
        if (c == '*' || c == ' ' || c == ',' || c == '.' || c == '@' || c == '?' || c == '!' || c == '\t' || c == '\r' || c == '\n')
            return false;
    }
    return true;
}

void Command::USER(const std::vector<std::string>& params, Client& client, Server& server) {
    (void)server;
    if (params.size() < 4)
    {
        std::cout << "Error: Not enough parameters for command USER." << std::endl;
        return;
    }

    client.setUsername(params[0]);
    client.setRealname(params[3]);
    client.HasSentUser(true);
}


void Command::JOIN(const std::vector<std::string>& params, Client& client, Server& server) {
    if (params.size() < 2)
    {
        std::string error1 = "Error: Not enough parameters for command JOIN\r\n";
        send(client.getFd(), error1.c_str(), error1.length(), 0);
        std::cout << "Error: Not enough parameters for command JOIN." << std::endl;
        return;
    }
    if (!client.isAuth()) {
        std::string error2 = "Error: Client not authenticated\r\n";
        send(client.getFd(), error2.c_str(), error2.length(), 0);
        std::cout << "Error: client " << client.getNickname() << " is not authenticated. You Should provide Password for the Server, Nickname and Username." << std::endl;
        return;
    }

    const std::string channelName = params[0];
    const std::string channelKey = params[1];
    if (channelName.empty() || channelName[0] != '#') {
        std::string error3 = "Error: Invalid channel name " + channelName + ".\r\n";
        send(client.getFd(), error3.c_str(), error3.length(), 0);
        std::cout << "Error: No such channel " << channelName << "." << std::endl;
        return;
    }

    Channel* channel = server.createOrGetChannel(channelName);


    if (channel->isInviteOnly() && !channel->isInvited(client.getNickname())) {
        std::string error4 = "Error: Channel " + channelName + " is invite-only. (+i)\r\n";
        send(client.getFd(), error4.c_str(), error4.length(), 0);
        std::cout << "Error: Channel " << channelName << " is invite-only. (+i)" << std::endl;
        return;
    }

    if (channel->hasKey()) {
        if (channelKey != channel->getKey()) {
            std::string error5 = "Error: Bad channel key for " + channelName + ". (+k)\r\n";
            send(client.getFd(), error5.c_str(), error5.length(), 0);
            std::cout << "Error: Bad channel key for channel (+k)" << channelName << "." << std::endl;
            return;
        }
    }

    if (channel->hasClientLimit()) {
        if (channel->isFull()) {
            std::string error6 = "Error: Channel " + channelName + " is full. (+l)\r\n";
            send(client.getFd(), error6.c_str(), error6.length(), 0);
            std::cout << "Error: Channel " << channelName << " is full. (+l)" << std::endl;
            return;
        }
    }

    if (!channel->hasClient(client.getNickname())){
        channel->addClient(&client);

        if (channel->getClients().size() == 1)
            channel->setOperator(client.getNickname());
    }

    channel->removeInvitation(client.getNickname());

    std::string joinMsg = ":" + client.getNickname() + " JOIN " + channelName + "\r\n";
    for (std::set<Client*>::iterator it = channel->getClients().begin(); it != channel->getClients().end(); ++it){
        send((*it)->getFd(), joinMsg.c_str(), joinMsg.length(), 0);
    }

    if (!channel->getTopic().empty()) {
        std::string topicMsg = client.getNickname() + " " + channelName + " :" + channel->getTopic() + "\r\n";
        send(client.getFd(), topicMsg.c_str(), topicMsg.length(), 0);
        std::cout << "Topic for channel " << channelName << " is: " << channel->getTopic() << std::endl;
    } else {
        std::string noTopicMsg = client.getNickname() + " " + channelName + " :No topic is set\r\n";
        send(client.getFd(), noTopicMsg.c_str(), noTopicMsg.length(), 0);
        std::cout << "No topic is set for channel " << channelName << "." << std::endl;
    }

    std::string namesList = client.getNickname() + "=" + channelName + " :";
    for (std::set<Client*>::iterator it = channel->getClients().begin(); it != channel->getClients().end(); ++it) {
        if (channel->isOperator(*it)) {
            namesList += "@" + (*it)->getNickname() + " ";
        } else {
            namesList += (*it)->getNickname() + " ";
        }
    }
    namesList += "\r\n";
    send(client.getFd(), namesList.c_str(), namesList.length(), 0);
}

void Command::KICK(const std::vector<std::string>& params, Client& client, Server& server) {
    if (params.size() < 3)
    {
        std::cout << "Error: Not enough parameters for command KICK." << std::endl;
        return;
    }
    std::string channelName = params[0];
    std::string targetNickname = params[1];
    std::string comment = params[2];

    Channel* channel = server.getChannel(channelName);
    if (!channel) {
        std::cout << "Error: No such channel " << channelName << "." << std::endl;
        return;

    }
    if (!channel->isOperator(client.getNickname())) {
        std::cout << "Error: You need channel operator privileges for channel " << channelName << "." << std::endl;
        return;
    }
    Client* targetClient = server.getClientByNickname(targetNickname);

    if (!targetClient || !channel->hasClient(targetNickname)) {
        std::cout << "Error: client " << targetNickname << " is not in channel " << channelName << "." << std::endl;
        return;
    }

    channel->removeClient(targetNickname);
    
    std::string kickMsg = ":" + client.getNickname() + " KICK " + channelName + " " + targetNickname + " :" + comment + "\r\n";
    for (std::set<Client*>::iterator it = channel->getClients().begin(); it != channel->getClients().end(); ++it) {
        send((*it)->getFd(), kickMsg.c_str(), kickMsg.length(), 0);
    }
}


void Command::INVITE(const std::vector<std::string>& params, Client& client, Server& server) {
    if (params.size() < 2)
    {
        std::cout << "Error: Not enough parameters for command INVITE." << std::endl;
        return;
    }
    std::string targetNickname = params[0];
    std::string channelName = params[1];

    Channel* channel = server.getChannel(channelName);
    if (!channel) {
        std::cout << "Error: No such channel " << channelName << "." << std::endl;
        return;
    }
    if (!channel->isOperator(client.getNickname())) {
        std::cout << "Error: You need channel operator privileges for channel " << channelName << "." << std::endl;
        return;
    }
    Client* targetClient = server.getClientByNickname(targetNickname);
    if (!targetClient) {
        std::cout << "Error: No such nick " << targetNickname << "." << std::endl;
        return;
    }

    if (channel->hasClient(targetNickname)) {
        std::cout << "Error: client " << targetNickname << " is already on channel " << channelName << "." << std::endl;
        return;
    }

    if (channel->isInvited(targetNickname)) {
        std::cout << "Error: client " << targetNickname << " is already on channel " << channelName << "." << std::endl;
        return;
    }

    channel->addInvitation(targetNickname);

    std::string inviteMsg = ":" + client.getNickname() + " INVITE " + targetNickname + " :" + channelName + "\r\n";
    send(targetClient->getFd(), inviteMsg.c_str(), inviteMsg.length(), 0);
}

void Command::TOPIC(const std::vector<std::string>& params, Client& client, Server& server) {
    if (params.size() < 2)
    {
        std::cout << "Error: Not enough parameters for command TOPIC." << std::endl;
        return;
    }

    std::string channelName = params[0];

    Channel* channel = server.getChannel(channelName);
    if (!channel) {
        std::cout << "Error: No such channel " << channelName << "." << std::endl;
        return;
    }

    if (!channel->hasClient(client.getNickname())) {
        std::cout << "Error: You are not in channel " << channelName << "." << std::endl;
        return;
    }

    // SET TOPIC
    if (params.size() >= 2) {
        if (channel->hasMode('t') && !channel->isOperator(&client)) {
            std::string error = ":ircserv 482 " + channelName + " :You're not a channel operator\r\n";
            send(client.getFd(), error.c_str(), error.length(), 0);
            return;
        }

        // Combine topic text
        std::string newTopic = params[1];
        for (size_t i = 2; i < params.size(); ++i) {
            newTopic += " " + params[i];
        }
        // Remove leading ':' if present
        if (!newTopic.empty() && newTopic[0] == ':')
            newTopic = newTopic.substr(1);

        channel->setTopic(newTopic);

        std::string topicMsg = ":" + client.getNickname() + " TOPIC " + channelName + " :" + newTopic + "\r\n";
        for (std::set<Client*>::iterator it = channel->getClients().begin(); it != channel->getClients().end(); ++it) {
            send((*it)->getFd(), topicMsg.c_str(), topicMsg.length(), 0);
        }
    }
    // VIEW TOPIC
    else {
        if (!channel->getTopic().empty()) {
            std::string topic = ":ircserv 332 " + client.getNickname() + " " + channelName + " :" + channel->getTopic() + "\r\n";
            send(client.getFd(), topic.c_str(), topic.length(), 0);
        } else {
            std::string notopic = ":ircserv 331 " + client.getNickname() + " " + channelName + " :No topic is set\r\n";
            send(client.getFd(), notopic.c_str(), notopic.length(), 0);
        }
    }
}

void Command::MODE(const std::vector<std::string>& params, Client& client, Server& server) {
    if (params.size() < 2)
    {
        std::cout << "Error: Not enough parameters for command MODE." << std::endl;
        return;
    }

    std::string channelName = params[0];
    std::string modeChanges = params[1];

    Channel* channel = server.getChannel(channelName);

    if (!channel) {
        std::cout << "Error: No such channel " << channelName << "." << std::endl;
        return;
    }

    if (!channel->isOperator(client.getNickname())) {
        std::cout << "Error: You need channel operator privileges for channel " << channelName << "." << std::endl;
        return;
    }
    
    char sign = modeChanges[0];
    if (sign != '+' && sign != '-') {
        std::string error = ":ircserv 472 " + modeChanges + " :is unknown mode char\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        return;
    }

    size_t argIndex = 2;

    std::string modesConfirmed = ":" + client.getNickname() + " MODE " + channelName + " " + modeChanges;

    for (size_t i = 1; i < modeChanges.length(); ++i) {
        char mode = modeChanges[i];

        switch (mode) {
            case 'i':
                if (sign == '+')
                    channel->addMode('i');
                else
                    channel->removeMode('i');
                break;

            case 't':
                if (sign == '+')
                    channel->addMode('t');
                else
                    channel->removeMode('t');
                break;

            case 'k':
                if (sign == '+') {
                    if (params.size() <= argIndex) {
                        std::string error = ":ircserv 461 MODE :Missing key parameter for +k\r\n";
                        send(client.getFd(), error.c_str(), error.length(), 0);
                        return;
                    }
                    channel->setKey(params[argIndex++]);
                } else {
                    channel->removeKey();
                }
                break;

            case 'l':
                if (sign == '+') {
                    if (params.size() <= argIndex) {
                        std::string error = ":ircserv 461 MODE :Missing parameter for +l\r\n";
                        send(client.getFd(), error.c_str(), error.length(), 0);
                        return;
                    }

                    size_t limit = 0;
                    for (size_t j = 0; j < params[argIndex].length(); ++j) {
                        if (params[argIndex][j] >= '0' && params[argIndex][j] <= '9') {
                            limit = limit * 10 + (params[argIndex][j] - '0');
                        } else {
                            std::string error = ":ircserv 461 MODE :Invalid limit value\r\n";
                            send(client.getFd(), error.c_str(), error.length(), 0);
                            return;
                        }
                    }
                    argIndex++;

                    channel->setClientLimit(limit);
                } else {
                    channel->removeClientLimit();
                }
                break;

            case 'o':
                if (params.size() <= argIndex) {
                    std::string error = ":ircserv 461 MODE :Missing nickname parameter for +o/-o\r\n";
                    send(client.getFd(), error.c_str(), error.length(), 0);
                    return;
                }

                {
                    std::string targetNick = params[argIndex++];
                    Client* target = server.getClientByNickname(targetNick);

                    if (!target || !channel->hasClient(targetNick)) {
                        std::string error = ":ircserv 441 " + targetNick + " " + channelName + " :They aren't on that channel\r\n";
                        send(client.getFd(), error.c_str(), error.length(), 0);
                        return;
                    }

                    if (sign == '+')
                        channel->setOperator(targetNick);
                    else
                        channel->removeOperator(target);

                    // Inform everyone about the operator change
                    std::string opChange = ":" + client.getNickname() + " MODE " + channelName + " " + sign + "o " + targetNick + "\r\n";
                    for (std::set<Client*>::iterator it = channel->getClients().begin(); it != channel->getClients().end(); ++it)
                        send((*it)->getFd(), opChange.c_str(), opChange.length(), 0);
                }
                break;

            default:
                std::string error = ":ircserv 472 ";
                error += mode;
                error += " :is unknown mode char\r\n";
                send(client.getFd(), error.c_str(), error.length(), 0);
                return;
        }
    }

    modesConfirmed += "\r\n";

    // Broadcast the overall mode change to all users in the channel
    for (std::set<Client*>::iterator it = channel->getClients().begin(); it != channel->getClients().end(); ++it) {
        send((*it)->getFd(), modesConfirmed.c_str(), modesConfirmed.length(), 0);
    }
}

void Command::PRIVMSG(const std::vector<std::string>& params, Client& client, Server& server) {
    if (params.size() < 2)
    {
        std::cout << "Error: Not enough parameters for command PRIVMSG." << std::endl;
        return;
    }

    std::string target = params[0];
    std::string message = params[1];
    for (size_t i = 2; i < params.size(); ++i) {
            message += " " + params[i];
    }

    if (target[0] == '#') {
        Channel* channel = server.getChannel(target);
        if (!channel) {
            std::cout << "Error: No such channel " << target << "." << std::endl;
            return;
        }
        if (!channel->hasClient(client.getNickname())) {
            std::cout << "Error: Cannot send to channel " << target << "." << std::endl;
            return;
        }

        std::string privMsg = ":" + client.getNickname() + " PRIVMSG " + target + " :" + message + "\r\n";
        for (std::set<Client*>::iterator it = channel->getClients().begin(); it != channel->getClients().end(); ++it) {
            if ((*it)->getNickname() != client.getNickname()) {
                send((*it)->getFd(), privMsg.c_str(), privMsg.length(), 0);
            }
        }
    } else {
        Client* targetClient = server.getClientByNickname(target);
        if (!targetClient) {
            std::cout << "Error: No such nick " << target << "." << std::endl;
            return;
        }

        std::string privMsg = ":" + client.getNickname() + " PRIVMSG " + target + " :" + message + "\r\n";
        send(targetClient->getFd(), privMsg.c_str(), privMsg.length(), 0);
    }
}