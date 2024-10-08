#include "../headers/request.hpp"

/*
	Revoir que thom a dit sur le traitement du chemin
		- pas a moi de faire les verifs ni de traiter les erreurs --> se focus sur les erreurs de parsing
*/

// est ce que le path de len tete de la requete est un chemin absolu --> quel chemin absolu prendre
// 

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
		|| _version.empty() == true) // URI mal formule --> ex : GET /index.html URI HTTP/1.1 ou ex : GET HTTP/1.1
	{
		std::cerr << "parsRequestLine Error 400: Bad request\n";
		exit (1);
	}
	// std::cout << _method << std::endl;
	// std::cout << _path << std::endl;
	// std::cout << _version << std::endl;
}

void	Request::checkMethod()
{
	if (_method != "GET" 
		&& _method != "POST"
		&& _method != "DELETE"
		&& _method != "PUT")
		{
			std::cerr << "Error 405: Method Not Allowed\n";
			exit (1); 
		}
}

void	Request::checkVersion()
{
	if (_version != "HTTP/1.1")
	{
		std::cerr << "Error 505: HTTP Version Not Supported\n";
		exit (1);
	}
}

/*
	- parfois certans bugs --> message non recu dans sa totalite 
		--> verifier le retour de recv et etre sur que tout la chaine a ete transmise
*/

void	Request::parsRequest(Server i, const std::string& buffer)
{
	std::stringstream ss(buffer);
	std::cout << "\n\n\n" << buffer << "\n\n\n";

	parsRequestLine(buffer);
	checkMethod();
	checkVersion();
	// std::cout << _method << std::endl;
	// std::cout << _path << std::endl;
	// std::cout << _version << std::endl;
	if (_method == "GET")
		parsingGET(i, buffer);
	else if(_method == "POST")
		parsingPOST(i, buffer);
	// std::cout << _port << std::endl;
	// std::cout << _host << std::endl;
	// else if (_method == "DELETE")
		// parsingDELETE();
	
	// std::cout << _path << std::endl;
	// std::map<std::string, std::string>::iterator it;
	// std::map<std::string, std::string>::iterator ite;

	// it = _queryParameter.begin();
	// ite = _queryParameter.end();
	// while (it != ite)
	// {
	// 	std::cout << it->first << std::endl;
	// 	std::cout << it->second << std::endl;
	// 	it++;
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
	int port = ntohs(local_addr.sin_port);

	std::ostringstream oss;
	oss << _host << ":" << port;
	_host = oss.str();
	std::cout << "HOST = " << _host << std::endl;
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