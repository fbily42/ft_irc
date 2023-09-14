/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fbily <fbily@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/06 17:28:51 by fbily             #+#    #+#             */
/*   Updated: 2023/09/14 18:34:37 by fbily            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Channel.hpp"

/******		Constructors	******/

Channel::Channel() {}

Channel::Channel(const Channel& obj)
{
	(void)obj;
}

Channel::Channel(std::string name, std::string key, User *user) : _name(name), _topic(""), _key(key), _isKey(false), _inviteMode(false), _topicMode(true),
	_maxUsers(0), _nbUser(1), _topicTime(""), _topicUser("")
{
	time_t now = time(0);
	std::ostringstream oss;
	oss << now;
	this->_Operators.push_back(user);
	this->_creationDate = oss.str();
}

/******		Destructor	******/

Channel::~Channel() {}

/******		Operator	******/

Channel& Channel::operator=(const Channel& rhs)
{
	(void)rhs;
	return (*this);
}

/******		Member functions	******/

std::string	Channel::getName() const
{
	return (this->_name);
}

std::string Channel::getTopic() const
{
	return (this->_topic);
}

bool Channel::getModeTopic() const
{
	return (this->_topicMode);
}

std::string	Channel::getTopicDate() const
{
	return (this->_topicTime);
}

std::string	Channel::getTopicUser() const
{
	return (this->_topicUser);
}

std::string Channel::getCreationDate() const
{
	return (this->_creationDate);
}

std::string Channel::getModestring() const
{
	std::string	modestring = "+";

	if (this->_inviteMode)
		modestring.append("i");
	if (this->_topicMode)
		modestring.append("t");
	if (this->_isKey == true)
		modestring.append("k");
	if (this->_maxUsers != 0)
	{
		std::ostringstream oss;
		oss << this->_maxUsers;
		modestring.append("l ");
		modestring.append(oss.str());
	}
	if (modestring == "+")
		modestring.clear();
	return (modestring);
}

unsigned int Channel::getNbUser() const
{
	return (this->_nbUser);
}

void Channel::setNbUser(int nb)
{
	this->_nbUser += nb;
}

void Channel::setTopic(User *user, std::string newTopic)
{
	time_t	now = time(0);
	std::ostringstream oss;
	oss << now;
	if(newTopic == ":")
		this->_topic = "";
	else
		this->_topic = newTopic;
	this->_topicTime = oss.str(); // retiens l'heure de modif
	this->_topicUser = user->getNickname(); // retien qui l'a modif
	sendMessage(user, RPL_TOPIC(user->getNickname(), user->getServer(), this->_name, this->_topic));
	sendMessage(user, RPL_TOPICWHOTIME(user->getNickname(), user->getServer(), this->_name, this->_topicUser, this->_topicTime));
}

std::string	Channel::joinChannel(std::string key, User *user)
{
	if (_inviteMode ==  true)
	{
		if (foundInvited(user->getNickname()) == false)
			return (ERR_INVITEONLYCHAN(user->getNickname(), this->getName()));//il est pas invite ce batard  
	}
	if (this->_maxUsers != 0 && this->_nbUser >= this->_maxUsers)
		return (ERR_CHANNELISFULL(user->getNickname(), this->getName()));//erreur plus de place
	
	if (this->_isKey == false || (this->_isKey == true && key == this->_key))
	{
		this->_Users.push_back(user);
		this->_nbUser++;
		return (':' + user->getNickname() + "!" + user->getNickname() + "@" + user->getHostname() + " JOIN " + this->getName());
	}
	else
		return (ERR_BADCHANNELKEY(user->getNickname(), this->getName()));//erreur de mot de passe 
}

bool Channel::foundUser(std::string nickname)
{
	std::vector<User *>::iterator it = this->_Users.begin();
	while (it != this->_Users.end())
	{
		if (nickname != (*it)->getNickname())
			it++;
		else
			return (true);//trouve
	}
	return (false);
}

bool Channel::foundOperator(std::string nickname)
{
	std::vector<User *>::iterator it = this->_Operators.begin();
	while (it != this->_Operators.end())
	{
		if (nickname != (*it)->getNickname())
			it++;
		else
			return (true);
	}
	return(false);
}

bool Channel::foundInvited(std::string nickname)
{
	std::vector<User *>::iterator it = this->_Invited.begin();
	while (it != this->_Invited.end())
	{
		if (nickname != (*it)->getNickname())
			it++;
		else
			return (true);
	}
	return(false);
}

