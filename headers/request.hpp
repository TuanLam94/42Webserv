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
	std::string	_contentLength;
	// int	_client_fd;

	// attributs GET et POST
	std::fstream	_input;
	std::vector<std::pair<std::string, std::string> >	_headersHttp;
	std::map<std::string, std::string>	_queryParameter;
	
	// attributs POST
	std::map<std::string, std::string>	_FormDataName; // multipart/form-data
	std::map<std::string, std::string>	_FormDataFilename; // multipart/form-data
	std::map<std::string, std::string>	_jsonParam; // application/json
	std::map<std::string, std::string>	_urlParam; // application/x-www-form-urlencoded
	std::string	_boundary;

	public:
	Request() { _status_code = 0; };
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
	// --------- GETTERS -------------
<<<<<<< HEAD
	std::string	getMethod() const;
	std::string	getPath() const;
	std::string	getVersion() const;
	std::string	getBody() const;
	std::string	getContentType() const;
	std::string	getResponse() const;
	std::string	getHost() const;
	std::string	getServerName() const;
	std::string	getCodeStatus() const;
	//------------SETTERS------------
	void setServer(Server& server);
	//Utils
	void printRequest() const;
=======
		std::string	getMethod() const;
		std::string	getPath() const;
		std::string	getVersion() const;
		std::string	getBody() const;
		std::string getContentType() const;
		std::string getResponse() const;
		std::string getHost() const;
		std::string getServerName() const;
		const Server& getServer() const;
		int getPort() const;
		int getStatusCode() const;
		const std::map<std::string, std::string>& getFormDataName() const;
		const std::map<std::string, std::string>& getFormDataFilename() const;
		const std::map<std::string, std::string>& getJsonParam() const;
		const std::map<std::string, std::string>& getUrlParam() const;

		//------------SETTERS------------
		void setServer(Server& server);
		void setRequestStatusCode(int status_code);
		//Utils
		void printRequest() const;
>>>>>>> f8141b11ccc16231bcd39f52959af02fceffd1bb
};

bool	checkValidChar(char c);
// void	checkISS(char c1, char c2);
// bool	checkValidHeader(char c);
void	checkKey(std::string key);
void	checkValue(std::string value);


#endif