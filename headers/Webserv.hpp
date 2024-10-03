#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "server.hpp"
#include <arpa/inet.h>
#include <netinet/in.h>

class Webserv
{
	private:
		std::vector<Server> 			_servers;
		int								_server_fd;
		int								_epoll_fd;
		std::vector<struct epoll_event> _events;

		void parseConfigFile(std::ifstream& input);
		void serversInit();
		void epollInit();
		void eventLoop();
	public:
		Webserv(std::string config);
		~Webserv();
		void run();
		//Getters
		int getServerFd();
		int getEpollFd();
		std::vector<Server> getServers();
};

#endif