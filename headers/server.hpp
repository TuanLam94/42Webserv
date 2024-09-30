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
#include <sstream>
#include "request.hpp"
#include "response.hpp"

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
		std::string _server_name;
		std::string _upload_dir;
		std::string _redirection;
		int	_max_client_body_size;
		std::vector<std::string> _methods;
        // std::vector<int>    fds;
        void parseConfigFile(std::ifstream& input);
        void parseRoutes(std::string path);
        void parseErrors(std::string path);
		void parseMethods(std::string input);
    public:
        Server(const std::string config);
        int getServerFd();
        int getPort();
        int getTimeout();
        const std::string& getHost();
        const std::string& getErrorLog();
		const std::string& getRoutesPath();
		const std::string& getErrorsPath();
		const std::string& getServerName();
		const std::string& getUploadDir();
		const std::string& getRedirection();
		int	getMaxBodySize();
        const std::vector<std::string>& getRoutes();
        const std::vector<std::string>& getErrors();
		const std::vector<std::string>& getMethods();
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
size_t findWhiteSpace(std::string line);

#endif