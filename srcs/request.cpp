#include "../headers/request.hpp"

/*
	Revoir que thom a dit sur le traitement du chemin
		- pas a moi de faire les verifs ni de traiter les erreurs --> se focus sur les erreurs de parsing
*/

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
		else
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
	// new_path = final_path + "?";
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
			// new_path += key + "=" + value + "&";
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
	std::cout << key << std::endl;
	std::cout << value << std::endl;
	if (trim(key).empty() == true || trim(value).empty() == true)
	{
		std::cout << "parsParamPath Error 400: Bad Request\n";
		exit (1);
	}
	// new_path += key + "=" + value;
	_path.clear();
	_path = final_path;
	_queryParameter.insert(std::pair<std::string, std::string>(key, value));
}

// est ce que le path de len tete de la requete est un chemin absolu --> quel chemin absolu prendre
// 

void	Request::parsPath(Server obj)
{
	std::string	new_path;
	
	new_path = obj.getRoutesPath() + _path;
	_path.clear();
	_path = new_path;
}

void	Request::parsHeaders(const std::string& buff)
{
	std::string	key;
	std::string	value;
	std::string	line;
	unsigned long int	i = 0;
	bool	index = true;

	while (buff[i] != 10)
		i++;
	i++;
	while (i < buff.size())
	{
		while (i < buff.size() && buff[i] != 10)
		{
			if (index == true && buff[i] == 58)
			{
				index = false;
				key += buff[i];
				i++;
				while (i < buff.size() && buff[i] == 32)
					i++;
			}
			if (index == true)
				key += buff[i];
			else
				value += buff[i];
			i++;
		}
		i++;
		// std::cout << "key :" << key << std::endl;
		// std::cout << "   value : " << value << std::endl;
		if (key[0] < 32 && value[0] < 32)
		{
			while (i < buff.size() && buff[i] != 10)
			{
				_body += buff[i];
				i++;
			}
			return ;
		}
		// std::cout << "key : " << key << "   value : " << value << std::endl;
		_headersHttp.push_back(std::pair<std::string, std::string>(key, value));
		// std::cout << key << std::endl;
		// std::cout << value << std::endl;
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
		std::cout << it->first << std::endl;
		if (trim(it->first) == "Host:")
			host++;
		checkKey(it->first);
		checkValue(it->second);
		// std::cout << "key : ";
		// std::cout << it->first << std::endl;
		// std::cout << "value : ";
		// std::cout << it->second << std::endl;
		it++;
	}
	std::cout << host << std::endl;
	if (host != 1)
	{
		std::cerr << "checkHeaderName Error 400: Bad Request\n";
		exit (1);
	}
	exit (1);
}

void	Request::parsingGET(Server i, const std::string& buffer)
{
	size_t	pos = _path.find("?");
	if (pos != std::string::npos)
		parsParamPath();
	parsPath(i);
	parsHeaders(buffer);
	checkHeaderName();	
	// exit (1);
	_input.open(_path.c_str());
	if (!_input.is_open())
	{
		std::cerr << "Can't open input\n";
		exit (1);
	}
	std::string	line;
	// std::cout << buffer << std::endl;
	// unsigned long int i  = 0;
	// while (i < buffer.size())
	// {
		
	// }
	// std::cout << "ceci est mon body\n" << _body << std::endl;
	while (std::getline(_input, line))
	{
		std::cout << line << std::endl;
		_body += line;
		_body += "\n";
	}
}

void	Request::parserJson()
{
	std::string	key;
	std::string	value;
	unsigned long int	i = 0;
	
	while (i < _body.size())
	{
		while (i < _body.size() && !((_body[i] >= 65 && _body[i] <= 90)
			|| (_body[i] >= 97 && _body[i] <= 122)
			|| (_body[i] >= 48 && _body[i] <= 57)))
			i++;
		while (i < _body.size() && ((_body[i] >= 65 && _body[i] <= 90)
			|| (_body[i] >= 97 && _body[i] <= 122)
			|| (_body[i] >= 48 && _body[i] <= 57)))
		{
			key += _body[i];
			i++;
		}
		while (i < _body.size() && !((_body[i] >= 65 && _body[i] <= 90)
			|| (_body[i] >= 97 && _body[i] <= 122)
			|| (_body[i] >= 48 && _body[i] <= 57)))
			i++;
		while (i < _body.size() && ((_body[i] >= 65 && _body[i] <= 90)
			|| (_body[i] >= 97 && _body[i] <= 122)
			|| (_body[i] >= 48 && _body[i] <= 57)))
		{
			value += _body[i];
			i++;
		}
		_jsonParam.insert(std::pair<std::string, std::string>(key, value));
		key.clear();
		value.clear();
		i++;
	}
}


void	Request::parserUrlencoded_bis(std::string new_body)
{
	std::string	hexa_help;
	std::string	key;
	std::string	value;
	char	*end;
	int	hexa;
	bool	index = false;
	unsigned long int	i = 0;

	while (i < new_body.size())
	{
		if (new_body[i] == 61)
		{
			index = true;
			i++;
		}
		if (index == false)
		{
			if (new_body[i] == 37) // %
			{
				hexa_help += new_body[i + 1];
				hexa_help += new_body[i + 2];
				hexa = strtol(hexa_help.c_str(), &end, 16);
				hexa_help.clear();
				key += hexa;
				i += 2;
			}
			else if (new_body[i] == 43) // +
				key += 32;
			else
				key += new_body[i];
		}
		else // index == true
		{
			if (new_body[i] == 37) // %
			{		
				hexa_help += new_body[i + 1];
				hexa_help += new_body[i + 2];
				hexa = strtol(hexa_help.c_str(), &end, 16);
				hexa_help.clear();
				value += hexa;
				i += 2;
			}
			else if (new_body[i] == 43) // +
				value += 32;
			else
				value += new_body[i];
		}
		i++;
	}
	_urlParam.insert(std::pair<std::string, std::string>(key, value));
}

