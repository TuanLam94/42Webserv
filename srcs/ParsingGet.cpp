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
			// std::cout << "key : " << key << std::endl;
			// std::cout << "value : " << value << std::endl;
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
	// std::cout << "key : " << key << std::endl;
	// std::cout << "value : " << value << std::endl;
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
	// std::cout << "PATH BEFORE : " << _path << std::endl;
	if (_method == "GET") {
		if (_cgiIsHere)
		{
			new_path = "config" + _path;		//a revoir pour GET
		}
		else
		{
			if (_path.find("errors") != std::string::npos)
			{
				new_path = "config" + _path;
			}
			else if (_path.find("redirect/") != std::string::npos)
			{
				_isRedirect = true;
				new_path = parsRedirectPath(obj);
			}
			else
				new_path = obj.getRoutesPath() + _path;
			// std::cout << _path << std::endl;
			// std::cout << "NEW PATH : " << new_path << std::endl;
		}
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
	// std::cout << "PATH AFTER : " << _path << std::endl;
}

std::string Request::parsRedirectPath(Server& obj)
{
	// std::cout << "SERVER REDIRECTION = " << obj.getRedirection() << std::endl;

	size_t pos = _path.find(obj.getRedirection());

	std::string prePath = _path.substr(0, pos);
	std::string postPath = _path.substr(pos + obj.getRedirection().size());

	std::string new_path = obj.getRoutesPath() + prePath + postPath;

	// std::cout << "NEWPATH = " << new_path << std::endl;
	return new_path;
}


// creer sa fonction findPosition std::string et std::vector

size_t	Request::findPosition(std::string str, const std::string& buff, size_t start)
{
	size_t	pos;

	pos = buff.find(str, start);
	return (pos);
}

size_t	Request::findPositionVec(std::string str, size_t start)
{
	std::vector<unsigned char>::iterator	it;
	std::vector<unsigned char> seq(str.begin(), str.end());
	size_t	index;

	it = std::search(_my_v.begin() + start, _my_v.end(), seq.begin(), seq.end());
	if (it != _my_v.end())
		index = std::distance(_my_v.begin(), it);
	else
	{
		return (-1);
	}
	return (index);
}

size_t	Request::findPositionBody(std::string str, size_t start)
{
	std::vector<unsigned char>::iterator	it;
	std::vector<unsigned char> seq(str.begin(), str.end());
	size_t	index;

	it = std::search(_my_body.begin() + start, _my_body.end(), seq.begin(), seq.end());
	if (it != _my_body.end())
		index = std::distance(_my_body.begin(), it);
	else
	{
		return (-1);
	}
	return (index);
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

void	Request::fillBody()
{
	size_t	j;

	j = findPositionVec("\r\n\r\n", 0);
	if (j == -1)
	{
		std::cerr << "fillBody Error 400: Bad Request.\n" << std::endl;
		throw MyExcep();
	}
	j += 4;
	while (j < _my_v.size())
	{
		_my_body.push_back(_my_v[j]);
		j++;
	}
}

void	Request::parsHeaders()
{
	std::string	key;
	std::string	value;
	std::string	line;
	unsigned long int	i = 0;
	bool	index = true;
	size_t	pos;

	i = _pos;
	_pos = findPositionVec("\r\n\r\n", i);
	if (_pos != -1)
	{
		while (i < _my_v.size() && i < _pos)
		{
			pos = findPositionVec("\r\n", i);
			while (i < _my_v.size() && i < pos)
			{
				if (index == true && _my_v[i] == 58) // :
				{
					index = false;
					key += _my_v[i];
					i++;
					while (i < _my_v.size() && _my_v[i] == 32)
						i++;
				}
				if (index == true /*&& checkValidHeaderKey(buff[i]) == true*/)
					key += _my_v[i];
				else if (checkValidHeaderValue(_my_v[i]) == true)
					value += _my_v[i];
				else
				{
					_status_code = 400;
					std::cerr << "parsHeaders1 Error 400: Bad Request\n";
					throw MyExcep();
				}
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
	{
		_status_code = 400;
		std::cerr << "parsHeaders2 Error 400: Bad Request\n";
		throw MyExcep();
	}
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
	if (it == ite)
	{
		_status_code = 400;
		std::cerr << "checkHeaderName1 Error 400: Bad Request.\n";
		throw MyExcep();
	}
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
		std::cerr << "checkHeaderName2 Error 400: Bad Request\n";
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

void	Request::parsingGET(Server i)
{
	size_t	pos = _path.find("?");

	try
	{
		if (pos != std::string::npos)
			parsParamPath(pos);
		parsPath(i);
		parsHeaders();
		fillBody();
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
		_status_code = 400;
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
