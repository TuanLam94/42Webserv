#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "server.hpp"

class Webserv
{
	private:
		std::vector<Server> 			_servers;
		int								_epoll_fd;
		std::vector<struct epoll_event> _events;

		void parseConfigFile(std::ifstream& input);
		void serversInit();
		void epollInit();
	public:
		Webserv(std::string config);
		~Webserv();
}



#endif