void Channel::addGuest(User *user)
{
	this->_Invited.push_back(user);
}

void Channel::sendMessage(User *user , std::string message)
{
	std::vector<User *>::iterator it;
	it = this->_Operators.begin();
	while (it != this->_Operators.end())
	{
		if ((*it) != user)
			(*it)->sendReply(message);
		it++;
	}
	it = this->_Users.begin();
	while (it != this->_Users.end())
	{
		if ((*it) != user)
			(*it)->sendReply(message);
		it++;
	}
}

void	Channel::kickUser(User *user, std::string target, std::string comment)
{
	std::vector<User *>::iterator	it = this->_Users.begin();
	while (it != this->_Users.end())
	{
		if ((*it)->getNickname() == target)
		{
			this->sendMessage(user, ":" + user->getNickname() + " KICK " + this->_name + ' ' + target + ' ' + comment);
			user->sendReply(":" + user->getNickname() + " KICK " + this->_name + ' ' + target + ' ' + comment);
			this->_Users.erase(it);
			break ;
		}
		it++;
	}
}

void	Channel::listUsersOnChannel(User *user)
{
	std::string result;
	result.append(":" + user->getServer() + " 353 " + user->getNickname() + " = " + this->getName() + " :");
	std::vector<User *>::iterator it = this->_Operators.begin();
	while (it != this->_Operators.end())
	{
		if (it != this->_Operators.begin())
			result += ' ';
		result.append('@' + (*it)->getNickname());
		it++;
	}
	it = this->_Users.begin();
	while (it != this->_Users.end())
	{
		result.append(' ' + (*it)->getNickname());
		it++;
	}
	user->sendReply(result);
	user->sendReply(":" + user->getServer() + " 366 " + user->getNickname() + ' ' + this->getName() + " :End of /NAMES list");
}

void	Channel::whoList(User *user)
{
	std::vector<User *>::iterator it = this->_Operators.begin();
	while (it != this->_Operators.end())
	{
		user->sendReply(":" + user->getServer() + " 352 " + user->getNickname() + ' ' + this->_name + ' ' + (*it)->getUsername() + ' ' + (*it)->getHostname() + ' ' + (*it)->getServer() + ' ' + (*it)->getNickname() + " @ :0 " + (*it)->getRealname());
		it++;
	}
	it = this->_Users.begin();
	while (it != this->_Users.end())
	{
		user->sendReply(":" + user->getServer() + " 352 " + user->getNickname() + ' ' + this->_name + ' ' + (*it)->getUsername() + ' ' + (*it)->getHostname() + ' ' + (*it)->getServer() + ' ' + (*it)->getNickname() + " H :0 " + (*it)->getRealname());
		it++;
	}
}

bool Channel::hasDuplicates(const std::string& str) 
{
	if(str.size() > 6 )
		return (false);
    for (std::size_t i = 0; i < str.size(); i++) 
	{
        for (std::size_t j = i + 1; j < str.size(); j++) 
		{
            if (str[i] == str[j])
                return true;
        }
    }
    return false;
}

void	Channel::set_inviteMode(User *user, int sign)
{
	if (sign == 1)
	{
		this->_inviteMode = true;
		user->sendReply(":" + user->getNickname() + " MODE " + this->_name + " +i");
		sendMessage(user, ":" + user->getNickname() + " MODE " + this->_name + " +i");
	}
	if (sign == -1)
	{
		this->_inviteMode = false;
		user->sendReply(":" + user->getNickname() + " MODE " + this->_name + " -i");
		sendMessage(user, ":" + user->getNickname() + " MODE " + this->_name + " -i");
	}
}

void	Channel::set_topicMode(User *user, int sign)
{
	if (sign == 1)
	{
		this->_topicMode = true;
		user->sendReply(":" + user->getNickname() + " MODE " + this->_name + " +t");
		sendMessage(user, ":" + user->getNickname() + " MODE " + this->_name + " +t");
	}
	if (sign == -1)
	{
		this->_topicMode = false;
		user->sendReply(":" + user->getNickname() + " MODE " + this->_name + " -t");
		sendMessage(user, ":" + user->getNickname() + " MODE " + this->_name + " -t");
	}
}

