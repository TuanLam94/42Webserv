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
	_here = 0;
	_RequestMethod = "REQUEST_METHOD="; // methode utilise oui
	_ContentType = "CONTENT_TYPE="; // type de contenu de la requete oui
	_ContentLength = "CONTENT_LENGTH="; // taille du body de la requete oui
	_QueryString = "QUERY_STRING="; // param de lurl apres le '?'
	_ScriptName = "SCRIPT_NAME="; // chemin virtule vers le script execute ("/cgi-bin/script.py") oui
	_ServerName = "SERVER_NAME="; // nom de lhote ou adresse ip du serveur oui
	_ServerPort = "SERVER_PORT="; // numero du port sur lequel la requete est send
	_ServerProtocol = "SERVER_PROTOCOL="; // version http -> HTTP/1.1
	_GatewayInterface = "GATEWAY_INTERFACE="; // CGI/1.1 oui
	_PathInfo = "PATH_INFO"; // (falcultatif cf. apres)
	_RemoteAddr = "REMOTE_ADDR="; // adresse IP du client
	_RemoteHost = "REMOTE_HOST="; // nom d'hote du client
	_HttpHost = "HTTP_HOST="; // a re voir
	_HttpUserAgent = "HTTP_USER_AGENT="; // le navigateur utilise par le client oui
	_HttpAccept = "HTTP_ACCEPT="; // type de donnees MIME que le client accepete de recevoir oui
	_HttpAcceptLanguage = "HTTP_ACCEPT_LANGUAGE="; // langue dans laquelle le client accepte de recevoir oui
	_HttpAcceptEncoding = "HTTP_ACCEPT_ENCODING="; // definit le type de codage sur le contenu renvoyer au client oui
	_HttpReferer = "HTTP_REFERER="; // http://localhost:8080/index.html --> adresse absolu/partielle de la page web oui
	_HttpConnection = "HTTP_CONNECTION="; // Contient des informations sur l'état de la connexion HTTP entre le serveur et le navigateur appelant oui
	_RemoteUser = "REMOTE_USER=";
	_AuthType = "AUTH_TYPE=";
	_RedirectStatus = "REDIRECT_STATUS=";
	_HttpOrigin = "HTTP_ORIGIN="; // oui
}

bool	Request::checkValidCharRequest(char c)
{
	// if (!(c >= 48 && c <= 57)
	// 	&& !(c >= 65 && c <= 90)
	// 	&& !(c >= 97 && c <= 122)
	// 	&& c != 37 && c != 43 && c != 47 
	// 	&& c != 46 && c != 45 && c != 95
	// 	&& c != 126 && c != 61 && c != 63
	// 	&& c != 38)
	// {
	// 	_status_code = 400;
	// 	std::cerr << "checkValidChar Error 400: Bad Request.\n";
	// 	return (false);
	// }
	(void)c;
	return (true);
}

void	Request::parsRequestLine()
{
	size_t	i = 0;
	int	space = 0;

	_pos = findPosition("\r\n", _buffer, _pos);
	if (_pos != std::string::npos)
	{
		while (i < _buffer.size() && i < _pos)
		{
			if (_buffer[i] == 32)
				space++;
			else if (space == 0 && checkValidCharRequest(_buffer[i]) == true)
				_method += _buffer[i];
			else if (space == 1 && checkValidCharRequest(_buffer[i]) == true)
				_path += _buffer[i];
			else if (space == 2 && checkValidCharRequest(_buffer[i]) == true)
				_version += _buffer[i];
			// else if (checkStatusCode() == true)
			// 	return ;
			i++;
		}
		_pos += 2;
	}
	// std::cout << _method << std::endl;
	// std::cout << _path << std::endl;
	// std::cout << _version << std::endl;
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
		_status_code = 414;
		throw MyExcep();
	}
}

bool	Request::checkStatusCode()
{
	if (_status_code == 0)
		return (true);
	return (false);
}

