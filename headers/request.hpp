#ifndef REQUEST_HPP
#define REQUEST_HPP

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

class Server;

class	Request
{
	private: 
	// attributs remplis apres parsing de la requete 
	std::string _method;
	std::string _path;
	std::string _version;
	std::string _response;
	std::string	_body;
	std::string _contentType;
	// int	_client_fd;

	// attributs GET
	std::fstream	_input;
	// std::map<std::string, std::string>	_queryParameter;
	// std::map<std::string, std::string>	_headersHttp;/
	std::vector<std::pair<std::string, std::string> >	_queryParameter;
	std::vector<std::pair<std::string, std::string> >	_headersHttp;
	public:
	Request() {};
	Request(const Request& copy);
	Request& operator=(const Request& other);
	~Request() {};
	// Request(int client_fd);
	void	parsRequest(Server i, const std::string& buffer);
	void	parsParamPath();
	void	parsPath(Server obj);
	void	checkPath();
	void	parsHeaders(const std::string& buff);
	void	find_request();
	std::string	GET_method();
	// void	POST_method();
	// void	DELETE_method();
	std::string	build_response(const std::string& body, const std::string& content_type);
	std::string	getMethod() const;
	std::string	getPath() const;
	std::string	getVersion() const;
	std::string	getBody() const;
	std::string getContentType() const;
	std::string getResponse() const;
};

#endif