void	Channel::set_key(User *user, std::string argument, int pos_argument, int sign)
{
	if (sign == -1)
	{
		this->_isKey = false;
		this->_key = "";
		user->sendReply(":" + user->getNickname() + " MODE " + this->_name + " -k");
		sendMessage(user, ":" + user->getNickname() + " MODE " + this->_name + " -k");
		return ;
	}
	std::string arg_1;
	int pos_bis = 0;
	size_t pos;
	
	while (!argument.empty() && pos_bis <= pos_argument)
	{
		pos = argument.find(' ');
		arg_1 = argument.substr(0, argument.find(' '));
		if (pos_bis == pos_argument)
			break;
		if (pos == std::string::npos)
		{
			argument.erase(0, arg_1.length());  
			break;
		}
		else
			argument.erase(0, arg_1.length() + 1);// +1 pour supprimer l'espace suivant
		pos_bis++;
	}
	if (!argument.empty())
	{
		if (arg_1 == "x")
			return (user->sendReply(":" + user->getServer() + " 525 " + user->getNickname() + ' ' + this->_name + " :Key is not well-formed (x is not a valid key)"));
		this->_key = true;
		this->_key = arg_1;
		user->sendReply(":" + user->getNickname() + " MODE " + this->_name + " +k");
		sendMessage(user, ":" + user->getNickname() + " MODE " + this->_name + " +k");
	}
}

void	Channel::kickModeOperator(std::string target)
{
	std::vector<User *>::iterator	it = this->_Operators.begin();
	while (it != this->_Operators.end())
	{
		if ((*it)->getNickname() == target)
		{
			this->_Operators.erase(it);
			break ;
		}
		it++;
	}
}

void	Channel::kickModeUser(std::string target)
{
	std::vector<User *>::iterator	it = this->_Users.begin();
	while (it != this->_Users.end())
	{
		if ((*it)->getNickname() == target)
		{
			this->_Users.erase(it);
			break ;
		}
		it++;
	}
}

void	Channel::kickModeInvited(std::string target)
{
	std::vector<User *>::iterator	it = this->_Invited.begin();
	while (it != this->_Invited.end())
	{
		if ((*it)->getNickname() == target)
		{
			this->_Invited.erase(it);
			break ;
		}
		it++;
	}
}

User* Channel::returnOperator(std::string nickname)
{
	std::vector<User *>::iterator it = this->_Operators.begin();
	while (it != this->_Operators.end())
	{
		if (nickname != (*it)->getNickname())
			it++;
		else
			return (*it);
	}
	return (*it);
}

User* Channel::returnUser(std::string nickname)
{
	std::vector<User *>::iterator it = this->_Users.begin();
	while (it != this->_Users.end())
	{
		if (nickname != (*it)->getNickname())
			it++;
		else
			return (*it);
	}
	return (*it);
}

void	Channel::set_op(User *user, std::string argument, int pos_argument, int sign)
{
	std::string arg_1;
	int pos_bis = 0;
	size_t pos = 0;

	while (!argument.empty() && pos_bis <= pos_argument)
	{
		pos = argument.find(' ');
		arg_1 = argument.substr(0, argument.find(' '));
		if (pos_bis == pos_argument)
			break;
		if (pos == std::string::npos)
		{
			argument.erase(0, arg_1.length());  
			break;
		}
		else
			argument.erase(0, arg_1.length() + 1);// +1 pour supprimer l'espace suivant
		pos_bis++;
	}
	if (foundOperator(user->getNickname()))
	{
		if (sign == -1 && foundOperator(arg_1) == true)
		{
			this->_Users.push_back(returnOperator(arg_1));
			kickModeOperator(arg_1);
			user->sendReply(":" + user->getNickname() + " MODE " + this->_name + " -o " + arg_1);
			sendMessage(user, ":" + user->getNickname() + " MODE " + this->_name + " -o " + arg_1);
		}
		if (sign == 1 && foundOperator(arg_1) == false)
		{
			this->_Operators.push_back(returnUser(arg_1));
			kickModeUser(arg_1);
			user->sendReply(":" + user->getNickname() + " MODE " + this->_name + " +o " + arg_1);
			sendMessage(user, ":" + user->getNickname() + " MODE " + this->_name + " +o " + arg_1);
		}
	}
	else
		return ; //Un user ne peut pas retirer de privilege
}

// bool safeStringToInt(const std::string& str, int& result)
// {
//     std::stringstream ss(str);
//     long temp; // Utiliser un type plus grand pour capturer les valeurs en dehors des limites de int

//     ss >> temp; // Effectuer la conversion

