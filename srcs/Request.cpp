#include "../headers/request.hpp"
#include "../headers/utils.hpp"

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
	_pos = 0;
	_RequestMethod = "REQUEST_METHOD="; // methode utilise
	_ContentType = "CONTENT_TYPE="; // type de contenu de la requete
	_ContentLength = "CONTENT_LENGTH="; // taille du body de la requete
	_QueryString = "QUERY_STRING="; // param de lurl apres le '?'
	_ScriptName = "SCRIPT_NAME="; // chemin virtule vers le script execute ("/cgi-bin/script.py")
	_ServerName = "SERVER_NAME="; // nom de lhote ou adresse ip du serveur 
	_ServerPort = "SERVER_PORT="; // numero du port sur lequel la requete est send
	_ServerProtocol = "SERVER_PROTOCOL="; // version http -> HTTP/1.1
	_GatewayInterface = "GATEWAY_INTERFACE="; // CGI/1.1
	_PathInfo = "PATH_INFO"; // (falcultatif cf. apres)
	_RemoteAddr = "REMOTE_ADDR="; // adresse IP du client
	_RemoteHost = "REMOTE_HOST="; // nom d'hote du client
	_HttpHost = "HTTP_HOST="; // a re voir
	_HttpUserAgent = "HTTP_USER_AGENT="; // le navigateur utilise par le client
	_HttpAccept = "HTTP_ACCEPT="; // type de donnees MIME que le client accepete de recevoir
	_HttpAcceptLanguage = "HTTP_ACCEPT_LANGUAGE="; // langue dans laquelle le client accepte de recevoir
	_HttpAcceptEncoding = "HTTP_ACCEPT_ENCODING="; // definit le type de codage sur le contenu renvoyer au client
	_HttpReferer = "HTTP_REFERER="; // http://localhost:8080/index.html --> adresse absolu/partielle de la page web
	_HttpConnection = "HTTP_CONNECTION="; // Contient des informations sur l'état de la connexion HTTP entre le serveur et le navigateur appelant
	_RemoteUser = "REMOTE_USER=";
	_AuthType = "AUTH_TYPE=";
	_RedirectStatus = "REDIRECT_STATUS=";
	_HttpOrigin = "HTTP_ORIGIN=";
}

void	Request::parsRequestLine(std::string buff)
{
	size_t	i = 0;
	int	space = 0;

	_pos = findPosition("\r\n", buff, _pos);
	if (_pos != std::string::npos)
	{
		while (i < buff.size() && i < _pos)
		{
			if (buff[i] == 32)
				space++;
			else if (space == 0 && checkValidChar(buff[i]) == true)
				_method += buff[i];
			else if (space == 1 && checkValidChar(buff[i]) == true)
				_path += buff[i];
			else if (space == 2 && checkValidChar(buff[i]) == true)
				_version += buff[i];
			// else if (checkStatusCode() == true)
			// 	return ;
			i++;
		}
		_pos += 2;
	}
	if (space != 2
		|| _method.empty() == true
		|| _path.empty() == true
		|| _version.empty() == true)
	{
		_status_code = 400;
		std::cerr << "parsRequestLine Error 400: Bad Request.\n";
		throw MyExcep();
	}
}

void	Request::checkMethod()
{
	if (_method != "GET" 
		&& _method != "POST"
		&& _method != "DELETE")
		{
			std::cerr << "checkMethod Error 405: Method Not Allowed.\n";
			_status_code = 405;
			throw MyExcep();
		}
}

void	Request::checkVersion()
{
	if (_version != "HTTP/1.1")
	{
		std::cerr << "checkVersion Error 505: Version Not Supported.\n";
		_status_code = 505;
		throw MyExcep();
	}
}

void	Request::checkCgi()
{
	size_t	pos;

	pos = _path.find("cgi-bin");
	if (pos != std::string::npos)
		_cgiIsHere = true;
}

void	Request::checkUri()
{
	if (_path.size() > 2048)
	{
		std::cerr << "checkUri Error 414: Uri Too Long.\n";
		_status_code = 505;
		throw MyExcep();
	}
}

bool	Request::checkStatusCode()
{
	if (_status_code == 0)
		return (true);
	return (false);
}

void	Request::parsRequest(const std::string& buffer)
{
	std::cout << buffer << std::endl;

	try
	{
		parsRequestLine(buffer);
		checkMethod();
		checkVersion();
		checkUri();
		checkCgi();
	}
	catch(std::exception &ex)
	{
		return ;
	}
}

void	Request::parsRequestBis(Server i, const std::string& buffer)
{
	_max_client_body_size = i.getMaxBodySize();
	
	if (checkStatusCode() == false)
		return ;
	if (_method == "GET")
	{
		parsingGET(i, buffer);
		if (_cgiIsHere == true)
			fillCgiGet();
	}
	else if(_method == "POST")
	{
		parsingPOST_v1(i, buffer);
		if (_cgiIsHere == true)
			fillCgiPost();
	}
	else if (_method == "DELETE")
		parsingDELETE(i, buffer);
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

bool Request::isRequestComplete()
{
	size_t headerEnd = _buffer.find("\r\n\r\n");
	if (headerEnd != std::string::npos) {
		size_t chunkedPos = _buffer.find("Transfer-Encoding: chunked");
		if (chunkedPos != std::string::npos)
			return isChunkedRequestComplete(_buffer.substr(headerEnd + 4));

		size_t contentLengthPos = _buffer.find("Content-Length:");
		if (contentLengthPos != std::string::npos) {
			size_t contentLengthStart = contentLengthPos + strlen("Content-Length: ");
			int contentLength = hexStringToInt(_buffer.substr(contentLengthStart));

			size_t totalSize = headerEnd + 4 + contentLength;
			return _buffer.size() >= totalSize;
		}
		return true;
	}
	return false;
}

bool Request::isChunkedRequestComplete(const std::string& body) {
    size_t pos = 0;

    while (pos < body.size()) {
        size_t chunkSizeEnd = body.find("\r\n", pos);
        if (chunkSizeEnd == std::string::npos) return false;

        std::string chunkSizeStr = body.substr(pos, chunkSizeEnd - pos);
        int chunkSize = hexStringToInt(chunkSizeStr);

        if (chunkSize < 0)
            return false;

        if (chunkSize == 0) {
            size_t finalEnd = body.find("\r\n", chunkSizeEnd + 2);
            return finalEnd != std::string::npos;
        }

        size_t chunkEnd = chunkSizeEnd + 2 + chunkSize;
        if (chunkEnd > body.size()) {
            return false;
        }

        pos = chunkEnd;
    }

    return false;
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
		_status_code = other.getStatusCode();
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