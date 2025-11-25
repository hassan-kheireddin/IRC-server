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

    // Map commands to their corresponding functions
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
    }
    else {
        Messages::ERR_UNKNOWNCOMMAND(command);
    }
}

static void Command::AUTHENTICATE(Client& client, Server& server) {
        if (client.isAuth()) {
            Messages::ERR_ALREADYREGISTRED(client.getNickname());
            return;
        }
        if (client.hasSentPass() && client.hasSentNick() && client.hasSentUser()) {
            client.setAuth(true);
        } else {
            Messages::ERR_NOTREGISTERED(client.getNickname());
        }
}

static void Command::PASS(const vector<string>& params, Client& client, Server& server) {
    if (params.size() < 2)
    {
        Messages::ERR_NEEDMOREPARAMS("PASS");
        return;
    }

    if (client.isReg()) {
        Messages::ERR_ALREADYREGISTRED(client.getNickname());
        return;
    }

    if (params[1] == server.getPassword()) {
        client.setReg(true);
        client.HasSentPass(true);
        Messages::RPL_WELCOME(client.getNickname());
    } else {
        Messages::ERR_PASSWDMISMATCH();
    }
}

static void Command::NICK(const vector<string>& params, Client& client, Server& server) {
    if (params.size() < 2)
    {
        string error = "Error: Not enough parameters for command NICK\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        Messages::ERR_NEEDMOREPARAMS("NICK");
        return;
    }

    string nickname = params[1];

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
    if (params.size() < 5)
    {
        Messages::ERR_NEEDMOREPARAMS("USER");
        return;
    }

    client.setUsername(params[1]);
    client.setRealname(params[4]);
    client.HasSentUser(true);
}


static void Command::JOIN(const vector<string>& params, Client& client, Server& server) {
    // Implementation of JOIN command
}
