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
    int fd_number;
    int value = 1;

    socketInit();
    nonBlockingSocket();
    bindInit();
    listenInit();
    // acceptInit();
    epollInit();
    // debut de la surveillance des connexions entrantes dans une boucle i guess
    // utiliser epoll_wait() --> cf note_server.txt
    while (value)
    {
        // reception des requetes + traitement 
        fd_number = epoll_wait(_epoll_fd, &_event, 1, -1);
        if (fd_number <= 0)
        {
            std::cerr << "aucune connexion en attente\n";
            exit (1);
        }
        else
        {
            // if (_server_fd == _epoll_fd)
            // {
                std::cout << "test\n";
                // break ;
            // }
        }
    }
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
    memset(&_address, 0, sizeof(_address));
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
		std::cerr << "accept failed\n";
		exit (1);
	}
}

void    Server::epollInit()
{
    _epoll_fd = epoll_create(1); // epoll_create() preferable car possibilite de specifier des flags mais sujet autorise pas ?
    if (_epoll_fd < 0)
    {
        std::cout << strerror(errno) << std::endl;
        std::cerr << "epoll create1 failed\n";
        exit (1);
    }
    _event.events = EPOLLIN;
    _event.data.fd = 0;
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _server_fd, &_event)) // surveille le fd de socket, la socket principale, mais doit surveiller aussi tous les connexions entrantes avec accept je supppose
    {
        std::cerr << "epoll_ctl failed\n";
        exit (1);
    }
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