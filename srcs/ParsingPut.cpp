#include "../headers/request.hpp"


void	Request::initContentLength()
{
	std::vector<std::pair<std::string, std::string> >::iterator it;
	std::vector<std::pair<std::string, std::string> >::iterator ite;

	it = _headersHttp.begin();
	ite = _headersHttp.end();
	while (it != ite)
	{
		if (it->first == "Content-Length:")
		{
			std::istringstream ss(it->second);
			ss >> _contentLength;
			// std::cout << _contentLength << std::endl;
			// std::cout << _body.size() << std::endl;
			// std::cout << _body << std::endl;
			if (_contentLength != _body.size())
			{
				_status_code = 400;
				std::cerr << "initContentLength Error 400: Bad Request\n";
				exit (1);
			}
		}
		it++;
	}
}

void	Request::parsingPUT(Server i, const std::string& buffer)
{
	std::vector<std::pair<std::string, std::string> >::iterator it;
	std::vector<std::pair<std::string, std::string> >::iterator ite;

	size_t	pos = _path.find("?");
	if (pos != std::string::npos)
		parsParamPath();
	parsPath(i);
	parsHeaders(buffer);
	checkHeaderName();
	if (!checkContentType())
	{
		_status_code = 400;
		std::cerr << "parsingPost Error 400: Bad Request\n";
		exit (1);
	}
	initContentLength();
	it = _headersHttp.begin();
	ite = _headersHttp.end();
	while (it != ite)
	{
		if (it->first == "Content-Type:")
		{
			pos = it->second.std::string::find("multipart/form-data");
			if (it->second == "application/json") // json utiliser pour la creation de ressource
				parserJson();
			else if (it->second == "application/x-www-form-urlencoded")
				parserUrlencoded();
			else if (pos != std::string::npos)
			{
				it->second = parserFormData(it->second, buffer);
			}
			else
			{
				_status_code = 415;
				std::cerr << "parsingPUT Error 415: Unsupported Media Type.\n";
				exit (1);
			}
		}
		it++;
	}
}