void	Request::parserUrlencoded()
{
	std::string	new_body;
	unsigned long int	i = 0;
	
	while (i < _body.size())
	{
		if (_body[i] == 38) // &
		{
			parserUrlencoded_bis(new_body);
			new_body.clear();
		}
		else
			new_body += _body[i];
		i++;
	}
	parserUrlencoded_bis(new_body);
}

std::string	Request::parserFormData(std::string second)
{
	std::string	new_second;
	unsigned long int	i = 0;

	while (i < second.size() && second[i] != 59)
	{
		new_second += second[i];
		i++;
	}
	i++;
	// std::cout << std::endl;
	while (i < second.size() && second[i] != 45)
		i++;
	while (i < second.size())
	{
		_boundary += second[i];
		i++;
	}
	// std::cout << _boundary << std::endl;
	return (new_second);
}

void	Request::parsingPOST(const std::string& buffer)
{
	std::vector<std::pair<std::string, std::string> >::iterator it;
	std::vector<std::pair<std::string, std::string> >::iterator ite;

	parsHeaders(buffer);

	it = _headersHttp.begin();
	ite = _headersHttp.end();

	while (it != ite)
	{
		if (trim(it->second) == "application/json") // json utiliser pour la creation de ressource
			parserJson();
		else if (trim(it->second) == "application/x-www-form-urlencoded")
			parserUrlencoded();
		else
		{
			size_t pos = it->second.std::string::find("multipart/form-data");
			if (pos == 0)
				it->second = parserFormData(it->second);
		}
		it++;
	}
}

void	Request::parsRequestLine(std::string buff)
{
	unsigned long int	i = 0;
	int	space = 0;

	// size_t	pos = _path.find("?");
	// if (pos != std::string::npos)
	// 	parsParamPath();
	// std::cout << _path << std::endl;
	while (i < buff.size() && buff[i] != 10)
	{
		if (buff[i] == 32)
			space++;
		else if (space == 0)
			_method += buff[i];
		else if (space == 1)
			_path += buff[i];
		else if (space == 2)
			_version += buff[i];
		i++;
	}
	if (space != 2
		|| _method.empty() == true
		|| _path.empty() == true
		|| _version.empty() == true) // URI mal formule --> ex : GET /index.html URI HTTP/1.1 ou ex : GET HTTP/1.1
	{
		std::cerr << "parsRequestLine Error 400: Bad request\n";
		exit (1);
	}
	// std::cout << _method << std::endl;
	// std::cout << _path << std::endl;
	// std::cout << _version << std::endl;
}

void	Request::checkMethod()
{
	if (_method != "GET" 
		&& _method != "POST"
		&& _method != "DELETE"
		&& _method != "PUT")
		{
			std::cerr << "Error 405: Method Not Allowed\n";
			exit (1); 
		}
}

// voir pourquoi la version renvoie tjrs true ?

void	Request::checkVersion()
{
	if (_version != trim("HTTP/1.1"))
	{
		std::cout << _version << std::endl;
		std::cerr << "Error 505: HTTP Version Not Supported\n";
		exit (1);
	}
}

void	Request::parsRequest(Server i, const std::string& buffer)
{
	std::stringstream ss(buffer);
	// ss >> _method >> _path >> _version;
	// std::cout << "\n\n\n" << buffer << std::endl;

	parsRequestLine(buffer);
	checkMethod();
	// checkVersion();// pb -> rentre quand meme dans le if ?
	if (_method == "GET")
		parsingGET(i, buffer);
	else if(_method == "POST")
		parsingPOST(buffer);
	// else if (_method == "DELETE")
		// parsingDELETE();
	// std::cout << _path << std::endl;
	// std::map<std::string, std::string>::iterator it;
	// std::map<std::string, std::string>::iterator ite;

	// it = _queryParameter.begin();
	// ite = _queryParameter.end();
	// while (it != ite)
	// {
	// 	std::cout << it->first << std::endl;
	// 	std::cout << it->second << std::endl;
	// 	it++;
	// }
}

//-----------------------------GETTERS-----------------------------//

Request::Request(const Request& copy)
{
	*this = copy;
}

Request& Request::operator=(const Request& other)
{
	if (this != &other) {
		_method = other.getMethod();
		_path = other.getPath();
		_version = other.getVersion();
		_response = other.getResponse();
		_body = other.getBody();
		_contentType = other.getContentType();
	}
	return *this;
}

std::string	Request::getMethod() const
{
	return (_method);
}

std::string	Request::getPath() const
{
	return (_path);
}

std::string	Request::getVersion() const
{
	return (_version);
}

std::string Request::getBody() const
{
	return (_body);
}

std::string Request::getContentType() const
{
	return (_contentType);
}

std::string Request::getResponse() const
{
	return (_response);
}

