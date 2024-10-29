#include "../headers/request.hpp"


bool	Request::checkValidChar(char c)
{
	if (!(c >= 48 && c <= 57)
		&& !(c >= 65 && c <= 90)
		&& !(c >= 97 && c <= 122)
		&& c != 37 // %
		&& c != 43 // + 
		&& c != 47 // /
		&& c != 46 // .
		&& c != 45 // - 
		&& c != 95 // _
		&& c != 126 // ~
		&& c != 61 // =
		&& c != 63) // ?
	{
		_status_code = 400;
		std::cerr << "checkValidChar Error 400: Bad Request.\n";
		return (false);
	}
	return (true);
}

std::string	Request::parsParamPath_bis(std::string str)
{
	std::string	hexa_help;
	std::string	new_str;
	char	*end;
	int	hexa;
	unsigned long int	i = 0;

	while (i < str.size())
	{
		if (str[i] == 37) // %
		{
			hexa_help += str[i + 1];
			hexa_help += str[i + 2];
			hexa = strtol(hexa_help.c_str(), &end, 16);
			hexa_help.clear();
			new_str += hexa;
			i += 2;
		}
		if (str[i] == 43) // +
			new_str += 32;
		else if (checkValidChar(str[i]) == true)
			new_str += str[i];
		i++;
	}
	str.clear();
	str = new_str;
	return (str);
}

void	Request::parsParamPath(size_t pos)
{
	std::string	new_path;
	std::string	final_path;
	std::string	key;
	std::string	value;
	unsigned long int	i = 0;
	int	index = false;

	while (i < _path.size() && i < pos) // "?"
	{
		final_path += _path[i];
		i++;
	}
	i++;
	while (i < _path.size())
	{
		if (_path[i] == 61) // "="
		{
			index = true;
			i++;
		}
		if (_path[i] == 38) //"&"
		{
			index = false;
			key = parsParamPath_bis(key);
			value = parsParamPath_bis(value);
			_queryParameter.insert(std::pair<std::string, std::string>(key, value));
			key.clear();
			value.clear();
			i++;
		}
		if (index == false)
			key += _path[i];
		else
			value += _path[i];
		i++;
	}
	key = parsParamPath_bis(key);
	value = parsParamPath_bis(value);
	if (key.empty() == true || value.empty() == true)
	{
		_status_code = 400;
		std::cout << "parsParamPath Error 400: Bad Request\n";
		throw MyExcep();
	}
	_path.clear();
	_path = final_path;
	_queryParameter.insert(std::pair<std::string, std::string>(key, value));
}

void	Request::parsPath(Server obj) // rajouter le parsPath --> securite "../../" -> dans l'uri 
{
	std::string	new_path;

	if (_method == "GET") {
		if (_cgiIsHere)
			new_path = "config" + _path;			//a revoir pour GET
		else
			new_path = obj.getRoutesPath() + _path;
	}
	else if (_method == "POST") {
		if (_cgiIsHere)
			new_path = "config" + _path;
		else
			new_path = obj.getUploadDir() + _path;
	}
	else if (_method == "DELETE")
		new_path = _path.substr(1);
	_path.clear();
	_path = new_path;
}


size_t	Request::findPosition(std::string str, const std::string& buff, size_t start)
{
	size_t	pos;

	pos = buff.find(str, start);
	return (pos);
}

std::string	Request::helpHeaderHost(std::string value, std::string line)
{
	size_t	pos = 0;
	size_t	j = 0;
	std::string	port;

	pos = value.find(":");
	if (pos != std::string::npos)
	{
		line.clear();
		while (j < value.size() && value[j] != ':')
		{
			line += value[j];
			j++;
		}
		j++;
		while (j < value.size())
		{
			port += value[j];
			j++;
		}
		std::istringstream	ss(port);
		ss >> _port;
		value.clear();
		value = line;
	}
	return (value);
}

bool	Request::checkValidHeaderValue(char c)
{
	if (!(c >= 48 && c <= 57)
		&& !(c >= 65 && c <= 90)
		&& !(c >= 97 && c <= 122)
		&& c != 32 && c != '!'  && c != '#'
		&& c != '$' && c != '%' && c != '&'
		&& c != '\'' && c != '*' && c != '+'
		&& c != '-' && c != '.' && c != '_'
		&& c != '|' && c != '~' && c != ':'
		&& c != '/' && c != '(' && c != ')'
		&& c != ';' && c != ',' && c != '='
		&& c != '?' && c != '\"')
	{
		_status_code = 400;
		std::cerr << "checkValidHeader1 Error 400: Bad Request.\n";
		return (false);
	}
	return (true);
}

