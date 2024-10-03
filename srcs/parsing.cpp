#include "../headers/server.hpp"
#include "../headers/utils.hpp"

std::vector<std::string> parseConfig(std::string config)
{
    std::ifstream input(config.c_str());
    if (!input.is_open()) {
        std::cerr << "Can't open " << config << std::endl;
        exit (-1);
    }

    return (parseConfigFile(input));
}

std::vector<std::string> parseConfigFile(std::ifstream& input)
{
    std::vector<std::string> vector;
    std::string line;

    while (std::getline(input, line)) {
        std::string newConfig;
        if (trim(line) == "server {") {
            while (std::getline(input, line) && trim(line) != "}") {
                newConfig += line + "\n";
            }
        }
        if (!newConfig.empty())
        vector.push_back(newConfig);
    }
    return vector;
}

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

	for (size_t i = 0; i < configVec.size(); i++) {
		serverVec[i].initAll();
	}



    printServerVector(serverVec);

    // Server Server(config);

    // Server.printServer();
    // Server.start();
    return 0;
}