#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <dirent.h>
#include <cstdlib>
#include <fcntl.h>
#include <sys/epoll.h>
#include <cstring>
#include <unistd.h>

class Server
{
    private:
        int _server_fd;
        int _epoll_fd;
        struct sockaddr_in _address;
        struct epoll_event _event;
        std::string _host;
        int _port;
        int _timeout;
        std::string _error_log;
        std::vector<std::string> _routes;
		std::string _routes_path;
        std::vector<std::string> _errors;
		std::string _errors_path;
        // std::vector<int>    fds;
        void parseConfigFile(std::ifstream& input);
        void parseRoutes(std::string path);
        void parseErrors(std::string path);

    public:
        Server(const std::string config);
        int getServerFd();
        std::string getHost();
        int getPort();
        int getTimeout();
        std::string getErrorLog();
        std::vector<std::string> getRoutes();
        std::vector<std::string> getErrors();
        void printServer();
        void start();
        void socketInit();
        void bindInit();
        void listenInit();
        void acceptInit();
        void nonBlockingSocket();
        void epollInit();
};

//------------------------utils-------------------//

std::string trim(std::string str);

#endif