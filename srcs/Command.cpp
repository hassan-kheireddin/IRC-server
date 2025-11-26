#include "../includes/Command.hpp"

using namespace std;

void Command::executeCommand(const string& commandLine, Client& client, Server& server) {

    vector<string> tokens;
    size_t pos = 0, found;
    while((found = commandLine.find_first_of(' ', pos)) != string::npos) {
        tokens.push_back(commandLine.substr(pos, found - pos));
        pos = found + 1;
    }
    tokens.push_back(commandLine.substr(pos));

    if (tokens.empty()) return;

    string command = tokens[0];
    vector<string> params(tokens.begin() + 1, tokens.end());

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
        cout << "Error: Unknown command " << command << "." << endl;
    }
}

static void Command::AUTHENTICATE(Client& client, Server& server) {
        if (client.isAuth()) {
            cout << "Error: client " << client.getNickname() << " is already authenticated." << endl;
            return;
        }
        if (client.hasSentPass() && client.hasSentNick() && client.hasSentUser()) {
            client.setAuth(true);
            cout << client.getNickname() << ", You have been successfully authenticated!" << endl;
        } else {
            cout << "Error: client " << client.getNickname() << " is not authenticated. You Should provide Password for the Server, Nickname and Username." << endl;
        }
}

static void Command::PASS(const vector<string>& params, Client& client, Server& server) {
    if (params.size() < 1)
    {
        cout << "Error: Not enough parameters for command PASS." << endl;
        return;
    }

    if (client.isReg()) {
        cout << "Error: client " << client.getNickname() << " is already registered." << endl;
        return;
    }

    if (params[0] == server.getPassword()) {
        client.setReg(true);
        client.HasSentPass(true);
        cout << client.getNickname() << ", Welcome to the server! If you want to Join Channels you must be authenticated." << endl;
    } else {
        cout << "Error: Password mismatch for client." << endl;
    }
}

static void Command::NICK(const vector<string>& params, Client& client, Server& server) {
    if (params.size() < 1)
    {
        string error = "Error: Not enough parameters for command NICK\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        cout << "Error: Not enough parameters for command NICK." << endl;
        return;
    }

    string nickname = params[0];

    if (nickname.empty()) {
        string error = "Error: No nickname given\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        cout << "Error: No nickname given." << endl;
        return;
    }

    if (!isValidNickname(nickname)) {
        string error = "Error: Erroneous nickname " + nickname + ".\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        cout << "Error: Erroneous nickname " << nickname << "." << endl;
        return;
    }

    if (server.manageNickname(nickname, nullptr, CHECK)) {
        string error = "Error: Nickname " + nickname + " is already in use.\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        cout << "Error: Nickname " << nickname << " is already in use." << endl;
        return;
    }

    string oldNickname = client.getNickname();
    if (!oldNickname.empty()) {
        server.manageNickname(oldNickname, nullptr, UNREGISTER);
    }

    server.manageNickname(nickname, &client, REGISTER);
    client.setNickname(nickname);
    client.HasSentNick(true);
}

