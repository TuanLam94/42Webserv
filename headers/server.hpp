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

//first host then server_name;

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
        void parseRoutes(std::string path);
        void parseErrors(std::string path);
		void parseMethods(std::string input);
    public:
        Server() {};
        Server(const std::string config);
        void printServer();
        void start();
		void initAll();
        void socketInit();
        void bindInit();
        void listenInit();
        void acceptInit();
        void nonBlockingSocket();
        void epollInit(int epoll_fd);
		void handleNewConnection();
		void handleRequest(/*int client_fd*/);
		//getters
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
		struct epoll_event& getEvent();
		struct sockaddr_in& getAddress();
		int	getMaxBodySize();
        const std::vector<std::string>& getRoutes();
        const std::vector<std::string>& getErrors();
		const std::vector<std::string>& getMethods();
		//setters
		void setEvent(struct epoll_event& event);

};

//------------------------utils-------------------//

std::string trim(std::string str);
size_t findWhiteSpace(std::string line);
std::vector<std::string> parseConfig(std::string config);
std::vector<std::string> parseConfigFile(std::ifstream& input);

#endif