#include <iostream>
#include "../headers/server.hpp"
#include "../headers/Webserv.hpp"
#include "../headers/utils.hpp"

int main(int argc, char **argv)
{
    if (argc != 2) {
        std::cout << "Error, launch like this : ./webserv configfile" << std::endl;
        return -1;
    }

    std::string config(argv[1]);

	Webserv Webserv(config);
	Webserv.run();

    return 0;
}