void	Request::parsRequest()
{
	try
	{
		parsRequestLine();
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

void	Request::parsRequestBis(Server i)
{
	_max_client_body_size = i.getMaxBodySize();
	if (checkStatusCode() == false)
		return ;
	if (_method == "GET")
	{
		parsingGET(i);
		if (_cgiIsHere == true)
			fillCgiGet();
	}
	else if(_method == "POST")
	{
		parsingPOST_v1(i);
		if (_cgiIsHere == true)
			fillCgiPost();
	}
	else if (_method == "DELETE") {
		// std::cout << "Method is delete. Path is " << _path << std::endl;
		parsingDELETE(i);
		// std::cout << "still delete, path is " << _path << std::endl;
	}
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

size_t	fillLength(std::string buffer, size_t start)
{
	size_t res;
	std::string	str;

	while (start < buffer.size() && buffer[start] == 32)
		start++;
	while (buffer[start] >= 48 && buffer[start] <= 57)
	{
		str += buffer[start];
		start++;
	}
	std::istringstream	ss(str);
	ss >> res;
	return res;
}

// si content-length pas egal a body --> return error 400 bad request ou 500 internal errror timeout
// si pas de \r\n\r\n a la fin --> boucle infinie request never complete return error 400 bad request

bool Request::isRequestComplete()
{
	if (_buffer.empty() == true)
		return true;
	size_t headerEnd = _buffer.find("\r\n\r\n");
	if (headerEnd != std::string::npos) {
		size_t chunkedPos = _buffer.find("Transfer-Encoding: chunked");
		if (chunkedPos != std::string::npos)
			return isChunkedRequestComplete();
		else {
			size_t contentLengthPos = _buffer.find("Content-Length:");
			if (contentLengthPos != std::string::npos) {
				size_t contentLengthStart = contentLengthPos + strlen("Content-Length: ");
				contentLengthStart = fillLength(_buffer, contentLengthStart);
				// size_t i = headerEnd + 4;
				size_t j = 0;
				for (; j < _my_v.size(); j++);			
				// std::cout << j << std::endl;
				// std::cout << contentLengthStart << std::endl;
				if (j == contentLengthStart)
				{
					return (true);
				}
				else
				{
					return false;
				}
			}
			return true;
		}
	}
	return false;
}

bool Request::isChunkedRequestComplete()
{
	return (true);
	size_t pos = 0;
	while (pos < _my_v.size()) 
	{
		size_t chunkSizeEnd = findPositionVec("\r\n", pos);
		if (static_cast<int>(chunkSizeEnd) == -1)
			return false;

		// std::string chunkSizeStr = body.substr(pos, chunkSizeEnd - pos);
		std::string chunkSizeStr;
		while (pos < chunkSizeEnd)
		{
			chunkSizeStr += _my_v[pos];
			pos++;
		}
		int chunkSize = hexStringToInt(chunkSizeStr);

		if (chunkSize < 0)
			return false;

		if (chunkSize == 0)
		{
			size_t finalEnd = findPositionVec("\r\n", chunkSizeEnd + 2);
			return static_cast<int>(finalEnd) != -1;
		}
		size_t chunkEnd = chunkSizeEnd + 2 + chunkSize;
		if (chunkEnd > _my_v.size())
		{
        		return false;
		}

		pos = chunkEnd;
    	}
	return false;
}

// bool Request::isRequestComplete()
// {
//     // First check - empty buffer
//     if (_buffer.empty())
//         return false;

//     // Find end of headers
//     size_t headerEnd = _buffer.find("\r\n\r\n");
//     if (headerEnd == std::string::npos)
//         return false;  // Headers not complete yet

//     // Get the expected content size from headers
//     size_t expectedSize = 0;

//     // Check for chunked encoding first
//     size_t chunkedPos = _buffer.find("Transfer-Encoding: chunked");
//     if (chunkedPos != std::string::npos)
//     {
//         return isChunkedRequestComplete(_buffer.substr(headerEnd + 4));
//     }
//     // Check for Content-Length header
//     else 
//     {
//         size_t contentLengthPos = _buffer.find("Content-Length:");
//         if (contentLengthPos != std::string::npos)
//         {
//             size_t contentLengthStart = contentLengthPos + strlen("Content-Length: ");
//             expectedSize = fillLength(_buffer, contentLengthStart);

            
//             // Calculate actual body size
//             size_t bodySize = _buffer.size() - (headerEnd + 4);  // +4 for \r\n\r\n
            
//             // Debug output
//             std::cout << "Expected size: " << expectedSize << std::endl;
//             std::cout << "Current body size: " << bodySize << std::endl;
//         //     std::cout << "Expected size: " << expectedSize << std::endl;
//         //     std::cout << "Current body size: " << bodySize << std::endl;
            
//             return (bodySize >= expectedSize);
//         }
//         // No Content-Length header - typically means no body (GET requests, etc)
//         return true;
//     }
// }

// bool Request::isChunkedRequestComplete(const std::string& body) 
// {
//     size_t pos = 0;
//     size_t totalProcessed = 0;

//     while (pos < body.size()) 
//     {
//         // Find chunk size end
//         size_t chunkSizeEnd = body.find("\r\n", pos);
//         if (chunkSizeEnd == std::string::npos)
//             return false;

//         // Get chunk size in hex
//         std::string chunkSizeStr = body.substr(pos, chunkSizeEnd - pos);
//         int chunkSize = hexStringToInt(chunkSizeStr);
        
//         // Debug output
//         std::cout << "Processing chunk size: " << chunkSize << " (hex: " << chunkSizeStr << ")" << std::endl;

//         if (chunkSize < 0)
//             return false;

//         // Found last chunk (size 0)
//         if (chunkSize == 0) 
//         {
//             size_t finalEnd = body.find("\r\n", chunkSizeEnd + 2);
//             return (finalEnd != std::string::npos);
//         }

//         // Calculate where this chunk should end
//         size_t chunkEnd = chunkSizeEnd + 2 + chunkSize + 2;  // +2 for \r\n after size, +2 for \r\n after data
//         if (chunkEnd > body.size()) 
//         {
//             std::cout << "Chunk incomplete. Expected end: " << chunkEnd << ", Current size: " << body.size() << std::endl;
//             return false;
//         }

//         totalProcessed += chunkSize;
//         pos = chunkEnd;
//     }

//     return false;  // If we get here, we haven't found the terminating chunk
// }

bool Request::isBodySizeTooLarge()
{
	return _body.size() > static_cast<unsigned long>(_server.getMaxBodySize());
}

//-----------------------------GETTERS-----------------------------//

Request::Request(const Request& copy) 
{
    _client_fd = copy._client_fd;
    _cgiIsHere = copy._cgiIsHere;
    _cgiType = copy._cgiType;
    _RequestMethod = copy._RequestMethod;
    _ContentLength = copy._ContentLength;
    _ContentType = copy._ContentType;
    _QueryString = copy._QueryString;
    _ScriptName = copy._ScriptName;
    _ServerName = copy._ServerName;
    _ServerPort = copy._ServerPort;
    _ServerProtocol = copy._ServerProtocol;
    _GatewayInterface = copy._GatewayInterface;
    _PathInfo = copy._PathInfo;
    _RemoteAddr = copy._RemoteAddr;
    _RemoteHost = copy._RemoteHost;
    _HttpHost = copy._HttpHost;
    _HttpUserAgent = copy._HttpUserAgent;
    _HttpAccept = copy._HttpAccept;
    _HttpAcceptLanguage = copy._HttpAcceptLanguage;
    _HttpAcceptEncoding = copy._HttpAcceptEncoding;
    _HttpReferer = copy._HttpReferer;
    _HttpConnection = copy._HttpConnection;
    _RemoteUser = copy._RemoteUser;
    _AuthType = copy._AuthType;
    _RedirectStatus = copy._RedirectStatus;
    _HttpOrigin = copy._HttpOrigin;
    _HttpCookie = copy._HttpCookie;
    _method = copy._method;
    _path = copy._path;
    _version = copy._version;
    _response = copy._response;
    _body = copy._body;
    _contentType = copy._contentType;
    _host = copy._host;
    _serverName = copy._serverName;
    _port = copy._port;
    _server = copy._server;  // Assuming Server has a copy constructor
	// _input = copy._input;  // Caution: This may not behave as expected
    _status_code = copy._status_code;
    _contentLength = copy._contentLength;
    _max_client_body_size = copy._max_client_body_size;
    _pos = copy._pos;
    _headersHttp = copy._headersHttp;
    _queryParameter = copy._queryParameter;
    _FormDataName = copy._FormDataName;
    _FormDataFilename = copy._FormDataFilename;
    _jsonParam = copy._jsonParam;
    _urlParam = copy._urlParam;
    _boundary = copy._boundary;
    _dataBrut = copy._dataBrut;
    _isChunk = copy._isChunk;
    _buffer = copy._buffer;
    _my_v = copy._my_v;
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

std::string	Request::getBuffer() const
{
	return (_buffer);
}

std::string	Request::getBoundary() const
{
	return (_boundary_full);
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

const std::map<std::string, std::string>& Request::getFormDataFileName() const
{
	return (_FormDataFilename);
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

bool	Request::getIsCgiHere()const
{
	return (_cgiIsHere);
}

int Request::getClientFD() const
{
	return (_client_fd);
}

unsigned long int	Request::getContentLength() const
{
	return (_contentLength);
}

int	Request::getHere() const
{
	return (_here);
}

std::vector<unsigned char>	Request::getMyV() const
{
	return (_my_v);
}

// std::string Request::getRemotePort() const
// {
// 	return _RemotePort;
// }

//-----------------------------------------------------SETTERS--------------------------------------

void	Request::setStatusCode(int code)
{
	_status_code = code;
}

void	Request::setHere(int here)
{
	_here = here;
}

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

void Request::setClientFD(int fd)
{
	_client_fd = fd;
}