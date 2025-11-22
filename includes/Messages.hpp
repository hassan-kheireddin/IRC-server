#ifndef MESSAGES_HPP
#define MESSAGES_HPP

#include <iostream>
#include <string>
namespace std


class Messages
{
    public:
    //welcome and server creation messages
        void RPL_WELCOME(const std::string& user);
        void RPL_CREATED(const std::string& date);

    //no enough parameters error used by PASS, USER, JOIN, TOPIC, INVITE, PRIVMSG, Kick.
        void ERR_NEEDMOREPARAMS(const std::string& command);
    //already registered error used by PASS.
        void ERR_ALREADYREGISTRED(const std::string& user);
    //password mismatch error used by PASS.
        void ERR_PASSWDMISMATCH(const std::string& user);

    //no nickname given error used by NICK.
        void ERR_NONICKNAMEGIVEN();
    //error forbidden character nickname error used by NICK.
        void ERR_ERRONEUSNICKNAME(const std::string& nickname);
    //nickname in use error used by NICK.
        void ERR_NICKNAMEINUSE(const std::string& nickname);
    
    //no such channel error used by JOIN, TOPIC, INVITE, MODE, KICK.
        void ERR_NOSUCHCHANNEL(const std::string& channel);
    //bad channel key error used by JOIN.
        void ERR_BADCHANNELKEY(const std::string& channel);
    //channel is full error used by JOIN.
        void ERR_CHANNELISFULL(const std::string& channel);
    //invite only channel error used by JOIN.
        void ERR_INVITEONLYCHAN(const std::string& channel);
    //topic used by TOPIC, JOIN.
        void RPL_TOPIC(const std::string& channel, const std::string& topic);

    //not in channel error used by TOPIC, INVITE, KICK.
        void ERR_NOTINCHANNEL(const std::string& channel);
    //channel operator privileges needed error used by MODE, KICK.
        void ERR_CHANOPRIVSNEEDED(const std::string& channel);
    //no topic is set message used by TOPIC, JOIN.
        void RPL_NOTOPIC(const std::string& channel);
    
    //invite message used by INVITE.
        void RPL_INVITING(const std::string& channel, const std::string& user);
    //user on channel error used by INVITE.
        void ERR_USERONCHANNEL(const std::string& user, const std::string& channel);
    
    //user not in channel error used by KICK.
        void ERR_USERNOTINCHANNEL(const std::string& user, const std::string& channel);

    //no such nick error used by PRIVMSG, MODE.
        void ERR_NOSUCHNICK(const std::string& nickname);
    //no such server error used by PRIVMSG, MODE.
        void ERR_NOSUCHSERVER(const std::string& server);
    //cannot send to channel error used by PRIVMSG.
        void ERR_CANNOTSENDTOCHAN(const std::string& channel);
    //no text to send error used by PRIVMSG.
        void ERR_NOTEXTTOSEND();
    //no recipient error used by PRIVMSG.
        void ERR_NORECIPIENT(const std::string& command);

    //unknown mode flag error used by MODE.
        void ERR_UMODEUNKNOWNFLAG(const std::string& flag);
    //users don't match error used by MODE.
        void ERR_USERSDONTMATCH();
    //user mode is message used by MODE.
        void RPL_UMODEIS(const std::string& user, const std::string& mode);
    //channel mode is message used by MODE.
        void RPL_CHANMODEIS(const std::string& channel, const std::string& mode);
    
        //invalid key error used by MODE +k
        void ERR_INVALIDKEY(const std::string& channel);
    };