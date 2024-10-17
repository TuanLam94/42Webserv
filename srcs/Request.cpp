#include "../headers/request.hpp"

/*
	Revoir que thom a dit sur le traitement du chemin
		- pas a moi de faire les verifs ni de traiter les erreurs --> se focus sur les erreurs de parsing
*/

// est ce que le path de len tete de la requete est un chemin absolu --> quel chemin absolu prendre
// 


Request::Request()
{
	_cgiIsHere = false;
	_isChunk = false;
	_status_code = 0;
	_RequestMethod = "REQUEST_METHOD=";
	_ContentType = "CONTENT_TYPE=";
	_ContentLength = "CONTENT_LENGTH=";
	_QueryString = "QUERY_STRING=";
	_ScriptName = "SCRIPT_NAME=";
	_ServerName = "SERVER_NAME=";
	_ServerPort = "SERVER_PORT=";
	_ServerProtocol = "SERVER_PROTOCOL=";
	_GatewayInterface = "GATEWAY_INTERFACE=";
	_PathInfo = "PATH_INFO";
	_RemoteAddr = "REMOTE_ADDR=";
	_RemoteHost = "REMOTE_HOST=";
	_HttpHost = "HTTP_HOST=";
	_HttpUserAgent = "HTTP_USER_AGENT=";
	_HttpAccept = "HTTP_ACCEPT=";
	_HttpAcceptLanguage = "HTTP_ACCEPT_LANGUAGE=";
	_HttpAcceptEncoding = "HTTP_ACCEPT_ENCODING=";
	_HttpReferer = "HTTP_REFERER=";
	_HttpConnection = "HTTP_CONNECTION=";
	_RemoteUser = "REMOTE_USER=";
	_AuthType = "AUTH_TYPE=";
	_RedirectStatus = "REDIRECT_STATUS=";
}

void	Request::parsRequestLine(std::string buff)
{
	unsigned long int	i = 0;
	int	space = 0;

	while (i < buff.size() && buff[i] != 13 && buff[i + 1] != 10)
	{
		if (buff[i] == 32)
			space++;
		else if (space == 0 && checkValidChar(buff[i]) == true)
			_method += buff[i];
		else if (space == 1 && checkValidChar(buff[i]) == true)
			_path += buff[i];
		else if (space == 2 && checkValidChar(buff[i]) == true)
			_version += buff[i];
		i++;
	}
	if (space != 2
		|| _method.empty() == true
		|| _path.empty() == true
		|| _version.empty() == true)
	{
		_status_code = 400;
	}
}

void	Request::checkMethod()
{
	if (_method != "GET" 
		&& _method != "POST"
		&& _method != "DELETE")
			_status_code = 405;
}

void	Request::checkVersion()
{
	if (_version != "HTTP/1.1")
		_status_code = 505;
}

void	Request::checkCgi()
{
	size_t	pos;

	pos = _path.find("cgi-bin");
	if (pos != std::string::npos)
		_cgiIsHere = true;
}

/*
	- parfois certans bugs --> message non recu dans sa totalite 
		--> verifier le retour de recv et etre sur que tout la chaine a ete transmise
*/


void	Request::parsRequest(const std::string& buffer)
{
	size_t	pos;
	// std::cout << buffer << std::endl;
	// exit (1);
	parsRequestLine(buffer);
	checkMethod();
	checkVersion();
	checkCgi();
	pos = buffer.find("Transfer-Encoding: chunked");
	if (pos != std::string::npos)
	{
		// gerer les requete fragementes + test/plain
		// std::cout << buffer << std::endl;
	}
}

void Request::parsRequestBis(Server i, const std::string& buffer)
{
	std::stringstream ss(buffer);
	std::cout << "\n\n\n" << buffer << "\n\n\n"; 
	// exit (1);
	_max_client_body_size = i.getMaxBodySize();
	if (_method == "GET")
	{
		parsingGET(i, buffer);
		if (_cgiIsHere == true)
			fillCgiGet();
	}
	else if(_method == "POST")
	{
		parsingPOST(i, buffer);
		if (_cgiIsHere == true)
			fillCgiPost();
	}
	// else if (_method == "DELETE")
	// {
	// 	parsingDELETE(i, buffer);
	// 	// if (_cgiIsHere == true)
	// 	// 	fillCgiDelete();
	// }
}

void	Request::getClientIPPort(int clientfd)
{
	struct sockaddr_in local_addr;
	socklen_t addr_len = sizeof(local_addr);

   	if (getsockname(clientfd, (struct sockaddr*)&local_addr, &addr_len) == -1) {
        std::cerr << "Failed to get server address\n";
        return;
    }

	_host = inet_ntoa(local_addr.sin_addr);
	_port = ntohs(local_addr.sin_port);

	std::ostringstream oss;
	oss << _host << ":" << _port;
	_host = oss.str();
}

//-----------------------------GETTERS-----------------------------//

Request::Request(const Request& copy)
{
	*this = copy;
}

Request& Request::operator=(const Request& other)
{
	if (this != &other) {
		_method = other.getMethod();
		_path = other.getPath();
		_version = other.getVersion();
		_response = other.getResponse();
		_body = other.getBody();
		_contentType = other.getContentType();
	}
	return *this;
}

std::string	Request::getMethod() const
{
	return (_method);
}

std::string	Request::getPath() const
{
	return (_path);
}

std::string	Request::getVersion() const
{
	return (_version);
}

std::string Request::getBody() const
{
	return (_body);
}

std::string Request::getContentType() const
{
	return (_contentType);
}

std::string Request::getResponse() const
{
	return (_response);
}


std::string Request::getHost() const
{
	return (_host);
}

std::string Request::getServerName() const
{
	return (_serverName);
}

int Request::getStatusCode() const
{
	return (_status_code);
}

int Request::getPort() const
{
	return (_port);
}

const Server& Request::getServer() const
{
	return (_server);
}

const std::map<std::string, std::string>& Request::getFormDataName() const
{
	return (_FormDataName);
}

const std::map<std::string, std::string>& Request::getJsonParam() const
{
	return (_jsonParam);
}

const std::map<std::string, std::string>& Request::getUrlParam() const
{
	return (_urlParam);
}

int Request::getMaxBodySize() const
{
	return (_max_client_body_size);
}

bool Request::isCgi() const
{
	return (_cgiIsHere);
}

std::string Request::getQueryString() const
{
	return (_QueryString);
}

std::string Request::getPathInfo() const
{
	return _PathInfo;
}

std::string Request::getScriptName() const
{
	return _ScriptName;
}

std::string Request::getServerPort() const
{
	return _ServerPort;
}

std::string Request::getRemoteAddr() const
{
	return _RemoteAddr;
}

// std::string Request::getRemotePort() const
// {
// 	return _RemotePort;
// }

//-----------------------------------------------------SETTERS--------------------------------------

void Request::setServer(Server& server)
{
	_server = server;
}

void Request::setRequestStatusCode(int status_code)
{
	_status_code = status_code;
}

void Request::printRequest() const
{
    std::cout << "----REQUEST----" << std::endl;
    std::cout << "Method = " << getMethod() << std::endl;
    std::cout << "Path = " << getPath() << std::endl;
    std::cout << "Version = " << getVersion() << std::endl;
	std::cout << "Response = " << getResponse() << std::endl;
	std::cout << "Body = " << getBody() << std::endl;
	std::cout << "Content Type = " << getContentType() << std::endl;
}