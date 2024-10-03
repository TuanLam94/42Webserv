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
// #include "response.hpp"
#include "../headers/server.hpp"
#include <sstream>

class Server;
class Response;

class	Request
{
	private: 
	// attributs GET, POST
	std::string _method;
	std::string _path;
	std::string _version;
	std::string _response;
	std::string	_body;
	std::string _contentType;
	// int	_client_fd;

	// attributs GET
	std::fstream	_input;
	std::vector<std::pair<std::string, std::string> >	_headersHttp;
	std::map<std::string, std::string>	_queryParameter;
	// attributs POST
	std::map<std::string, std::string>	_jsonParam; // application/json
	std::map<std::string, std::string>	_urlParam; // application/x-www-form-urlencoded
	std::string	_boundary;

	public:
	Request() {};
	Request(const Request& copy);
	Request& operator=(const Request& other);
	~Request() {};
	void	parsRequest(Server i, const std::string& buffer);
	void	parsRequestLine(std::string buff);
	void	checkMethod();
	void	checkVersion();
	void	checkHeaderName();
	// void	checkKey(std::string key);
	// void	checkValue(std::string value);
	void	parsParamPath();
	std::string	parsParamPath_bis(std::string str);
	void	parsPath(Server obj);
	void	parsHeaders(const std::string& buff);
	void	parsingGET(Server i, const std::string& buffer);
	void	parsingPOST(const std::string& buffer);
	void	parserJson();
	void	parserUrlencoded();
	void	parserUrlencoded_bis(std::string new_body);
	std::string	parserFormData(std::string second);
	// --------- GETTERS -------------
	std::string	getMethod() const;
	std::string	getPath() const;
	std::string	getVersion() const;
	std::string	getBody() const;
	std::string	getContentType() const;
	std::string	getResponse() const;
};

#endif