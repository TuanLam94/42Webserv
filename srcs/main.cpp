#include <iostream>
#include "../headers/server.hpp"
#include "../headers/Webserv.hpp"
#include "../headers/utils.hpp"

void    signalHandler(int sig)
{
    return ;
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        std::cout << "Error, launch like this : ./webserv configfile" << std::endl;
        return -1;
    }
    signal(SIGPIPE, SIG_IGN);           //ignore sigpipes for the whole program(even new process)
    // signal(SIGINT, SIG_IGN);
    std::string config(argv[1]);

	Webserv Webserv(config);
	Webserv.run();

    return 0;
}