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

std::string	fillScriptName(std::string path)
{
	std::string	value;
	size_t	pos;

	pos = path.find("cgi-bin");
	if (pos != std::string::npos)
	{
		while (pos < path.size())
		{
			value += path[pos];
			pos++;
		}
	}
	return (value);
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
	else if (_contentType == "application/json")
	{
		it = _jsonParam.begin();
		ite = _jsonParam.end();
	}
	else if (_contentType == "application/x-www-form-urlencoded")
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
	it1 = _headersHttp.begin();
	ite1 = _headersHttp.end();
	while (it1 != ite1)
	{
		if (it1->first == "Host:" || it1->first == "HOST:" || it1->first == "host:")
			_ServerName = fillCgiToUpper(_ServerName, it1->second);
		else if (it1->first == "Content-Type:")
			_ContentType = fillCgiToUpper(_ContentType, it1->second);
		else if (it1->first == "Content-Length:")
			_ContentLength = fillCgiToUpper(_ContentLength, it1->second);
		else if (it1->first == "User-Agent:")
		{
			_HttpUserAgent = fillCgiToUpper(_HttpUserAgent, it1->second);
			fillUserAgent();
		}
		else if (it1->first == "Accept:")
			_HttpAccept = fillCgiToUpper(_HttpAccept, it1->second);
		else if (it1->first == "Accept-Language:")
			_HttpAcceptLanguage = fillCgiToUpper(_HttpAcceptLanguage, it1->second);
		else if (it1->first == "Accept-Encoding:")
			_HttpAcceptEncoding = fillCgiToUpper(_HttpAcceptEncoding, it1->second);
		else if (it1->first == "Connection:")
			_HttpConnection = fillCgiToUpper(_HttpConnection, it1->second);
		else if (it1->first == "Origin:")
			_HttpOrigin = fillCgiToUpper(_HttpOrigin, it1->second);
		else if (it1->first == "Referer:")
			_HttpReferer = fillCgiToUpper(_HttpReferer, it1->second);
		it1++;
	}
	_RequestMethod = _method;
	_ServerProtocol = _version;
	_GatewayInterface = "CGI/1.1.";
	_ScriptName = fillScriptName(_path);
	_ServerPort = fillServerPort(_port);
}

std::string	fillServerPort(int port)
{
	std::stringstream	ss;
	std::string	value;

	ss << port;
	value = ss.str();
	return (value);
}