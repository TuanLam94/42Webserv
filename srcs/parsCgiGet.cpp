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
	// std::cout << _QueryString << std::endl;
	it1 = _headersHttp.begin();
	ite1 = _headersHttp.end();
	while (it1 != ite1)
	{
		if (it1->first == "Host:" || it1->first == "HOST:" || it1->first == "host:")
		{
			_ServerName = fillCgiToUpper(_ServerName, it1->second);
			// std::cout << "servername : " << _ServerName << std::endl;
		}
		else if (it1->first == "Content-Type:")
		{
			_ContentType = fillCgiToUpper(_ContentType, it1->second);
			// std::cout << "content-type : " << _ContentType << std::endl;
		}
		else if (it1->first == "Content-Length:")
		{
			_ContentLength = fillCgiToUpper(_ContentLength, it1->second);
			// std::cout << "content-length : " << _ContentLength << std::endl;
		}
		else if (it1->first == "User-Agent:")
		{
			_HttpUserAgent = fillCgiToUpper(_HttpUserAgent, it1->second);
			fillUserAgent();
			// std::cout << "HttpUserAgent : " << _HttpUserAgent << std::endl;
		}
		else if (it1->first == "Accept:")
		{
			_HttpAccept = fillCgiToUpper(_HttpAccept, it1->second);
			// std::cout << "HttpAccept: " << _HttpAccept << std::endl;
		}
		else if (it1->first == "Accept-Language:")
		{
			_HttpAcceptLanguage = fillCgiToUpper(_HttpAcceptLanguage, it1->second);
			// std::cout << "_httpAcceptLanguage : " << _HttpAcceptLanguage << std::endl;
		}
		else if (it1->first == "Accept-Encoding:")
		{
			_HttpAcceptEncoding = fillCgiToUpper(_HttpAcceptEncoding, it1->second);
			// std::cout << "_HttpAcceptEncoding : " << _HttpAcceptEncoding << std::endl;
		}
		else if (it1->first == "Connection:")
		{
			_HttpConnection = fillCgiToUpper(_HttpConnection, it1->second);
			// std::cout << "_HttpConnection : " << _HttpConnection << std::endl;	
		}
		else if (it1->first == "Origin:")
		{
			_HttpOrigin = fillCgiToUpper(_HttpOrigin, it1->second);
			// std::cout << "_HttpOrigin : " << _HttpOrigin << std::endl;
		}
		else if (it1->first == "Referer:")
		{
			_HttpReferer = fillCgiToUpper(_HttpReferer, it1->second);
			// std::cout << "_HttpReferer : " << _HttpReferer << std::endl;
		}
		it1++;
	}
	_RequestMethod += _method;
	// std::cout << "request-method : " << _RequestMethod << std::endl;
	_ServerProtocol += _version;
	// std::cout << "server-protocol : " << _ServerProtocol << std::endl;
	_GatewayInterface = "CGI/1.1.";
	// std::cout << "_GatewayInterface : " << _GatewayInterface << std::endl;
	_ScriptName = fillScriptName(_path);
	// std::cout << "_ScriptName : " << _ScriptName << std::endl;
	// std::cout << std::endl;
}
