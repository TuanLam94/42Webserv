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
#include <string.h>
#include <unistd.h>
#include <algorithm>
#include <iterator>
// #include "response.hpp"
#include "../headers/server.hpp"
#include <sstream>
#include <ctime>

class Server;
class Response;

class MyExcep : public std::exception
{
	const char *what()const throw()
	{
		return("ErrorException.\n");
	}
};

class	Request
{
	private:
	int		_client_fd;
	bool	_cgiIsHere;
	bool	_isRedirect;
	long long int _start;
	std::string	_listing;
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
	std::string	_HttpUserAgent;
	std::string	_HttpAccept; // yes
	std::string	_HttpAcceptLanguage; // yes
	std::string	_HttpAcceptEncoding; // yes
	std::string	_HttpReferer;
	std::string	_HttpConnection;
	std::string	_RemoteUser;
	std::string	_AuthType;
	std::string	_RedirectStatus;
	std::string	_HttpOrigin;
	std::string	_HttpCookie;
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
	size_t	_pos; // garde le fil de la postion de \r\n
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
	std::string	_boundary_full;
	std::string	_dataBrut; // requete POST avec content-type -> text/plain
	bool	_isChunk; // pour verifier si requete fragmente
	std::vector<unsigned char> _my_v;
	std::vector<unsigned char> _my_body;
	// std::string _buffer;
	int	_isComplete;
	public:
	// std::vector<std::vector<unsigned char> >	_my_v;x
	Request();
	Request(const Request& copy);
	Request& operator=(const Request& other);
	~Request() {};
	void	parsRequest();
	void	parsRequestBis(Server i);
	void	parsRequestLine();
	void	checkMethod();
	void	checkVersion();
	void	checkUri();
	bool	checkStatusCode();
	void	checkHeaderName();
	void	parsParamPath(size_t pos);
	std::string	parsParamPath_bis(std::string str);
	void	parsPath(Server obj);
	std::string parsRedirectPath(Server& obj);
	void	parsHeaders();
	void	parsingGET(Server i);
	void	parsingPOST_v1(Server i);
	void	parsingPOST_v2();
	void	parsingDELETE(Server i);
	void	parserJson();
	int	parserJsonBis(size_t pos_start, size_t pos_comma);
	bool	checkFirstAccolade(size_t pos);
	bool	checkLastAccolade(size_t pos);
	void	checkJsonAccolade();
	void	parserUrlencoded();
	void	parserUrlencoded_bis(std::string new_body);
	void	parserFormData();
	void	parserFormData_bis(size_t pos);
	void	formDataGetName(size_t pos);
	void	formDataGetFilename(size_t pos);
	void	parserTextPlain();
	void	fillVar();
	int		checkContentType();
	void	getClientIPPort(int clientfd);
	std::string parsServerName();
	bool	parserFormData_help(unsigned long int i);
	bool	isRequestComplete();
	bool	isChunkedRequestComplete(size_t pos);
	bool	isRequestCompleteBis(unsigned char buffer[1024]);
	void	checkISS(char c1, char c2);
	bool	checkValidHeader(char c);
	bool	checkValidHeaderValue(char c);
	bool	checkValidChar(char c);
	bool	checkValidCharRequest(char c);
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
	std::string	fillCgiToUpper(std::string fillvar, std::string var);
	void	fillUserAgent();
	size_t	findPosition(std::string str, const std::string& buff, size_t start);
	size_t	findPositionVec(std::string str, size_t start);
	size_t	findPositionBody(std::string str, size_t start);
	std::string	helpHeaderHost(std::string value, std::string line);
	void	fillBody();
	bool	isBodySizeTooLarge();
	int	checkIsDigit(size_t pos_start);
	void	setBoundaryFull();
	void	createData(unsigned char buffer[1024], int bytes);
	bool	checkIfNext(size_t i);
	std::string	constructBoundary();
	void	makeClear();
	int	checkUriSize();
	int	checkHeadersSize();
	int	checkBodySize();
	int	checkContentLengthSize();
	int	checkUrlEncoded();
	void	listing(DIR *dir);
	// --------- GETTERS -------------
	std::vector<unsigned char>	getMyBodyV() const;
	std::string	getBoundary() const;
	std::string	getMethod() const;
	std::string	getPath() const;
	std::string	getVersion() const;
	std::string	getBody() const;
	std::string	getContentType() const;
	std::string	getResponse() const;
	std::string	getHost() const;
	std::string	getServerName() const;
	std::string	getBuffer() const;
	bool	getIsChunk() const;
	unsigned long int	getContentLength() const;
	int getMaxBodySize() const;
	const Server&	getServer() const;
	int getPort() const;
	int getStatusCode() const;
	const std::map<std::string, std::string>& getFormDataName() const;
	const std::map<std::string, std::string>& getFormDataFileName() const;
	const std::map<std::string, std::string>& getJsonParam() const;
	const std::map<std::string, std::string>& getUrlParam() const;
	bool	isCgi() const;
	std::string getQueryString() const;
	std::string getPathInfo() const;
	std::string getScriptName() const;
	std::string getServerPort() const;
	std::string getRemoteAddr() const;
	std::string getRemotePort() const;
	bool	getIsCgiHere()const;
	bool	getIsRedirect() const;
	int	getClientFD() const;
	int	getComplete() const;
	std::vector<unsigned char>	getMyV() const;
	long long int getStart() const;
	//------------SETTERS------------
	void	setComplete(int complete);
	void	setStatusCode(int code);
	void	setServer(Server& server);
	void	setRequestStatusCode(int status_code);
	void	setClientFD(int fd);
	void	setStart(long long int time);
	//Utils
	void	printRequest() const;


	bool	isRequestComplete(std::string buff);
	void	constructBody();
};

bool	isDigit(char str);
bool	checkValidChar(char c);
// void	checkISS(char c1, char c2);
// bool	checkValidHeader(char c);
void	checkKey(std::string key);
void	checkValue(std::string value);
int	checkUrlEncoded(std::string body);
std::string	fillScriptName(std::string path);
std::string	fillServerPort(int port);

#endif