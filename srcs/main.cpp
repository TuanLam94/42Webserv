#include <iostream>
#include "../headers/server.hpp"

int main(int argc, char **argv)
{
    if (argc != 2) {
        std::cout << "Error, launch like this : ./webserv configfile" << std::endl;
        return -1;
    }

    std::string config(argv[1]);

    Server Server(config);

    Server.printServer();
    Server.start();
    return 0;
}