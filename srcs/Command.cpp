#include "../includes/Command.hpp"
#include <iostream>

void Command::executeCommand(const std::string& commandLine, Client& client, Server& server) {

    std::vector<std::string> tokens;
    size_t pos = 0;
    
    // Parse tokens, handling colon-prefixed trailing parameters (IRC protocol)
    while (pos < commandLine.length()) {
        // Skip leading spaces
        while (pos < commandLine.length() && commandLine[pos] == ' ')
            pos++;
        
        if (pos >= commandLine.length())
            break;
            
        // Check for trailing parameter (starts with :)
        // Everything after : is treated as a single parameter
        if (commandLine[pos] == ':') {
            // Skip the colon and add everything remaining as one token
            std::string trailing = commandLine.substr(pos + 1);
            if (!trailing.empty())
                tokens.push_back(trailing);
            break;
        }
        
        // Find next space
        size_t found = commandLine.find(' ', pos);
        if (found == std::string::npos) {
            // No more spaces, add remaining string
            tokens.push_back(commandLine.substr(pos));
            break;
        }
        
        // Add token from pos to found
        tokens.push_back(commandLine.substr(pos, found - pos));
        pos = found;
    }

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
    } else if (command == "NOTICE") {
        NOTICE(params, client, server);
    }
    else {
        std::string error = "Error: Unknown command " + command + ".\r\n";
        send(client.getSocketFd(), error.c_str(), error.length(), 0);
    }
}

void Command::AUTHENTICATE(Client& client, Server& server) {
        (void)server;
        if (client.isAuth()) {
            std::string error = "Error: You are already authenticated.\r\n";
            send(client.getSocketFd(), error.c_str(), error.length(), 0);
            return;
        }
        if (client.hasSentPass() && client.hasSentNick() && client.hasSentUser()) {
            client.setAuth(true);
            std::string msg = client.getNickname() + ", You have been successfully authenticated!\r\n";
            send(client.getSocketFd(), msg.c_str(), msg.length(), 0);
        } else {
            std::string error = "Error: Authentication incomplete. You must send PASS, NICK, and USER commands first.\r\n";
            send(client.getSocketFd(), error.c_str(), error.length(), 0);
        }
}

void Command::PASS(const std::vector<std::string>& params, Client& client, Server& server) {
    if (params.size() != 1)
    {
        std::string error = "Error: Invalid parameters for command PASS\r\n";
        send(client.getSocketFd(), error.c_str(), error.length(), 0);
        return;
    }

    if (client.hasSentNick() || client.hasSentUser()) {
        std::string error = "Error: PASS must be sent before NICK and USER commands\r\n";
        send(client.getSocketFd(), error.c_str(), error.length(), 0);
        return;
    }

    if (client.isReg()) {
        std::string error = "Error: You are already registered\r\n";
        send(client.getSocketFd(), error.c_str(), error.length(), 0);
        return;
    }

    if (params[0] == server.getPassword()) {
        client.setReg(true);
        client.HasSentPass(true);
        std::string msg = "Welcome to the server! You must authenticate to join channels.\r\n";
        send(client.getSocketFd(), msg.c_str(), msg.length(), 0);
    } else {
        std::string error = "Error: Password incorrect\r\n";
        send(client.getSocketFd(), error.c_str(), error.length(), 0);
    }
}

