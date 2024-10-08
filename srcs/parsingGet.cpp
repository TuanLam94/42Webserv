#include "../headers/request.hpp"


bool	checkValidChar(char c)
{
	// std::cout << c << std::endl;
	if (!(c >= 48 && c <= 57)
		&& !(c >= 65 && c <= 90)
		&& !(c >= 97 && c <= 122)
		&& c != 47 // /
		&& c != 46 // .
		&& c != 45 // - 
		&& c != 95 // _
		&& c != 126 /* ~ */
		&& c != 61
		&& c != 63)
	{
		std::cerr << "checkValidChar Error 400: Bad Request.\n";
		exit (1);
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
		else if (str[i] == 43) // +
			new_str += 32;
		else if (checkValidChar(str[i]) == true)
			new_str += str[i];
		i++;
	}
	str.clear();
	str = new_str;
	return (str);
}

void	Request::parsParamPath()
{
	std::string	new_path;
	std::string	final_path;
	std::string	key;
	std::string	value;
	unsigned long int	i = 0;
	int	index = false;

	while (i < _path.size() && _path[i] != 63) // "?"
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
	if (trim(key).empty() == true || trim(value).empty() == true)
	{
		std::cout << "parsParamPath Error 400: Bad Request\n";
		exit (1);
	}
	_path.clear();
	_path = final_path;
	_queryParameter.insert(std::pair<std::string, std::string>(key, value));
}

void	Request::parsPath(Server obj)
{
	std::string	new_path;
	
	new_path = obj.getRoutesPath() + _path;
	_path.clear();
	_path = new_path;
}

void	checkISS(char c1, char c2)
{
	if (c1 != 13 || c2 != 10)
	{
		std::cerr << "checkISS Error 400: Bad Request.\n";
		exit (1);
	}
}

bool	checkValidHeader(char c)
{
	// std::cout << c << std::endl;
	if (!(c >= 48 && c <= 57)
		&& !(c >= 65 && c <= 90)
		&& !(c >= 97 && c <= 122)
		&& c != 32
		&& c != 42
		&& c != 45
		&& c != 46
		&& c != 47
		&& c != 58
		&& c != 59
		&& c != 61)
	{
		std::cerr << "checkValidHeader Error 400: Bad Request.\n";
		exit (1);
	}
	return (true);
}

void	Request::parsHeaders(const std::string& buff)
{
	std::string	key;
	std::string	value;
	std::string	line;
	unsigned long int	i = 0;
	bool	index = true;

	while (i < buff.size() && buff[i] != 13 && buff[i + 1] != 10)
		i++;
	checkISS(buff[i], buff[i + 1]);
	i += 2;
	while (i < buff.size())
	{
		while (i < buff.size() && buff[i] != 13 && buff[i + 1] != 10)
		{
			if (index == true && buff[i] == 58)
			{
				index = false;
				key += buff[i];
				i++;
				while (i < buff.size() && buff[i] == 32)
					i++;
			}
			if (index == true && checkValidHeader(buff[i]) == true)
				key += buff[i];
			else if (checkValidHeader(buff[i]) == true)
				value += buff[i];
			i++;
		}
		checkISS(buff[i], buff[i + 1]);
		i += 2;
		if (key[0] < 32 && value[0] < 32)
		{
			while (i < buff.size() && buff[i] != 13 && buff[i + 1] != 10)
			{
				_body += buff[i];
				i++;
			}
			return ;
		}
		// std::cout << "key : " << key << "   value : " << value << std::endl;
		_headersHttp.push_back(std::pair<std::string, std::string>(key, value));
		key.clear();
		value.clear();
		index = true;
	}
}

void	checkKey(std::string key)
{
	unsigned long int i = 0;
	size_t	pos;

	if (key.empty() == true)
	{
		std::cerr << "checkKey 1 Error 400: Bad Request\n";
		exit (1);
	}
	while (i < key.size())
	{
		if ((!(key[i] >= 48 && key[i] <= 57)
			&& !(key[i] >= 65 && key[i] <= 90)
			&& !(key[i] >= 97 && key[i] <= 122)
			&& !(key[i] == 45)
			&& !(key[i] == 58)))
		{  
			std::cerr << "checkKey 2 Error 400: Bad Request\n";
			exit (1);
		}
		i++;
	}
	pos = key.find(":");
	if (pos != std::string::npos)
		return ;
	else
	{
		std::cerr << "checkKey 3 Error 400: Bad Request\n";
		exit (1);
	}
}

void	checkValue(std::string value)
{
	if (trim(value).empty() == true)
	{
		std::cerr << "chechValue Error 400: Bad Request\n";
		exit (1);
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
		// std::cout << it->first << std::endl;
		// std::cout << it->second << std::endl;
		if (it->first == "Host:" || it->first == "HOST:" || it->first == "host:")
			host++;
		checkKey(it->first);
		checkValue(it->second);
		// std::cout << "key : ";
		// std::cout << it->first << std::endl;
		// std::cout << "value : ";
		// std::cout << it->second << std::endl;
		it++;
	}
	if (host != 1)
	{
		std::cerr << "checkHeaderName Error 400: Bad Request\n";
		exit (1);
	}
}

// std::string	findPort(std::string second)
// {
// 	std::string	port;
// 	unsigned long int	i = 0;
	
// 	while (i < second.size() && second[i] != ':')
// 		i++;
// 	while (i < second.size())
// 	{
// 		port += second[i];
// 		i++;
// 	}
// 	return (port);
// }
/*
	- si ya pas de port --> juste host dans var host sous forme adresse url tout simple
	- si ya port --> port dans other variable
*/


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
	unsigned long int	i = 0;
	std::vector<std::pair<std::string, std::string> >::iterator	it;
	std::vector<std::pair<std::string, std::string> >::iterator	ite;

	it = _headersHttp.begin();
	ite = _headersHttp.end();
	while (it != ite)
	{
		if (it->first == "host:" || it->first == "Host:" || it->first == "HOST:")
		{
			_serverName = it->second;
			if (checkPort(it->second))
			{
				while (it->second[i] != ':')
					i++;
				i++;
				while (i < it->second.size())
				{
					_serverName += it->second[i];					
					i++;
				}
			}
		}
		it++;
	}
}

void	Request::parsingGET(Server i, const std::string& buffer)
{
	size_t	pos = _path.find("?");

	if (pos != std::string::npos)
		parsParamPath();
	parsPath(i);
	parsHeaders(buffer);
	checkHeaderName();
	fillVar();
	_input.open(_path.c_str());

	if (!_input.is_open())
	{
		std::cerr << "Can't open input\n";
		exit (1);
	}
	std::string	line;
	while (std::getline(_input, line))
	{
		_body += line;
		_body += "\n";
	}
	// std::map<std::string, std::string>::iterator	it;
	// std::map<std::string, std::string>::iterator	ite;

	// it = _queryParameter.begin();
	// ite = _queryParameter.end();
	// while (it != ite)
	// {
	// 	std::cout << it->first << std::endl;
	// 	std::cout << it->second << std::endl;
	// 	it++;
	// }
}
