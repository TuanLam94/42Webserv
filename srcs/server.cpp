#include "../headers/server.hpp"

Server::Server(const std::string config)
{
    std::ifstream input(config.c_str());
    if (!input.is_open()) {
        std::cerr << "Can't open " << config << std::endl;
        exit(-1);
    }
    parseConfigFile(input);
}

void Server::parseConfigFile(std::ifstream& input)
{
    std::string line;
    while (std::getline(input, line)) {
        line = trim(line);
        size_t pos = line.find(" ");

        if (line.substr(0, pos) == "host")
            _host = trim(line.substr(pos));
        
        else if (line.substr(0, pos) == "port")
            _port = std::atoi(line.substr(pos).c_str());

        else if (line.substr(0, pos) == "timeout")
            _timeout = std::atoi(line.substr(pos).c_str());

        else if (line.substr(0, pos) == "error_log")
            _error_log = trim(line.substr(pos));
        
        else if (line.substr(0, pos) == "routes")
            parseRoutes(trim(line.substr(pos)));

        else if (line.substr(0, pos) == "errors")
            parseErrors(trim(line.substr(pos)));
    }
}

void Server::parseRoutes(std::string path)
{
    DIR* dir;
    struct dirent* entry;

    dir = opendir(path.c_str());
    if (dir == NULL) {
        std::cerr << "Error opening routes directory." << std::endl;
        exit(-1);
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] != '.')
            _routes.push_back(entry->d_name);
    }

    closedir(dir);
}

void Server::parseErrors(std::string path)
{
    DIR* dir;
    struct dirent* entry;

    dir = opendir(path.c_str());
    if (dir == NULL) {
        std::cerr << "Error opening errors directory." << std::endl;
        exit (-1);
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] != '.')
            _errors.push_back(entry->d_name);
    }

    closedir(dir);
}

//-----------------------Lancement-serveur------------------------//

/*
	- htonl = convertir un long int en representation reseau
	- htons = convertir un short en representation reseau
*/

void	Server::start()
{
    int new_socket;

    socketInit();
    nonBlockingSocket();
    bindInit();
    listenInit();
    acceptInit();
    // initialisation de epoll() pour surveiller les sockets ouvert
    // epollInit();
    // while (1)
    // {
    //     // reception des requetes + traitement 
    // }
}

void    Server::socketInit()
{
    _server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_fd < 0)
	{
		std::cerr << "socket failed\n";
		exit (1);
	}
}

void    Server::nonBlockingSocket()
{
    int flags;    

    flags = fcntl(_server_fd, F_GETFL, 0);
    if (flags < 0)
    {
        std::cerr << "fcntl failed\n";
        exit (1);
    }
    flags = fcntl(_server_fd, F_SETFL, O_NONBLOCK);
    if (flags < 0)
    {
        std::cerr << "fcntl failed\n";
        exit (1);
    }

}

void    Server::bindInit()
{
    // memset sur la structure en theorie
	_address.sin_family = AF_INET;
	_address.sin_port = htons(_port);
	_address.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(_server_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0)
	{
		std::cerr << "bind failed\n";
		exit (1);
	}
}

void    Server::listenInit()
{
    if (listen(_server_fd, 10) < 0)
    {
        std::cerr << "liste failed\n";
        exit (1);
    }
}

void    Server::acceptInit()
{
    socklen_t   addrlen = sizeof(_address);
	if (accept(_server_fd, (struct sockaddr *)&_address, &addrlen) < 0)
	{
		std::cerr << "bind failed\n";
		exit (1);
	}
}

void    Server::epollInit()
{
    int epoll_fd;
    struct epoll_event  event;
    epoll_fd = epoll_create(0); // epoll_create1() preferable car possibilite de specifier des flags mais sujet autorise pas ?
    if (epoll_fd < 0)
    {
        std::cerr << "epoll create1 failed\n";
        exit (1);
    }
    event.events = EPOLLIN;
    event.data.fd = 0;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, _server_fd, &event)) // surveille le fd de socket, la socket principale, mais doit surveiller aussi tous les connexions entrantes avec accept je supppose

    {
        std::cerr << "epoll_ctl failed\n";
        exit (1);
    }
    // debut de la surveillance des connexions entrantes dans une boucle i guess
    // utiliser epoll_wait() --> cf note_server.txt
}

//-----------------------------GETTERS-----------------------------//

int Server::getServerFd()
{
    return _server_fd;
}

std::string Server::getHost()
{
    return _host;
}

int Server::getPort()
{
    return _port;
}

int Server::getTimeout()
{
    return _timeout;
}

std::string Server::getErrorLog()
{
    return _error_log;
}

std::vector<std::string> Server::getRoutes()
{
    return _routes;
}

std::vector<std::string> Server::getErrors()
{
    return _errors;
}

//----------------------------------UTILS------------------------------//

std::ostream& operator << (std::ostream& os, const std::vector<std::string>& vec)
{
    for (size_t i = 0; i < vec.size(); i++) {
        os << "vector [" << i << "] " << vec[i] << std::endl;
    }
    return os;
}

void Server::printServer()
{
    std::cout << "Server fd = " << getServerFd() << std::endl;
    std::cout << "Host = " << getHost() << std::endl;
    std::cout << "Port = " << getPort() << std::endl;
    std::cout << "Timeout = " << getTimeout() << std::endl;
    std::cout << "Error Log = " << getErrorLog() << std::endl;
    std::cout << "Routes = " << getRoutes() << std::endl;
    std::cout << "Errors = " << getErrors() << std::endl;
}