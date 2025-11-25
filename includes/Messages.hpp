#ifndef MESSAGES_HPP
#define MESSAGES_HPP

#include <iostream>
#include <string>
using namespace std;


class Messages
{
    public:
    //welcome and server creation messages
        void RPL_WELCOME(const string& client);
        void RPL_CREATED(const string& message);
    //authentication messages
        void RPL_AUTHENTICATED(const string& client);
        void ERR_NOTAUTHENTICATED(const string& client);
        void ERR_ALREADYAUTHENTICATED(const string& client);
    //no enough parameters error used by PASS, NICK, JOIN, TOPIC, INVITE, PRIVMSG, Kick.
        void ERR_NEEDMOREPARAMS(const string& command);
    //already registered error used by PASS.
        void ERR_ALREADYREGISTRED(const string& client);
    //password mismatch error used by PASS.
        void ERR_PASSWDMISMATCH();

    //no nickname given error used by NICK.
        void ERR_NONICKNAMEGIVEN();
    //error forbidden character nickname error used by NICK.
        void ERR_ERRONEUSNICKNAME(const string& nickname);
    //nickname in use error used by NICK.
        void ERR_NICKNAMEINUSE(const string& nickname);
    
    //no such channel error used by JOIN, TOPIC, INVITE, MODE, KICK.
        void ERR_NOSUCHCHANNEL(const string& channel);
    //bad channel key error used by JOIN.
        void ERR_BADCHANNELKEY(const string& channel);
    //channel is full error used by JOIN.
        void ERR_CHANNELISFULL(const string& channel);
    //invite only channel error used by JOIN.
        void ERR_INVITEONLYCHAN(const string& channel);
    //topic used by TOPIC, JOIN.
        void RPL_TOPIC(const string& channel, const string& topic);

    //not in channel error used by TOPIC, INVITE, KICK.
        void ERR_NOTINCHANNEL(const string& channel);
    //channel operator privileges needed error used by MODE, KICK.
        void ERR_CHANOPRIVSNEEDED(const string& channel);
    //no topic is set message used by TOPIC, JOIN.
        void RPL_NOTOPIC(const string& channel);
    
    //invite message used by INVITE.
        void RPL_INVITING(const string& channel, const string& client);
    //client on channel error used by INVITE.
        void ERR_clientONCHANNEL(const string& client, const string& channel);
    
    //client not in channel error used by KICK.
        void ERR_clientNOTINCHANNEL(const string& client, const string& channel);

    //no such nick error used by PRIVMSG, MODE.
        void ERR_NOSUCHNICK(const string& nickname);
    //no such server error used by PRIVMSG, MODE.
        void ERR_NOSUCHSERVER(const string& server);
    //cannot send to channel error used by PRIVMSG.
        void ERR_CANNOTSENDTOCHAN(const string& channel);
    //no text to send error used by PRIVMSG.
        void ERR_NOTEXTTOSEND();
    //no recipient error used by PRIVMSG.
        void ERR_NORECIPIENT(const string& command);

    //unknown mode flag error used by MODE.
        void ERR_UMODEUNKNOWNFLAG(const string& flag);
    //clients don't match error used by MODE.
        void ERR_clientSDONTMATCH();
    //client mode is message used by MODE.
        void RPL_UMODEIS(const string& client, const string& mode);
    //channel mode is message used by MODE.
        void RPL_CHANMODEIS(const string& channel, const string& mode);
    
        //invalid key error used by MODE +k
        void ERR_INVALIDKEY(const string& channel);

        void ERR_UNKNOWNCOMMAND(const string& command);
    };

#endif