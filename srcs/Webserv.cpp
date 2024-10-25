#include "../headers/Webserv.hpp"

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
			if (!isServerSocket) {
				if (_events[i].events & EPOLLIN) {
					Request* request = findAppropriateRequest(event_fd);
					handleClientRequest(event_fd, *request);
				}
				if (_events[i].events & EPOLLOUT) {
					Request* request = findAppropriateRequestToWrite(event_fd);
					if (request != NULL /*&& request->isRequestComplete() && !request._buffer.empty()*/) {
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
	for (size_t i = 0; i < _requests.size(); i++) {
		if (_requests[i].getClientFD() == event_fd) {
			// std::cout << "FOUND EXISTING REQUEST TO READ\n";
			return &_requests[i];
		}
	}
	// std::cout << "CREATING NEW REQUEST\n";
    _requests.push_back(Request());
    _requests.back().setClientFD(event_fd);
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
	if (request.isRequestComplete()) {
		Response response(request);
		response.handleRequest();
		response.sendResponse(event_fd);
	}
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

void Webserv::handleClientRequest(int client_fd, Request& request)
{
	char buffer[1024] = {0};

    int bytes = recv(client_fd, buffer, sizeof(buffer), 0);
    if (bytes <= 0) {
        close(client_fd);
        epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
        return;
    }
	// buffer[bytes] = 0;
	// std::cout << "BUFFER = " << buffer << "\n\n";
	request._buffer += std::string(buffer);
	// std::cout << "INCOMPLETE BUFFER = " << request._buffer << "\n\n";

	if (request.isRequestComplete()) {
		std::cout << "COMPLETE BUFFER = \n" << request._buffer << "\n\n";
		request.parsRequest(request._buffer);			//PATH IS HERE
		request.getClientIPPort(client_fd);

		Server* correct_server = findAppropriateServer(request);

		if (correct_server != NULL) {
			// std::cout << "\nparsing again...\n\n";
			request.setServer(*correct_server);
			request.parsRequestBis(*correct_server, request._buffer);
			// std::cout << "request succesfully parsed!\n";
			// std::cout << "Request Path is " << request.getPath() << std::endl;
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