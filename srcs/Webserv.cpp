#include "../headers/Webserv.hpp"

int	checkUriSize(std::string buff)
{
	std::vector<unsigned char>::iterator it;
	std::vector<unsigned char>::iterator ite;
	std::vector<unsigned char>::iterator ite1;
	size_t	pos;

	pos = buff.find("\r\n");
	if (pos != std::string::npos)
	{
		for (size_t i = 0; i < pos; i++)
		{
			if (i > 2048)
			{
				std::cerr << "checkUriSize Error 414: Uri too long.\n";
				return (414);
			}
		}
	}
	else
	{
		for (size_t i = 0; i < buff.size(); i++)
		{
			if (i > 2048)
			{
				std::cerr << "checkUriSize Error 414: Uri too long.\n";
				return (414);
			}
		}
	}
	return (0);
}

int	checkHeadersSize(std::string buff)
{
	size_t	pos;
	size_t	pos1;
	size_t	pos2;

	pos = buff.find("\r\n");
	if (pos == std::string::npos)
		return (0);
	else
	{
		pos += 2;
		pos1 = buff.find("\r\n\r\n", pos);
		if (pos1 == std::string::npos)
		{
			for (size_t i = 0; i < buff.size(); i++)
			{
				pos2 = buff.find("\r\n", pos1);
				if (pos2 == std::string::npos)
					return (0);
				// {
				// 	int k = 0;
				// 	for (size_t j = pos; j < pos2; j++, k++)
				// 	{
				// 		std::cout << buff[k];
				// 		if (k > 2048)
				// 		{
				// 			std::cerr << "checkHeadersSize1 Error 431: Header Field Too Large.\n";
				// 			return (431);
				// 		}
				// 	}
				// }
				else
				{
					int i = 0;
					for (size_t j = pos; j < pos2; j++, i++)
					{
						if (i > 2048)
						{
							std::cerr << "checkHeadersSize2 Error 431: Header Field Too Large.\n";
							return (431);
						}
					}
				}
				if (i > 8192)
				{
					std::cerr << "checkHeadersSize3 Error 431: Header Field Too Large.\n";
					return (431);
				}
				pos2 += 2;
				pos = pos2;
			}
		}
		else
		{
			for (size_t i = 0; i < pos1; i++)
			{
				pos2 = buff.find("\r\n", pos1);
				if (pos2 == std::string::npos)
					return (0);
				else
				{
					int i = 0;
					for (size_t j = pos; j < pos2; j++, i++)
					{
						if (i > 2048)
						{
							std::cerr << "checkHeadersSize4 Error 431: Header Field Too Large.\n";
							return (431);
						}
					}
				}
				if (i > 8192)
				{
					std::cerr << "checkHeadersSize5 Error 431: Header Field Too Large.\n";
					return (431);
				}
				pos2 += 2;
				pos = pos2;
			}
		}
	}
	return (0);
}

int	checkBodySize(Request request)
{
	size_t	pos;
	std::string	buff;

	buff = request.getBuffer();
	pos = buff.find("\r\n\r\n");
	if (pos == std::string::npos)
		return (0);
	else
	{
		pos += 4;
		for (size_t i = pos; i < buff.size(); i++)
		{
			if (static_cast<int>(i) > request.getMaxBodySize())
			{
				// std::cout << i << std::endl;
				// std::cout << request.getMaxBodySize() << std::endl;
				std::cerr << "checkBodySize Error 413: Payload Too Large.\n";
				return (413);
			}
		}
	}
	return (0);
}

int	checkContentLengthSize(std::string buff)
{
	size_t	pos;
	std::string	content = "Content-Length: ";
	std::string	nbr;

	pos = buff.find(content);
	if (pos == std::string::npos)
		return (0);
	else
	{
		pos += content.size();
		size_t	pos1 = buff.find("\r\n", pos);
		if (pos1 == std::string::npos)
			return (0);
		else
		{
			for (size_t i = pos; i < pos1; i++)
			{
				if (buff[i] >= 48 && buff[i] <= 57)
				{
					nbr += buff[i];
				}
			}
			if (nbr.empty() == false)
			{
				unsigned int	integer;
				std::stringstream	ss(nbr);
				ss >> integer;

				if (integer > 10485760)
				{
					std::cerr << "checkContentLengthSize Error 413: Payload Too Large.\n";
					return (413);
				}
			}
		}
	}
	return (0);
}

Webserv::Webserv(std::string config)
{
	std::ifstream input(config.c_str());
	if (!input.is_open()) {
		std::cerr << "Can't open " << config << std::endl;
		exit (-1);
	}

	parseConfigFile(input);
	serversInit();
}

