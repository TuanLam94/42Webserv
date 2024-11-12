#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "server.hpp"
// #include <arpa/inet.h>
// #include <netinet/in.h>
#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <dirent.h>
#include <cstdlib>
#include <fcntl.h>
#include <sys/epoll.h>
#include <cstring>
#include <unistd.h>
#include <sstream>
#include "request.hpp"
#include "response.hpp"
#include "utils.hpp"
#include <sys/wait.h>
#include <dirent.h>
#include <cstring>
#include <set>
#include <map>

class Webserv
{
	private:
		std::vector<Server> 			_servers;
		std::vector<Request>			_requests;
		int								_server_fd;
		int								_epoll_fd;
		std::vector<struct epoll_event> _events;
		std::set<int> _client_fds;

		void parseConfigFile(std::ifstream& input);
		void serversInit();
		void epollInit();
		void eventLoop();
	public:
		Webserv(std::string config);
		~Webserv();
		void run();
		void handleClientRequest(int client_fd, Request& request);
		void handleClientWrite(int event_fd, Request& request);
		Server* findAppropriateServer(Request& request);
		Server* findServerByName(const Request& request);
		Server* findServerByPort(const Request& request);
		Server* redirectServer(Request& request);
		Request* findAppropriateRequest(int event_fd);
		Request* findAppropriateRequestToWrite(int event_fd);
		void sendErrorResponse(int client_fd, int statusCode);
		void removeRequest(int event_fd);
		//Getters
		int getServerFd();
		int getEpollFd();
		std::vector<Server> getServers();
};

#endif