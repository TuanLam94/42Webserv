#ifndef RESPONSE_HPP
#define RESPONSE_HPP

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
#include "../headers/server.hpp"

class	Response
{
	private: 
	// attributs remplis apres parsing de la requete 
	std::string _method;
	std::string _path;
	std::string _version;
	std::string _response;
	// int	_client_fd;

	// attributs GET
	std::fstream	_input;


	public:
	Response() {};
	~Response() {};
	// Request(int client_fd);
	void	open_file_GET(Server i, const std::string& buffer);
	void	find_request();
	std::string	GET_method();
	// void	POST_method();
	// void	DELETE_method();
	std::string	build_response(const std::string& body, const std::string& content_type);
	std::string	getMethod();
	std::string	getPath();
	std::string	getVersion();
};

#endif