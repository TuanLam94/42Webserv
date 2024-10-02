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
				i+=2;
			}
			if (index == true)
				key += buff[i];
			else
				value += buff[i];
			i++;
		}
		i++;
		if (key[0] < 32 || value[0] < 32)
		{
			while (i < buff.size() && buff[i] != 10)
			{
				_body += buff[i];
				i++;
			}
			return ;
		}
		_headersHttp.push_back(std::pair<std::string, std::string>(key, value));
		key.clear();
		value.clear();
		index = true;
	}
}

void	Request::parsingGET(Server i, const std::string& buffer)
{
	size_t	pos = _path.find("?");
	if (pos != std::string::npos)
		parsParamPath();
	parsPath(i);
	parsHeaders(buffer);
	std::cout << _path << std::endl;
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
	std::cout << std::endl;
	while (i < second.size() && second[i] != 45)
		i++;
	while (i < second.size())
	{
		_boundary += second[i];
		i++;
	}
	std::cout << _boundary << std::endl;
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

void	Request::parsRequest(Server i, const std::string& buffer)
{
	std::stringstream ss(buffer);

	ss >> _method >> _path >> _version;
	std::cout << "\n\n\n" << buffer << "\n\n\n";
	// std::cout << _method << std::endl;
	// std::cout << _path << std::endl;
	// std::cout << _version << std::endl;
	if (_method == "GET")
		parsingGET(i, buffer);
	else if(_method == "POST")
		parsingPOST(buffer);
	// else if (_method == "DELETE")
		// parsingDELETE();
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