static bool Command::isValidNickname(const string& nickname) {
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

static void Command::USER(const vector<string>& params, Client& client, Server& server) {
    if (params.size() < 4)
    {
        cout << "Error: Not enough parameters for command USER." << endl;
        return;
    }

    client.setUsername(params[0]);
    client.setRealname(params[3]);
    client.HasSentUser(true);
}


static void Command::JOIN(const vector<string>& params, Client& client, Server& server) {
    if (params.size() < 2)
    {
        string error1 = "Error: Not enough parameters for command JOIN\r\n";
        send(client.getFd(), error1.c_str(), error1.length(), 0);
        cout << "Error: Not enough parameters for command JOIN." << endl;
        return;
    }
    if (!client.isAuth()) {
        string error2 = "Error: Client not authenticated\r\n";
        send(client.getFd(), error2.c_str(), error2.length(), 0);
        cout << "Error: client " << client.getNickname() << " is not authenticated. You Should provide Password for the Server, Nickname and Username." << endl;
        return;
    }

    const string channelName = params[0];
    const string channelKey = params[1];
    if (channelName.empty() || channelName[0] != '#') {
        string error3 = "Error: Invalid channel name " + channelName + ".\r\n";
        send(client.getFd(), error3.c_str(), error3.length(), 0);
        cout << "Error: No such channel " << channelName << "." << endl;
        return;
    }

    Channel* channel = server.createOrGetChannel(channelName);


    if (channel->isInviteOnly() && !channel->isInvited(client.getNickname())) {
        string error4 = "Error: Channel " + channelName + " is invite-only. (+i)\r\n";
        send(client.getFd(), error4.c_str(), error4.length(), 0);
        cout << "Error: Channel " << channelName << " is invite-only. (+i)" << endl;
        return;
    }

    if (channel->hasKey()) {
        if (channelKey != channel->getKey()) {
            string error5 = "Error: Bad channel key for " + channelName + ". (+k)\r\n";
            send(client.getFd(), error5.c_str(), error5.length(), 0);
            cout << "Error: Bad channel key for channel (+k)" << channelName << "." << endl;
            return;
        }
    }

    if (channel->hasClientLimit()) {
        if (channel->isFull()) {
            string error6 = "Error: Channel " + channelName + " is full. (+l)\r\n";
            send(client.getFd(), error6.c_str(), error6.length(), 0);
            cout << "Error: Channel " << channelName << " is full. (+l)" << endl;
            return;
        }
    }

    if (!channel->hasClient(client.getNickname())){
        channel->addClient(&client);

        if (channel->getClients().size() == 1)
            channel->setOperator(client.getNickname());
    }

    channel->removeInvitation(client.getNickname());

    string joinMsg = ":" + client.getNickname() + " JOIN " + channelName + "\r\n";
    for (set<Client*>::iterator it = channel->getClients().begin(); it != channel->getClients().end(); ++it){
        send((*it)->getFd(), joinMsg.c_str(), joinMsg.length(), 0);
    }

    if (!channel->getTopic().empty()) {
        string topicMsg = client.getNickname() + " " + channelName + " :" + channel->getTopic() + "\r\n";
        send(client.getFd(), topicMsg.c_str(), topicMsg.length(), 0);
        cout << "Topic for channel " << channelName << " is: " << channel->getTopic() << endl;
    } else {
        string noTopicMsg = client.getNickname() + " " + channelName + " :No topic is set\r\n";
        send(client.getFd(), noTopicMsg.c_str(), noTopicMsg.length(), 0);
        cout << "No topic is set for channel " << channelName << "." << endl;
    }

    string namesList = client.getNickname() + "=" + channelName + " :";
    for (set<Client*>::iterator it = channel->getClients().begin(); it != channel->getClients().end(); ++it) {
        if ((*it)->getNickname() == channel->getOperator()) {
            namesList += "@" + (*it)->getNickname() + " ";
        } else {
            namesList += (*it)->getNickname() + " ";
        }
    }
    namesList += "\r\n";
    send(client.getFd(), namesList.c_str(), namesList.length(), 0);
}

static void Command::KICK(const vector<string>& params, Client& client, Server& server) {
    if (params.size() < 3)
    {
        cout << "Error: Not enough parameters for command KICK." << endl;
        return;
    }
    string channelName = params[0];
    string targetNickname = params[1];
    string comment = params[2];

    Channel* channel = server.getChannel(channelName);
    if (!channel) {
        cout << "Error: No such channel " << channelName << "." << endl;
        return;

    }
    if (!channel->isOperator(client.getNickname())) {
        cout << "Error: You need channel operator privileges for channel " << channelName << "." << endl;
        return;
    }
    Client* targetClient = server.getClientByNickname(targetNickname);

    if (!targetClient || !channel->hasClient(targetNickname)) {
        cout << "Error: client " << targetNickname << " is not in channel " << channelName << "." << endl;
        return;
    }

    channel->removeClient(targetNickname);
    
    string kickMsg = ":" + client.getNickname() + " KICK " + channelName + " " + targetNickname + " :" + comment + "\r\n";
    for (set<Client*>::iterator it = channel->getClients().begin(); it != channel->getClients().end(); ++it) {
        send((*it)->getFd(), kickMsg.c_str(), kickMsg.length(), 0);
    }
}


static void Command::INVITE(const vector<string>& params, Client& client, Server& server) {
    if (params.size() < 2)
    {
        cout << "Error: Not enough parameters for command INVITE." << endl;
        return;
    }
    string targetNickname = params[0];
    string channelName = params[1];

    Channel* channel = server.getChannel(channelName);
    if (!channel) {
        cout << "Error: No such channel " << channelName << "." << endl;
        return;
    }
    if (!channel->isOperator(client.getNickname())) {
        cout << "Error: You need channel operator privileges for channel " << channelName << "." << endl;
        return;
    }
    Client* targetClient = server.getClientByNickname(targetNickname);
    if (!targetClient) {
        cout << "Error: No such nick " << targetNickname << "." << endl;
        return;
    }

    if (channel->hasClient(targetNickname)) {
        cout << "Error: client " << targetNickname << " is already on channel " << channelName << "." << endl;
        return;
    }

    if (channel->hasInvitation(targetNickname)) {
        cout << "Error: client " << targetNickname << " is already on channel " << channelName << "." << endl;
        return;
    }

    channel->addInvitation(targetNickname);

    string inviteMsg = ":" + client.getNickname() + " INVITE " + targetNickname + " :" + channelName + "\r\n";
    send(targetClient->getFd(), inviteMsg.c_str(), inviteMsg.length(), 0);
}

static void Command::TOPIC(const vector<string>& params, Client& client, Server& server) {
    if (params.size() < 2)
    {
        cout << "Error: Not enough parameters for command TOPIC." << endl;
        return;
    }

    string channelName = params[0];

    Channel* channel = server.getChannel(channelName);
    if (!channel) {
        cout << "Error: No such channel " << channelName << "." << endl;
        return;
    }

    if (!channel->hasClient(client.getNickname())) {
        cout << "Error: You are not in channel " << channelName << "." << endl;
        return;
    }

    if (channel->hasMode('t') && !channel->isOperator(client)) {
            std::string error = ":ircserv 482 " + channelName + " :You're not a channel operator\r\n";
            send(client.getFd(), error.c_str(), error.length(), 0);
            return;
        }

        // Combine topic text
        std::string newTopic;
        for (size_t i = 2; i < args.size(); ++i) {
            if (i == 2 && args[i][0] == ':')
                newTopic = args[i].substr(1);
            else
                newTopic += " " + args[i];
        }

        channel->setTopic(newTopic);

        std::string topicMsg = ":" + client.getNickname() + " TOPIC " + channelName + " :" + newTopic + "\r\n";
        for (std::set<Client*>::iterator it = channel->getClients().begin(); it != channel->getClients().end(); ++it) {
            send((*it)->getFd(), topicMsg.c_str(), topicMsg.length(), 0);
    }

    //  VIEW TOPIC
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

static void Command::MODE(const vector<string>& params, Client& client, Server& server) {
    if (params.size() < 2)
    {
        cout << "Error: Not enough parameters for command MODE." << endl;
        return;
    }

    string channelName = params[0];
    string modeChanges = params[1];

    Channel* channel = server.getChannel(channelName);

    if (!channel) {
        cout << "Error: No such channel " << channelName << "." << endl;
        return;
    }

    if (!channel->isOperator(client.getNickname())) {
        cout << "Error: You need channel operator privileges for channel " << channelName << "." << endl;
        return;
    }
    
 char sign = modeString[0];
    if (sign != '+' && sign != '-') {
        std::string error = ":ircserv 472 " + modeString + " :is unknown mode char\r\n";
        send(client->getFd(), error.c_str(), error.length(), 0);
        return;
    }

    size_t argIndex = 2;

    std::string modesConfirmed = ":" + client->getNickname() + " MODE " + channelName + " " + modeString;

    for (size_t i = 1; i < modeString.length(); ++i) {
        char mode = modeString[i];

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
                    if (args.size() <= argIndex) {
                        std::string error = ":ircserv 461 MODE :Missing key parameter for +k\r\n";
                        send(client->getFd(), error.c_str(), error.length(), 0);
                        return;
                    }
                    channel->setKey(args[argIndex++]);
                } else {
                    channel->removeKey();
                }
                break;

            case 'l':
                if (sign == '+') {
                    if (args.size() <= argIndex) {
                        std::string error = ":ircserv 461 MODE :Missing parameter for +l\r\n";
                        send(client->getFd(), error.c_str(), error.length(), 0);
                        return;
                    }

                    std::istringstream iss(args[argIndex++]);
                    size_t limit;
                    if (!(iss >> limit)) {
                        std::string error = ":ircserv 461 MODE :Invalid limit value\r\n";
                        send(client->getFd(), error.c_str(), error.length(), 0);
                        return;
                    }

                    channel->setUserLimit(limit);
                } else {
                    channel->removeUserLimit();
                }
                break;

            case 'o':
                if (args.size() <= argIndex) {
                    std::string error = ":ircserv 461 MODE :Missing nickname parameter for +o/-o\r\n";
                    send(client->getFd(), error.c_str(), error.length(), 0);
                    return;
                }

                {
                    std::string targetNick = args[argIndex++];
                    Client* target = server.getClientByNickname(targetNick);

                    if (!target || !channel->hasClient(target)) {
                        std::string error = ":ircserv 441 " + targetNick + " " + channelName + " :They aren't on that channel\r\n";
                        send(client->getFd(), error.c_str(), error.length(), 0);
                        return;
                    }

                    if (sign == '+')
                        channel->addOperator(target);
                    else
                        channel->removeOperator(target);

                    // Inform everyone about the operator change
                    std::string opChange = ":" + client->getNickname() + " MODE " + channelName + " " + sign + "o " + targetNick + "\r\n";
                    for (std::set<Client*>::iterator it = channel->getClients().begin(); it != channel->getClients().end(); ++it)
                        send((*it)->getFd(), opChange.c_str(), opChange.length(), 0);
                }
                break;

            default:
                std::string error = ":ircserv 472 ";
                error += mode;
                error += " :is unknown mode char\r\n";
                send(client->getFd(), error.c_str(), error.length(), 0);
                return;
        }
    }

    modesConfirmed += "\r\n";

    // Broadcast the overall mode change to all users in the channel
    for (std::set<Client*>::iterator it = channel->getClients().begin(); it != channel->getClients().end(); ++it) {
        send((*it)->getFd(), modesConfirmed.c_str(), modesConfirmed.length(), 0);
    }
}

