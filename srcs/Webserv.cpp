#include "Webserv.hpp"

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

	for (size_t i = 0; i < _servers.size(); i++) {
		_servers[i].epollInit(_epoll_fd);
	}
}

void Webserv::eventLoop() {
	int maxEvents = 10;
	while (true) {
		int numEvents = epoll_wait(_epoll_fd, _events, maxEvents, -1);
		if (numEvents < 0) {
			std::cerr << "epoll_wait failed\n";
			exit (-1);
		}
	}

	for (int i = 0; i < numEvents; i++) {
		
	}
}

Webserv::~Webserv()
{
	close(_epoll_fd);
	for (size_t i = 0; i < _servers.size(); i++) {
		_servers[i].closeServer(); //tocode
	}
}