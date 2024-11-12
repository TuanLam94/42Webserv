#include <iostream>
#include "../headers/server.hpp"
#include "../headers/Webserv.hpp"
#include "../headers/utils.hpp"

Webserv* globalWebserv = NULL;
std::string config;

void sigintHandler(int signal)
{
    if (globalWebserv) {
        globalWebserv->closeAllFD();
        globalWebserv->closeAllSockets();
        globalWebserv->closeAcceptFD();
        std::cout << "\nSIGINT received, cleaned up and exiting." << std::endl;
        globalWebserv->~Webserv();
        exit(0);
    }
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        std::cout << "Error, launch like this : ./webserv configfile" << std::endl;
        return -1;
    }

    signal(SIGPIPE, SIG_IGN);           //ignore sigpipes for the whole program(even new process)
    
    config = argv[1];
	Webserv Webserv(config);
    globalWebserv = &Webserv;
    signal(SIGINT, sigintHandler);

	Webserv.run();

    return 0;
}