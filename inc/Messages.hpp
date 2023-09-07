/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Messages.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fbily <fbily@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/04 10:58:08 by meshahrv          #+#    #+#             */
/*   Updated: 2023/09/07 17:19:24 by fbily            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGES_HPP
# define MESSAGES_HPP

# include <string>

std::string RPL_WELCOME(std::string nick, std::string server, std::string user, std::string hostname);
std::string RPL_YOURHOST(std::string nick, std::string server);
std::string RPL_CREATED(std::string nick, std::string server , std::string date);
std::string RPL_MYINFO(std::string nick, std::string server);
std::string RPL_PONG(std::string source, std::string message);
std::string RPL_TOPIC(std::string nick, std::string server, std::string channel, std::string topic);
std::string RPL_NOTOPIC(std::string nick, std::string server, std::string channel);
std::string RPL_NAMREPLY(std::string nick, std::string server, std::string channel, std::string user);
std::string RPL_ENDOFNAMES(std::string nick, std::string server, std::string channel);
std::string RPL_INVITING(std::string nick, std::string server, std::string channel, std::string user);


std::string ERR_NOSUCHSERVER(std::string nick, std::string arg);
std::string ERR_PASSWDMISMATCH(std::string nick);
std::string ERR_NONICKNAMEGIVEN(std::string nick);
std::string ERR_NICKCOLLISION(std::string nick);
std::string ERR_ALREADYREGISTERED(std::string nick);
std::string ERR_NEEDMOREPARAMS(std::string nick, std::string arg);
std::string ERR_BADCHANNELKEY(std::string nick, std::string channel);
std::string ERR_CHANNELISFULL(std::string nick, std::string channel);
std::string ERR_INVITEONLYCHAN(std::string nick, std::string channel);
std::string ERR_NOSUCHCHANNEL(std::string nick, std::string channel);
std::string ERR_CHANOPRIVSNEEDED(std::string nick, std::string channel);
std::string ERR_USERNOTINCHANNEL(std::string nick, std::string channel, std::string user);
std::string ERR_NOTONCHANNEL(std::string nick, std::string channel);
std::string ERR_USERONCHANNEL(std::string nick, std::string channel, std::string user);
std::string ERR_UMODEUNKNOWNFLAG(std::string nick);

#endif