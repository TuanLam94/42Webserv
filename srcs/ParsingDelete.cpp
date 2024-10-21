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
		it++;
	}
	return (false);
}


void	Request::parsingDELETE(Server i, const std::string& buffer)
{
	size_t	pos = _path.find("?");

	try
	{
		if (pos != std::string::npos)
			parsParamPath(pos);
		parsPath(i);
		parsHeaders(buffer);
		fillBody(buffer);
		checkHeaderName();
		fillVar();
		if (!checkContentType() == true)
			throw MyExcep();
		initContentLength();
		parsingPOST_v2(buffer);
	}
	catch(std::exception &ex)
	{
		return ;
	}
}