void Webserv::parseConfigFile(std::ifstream& input)
{
	std::vector<std::string> configVec;
	std::string line;

	    while (std::getline(input, line)) {
        std::string newConfig;
        if (trim(line) == "server {") {
            while (std::getline(input, line) && trim(line) != "}") {
                newConfig += line + "\n";
            }
        }
        if (!newConfig.empty())
        configVec.push_back(newConfig);
    }

	for (size_t i = 0; i < configVec.size(); i++) {
		Server server(configVec[i]);
		_servers.push_back(server);
	}

	if (_servers.size() < 1) {
		std::cerr << "Error, invalid config file.\n";
		exit (1);
	}
}

void Webserv::serversInit()
{
	for (size_t i = 0; i < _servers.size(); i++) {
		_servers[i].initAll();
	}
}

void Webserv::run()
{
	epollInit();
	eventLoop();
}

void Webserv::epollInit()
{
	_epoll_fd = epoll_create(1);
	if (_epoll_fd < 0) {
		std::cerr << "epoll created failed\n";
		exit (-1);
	}

	_events.resize(_servers.size());

	for (size_t i = 0; i < _servers.size(); i++) {
		_servers[i].setEvent(_events[i]);
		_servers[i].epollInit(_epoll_fd);
	}
}

void Webserv::eventLoop() {
	int maxEvents = 10;// originally 10
	_events.resize(maxEvents);
	while (true) {
		int fd_number = epoll_wait(_epoll_fd, _events.data(), maxEvents, _servers[0].getTimeout());
		if (fd_number < 0) {
			std::cerr << "epoll_wait failed\n";
			exit (-1);
		}

		for (int i = 0; i < fd_number; i++) {
			int event_fd = _events[i].data.fd;

			bool isServerSocket = false;
			for (size_t j = 0; j < _servers.size(); j++) {
				if (_servers[j].getServerFd() == event_fd) {
					_servers[j].handleNewConnection();
					isServerSocket = true;
					break;
				}
			}
			// std::cout << "_requests size in loop == " << _requests.size() << std::endl;
			if (!isServerSocket) {
				if (_events[i].events & EPOLLIN) {
					Request* request = findAppropriateRequest(event_fd);
					handleClientRequest(event_fd, *request);
				}
				if (_events[i].events & EPOLLOUT) {
					Request* request = findAppropriateRequestToWrite(event_fd);
					if (request != NULL && request->isRequestComplete() && !request->getBuffer().empty()) {
						handleClientWrite(event_fd, *request);
						removeRequest(event_fd);
					}
				}
			}
		}
	}
}

Request* Webserv::findAppropriateRequest(int event_fd)
{
	// std::cout << "EVENT FD = " << event_fd << std::endl;
	// std::cout << "_requests.size() before == " << _requests.size() << std::endl;

	for (size_t i = 0; i < _requests.size(); i++) {
		// std::cout << "request[" << i << "]._client_fd = " << _requests[i].getClientFD() << std::endl;
		if (_requests[i].getClientFD() == event_fd) {
			// std::cout << "FOUND EXISTING REQUEST TO READ\n";
			return &_requests[i];
		}
	}
	// std::cout << "CREATING NEW REQUEST\n";
    	_requests.push_back(Request());
   	 _requests.back().setClientFD(event_fd);
	// std::cout << "_requests.size() after == " << _requests.size() << std::endl;
    	return &_requests.back();
}

Request* Webserv::findAppropriateRequestToWrite(int event_fd)
{
	for (size_t i = 0; i < _requests.size(); i++) {
		if (_requests[i].getClientFD() == event_fd) {
			// std::cout << "FOUND EXISTING REQUEST TO WRITE\n";
			return &_requests[i];
		}
	}
	return NULL;
}

void Webserv::handleClientWrite(int event_fd, Request& request)
{
	Response response(request);
	response.handleRequest();
	response.buildResponse();
	std::cout << "\n\n\nRESPONSE CONTENT TYPE == " << response.getContentType() << "\n\n";
	std::cout << "\nFULL RESPONSE = " << response.getResponseStr() << std::endl;
	response.sendResponse(event_fd);
	// std::vector<unsigned char> test = request.getMyV();
	// std::string str;
    	// std::cout << test.size() << std::endl;
	// exit (1);
}

void Webserv::removeRequest(int event_fd)
{
    for (std::vector<Request>::iterator it = _requests.begin(); it != _requests.end(); ++it) {
        if (it->getClientFD() == event_fd) {
            _requests.erase(it);
			// std::cout << "REMOVED REQUEST\n";
            break;
        }
    }
}

int	checkAllSize(Request request)
{
	std::string	buff;

	buff = request.getBuffer();

	if (checkUriSize(buff) == 414)
		return (414);
	if (checkHeadersSize(buff) == 431)
		return (431);
	if (checkBodySize(request) == 413) // revoir le body size pour avoir le bon
		return (413);
	if (checkContentLengthSize(buff) == 413)
		return (413);
	return (0);
}

