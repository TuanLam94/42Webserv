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