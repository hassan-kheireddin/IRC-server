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
        Messages::ERR_UNKNOWNCOMMAND(command);
    }
}

static void Command::AUTHENTICATE(Client& client, Server& server) {
        if (client.isAuth()) {
            Messages::ERR_ALREADYAUTHENTICATED(client.getNickname());
            return;
        }
        if (client.hasSentPass() && client.hasSentNick() && client.hasSentUser()) {
            client.setAuth(true);
            Messages::RPL_AUTHENTICATED(client.getNickname());
        } else {
            Messages::ERR_NOTAUTHENTICATED(client.getNickname());
        }
}

static void Command::PASS(const vector<string>& params, Client& client, Server& server) {
    if (params.size() < 1)
    {
        Messages::ERR_NEEDMOREPARAMS("PASS");
        return;
    }

    if (client.isReg()) {
        Messages::ERR_ALREADYREGISTRED(client.getNickname());
        return;
    }

    if (params[0] == server.getPassword()) {
        client.setReg(true);
        client.HasSentPass(true);
        Messages::RPL_WELCOME(client.getNickname());
    } else {
        Messages::ERR_PASSWDMISMATCH();
    }
}

static void Command::NICK(const vector<string>& params, Client& client, Server& server) {
    if (params.size() < 1)
    {
        string error = "Error: Not enough parameters for command NICK\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        Messages::ERR_NEEDMOREPARAMS("NICK");
        return;
    }

    string nickname = params[0];

    if (nickname.empty()) {
        string error = "Error: No nickname given\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        Messages::ERR_NONICKNAMEGIVEN();
        return;
    }

    if (!isValidNickname(nickname)) {
        string error = "Error: Erroneous nickname " + nickname + ".\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        Messages::ERR_ERRONEUSNICKNAME(nickname);
        return;
    }

    if (server.manageNickname(nickname, nullptr, CHECK)) {
        string error = "Error: Nickname " + nickname + " is already in use.\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        Messages::ERR_NICKNAMEINUSE(nickname);
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
        Messages::ERR_NEEDMOREPARAMS("USER");
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
        Messages::ERR_NEEDMOREPARAMS("JOIN");
        return;
    }
    if (!client.isAuth()) {
        string error2 = "Error: Client not authenticated\r\n";
        send(client.getFd(), error2.c_str(), error2.length(), 0);
        Messages::ERR_NOTAUTHENTICATED(client.getNickname());
        return;
    }

    const string channelName = params[0];
    const string channelKey = params[1];
    if (channelName.empty() || channelName[0] != '#') {
        string error3 = "Error: Invalid channel name " + channelName + ".\r\n";
        send(client.getFd(), error3.c_str(), error3.length(), 0);
        Messages::ERR_NOSUCHCHANNEL(channelName);
        return;
    }

    Channel* channel = server.createOrGetChannel(channelName);


    if (channel->isInviteOnly() && !channel->isInvited(client.getNickname())) {
        string error4 = "Error: Channel " + channelName + " is invite-only. (+i)\r\n";
        send(client.getFd(), error4.c_str(), error4.length(), 0);
        Messages::ERR_INVITEONLYCHAN(channelName);
        return;
    }

    if (channel->hasKey()) {
        if (channelKey != channel->getKey()) {
            string error5 = "Error: Bad channel key for " + channelName + ". (+k)\r\n";
            send(client.getFd(), error5.c_str(), error5.length(), 0);
            Messages::ERR_BADCHANNELKEY(channelName);
            return;
        }
    }

    if (channel->hasClientLimit()) {
        if (channel->isFull()) {
            string error6 = "Error: Channel " + channelName + " is full. (+l)\r\n";
            send(client.getFd(), error6.c_str(), error6.length(), 0);
            Messages::ERR_CHANNELISFULL(channelName);
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
        Messages::RPL_TOPIC(client.getNickname(), channelName, channel->getTopic());
    } else {
        string noTopicMsg = client.getNickname() + " " + channelName + " :No topic is set\r\n";
        send(client.getFd(), noTopicMsg.c_str(), noTopicMsg.length(), 0);
        Messages::RPL_NOTOPIC(client.getNickname(), channelName);
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
        Messages::ERR_NEEDMOREPARAMS("KICK");
        return;
    }
    string channelName = params[0];
    string targetNickname = params[1];
    string comment = params[2];

    Channel* channel = server.getChannel(channelName);
    if (!channel) {
        Messages::ERR_NOSUCHCHANNEL(channelName);
        return;

    }
    if (!channel->isOperator(client.getNickname())) {
        Messages::ERR_CHANOPRIVSNEEDED(channelName);
        return;
    }
    Client* targetClient = server.getClientByNickname(targetNickname);

    if (!targetClient || !channel->hasClient(targetNickname)) {
        Messages::ERR_USERNOTINCHANNEL(targetNickname, channelName);
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
        Messages::ERR_NEEDMOREPARAMS("INVITE");
        return;
    }
    string targetNickname = params[0];
    string channelName = params[1];

    Channel* channel = server.getChannel(channelName);
    if (!channel) {
        Messages::ERR_NOSUCHCHANNEL(channelName);
        return;
    }
    if (!channel->isOperator(client.getNickname())) {
        Messages::ERR_CHANOPRIVSNEEDED(channelName);
        return;
    }
    Client* targetClient = server.getClientByNickname(targetNickname);
    if (!targetClient) {
        Messages::ERR_NOSUCHNICK(targetNickname);
        return;
    }

    if (channel->hasClient(targetNickname)) {
        Messages::ERR_USERONCHANNEL(targetNickname, channelName);
        return;
    }

    if (channel->hasInvitation(targetNickname)) {
        Messages::ERR_USERONCHANNEL(targetNickname, channelName);
        return;
    }

    channel->addInvitation(targetNickname);

    string inviteMsg = ":" + client.getNickname() + " INVITE " + targetNickname + " :" + channelName + "\r\n";
    send(targetClient->getFd(), inviteMsg.c_str(), inviteMsg.length(), 0);
}

static void Command::TOPIC(const vector<string>& params, Client& client, Server& server) {
    if (params.size() < 2)
    {
        Messages::ERR_NEEDMOREPARAMS("TOPIC");
        return;
    }

    string channelName = params[0];

    Channel* channel = server.getChannel(channelName);
    if (!channel) {
        Messages::ERR_NOSUCHCHANNEL(channelName);
        return;
    }

    if (!channel->hasClient(client.getNickname())) {
        Messages::ERR_NOTONCHANNEL(channelName);
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
        Messages::ERR_NEEDMOREPARAMS("MODE");
        return;
    }

    string channelName = params[0];
    string modeChanges = params[1];

    Channel* channel = server.getChannel(channelName);

    if (!channel) {
        Messages::ERR_NOSUCHCHANNEL(channelName);
        return;
    }

    if (!channel->isOperator(client.getNickname())) {
        Messages::ERR_CHANOPRIVSNEEDED(channelName);
        return;
    }
    
    if (modeChanges[0] == '+') {
        for (size_t i = 1; i < modeChanges.length(); ++i) {
            channel->setMode(modeChanges[i], true);
        }
    } else if (modeChanges[0] == '-') {
        for (size_t i = 1; i < modeChanges.length(); ++i) {
            channel->setMode(modeChanges[i], false);
        }
    } else {
        Messages::ERR_UNKNOWNMODE(modeChanges);
        return;
    }
    


    string modeMsg = ":" + client.getNickname() + " MODE " + channelName + " " + modeChanges + "\r\n";
    for (set<Client*>::iterator it = channel->getClients().begin(); it != channel->getClients().end(); ++it) {
        send((*it)->getFd(), modeMsg.c_str(), modeMsg.length(), 0);
    }
}

static void Command::PRIVMSG(const vector<string>& params, Client& client, Server& server) {
    if (params.size() < 2)
    {
        Messages::ERR_NEEDMOREPARAMS("PRIVMSG");
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
            Messages::ERR_NOSUCHCHANNEL(target);
            return;
        }
        if (!channel->hasClient(client.getNickname())) {
            Messages::ERR_CANNOTSENDTOCHAN(target);
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
            Messages::ERR_NOSUCHNICK(target);
            return;
        }

        string privMsg = ":" + client.getNickname() + " PRIVMSG " + target + " :" + message + "\r\n";
        send(targetClient->getFd(), privMsg.c_str(), privMsg.length(), 0);
    }
}