#include <iostream>
#include "../headers/server.hpp"
#include "../headers/utils.hpp"

int main(int argc, char **argv)
{
    if (argc != 2) {
        std::cout << "Error, launch like this : ./webserv configfile" << std::endl;
        return -1;
    }

    std::string config(argv[1]);

    std::vector<std::string> configVec = parseConfig(config);

    std::vector<Server> serverVec;
    for (size_t i = 0; i < configVec.size(); i++) {
        Server server(configVec[i]);
        serverVec.push_back(server);
    }

    printServerVector(serverVec);

    // Server Server(config);

    // Server.printServer();
    // Server.start();
    return 0;
}