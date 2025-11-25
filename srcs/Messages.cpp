#include "../includes/Messages.hpp"
using namespace std;

void Messages::RPL_WELCOME(const string& client)
{
    cout << client << ", Welcome to the server! If you want to Join Channels you must be authenticated." << endl;
}

void Messages::RPL_AUTHENTICATED(const string& client)
{
    cout << client << ", You have been successfully authenticated!" << endl;
}

void Messages::RPL_CREATED(const string& message)
{
    cout << message << endl;
}

void Messages::ERR_UNKNOWNCOMMAND()
{
    cout << "Error: Unknown command " << command << "." << endl;
}

void Messages::ERR_NEEDMOREPARAMS(const string& command)
{
    cout << "Error: Not enough parameters for command " << command << "." << endl;
}

void Messages::ERR_ALREADYREGISTRED(const string& client)
{
    cout << "Error: client " << client << " is already registered." << endl;
}

void Messages::ERR_ALREADYAUTHENTICATED(const string& client)
{
    cout << "Error: client " << client << " is already authenticated." << endl;
}

void Messages::ERR_PASSWDMISMATCH()
{
    cout << "Error: Password mismatch for client." << endl;
}

void Messages::ERR_NOTAUTHENTICATED(const string& client)
{
    cout << "Error: client " << client << " is not authenticated. You Should provide Password for the Server, Nickname and Username." << endl;
}

void Messages::ERR_NONICKNAMEGIVEN()
{
    cout << "Error: No nickname given." << endl;
}

void Messages::ERR_ERRONEUSNICKNAME(const string& nickname)
{
    cout << "Error: Erroneous nickname " << nickname << "." << endl;
}

void Messages::ERR_NICKNAMEINUSE(const string& nickname)
{
    cout << "Error: Nickname " << nickname << " is already in use." << endl;
}

void Messages::ERR_NOSUCHCHANNEL(const string& channel)
{
    cout << "Error: No such channel " << channel << "." << endl;
}

void Messages::ERR_BADCHANNELKEY(const string& channel)
{
    cout << "Error: Bad channel key for channel (+k)" << channel << "." << endl;
}

void Messages::ERR_CHANNELISFULL(const string& channel)
{
    cout << "Error: Channel " << channel << " is full. (+l)" << endl;
}

void Messages::ERR_INVITEONLYCHAN(const string& channel)
{
    cout << "Error: Channel " << channel << " is invite-only. (+i)" << endl;
}

void Messages::RPL_TOPIC(const string& channel, const string& topic)
{
    cout << "Topic for channel " << channel << " is: " << topic << endl;
}

void Messages::ERR_NOTINCHANNEL(const string& channel)
{
    cout << "Error: You are not in channel " << channel << "." << endl;
}

void Messages::ERR_CHANOPRIVSNEEDED(const string& channel)
{
    cout << "Error: You need channel operator privileges for channel " << channel << "." << endl;
}

void Messages::RPL_NOTOPIC(const string& channel)
{
    cout << "No topic is set for channel " << channel << "." << endl;
}

void Messages::RPL_INVITING(const string& channel, const string& client)
{
    cout << "Inviting client " << client << " to channel " << channel << "." << endl;
}

void Messages::ERR_clientONCHANNEL(const string& client, const string& channel)
{
    cout << "Error: client " << client << " is already on channel " << channel << "." << endl;
}

void Messages::ERR_clientNOTINCHANNEL(const string& client, const string& channel)
{
    cout << "Error: client " << client << " is not in channel " << channel << "." << endl;
}

void Messages::ERR_NOSUCHNICK(const string& nickname)
{
    cout << "Error: No such nick " << nickname << "." << endl;
}

void Messages::ERR_NOSUCHSERVER(const string& server)
{
    cout << "Error: No such server " << server << "." << endl;
}

void Messages::ERR_CANNOTSENDTOCHAN(const string& channel)
{
    cout << "Error: Cannot send to channel " << channel << "." << endl;
}

void Messages::ERR_NOTEXTTOSEND()
{
    cout << "Error: No text to send." << endl;
}

void Messages::ERR_NORECIPIENT(const string& command)
{
    cout << "Error: No recipient given for command " << command << "." << endl;
}

void Messages::ERR_UMODEUNKNOWNFLAG(const string& flag)
{
    cout << "Error: Unknown mode flag " << flag << "." << endl;
}

void Messages::ERR_clientSDONTMATCH()
{
    cout << "Error: clients don't match." << endl;
}

void Messages::RPL_UMODEIS(const string& client, const string& mode)
{
    cout << "client mode for " << client << " is: " << mode << endl;
}

void Messages::RPL_CHANMODEIS(const string& channel, const string& mode)
{
    cout << "Channel mode for " << channel << " is: " << mode << endl;
}

void Messages::ERR_INVALIDKEY(const string& channel)
{
    cout << "Error: Invalid key for channel " << channel << ". (+k)" << endl;
}
