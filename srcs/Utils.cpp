#include "../headers/server.hpp"
#include "../headers/response.hpp"
#include "../headers/request.hpp"

std::string trim(std::string str)
{
	size_t start = 0;
	size_t end = str.length();

	while (start < end && std::isspace(str[start])) {
		start++;
	}
	while (end > start && std::isspace(str[end - 1])) {
		end--;
	}

	return (str.substr(start, end - start));
}

size_t findWhiteSpace(std::string line)
{
	int i = 0;
	while (!isspace(line[i])) {
		i++;
	}
	return i;
}

void printVector(std::vector<std::string> vector)
{
	std::cout << "---Vector---" << std::endl;

	for (size_t i = 0; i < vector.size(); i++) {
		std::cout << "Vector[" << i << "] = " << vector[i] << std::endl;
	}
}

void printServerVector(std::vector<Server> vector)
{
	std::cout << "---ServerVector---" << std::endl;

	for (size_t i = 0; i < vector.size(); i++) {
		vector[i].printServer();
		std::cout << std::endl;
	}
}

int hexStringToInt(const std::string& str)
{
	char *end;
	long chunkSize = strtol(str.c_str(), &end, 16);

	return static_cast<int>(chunkSize);
}