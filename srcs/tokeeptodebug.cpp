#include "../headers/Webserv.hpp"
#include "../headers/request.hpp"
#include "../headers/response.hpp"

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
}

void Webserv::serversInit()
{
	for (size_t i = 0; i < _servers.size(); i++) {
		_servers[i].initAll();
	}
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

void Webserv::run()
{
	epollInit();

	int fd_number;
	while (true) {
		for (size_t i = 0; i < _servers.size(); i++) {

			fd_number = epoll_wait(_epoll_fd, &_events.data(), _events.size(), -1);
			if (fd_number <= 0) {
				std::cerr << "aucune connexion en attente\n";
				exit(1);
			}

			if (_events[i].data.fd == _servers[i].getServerFd()) {
				socklen_t addrlen = sizeof(_servers[i].getAddress());
				int client_fd = accept(_servers[i].getServerFd(), (struct sockaddr*)&_servers[i].getAddress(), &addrlen);
				if (client_fd < 0) {
					std::cerr << "accept failed\n";
					continue;
				}

				int flags = fcntl(client_fd, F_GETFL, 0);
				if (flags < 0 || fcntl(client_fd, F_SETFL, flags | O_NONBLOCK) < 0) {
					std::cerr << "Failed to set client socket to non-blocking\n";
					close(client_fd);
					continue;
				}

				epoll_event client_event;
				client_event.data.fd = client_fd;
				client_event.events = EPOLLIN;
				if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event) < 0) {
					std::cerr << "epoll_ctl failed for client\n";
					close(client_fd);
					continue;
				}

				std::cout << "New client connected\n";
			}
			else {
				char buffer[1024];
				int bytes = recv(_events[i].data.fd, buffer, sizeof(buffer), 0);
				if (bytes < 0)
				{
					std::cerr << "Read error\n";
					close(_events[i].data.fd);
					epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, _events[i].data.fd, NULL);
					continue;
				}
				else if (bytes == 0)
				{
					std::cout << "Client disconnected\n";
					close(_events[i].data.fd);
					epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, _events[i].data.fd, NULL);
					continue;
				}
				else
				{
					// Process client data (for example, HTTP requests)
					// try
					// {
						Request request;
						std::string test;
						request.parsRequest(_servers[i], buffer);
						Response response(request);
						response.handleRequest();
						response.sendResponse(_events[i].data.fd);
						// response.sendResponse(_event.data.fd);
						// test = request.GET_method();
						// write(_event.data.fd, test.c_str(), test.size());
						// test.find_request();
					// }
					// catch(...)
					// {
					//     exit (1);
					// }
					// close(_events[i].data.fd);
					epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, _events[i].data.fd, NULL);
				}
			}
		}
	}
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