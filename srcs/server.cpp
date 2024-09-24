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