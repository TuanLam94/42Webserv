#ifndef UTILS_HPP
#define UTILS_HPP

//utils
std::string trim(std::string str);
size_t findWhiteSpace(std::string line);
void printVector(std::vector<std::string> vector);
void printServerVector(std::vector<Server> vector);

//Parsing
std::vector<std::string> parseConfig(std::string config);
std::vector<std::string> parseConfigFile(std::ifstream& input);

#endif