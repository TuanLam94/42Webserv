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

	if (pos != std::string::npos)
		parsParamPath(pos);
	if (checkStatusCode() == true)
		parsPath(i);
	if (checkStatusCode() == true)
		parsHeaders(buffer);
	if (checkStatusCode() == true)
		fillBody(buffer);
	if (checkStatusCode() == true)
		checkHeaderName();
	if (checkContentLength() == true)
		return ;
	if (checkStatusCode() == true)
		fillVar();
	else
		return ;
}