void Command::NICK(const std::vector<std::string>& params, Client& client, Server& server) {
    if (params.size() != 1)
    {
        std::string error = "Error: Invalid parameters for command NICK\r\n";
        send(client.getSocketFd(), error.c_str(), error.length(), 0);
        return;
    }

    if (!client.hasSentPass()) {
        std::string error = "Error: You must send PASS command first\r\n";
        send(client.getSocketFd(), error.c_str(), error.length(), 0);
        return;
    }

    std::string nickname = params[0];

    if (nickname.empty()) {
        std::string error = "Error: No nickname given\r\n";
        send(client.getSocketFd(), error.c_str(), error.length(), 0);
        return;
    }

    if (!isValidNickname(nickname)) {
        std::string error = "Error: Erroneous nickname " + nickname + ".\r\n";
        send(client.getSocketFd(), error.c_str(), error.length(), 0);
        return;
    }

    if (server.manageNickname(nickname, NULL, CHECK)) {
        std::string error = "Error: Nickname " + nickname + " is already in use.\r\n";
        send(client.getSocketFd(), error.c_str(), error.length(), 0);
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
    if (params.size() != 4)
    {
        std::string error = "Error: Invalid parameters for command USER\r\n";
        send(client.getSocketFd(), error.c_str(), error.length(), 0);
        return;
    }

    if (!client.hasSentPass()) {
        std::string error = "Error: You must send PASS command first\r\n";
        send(client.getSocketFd(), error.c_str(), error.length(), 0);
        return;
    }

    if (!client.hasSentNick()) {
        std::string error = "Error: You must send NICK command before USER\r\n";
        send(client.getSocketFd(), error.c_str(), error.length(), 0);
        return;
    }

    client.setUsername(params[0]);
    client.setRealname(params[3]);
    client.HasSentUser(true);
}


void Command::JOIN(const std::vector<std::string>& params, Client& client, Server& server) {
    if (params.size() < 1 || params.size() > 2)
    {
        std::string error1 = "Error: Invalid parameters for command JOIN\r\n";
        send(client.getSocketFd(), error1.c_str(), error1.length(), 0);
        return;
    }
    if (!client.isAuth()) {
        std::string error2 = "Error: Client not authenticated\r\n";
        send(client.getSocketFd(), error2.c_str(), error2.length(), 0);
        return;
    }

    const std::string channelName = params[0];
    const std::string channelKey = (params.size() >= 2) ? params[1] : "";
    if (channelName.empty() || channelName[0] != '#') {
        std::string error3 = "Error: Invalid channel name " + channelName + ".\r\n";
        send(client.getSocketFd(), error3.c_str(), error3.length(), 0);
        return;
    }

    Channel* channel = server.createOrGetChannel(channelName);


    if (channel->isInviteOnly() && !channel->isInvited(&client)) {
        std::string error4 = "Error: Channel " + channelName + " is invite-only. (+i)\r\n";
        send(client.getSocketFd(), error4.c_str(), error4.length(), 0);
        return;
    }

    if (channel->hasMode('k')) {
        if (channelKey.empty() || channelKey != channel->getKey()) {
            std::string error5 = "Error: Bad channel key for " + channelName + ". (+k)\r\n";
            send(client.getSocketFd(), error5.c_str(), error5.length(), 0);
            return;
        }
    }

    if (channel->hasClientLimit()) {
        if (channel->isFull()) {
            std::string error6 = "Error: Channel " + channelName + " is full. (+l)\r\n";
            send(client.getSocketFd(), error6.c_str(), error6.length(), 0);
            return;
        }
    }

    if (!channel->hasClient(client.getNickname())){
        channel->addClient(&client);

        if (channel->getClients().size() == 1) {
            channel->setOperator(client.getNickname());
            // If creating channel with a password, set it and enable +k mode
            if (!channelKey.empty()) {
                channel->setKey(channelKey);
                channel->addMode('k');
            }
        }
        
        // Remove invitation only after successful join
        channel->removeInvitation(&client);
    }

    std::string joinMsg = ":" + client.getNickname() + " JOIN " + channelName + "\r\n";
    for (std::set<Client*>::iterator it = channel->getClients().begin(); it != channel->getClients().end(); ++it){
        send((*it)->getSocketFd(), joinMsg.c_str(), joinMsg.length(), 0);
    }

    // Send topic to joining user (RPL_TOPIC = 332, RPL_NOTOPIC = 331)
    if (!channel->getTopic().empty()) {
        std::string topicMsg = ":ircserv 332 " + client.getNickname() + " " + channelName + " :" + channel->getTopic() + "\r\n";
        send(client.getSocketFd(), topicMsg.c_str(), topicMsg.length(), 0);
    } else {
        std::string noTopicMsg = ":ircserv 331 " + client.getNickname() + " " + channelName + " :No topic is set\r\n";
        send(client.getSocketFd(), noTopicMsg.c_str(), noTopicMsg.length(), 0);
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
    send(client.getSocketFd(), namesList.c_str(), namesList.length(), 0);
}

void Command::KICK(const std::vector<std::string>& params, Client& client, Server& server) {
    if (params.size() < 2)
    {
        std::string error = "Error: Not enough parameters for command KICK\r\n";
        send(client.getSocketFd(), error.c_str(), error.length(), 0);
        return;
    }
    std::string channelName = params[0];
    std::string targetNickname = params[1];
    
    // Combine all remaining parameters as the kick message
    std::string comment;
    if (params.size() >= 3) {
        comment = params[2];
        for (size_t i = 3; i < params.size(); ++i) {
            comment += " " + params[i];
        }
        // Remove leading ':' if present
        if (!comment.empty() && comment[0] == ':')
            comment = comment.substr(1);
    } else {
        comment = targetNickname; // Default kick message
    }

    Channel* channel = server.getChannel(channelName);
    if (!channel) {
        std::string error = "Error: No such channel " + channelName + "\r\n";
        send(client.getSocketFd(), error.c_str(), error.length(), 0);
        return;

    }
    if (!channel->isOperator(client.getNickname())) {
        std::string error = "Error: You need channel operator privileges\r\n";
        send(client.getSocketFd(), error.c_str(), error.length(), 0);
        return;
    }
    
    // Operator cannot kick themselves
    if (targetNickname == client.getNickname()) {
        std::string error = "Error: You cannot kick yourself\r\n";
        send(client.getSocketFd(), error.c_str(), error.length(), 0);
        return;
    }
    
    Client* targetClient = server.getClientByNickname(targetNickname);

    if (!targetClient || !channel->hasClient(targetNickname)) {
        std::string error = "Error: " + targetNickname + " is not in channel " + channelName + "\r\n";
        send(client.getSocketFd(), error.c_str(), error.length(), 0);
        return;
    }

    std::string kickMsg = ":" + client.getNickname() + " KICK " + channelName + " " + targetNickname + " :" + comment + "\r\n";
    
    // Send kick message to all clients in channel (including the one being kicked)
    for (std::set<Client*>::iterator it = channel->getClients().begin(); it != channel->getClients().end(); ++it) {
        send((*it)->getSocketFd(), kickMsg.c_str(), kickMsg.length(), 0);
    }
    
    // Now remove the client from the channel
    channel->removeClient(targetNickname);
}


void Command::INVITE(const std::vector<std::string>& params, Client& client, Server& server) {
    if (params.size() != 2)
    {
        std::string error = "Error: Invalid parameters for command INVITE\r\n";
        send(client.getSocketFd(), error.c_str(), error.length(), 0);
        return;
    }
    std::string targetNickname = params[0];
    std::string channelName = params[1];

    Channel* channel = server.getChannel(channelName);
    if (!channel) {
        std::string error = "Error: No such channel " + channelName + "\r\n";
        send(client.getSocketFd(), error.c_str(), error.length(), 0);
        return;
    }
    if (!channel->isOperator(client.getNickname())) {
        std::string error = "Error: You need channel operator privileges\r\n";
        send(client.getSocketFd(), error.c_str(), error.length(), 0);
        return;
    }
    Client* targetClient = server.getClientByNickname(targetNickname);
    if (!targetClient) {
        std::string error = "Error: No such nick " + targetNickname + "\r\n";
        send(client.getSocketFd(), error.c_str(), error.length(), 0);
        return;
    }

    if (channel->hasClient(targetNickname)) {
        std::string error = "Error: " + targetNickname + " is already on channel " + channelName + "\r\n";
        send(client.getSocketFd(), error.c_str(), error.length(), 0);
        return;
    }

    if (channel->isInvited(targetClient)) {
        std::string error = "Error: " + targetNickname + " is already invited to " + channelName + "\r\n";
        send(client.getSocketFd(), error.c_str(), error.length(), 0);
        return;
    }

    channel->addInvitation(targetClient);

    std::string inviteMsg = ":" + client.getNickname() + " INVITE " + targetNickname + " :" + channelName + "\r\n";
    send(targetClient->getSocketFd(), inviteMsg.c_str(), inviteMsg.length(), 0);
}

void Command::TOPIC(const std::vector<std::string>& params, Client& client, Server& server) {
    if (params.size() < 1)
    {
        std::string error = "Error: Not enough parameters for command TOPIC\r\n";
        send(client.getSocketFd(), error.c_str(), error.length(), 0);
        return;
    }

    std::string channelName = params[0];

    Channel* channel = server.getChannel(channelName);
    if (!channel) {
        std::string error = "Error: No such channel " + channelName + "\r\n";
        send(client.getSocketFd(), error.c_str(), error.length(), 0);
        return;
    }

    if (!channel->hasClient(client.getNickname())) {
        std::string error = "Error: You are not in channel " + channelName + "\r\n";
        send(client.getSocketFd(), error.c_str(), error.length(), 0);
        return;
    }

    // SET TOPIC
    if (params.size() >= 2) {
        if (channel->hasMode('t') && !channel->isOperator(&client)) {
            std::string error = ":ircserv 482 " + channelName + " :You're not a channel operator\r\n";
            send(client.getSocketFd(), error.c_str(), error.length(), 0);
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
            send((*it)->getSocketFd(), topicMsg.c_str(), topicMsg.length(), 0);
        }
    }
    // VIEW TOPIC
    else {
        if (!channel->getTopic().empty()) {
            std::string topic = ":ircserv 332 " + client.getNickname() + " " + channelName + " :" + channel->getTopic() + "\r\n";
            send(client.getSocketFd(), topic.c_str(), topic.length(), 0);
        } else {
            std::string notopic = ":ircserv 331 " + client.getNickname() + " " + channelName + " :No topic is set\r\n";
            send(client.getSocketFd(), notopic.c_str(), notopic.length(), 0);
        }
    }
}

void Command::MODE(const std::vector<std::string>& params, Client& client, Server& server) {
    if (params.size() < 1)
    {
        std::string error = "Error: Not enough parameters for command MODE\r\n";
        send(client.getSocketFd(), error.c_str(), error.length(), 0);
        return;
    }

    std::string channelName = params[0];

    Channel* channel = server.getChannel(channelName);

    if (!channel) {
        std::string error = "Error: No such channel " + channelName + "\r\n";
        send(client.getSocketFd(), error.c_str(), error.length(), 0);
        return;
    }

    // If only channel name provided, display current modes (RPL_CHANNELMODEIS = 324)
    if (params.size() == 1) {
        std::string modes = channel->getModes();
        std::string modeStr = "+";
        if (!modes.empty()) {
            modeStr += modes;
        }
        std::string response = ":ircserv 324 " + client.getNickname() + " " + channelName + " " + modeStr + "\r\n";
        send(client.getSocketFd(), response.c_str(), response.length(), 0);
        return;
    }

    std::string modeChanges = params[1];

    if (!channel->isOperator(client.getNickname())) {
        std::string error = "Error: You need channel operator privileges\r\n";
        send(client.getSocketFd(), error.c_str(), error.length(), 0);
        return;
    }
    
    char sign = modeChanges[0];
    if (sign != '+' && sign != '-') {
        std::string error = ":ircserv 472 " + modeChanges + " :is unknown mode char\r\n";
        send(client.getSocketFd(), error.c_str(), error.length(), 0);
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
                if (sign == '+') {
                    channel->addMode('t');
                    // If topic parameter is provided with +t, set the topic
                    if (params.size() > argIndex) {
                        std::string topic = params[argIndex];
                        // Combine remaining parameters as topic (in case of multi-word topic)
                        for (size_t j = argIndex + 1; j < params.size(); ++j) {
                            topic += " " + params[j];
                        }
                        // Remove leading ':' if present
                        if (!topic.empty() && topic[0] == ':')
                            topic = topic.substr(1);
                        channel->setTopic(topic);
                        argIndex = params.size(); // Consume all remaining params
                    }
                } else {
                    channel->removeMode('t');
                    // MODE -t deletes the topic
                    channel->setTopic("");
                }
                break;

            case 'k':
                if (sign == '+') {
                    if (params.size() <= argIndex) {
                        std::string error = ":ircserv 461 MODE :Missing key parameter for +k\r\n";
                        send(client.getSocketFd(), error.c_str(), error.length(), 0);
                        return;
                    }
                    channel->setKey(params[argIndex++]);
                    channel->addMode('k');
                } else {
                    channel->removeKey();
                    channel->removeMode('k');
                }
                break;

            case 'l':
                if (sign == '+') {
                    if (params.size() <= argIndex) {
                        std::string error = ":ircserv 461 MODE :Missing parameter for +l\r\n";
                        send(client.getSocketFd(), error.c_str(), error.length(), 0);
                        return;
                    }

                    size_t limit = 0;
                    for (size_t j = 0; j < params[argIndex].length(); ++j) {
                        if (params[argIndex][j] >= '0' && params[argIndex][j] <= '9') {
                            limit = limit * 10 + (params[argIndex][j] - '0');
                        } else {
                            std::string error = ":ircserv 461 MODE :Invalid limit value\r\n";
                            send(client.getSocketFd(), error.c_str(), error.length(), 0);
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
                    send(client.getSocketFd(), error.c_str(), error.length(), 0);
                    return;
                }

                {
                    std::string targetNick = params[argIndex++];
                    Client* target = server.getClientByNickname(targetNick);

                    if (!target || !channel->hasClient(targetNick)) {
                        std::string error = ":ircserv 441 " + targetNick + " " + channelName + " :They aren't on that channel\r\n";
                        send(client.getSocketFd(), error.c_str(), error.length(), 0);
                        return;
                    }

                   // Operat or cannot remove their own operator status with -o
                    if (sign == '-' && targetNick == client.getNickname()) {
                        std::string error = "Error: You cannot remove your own operator status\r\n";
                        send(client.getSocketFd(), error.c_str(), error.length(), 0);
                        return;
                    }

                    if (sign == '+')
                        channel->setOperator(targetNick);
                    else
                        channel->removeOperator(target);

                    // Inform everyone about the operator change
                    std::string opChange = ":" + client.getNickname() + " MODE " + channelName + " " + sign + "o " + targetNick + "\r\n";
                    for (std::set<Client*>::iterator it = channel->getClients().begin(); it != channel->getClients().end(); ++it)
                        send((*it)->getSocketFd(), opChange.c_str(), opChange.length(), 0);
                }
                break;

            default:
                std::string error = ":ircserv 472 ";
                error += mode;
                error += " :is unknown mode char\r\n";
                send(client.getSocketFd(), error.c_str(), error.length(), 0);
                return;
        }
    }

    modesConfirmed += "\r\n";

    // Broadcast the overall mode change to all users in the channel
    for (std::set<Client*>::iterator it = channel->getClients().begin(); it != channel->getClients().end(); ++it) {
        send((*it)->getSocketFd(), modesConfirmed.c_str(), modesConfirmed.length(), 0);
    }
}

void Command::PRIVMSG(const std::vector<std::string>& params, Client& client, Server& server) {
    if (params.size() < 2)
    {
        std::string error = "Error: Not enough parameters for command PRIVMSG\r\n";
        send(client.getSocketFd(), error.c_str(), error.length(), 0);
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
            std::string error = "Error: No such channel " + target + "\r\n";
            send(client.getSocketFd(), error.c_str(), error.length(), 0);
            return;
        }
        if (!channel->hasClient(client.getNickname())) {
            std::string error = "Error: Cannot send to channel " + target + "\r\n";
            send(client.getSocketFd(), error.c_str(), error.length(), 0);
            return;
        }

        std::string privMsg = ":" + client.getNickname() + " PRIVMSG " + target + " :" + message + "\r\n";
        for (std::set<Client*>::iterator it = channel->getClients().begin(); it != channel->getClients().end(); ++it) {
            if ((*it)->getNickname() != client.getNickname()) {
                send((*it)->getSocketFd(), privMsg.c_str(), privMsg.length(), 0);
            }
        }
    } else {
        Client* targetClient = server.getClientByNickname(target);
        if (!targetClient) {
            std::string error = "Error: No such nick " + target + "\r\n";
            send(client.getSocketFd(), error.c_str(), error.length(), 0);
            return;
        }

        std::string privMsg = ":" + client.getNickname() + " PRIVMSG " + target + " :" + message + "\r\n";
        send(targetClient->getSocketFd(), privMsg.c_str(), privMsg.length(), 0);
    }
}

void Command::NOTICE(const std::vector<std::string>& params, Client& client, Server& server) {
    // NOTICE never sends error replies to avoid loops
    if (params.size() < 2)
        return;

    std::string target = params[0];
    std::string message = params[1];
    for (size_t i = 2; i < params.size(); ++i) {
        message += " " + params[i];
    }

    if (target[0] == '#') {
        Channel* channel = server.getChannel(target);
        if (!channel)
            return;
        if (!channel->hasClient(client.getNickname()))
            return;

        std::string noticeMsg = ":" + client.getNickname() + " NOTICE " + target + " :" + message + "\r\n";
        for (std::set<Client*>::iterator it = channel->getClients().begin(); it != channel->getClients().end(); ++it) {
            if ((*it)->getNickname() != client.getNickname()) {
                send((*it)->getSocketFd(), noticeMsg.c_str(), noticeMsg.length(), 0);
            }
        }
    } else {
        Client* targetClient = server.getClientByNickname(target);
        if (!targetClient)
            return;

        std::string noticeMsg = ":" + client.getNickname() + " NOTICE " + target + " :" + message + "\r\n";
        send(targetClient->getSocketFd(), noticeMsg.c_str(), noticeMsg.length(), 0);
    }
}