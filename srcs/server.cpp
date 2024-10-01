#include "../headers/server.hpp"
#include "../headers/request.hpp"
#include "../headers/response.hpp"

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
        size_t pos = findWhiteSpace(line);

        if (line.substr(0, pos) == "host")
            _host = trim(line.substr(pos));
        else if (line.substr(0, pos) == "port")
            _port = std::atoi(line.substr(pos).c_str());
        else if (line.substr(0, pos) == "timeout")
            _timeout = std::atoi(line.substr(pos).c_str());
        else if (line.substr(0, pos) == "error_log")
            _error_log = trim(line.substr(pos));
        else if (line.substr(0, pos) == "routes") {
			_routes_path = trim(line.substr(pos));
            parseRoutes(trim(line.substr(pos)));
		}
        else if (line.substr(0, pos) == "errors") {
			_errors_path = trim(line.substr(pos));
            parseErrors(trim(line.substr(pos)));
		}
		else if (line.substr(0, pos) == "server_name")
			_server_name = trim(line.substr(pos));
		else if (line.substr(0, pos) == "methods")
			parseMethods(trim(line.substr(pos)));
		else if (line.substr(0, pos) == "upload_dir")
			_upload_dir = trim(line.substr(pos));
		else if (line.substr(0, pos) == "redirection")
			_redirection = trim(line.substr(pos));
		else if (line.substr(0, pos) == "max_client_body_size")
			_max_client_body_size = std::atoi(line.substr(pos).c_str());
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

void Server::parseMethods(std::string input)
{
	std::istringstream ss(input);
	std::string word;

	while (!ss.eof()) {
		getline(ss, word, ' ');
		_methods.push_back(word);
	}
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
	printServer();
    while (value)
    {
        // reception des requetes + traitement 
        fd_number = epoll_wait(_epoll_fd, &_event, 3, -1);
        if (fd_number <= 0)
        {
            std::cerr << "aucune connexion en attente\n";
            exit (1);
        }

        if (_event.data.fd == _server_fd)
        {
            socklen_t addrlen = sizeof(_address);
            int client_fd = accept(_server_fd, (struct sockaddr*)&_address, &addrlen);
            if (client_fd < 0)
            {
                std::cerr << "accept failed\n";
                continue;
            }

            int flags = fcntl(client_fd, F_GETFL, 0);
            if (flags < 0 || fcntl(client_fd, F_SETFL, flags | O_NONBLOCK) < 0)
            {
                std::cerr << "Failed to set client socket to non-blocking\n";
                close(client_fd);
                continue;
            }

            _event.data.fd = client_fd;
            _event.events = EPOLLIN;
            if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_fd, &_event) < 0)
            {
                std::cerr << "epoll_ctl failed for client\n";
                close(client_fd);
                continue;
            }

            std::cout << "New client connected\n";
        }
        else
        {
            char buffer[1024];
            int bytes = recv(_event.data.fd, buffer, sizeof(buffer), 0);
            if (bytes < 0)
            {
                std::cerr << "Read error\n";
                close(_event.data.fd);
                epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, _event.data.fd, NULL);
                continue;
            }
            else if (bytes == 0)
            {
                std::cout << "Client disconnected\n";
                close(_event.data.fd);
                epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, _event.data.fd, NULL);
                continue;
            }
            else
            {
                // Process client data (for example, HTTP requests)
                // try
                // {
                    Request request;
                    std::string test;
                    request.parsRequest(*this, buffer);
                    Response response(request);
                    response.handleRequest();
                    response.sendResponse(_event.data.fd);
                    // response.sendResponse(_event.data.fd);
                    // test = request.GET_method();
                    // write(_event.data.fd, test.c_str(), test.size());
                    // test.find_request();
                // }
                // catch(...)
                // {
                //     exit (1);
                // }
                close(_event.data.fd);
                epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, _event.data.fd, NULL);
            }
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

    const int trueFlag = 1;
    if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &trueFlag, sizeof(int)) < 0)
    {
        std::cerr << "setsockopt failed\n";
        exit(1);
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
		std::cerr << "bind failed\n" << "errno :" << errno << std::endl;
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
    _event.data.fd = _server_fd;
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _server_fd, &_event)) // surveille le fd de socket, la socket principale, mais doit surveiller aussi tous les connexions entrantes avec accept je supppose
    {
        std::cerr << "epoll_ctl failed\n";
        exit (1);
    }
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
	std::cout << "----SERVER----" << std::endl;

    std::cout << "Server fd = " << getServerFd() << std::endl;
    std::cout << "Host = " << getHost() << std::endl;
    std::cout << "Port = " << getPort() << std::endl;
    std::cout << "Timeout = " << getTimeout() << std::endl;
    std::cout << "Error Log = " << getErrorLog() << std::endl;
	std::cout << "Routes Path = " << getRoutesPath() << std::endl;
    std::cout << "Routes = " << getRoutes();
	std::cout << "Errors Path = " << getErrorsPath() << std::endl;
    std::cout << "Errors = " << getErrors();
	std::cout << "Server Name = " << getServerName() << std::endl;
	std::cout << "Upload directory = " << getUploadDir() << std::endl;
	std::cout << "Path to redirect = " << getRedirection() << std::endl;
	std::cout << "Max client body size = " << getMaxBodySize() << std::endl;
	std::cout << "Methods = " << getMethods();

	std::cout << "----SERVER END----" << std::endl;
}

//-----------------------------GETTERS-----------------------------//

int Server::getServerFd()
{
    return _server_fd;
}

const std::string& Server::getHost()
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

const std::string& Server::getErrorLog()
{
    return _error_log;
}

const std::string& Server::getRoutesPath()
{
	return _routes_path;
}

const std::string& Server::getErrorsPath()
{
	return _errors_path;
}

const std::string& Server::getServerName()
{
	return _server_name;
}

const std::string& Server::getUploadDir()
{
	return _upload_dir;
}

const std::string& Server::getRedirection()
{
	return _redirection;
}

const std::vector<std::string>& Server::getRoutes()
{
    return _routes;
}

const std::vector<std::string>& Server::getErrors()
{
    return _errors;
}

int Server::getMaxBodySize()
{
	return _max_client_body_size;
}

const std::vector<std::string>& Server::getMethods()
{
	return _methods;
}
