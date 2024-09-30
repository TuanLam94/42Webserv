#include "../headers/server.hpp"

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