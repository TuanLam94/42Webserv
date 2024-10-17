#include "../headers/request.hpp"

void	Request::fillUserAgent()
{
	unsigned long int	i = 0;
	std::string	newUserAgent;
	while (i < _HttpUserAgent.size() && _HttpUserAgent[i] != 32)
	{
		newUserAgent += _HttpUserAgent[i];
		i++;
	}
	_HttpUserAgent.clear();
	_HttpUserAgent = newUserAgent;
	// std::cout << newUserAgent << std::endl;
}

void	Request::fillCgiGet()
{
	unsigned long int	i = 0;
	std::map<std::string, std::string>::iterator	it;
	std::map<std::string, std::string>::iterator	ite;
	std::vector<std::pair<std::string, std::string> >::iterator	it1;
	std::vector<std::pair<std::string, std::string> >::iterator	 ite1;
	
	it = _queryParameter.begin();
	ite = _queryParameter.end();
	while (it != ite)
	{
		while (i < it->first.size() && it->first[i] != ':')
		{
			_QueryString += it->first[i];
			i++;
		}
		_QueryString += '=';
		i = 0;
		while (i < it->second.size())
		{
			_QueryString += it->second[i];
			i++;
		}
		it++;
	}
	std::cout << _QueryString << std::endl;
	it1 = _headersHttp.begin();
	ite1 = _headersHttp.end();
	while (it1 != ite1)
	{
		if (it1->first == "Host:" || it1->first == "HOST:" || it1->first == "host:")
		{
			_ServerName = fillCgiToUpper(_ServerName, it1->second);
			std::cout << "servername : " << _ServerName << std::endl;
		}
		if (it1->first == "Content-Type:")
		{
			_ContentType = fillCgiToUpper(_ContentType, it1->second);
			std::cout << "content-type : " << _ContentType << std::endl;
		}
		if (it1->first == "Content-Length:")
		{
			_ContentLength = fillCgiToUpper(_ContentLength, it1->second);
			std::cout << "content-length : " << _ContentLength << std::endl;
		}
		if (it1->first == "User-Agent:")
		{
			_HttpUserAgent = fillCgiToUpper(_HttpUserAgent, it1->second);
			fillUserAgent();
			std::cout << "HttpUserAgent : " << _HttpUserAgent << std::endl;
		}
		if (it1->first == "Accept:")
			std::cout << "Accept: " << it1->second << std::endl;
		if (it1->first == "Accept-Language:")
			std::cout << "Accept-Language : " << it1->second << std::endl;
		if (it1->first == "Accept-Encoding:")
			std::cout << "Accept-Encoding : " << it1->second << std::endl;
		it1++;
	}
	std::cout << std::endl;
}