// echo -ne "POST /submit HTTP/1.1\r\nHost: localhost\r\nContent-Type: text/plain\r\nConnection:close\r\n\r\n" | nc localhost 8080


int	findSubStr(unsigned char buffer[1024], const char *str)
{
	int	i = 0;
	
	while (buffer[i + 3] != '\0')
	{
		if (buffer[i] == str[0]
			&& buffer[i + 1] == str[1]
			&& buffer[i + 2] == str[2]
			&& buffer[i + 3] == str[3])
		{
			return (i);
		}
		i++;
	}
	return (-1);
}

void	Request::createData(unsigned char buffer[1024], int bytes)
{
	int pos = findSubStr(buffer, "\r\n\r\n");

	if (pos != -1 && _here == 0)
	{
		pos += 4;
		_here = 1;
		for (int i = 0; i < pos; i++)
		{
			_buffer += buffer[i];
			// std::cout << _buffer[i];
		}
		if (pos < bytes)
		{
			for (; pos < bytes; pos++)
			{
				// if (buffer[pos] >= 0 && buffer[pos] <= 127)
					// std::cout << buffer[pos];
				_my_v.push_back(buffer[pos]);
			}
		}
	}
	else if (_here > 0)
	{
		for (int i = 0; i < bytes; i++)
		{
			// if (buffer[i] >= 0 && buffer[i] <= 127)
				// std::cout << buffer[i];
			_my_v.push_back(buffer[i]);
		}
	}
	else
	{
		_buffer += std::string(reinterpret_cast<char*>(buffer));
	}
}

void	Webserv::handleClientRequest(int client_fd, Request& request)
{
	unsigned char buffer[1024] = {0};

	int bytes = recv(client_fd, buffer, sizeof(buffer), 0);
	if (bytes <= 0) {
		close(client_fd);
		epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
		return;
	}
	request.createData(buffer, bytes);

	request.setStatusCode(checkAllSize(request)); // heeeeeeeeeeeeeeeeeeeeeeeere
	if (request.getStatusCode() != 0) // send error Response
		return ;

	if (request.isRequestComplete()) {
		std::cout << "COMPLETE BUFFER = \n" << request.getBuffer() << "\n\n";
		// std::cout << "COMPLETE VECTOR = \n";
		// exit (1);
		request.setHere(0);
		request.parsRequest();		//PATH IS HERE
		request.getClientIPPort(client_fd);

		Server* correct_server = findAppropriateServer(request);

			// std::cout << request.getContentType() << std::endl;
			// exit (1);
		if (correct_server != NULL) {
			request.setServer(*correct_server);
			request.parsRequestBis(*correct_server);
			// std::cout << "request succesfully parsed!\n";
			// std::cout << "Request Path is " << request.getPath() << std::endl;
			// std::cout << "\nrequest parsed\n";
			if (request.isBodySizeTooLarge()) {
				request.setRequestStatusCode(413);
				return ;
			}
			// std::cout << "request path = " << request.getPath() << std::endl;
		}
		else
			std::cout << "500 Internal Server Error\n";
			// sendServerErrorResponse(client_fd); //tocode
	}
}

Server* Webserv::findAppropriateServer(Request& request)
{
	int count = 0;


	for (size_t i = 0; i < _servers.size(); i++) {
		if (_servers[i].getHost() == request.getHost())
			count++;
	}

	if (count == 1) {
		for (size_t i = 0; i < _servers.size(); i++) {
			if (_servers[i].getHost() == request.getHost()) {
				if (_servers[i].getPort() == request.getPort())
					return &_servers[i];
				else
					return (redirectServer(request));
			}
		}
	}
	else {
		if (request.getPort() != 0)
			return findServerByPort(request);
		else
			return (findServerByName(request));
	}
	return NULL;
}

Server* Webserv::redirectServer(Request& request)
{
	request.setRequestStatusCode(301);
	return findServerByPort(request);
}

Server* Webserv::findServerByName(const Request& request)
{
	for (size_t i = 0; i < _servers.size(); i++) {
		if (_servers[i].getServerName() == request.getServerName())
			return &_servers[i];
	}
	return NULL;
}

Server* Webserv::findServerByPort(const Request& request)
{
	for (size_t i = 0; i < _servers.size(); i++) {
		if (_servers[i].getPort() == request.getPort())
			return &_servers[i];
	}
	return NULL;
}

Webserv::~Webserv()
{
	close(_epoll_fd);
	// for (size_t i = 0; i < _servers.size(); i++) {
	// 	_servers[i].closeServer(); //tocode
	// }
}

//-------------------------------------Getters-------------------------------------

int Webserv::getServerFd()
{
	return _server_fd;
}

int Webserv::getEpollFd()
{
	return _epoll_fd;
}

std::vector<Server> Webserv::getServers()
{
	return _servers;
}