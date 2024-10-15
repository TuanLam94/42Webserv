#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
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
	bool	_cgiIsHere;
	// attributs execution CGI (mandatory + bonus)
	std::string	_cgiType;
	std::string	_RequestMethod; // yes
	std::string	_ContentLength; // yes
	std::string	_ContentType; // yes
	std::string	_QueryString; // yes
	std::string	_ScriptName; // yes
	std::string	_ServerName; // yes
	std::string	_ServerPort;
	std::string	_ServerProtocol; // yes
	std::string	_GatewayInterface; // version cgi utilise 
	std::string	_PathInfo; // yes
	std::string	_RemoteAddr; 
	std::string	_RemoteHost; 
	std::string	_HttpHost; // yes
	std::string	_HttpAccept; // yes
	std::string	_HttpAcceptLanguage; // yes
	std::string	_HttpAcceptEncoding; // yes
	std::string	_HttpReferer; 
	std::string	_HttpConnection;
	std::string	_RemoteUser;
	std::string	_AuthType;
	std::string	_RedirectStatus;
	// attributs GET, POST
	std::string	_method;
	std::string	_path;
	std::string	_version;
	std::string	_response;
	std::string	_body;
	std::string	_contentType;
	std::string	_host; // initailsier avec recuperationd des headers --> host == localhost remplacer par adresse ip correspondante
	std::string	_serverName; // same
	int	_port;
	Server		_server;
	int _status_code;
	unsigned long int	_contentLength;
	int _max_client_body_size;
	// int	_client_fd;

	// attributs GET et POST
	std::fstream	_input;
	std::vector<std::pair<std::string, std::string> >	_headersHttp;
	std::map<std::string, std::string>	_queryParameter;
	
	// attributs POST
	std::map<std::string, std::string>	_FormDataName; // multipart/form-data
	// std::map<std::string, std::string>	_FormDataFilename; // multipart/form-data
	std::map<std::string, std::string>	_jsonParam; // application/json
	std::map<std::string, std::string>	_urlParam; // application/x-www-form-urlencoded
	std::string	_boundary;

	public:
	Request();
	Request(const Request& copy);
	Request& operator=(const Request& other);
	~Request() {};
	void	parsRequest(Server i, const std::string& buffer);
	void	parsRequestLine(std::string buff);
	void	checkMethod();
	void	checkVersion();
	void	checkHeaderName();
	void	parsParamPath();
	std::string	parsParamPath_bis(std::string str);
	void	parsPath(Server obj);
	void	parsHeaders(const std::string& buff);
	void	parsingGET(Server i, const std::string& buffer);
	void	parsingPOST(Server i, const std::string& buffer);
	void	parsingDELETE(Server i, const std::string& buffer);
	void	parsingPUT(Server i, const std::string& buffer);
	void	parserJson();
	void	checkJsonAccolade();
	void	parserUrlencoded();
	void	parserUrlencoded_bis(std::string new_body);
	std::string	parserFormData(std::string second, const std::string& buff);
	void	parserFormData_bis(const std::string& buff);
	int	parserFormData_ter(const std::string& buff, unsigned long int i);
	void	fillVar();
	int	checkContentType();
	void	getClientIPPort(int clientfd);
	bool	parserFormData_help(const std::string& buff, unsigned long int i);

	void	checkISS(char c1, char c2);
	bool	checkValidHeader(char c);
	bool	checkValidChar(char c);
	void	checkKey(std::string key);
	void	checkValue(std::string value);
	bool	checkContentLength();
	void	initContentLength();
	void	checkKeyUrl(std::string key);
	void	checkValueUrl(std::string value);
	bool	checkMap(std::string key, std::map<std::string, std::string>::iterator it, std::map<std::string, std::string>::iterator ite);
	void	checkCgi();
	void	initVE();
	void	fillCgiGet();
	void	fillCgiPost();
	// void	fillCgiPost();
	// void	fillCgiDelete();
	std::string	fillCgiToUpper(std::string fillvar, std::string var);
	// --------- GETTERS -------------
	std::string	getMethod() const;
	std::string	getPath() const;
	std::string	getVersion() const;
	std::string	getBody() const;
	std::string	getContentType() const;
	std::string	getResponse() const;
	std::string	getHost() const;
	std::string	getServerName() const;
	int getMaxBodySize() const;
	const Server&	getServer() const;
	int getPort() const;
	int getStatusCode() const;
	const std::map<std::string, std::string>& getFormDataName() const;
	const std::map<std::string, std::string>& getJsonParam() const;
	const std::map<std::string, std::string>& getUrlParam() const;
	bool	isCgi() const;
	std::string getQueryString() const;
	std::string getPathInfo() const;
	std::string getScriptName() const;
	std::string getServerPort() const;
	std::string getRemoteAddr() const;
	std::string getRemotePort() const;
	//------------SETTERS------------
	void	setServer(Server& server);
	void	setRequestStatusCode(int status_code);
	//Utils
	void	printRequest() const;

};

bool	checkValidChar(char c);
// void	checkISS(char c1, char c2);
// bool	checkValidHeader(char c);
void	checkKey(std::string key);
void	checkValue(std::string value);
int	checkUrlEncoded(std::string body);

#endif