void	Request::fillBody(const std::string& buff)
{
	size_t	i = 0;

	_pos += 4;
	i = _pos;
	while (i < buff.size())
	{
		_body += buff[i];
		i++;
	}
}

void	Request::parsHeaders(const std::string& buff)
{
	std::string	key;
	std::string	value;
	std::string	line;
	unsigned long int	i = 0;
	bool	index = true;
	size_t	pos;

	i = _pos;
	_pos = findPosition("\r\n\r\n", buff, i);
	if (_pos != std::string::npos)
	{
		while (i < buff.size() && i < _pos)
		{
			pos = findPosition("\r\n", buff, i);
			while (i < buff.size() && i < pos)
			{
				if (index == true && buff[i] == 58) // :
				{
					index = false;
					key += buff[i];
					i++;
					while (i < buff.size() && buff[i] == 32)
						i++;
				}
				if (index == true /*&& checkValidHeaderKey(buff[i]) == true*/)
					key += buff[i];
				else if (checkValidHeaderValue(buff[i]) == true)
					value += buff[i];
				else
					throw MyExcep();
				i++;
			}
			i += 2;
			if (key == "Host:" || key == "host:" || key == "HOST:")
				value = helpHeaderHost(value, line);
			_headersHttp.push_back(std::pair<std::string, std::string>(key, value));
			key.clear();
			value.clear();
			index = true;
		}
	}
	else
		throw MyExcep();
}

void	Request::checkKey(std::string key)
{
	unsigned long int i = 0;
	size_t	pos;

	if (key.empty() == true || key.size () == 1)
	{
		_status_code = 400;
		std::cerr << "checkKey 1 Error 400: Bad Request\n";
		throw MyExcep();
	}
	while (i < key.size())
	{
		if ((!(key[i] >= 48 && key[i] <= 57)
			&& !(key[i] >= 65 && key[i] <= 90)
			&& !(key[i] >= 97 && key[i] <= 122)
			&& !(key[i] == 45)
			&& !(key[i] == 58)))
		{ 
			_status_code = 400;
			std::cerr << "checkKey 2 Error 400: Bad Request\n";
			throw MyExcep();
		}
		i++;
	}
	pos = key.find(":");
	if (pos != std::string::npos)
		return ;
	else
	{
		_status_code = 400;
		std::cerr << "checkKey 3 Error 400: Bad Request\n";
		throw MyExcep();
	}
}

void	Request::checkValue(std::string value)
{
	if (value.empty() == true)
	{
		_status_code = 400;
		std::cerr << "checkValue Error 400: Bad Request.\n";
		throw MyExcep();
	}
}

void	Request::checkHeaderName()
{
	int	host = 0;
	std::vector<std::pair<std::string, std::string> >::iterator	it;
	std::vector<std::pair<std::string, std::string> >::iterator	ite;

	it = _headersHttp.begin();
	ite = _headersHttp.end();
	while (it != ite)
	{
		it->first = trim(it->first);
		if (it->first == "Host:" || it->first == "HOST:" || it->first == "host:")
			host++;
		if (checkStatusCode() == true)
			checkKey(it->first);
		if (checkStatusCode() == true)
			checkValue(it->second);
		if (checkStatusCode() == false)
			throw MyExcep();
		it++;
	}
	if (host != 1)
	{
		_status_code = 400;
		std::cerr << "checkHeaderName Error 400: Bad Request\n";
		throw MyExcep();
	}
}

int	checkPort(std::string port)
{
	unsigned long int	i = 0;

	while (i < port.size())
	{
		if (port[i] == ':')
			return (1);
		i++;
	}
	return (0);
}

void	Request::fillVar()
{
	std::vector<std::pair<std::string, std::string> >::iterator	it;
	std::vector<std::pair<std::string, std::string> >::iterator	ite;

	it = _headersHttp.begin();
	ite = _headersHttp.end();
	while (it != ite)
	{
		if (it->first == "host:" || it->first == "Host:" || it->first == "HOST:")
			_serverName = it->second;
		it++;
	}
}

void	Request::parsingGET(Server i, const std::string& buffer)
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
		initContentLength();
	}
	catch(std::exception &ex)
	{
		return ;
	}
	_input.open(_path.c_str());
	// std::cout << _path << std::endl;
	if (!_input.is_open())
	{
		std::cerr << "Can't open input\n";
	}
	std::string	line;
	while (std::getline(_input, line))
	{
		_body += line;
		_body += "\n";
		if (line == "  <head>")
			_body += "    <link rel=\"icon\" href=\"/favicon.ico\" type=\"image/x-icon\">\n";
	}
}
