#include "../headers/request.hpp"

std::string	Request::fillCgiToUpper(std::string fillvar, std::string var)
{
	unsigned long int	i = 0;

	while (i < var.size())
	{
		fillvar += var[i];
		i++;
	}
	return (fillvar);
}

void	Request::fillCgiPost()
{
	unsigned long int	i = 0;
	std::map<std::string, std::string>::iterator	it;
	std::map<std::string, std::string>::iterator	ite;
	std::vector<std::pair<std::string, std::string> >::iterator	it1;
	std::vector<std::pair<std::string, std::string> >::iterator	 ite1;
	
	if (_contentType == "multipart/form-data")
	{
		it = _FormDataName.begin();
		ite  = _FormDataName.end();
	}
	if (_contentType == "application/json")
	{
		it = _jsonParam.begin();
		ite = _jsonParam.end();
	}
	if (_contentType == "application/x-www-form-urlencoded")
	{
		it = _urlParam.begin();
		ite = _urlParam.end();
	}
	while (it != ite)
	{
		while (i < it->first.size())
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
		if (it != ite)
			_QueryString += '&';
		i = 0;
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
			std::cout << "User-Agent : " << _HttpUserAgent << std::endl;
		}
		if (it1->first == "Accept:")
			std::cout << it1->second << std::endl;
		if (it1->first == "Accept-Language:")
			std::cout << it1->second << std::endl;
		if (it1->first == "Accept-Encoding:")
			std::cout << it1->second << std::endl;
		// if (it1->first == "Connection:")
		// 	std::cout << it1->second << std::endl;
		// if (it1->first == "Upgrade-Insecure-Requests:")
		// 	std::cout << it1->second << std::endl;
		it1++;
	}
	_RequestMethod += _method;
	std::cout << "request-method : " << _RequestMethod << std::endl;
	_ServerProtocol += _version;
	std::cout << "server-protocol : " << _ServerProtocol << std::endl;
	std::cout << std::endl;
}