static void Command::PRIVMSG(const vector<string>& params, Client& client, Server& server) {
    if (params.size() < 2)
    {
        cout << "Error: Not enough parameters for command PRIVMSG." << endl;
        return;
    }

    string target = params[0];
    string message = params[1];
    for (size_t i = 2; i < params.size(); ++i) {
            message += " " + params[i];
    }

    if (target[0] == '#') {
        Channel* channel = server.getChannel(target);
        if (!channel) {
            cout << "Error: No such channel " << target << "." << endl;
            return;
        }
        if (!channel->hasClient(client.getNickname())) {
            cout << "Error: Cannot send to channel " << target << "." << endl;
            return;
        }

        string privMsg = ":" + client.getNickname() + " PRIVMSG " + target + " :" + message + "\r\n";
        for (set<Client*>::iterator it = channel->getClients().begin(); it != channel->getClients().end(); ++it) {
            if ((*it)->getNickname() != client.getNickname()) {
                send((*it)->getFd(), privMsg.c_str(), privMsg.length(), 0);
            }
        }
    } else {
        Client* targetClient = server.getClientByNickname(target);
        if (!targetClient) {
            cout << "Error: No such nick " << target << "." << endl;
            return;
        }

        string privMsg = ":" + client.getNickname() + " PRIVMSG " + target + " :" + message + "\r\n";
        send(targetClient->getFd(), privMsg.c_str(), privMsg.length(), 0);
    }
}