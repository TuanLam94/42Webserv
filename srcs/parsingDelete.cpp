#include "../headers/request.hpp"


















bool	Request::checkContentLength()
{
	std::vector<std::pair<std::string, std::string> >::iterator	it;
	std::vector<std::pair<std::string, std::string> >::iterator	ite;

	it = _headersHttp.begin();
	ite = _headersHttp.end();
	while (it != ite)
	{
		if (it->first == "Content-Length:")
		{
			if (it->second != "0")
			{
				_status_code = 400;
				std::cerr << "checkContentLength Error 400: Bad Request\n";
				return (true);
			}
		}
		std::cout << it->first << std::endl;
		std::cout << it->second << std::endl;
		it++;
	}
	return (false);
}


void	Request::parsingDELETE(Server i, const std::string& buffer)
{
	size_t	pos = _path.find("?");

	if (pos != std::string::npos)
		parsParamPath();
	parsPath(i);
	parsHeaders(buffer);
	checkHeaderName();
	if (checkContentLength() == true)
		return ;
	fillVar();
	std::cout << _path << std::endl;
	std::map<std::string, std::string>::iterator	it;
	std::map<std::string, std::string>::iterator	ite;

	it = _queryParameter.begin();
	ite = _queryParameter.end();
	while (it != ite)
	{
		std::cout << it->first << std::endl;
		std::cout << it->second << std::endl;
		it++;
	}
	// std::vector<std::pair<std::string, std::string> >::iterator	it;
	// std::vector<std::pair<std::string, std::string> >::iterator	ite;

	// it = _headersHttp.begin();
	// ite = _headersHttp.end();
	// while (it != ite)
	// {
	// 	std::cout << it->first << std::endl;
	// 	std::cout << it->second << std::endl;
	// 	it++;
	// }
}