//     // Vérifier l'état du stringstream
//     if (ss.fail() || !ss.eof()) {
//         return false;
//     }

//     // Vérifier les limites pour le type int
//     if (temp > INT_MAX || temp < INT_MIN) {
//         return false;
//     }

//     // Tout est ok, affecter la valeur à result
//     result = static_cast<unsigned int>(temp);
//     return true;
// }

unsigned int	Channel::getMaxUsers() const
{
	return (this->_maxUsers);
}

void	Channel::set_maxUsers(User *user, std::string argument, int pos_argument, int sign)
{
	if (sign == -1)
	{
		this->_maxUsers = 0;
		user->sendReply(":" + user->getNickname() + " MODE " + this->_name + " -l");
		sendMessage(user, ":" + user->getNickname() + " MODE " + this->_name + " -l");
		return ;
	}
	//checker quelle est le max pour un channel
	std::string arg_1;//faire le parsing du int 
	size_t pos = 0;
	int pos_bis = 0;
	
	while (!argument.empty() && pos_bis <= pos_argument)
	{
		pos = argument.find(' ');
		arg_1 = argument.substr(0, argument.find(' '));
		if (pos_bis == pos_argument)
			break;
		if (pos == std::string::npos)
		{
			argument.erase(0, arg_1.length());  
			break;
		}
		else
			argument.erase(0, arg_1.length() + 1);// +1 pour supprimer l'espace suivant
		pos_bis++;
	}
	if (!argument.empty() && std::atoi(arg_1.c_str()) > static_cast<int>(this->_nbUser))
	{
		this->_maxUsers = std::atoi(arg_1.c_str());
		user->sendReply(":" + user->getNickname() + " MODE " + this->_name + " +l " + arg_1);
		sendMessage(user, ":" + user->getNickname() + " MODE " + this->_name + " +l " + arg_1);
	}
}

void	Channel::modeChannel(User *user, std::string modestring, std::string argument)
{
	/* est ce que on doit envoyer un message si on essaye de mettre a vrai une valeur deja vrai
	quelle erreur envoyer en cas de muavais modestring */
	/* MODE - Changer le mode du channel :
		— i : Définir/supprimer le canal sur invitation uniquement (pas besoin d'argument)
		— t : Définir/supprimer les restrictions de la commande TOPIC pour les opérateurs de canaux (pas besoin d'argument)
		()— k : Définir/supprimer la clé du canal (mot de passe) (besoin d'un argument)
		()— o : Donner/retirer le privilège de l’opérateur de canal (besoin d'un argument)
		()— l : Définir/supprimer la limite d’utilisateurs pour le canal ()*/
	/* Pour l'instant je suis parti du principe que tu peux m'envoyer un seul operator + ou -, et tous les modes en meme temps sans doublon
	ce qui fait donc au max 3 argument  */
	int pos_argument = 0;
	int sign = 0;
	
	//if (hasDuplicates(modestring))
	//	return ;//erreur doublon
	if(modestring[0] == '+' || modestring[0] == '-')//pour l'instant j'aurtorise que l'un des deux mais je peux sinon mettre en place la possibilite de swap entre les deux mais faudra changer la fonction qui check les duplicates
	{
		/* pour gerer le plus et le moins en meme temps, on peut juste rajouter (dans la boucle et avant):
		int operator = 0;
		if(modestring[i] == '+')
			operator = 0
		if(modestring[i] == '-')
			operator =  1
		puis envoyer operator au lieu de modestring dans les fonction*/
		if(modestring[0] == '+')
			sign = 1;
		if(modestring[0] == '-')
			sign = -1;
		for (size_t i = 1; i < modestring.size(); i++) 
		{
			if (modestring[i] == '+')
				sign = 1;
			if (modestring[i] == '-')
				sign = -1;
			if (modestring[i] == 'i')
				set_inviteMode(user, sign);
			if (modestring[i] == 't')
				set_topicMode(user, sign);
			if (modestring[i] == 'k')
			{
				set_key(user, argument, pos_argument, sign);
				pos_argument++;
			}
			if (modestring[i] == 'o')
			{
				set_op(user, argument, pos_argument, sign);
				pos_argument++;
			}
			if (modestring[i] == 'l')
			{
				set_maxUsers(user, argument, pos_argument, sign);
				pos_argument++;
			}
		}		
	}
	else
		return ; //erreur car il ne commence pas par + ou -
}
