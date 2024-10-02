#include "../headers/request.hpp"
#include <sstream>

void	Request::parsParamPath()
{
	std::string	key;
	std::string	value;
	unsigned long int	i = 0;
	int	index = false;

	while (i < _path.size() && _path[i] != 63) // "?"
		i++;
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
			_queryParameter.push_back(std::pair<std::string, std::string>(key, value));
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
	_queryParameter.push_back(std::pair<std::string, std::string>(key, value));
}

void	Request::parsPath(Server obj)
{
	std::vector<std::string> routes;
	std::string	path = "./config/routes";
	int	index = false;
	int	j  = 0;

	if (_path.size() == 1)
	{
		_path.clear();
		_path += "./config/errors/404.html";
	}
	else
	{
		routes = obj.getRoutes();
		while (j < 3)
		{
			if (_path == "/" + routes[j])
			{
				index = true;
				break ;
			}
			j++;
		}
		_path.clear();
		if (index == true)
			_path = path + "/" + routes[j];
		else
			_path = "./config/errors/404.html";
	}
}

void	Request::checkPath()
{
	if (access(_path.c_str(), F_OK) < 0)
	{
		std::cerr << "Error: Missing files\n";
		exit (1);
	}
	if (access(_path.c_str(), R_OK | W_OK) < 0)
	{
		std::cerr << "Error: Permission denied\n";
		exit (1);
	}
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
			if (buff[i] == 58)
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
				// std::cout << buff[i] << std::endl;
				i++;
			}
			// std::cout << _body << std::endl;
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
	if (_path.find("?") > 0)
		parsParamPath();
	parsPath(i);
	checkPath();
	parsHeaders(buffer);

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
	// std::map<std::string, std::string>::iterator	it;
	// std::map<std::string, std::string>::iterator	ite;
	// it = _jsonParam.begin();
	// ite = _jsonParam.end();
	// while (it != ite)
	// {
	// 	std::cout << it->first << std::endl;
	// 	std::cout << it->second << std::endl;
	// 	it++;
	// }
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
		// if (it->second.find("application/json") != std::string::npos) // je narrive pas comparer le it->second --> "application/json" --> mystere ?
		// 	std::cout << it->first << std::endl;
		// if (it->first == "Content-Type") // je narrive pas comparer le it->second --> "application/json" --> mystere ?
		// 	std::cerr << it->second << std::endl;
		if (trim(it->second) == "application/json") // json utiliser pour la creation de ressource
			parserJson();
			// std::cout << it->first << std::endl;
		it++;
	}
	std::cout << _body << std::endl;
}

void	Request::parsRequest(Server i, const std::string& buffer)
{
	std::stringstream ss(buffer);

	ss >> _method >> _path >> _version;
	// std::cout << "\n\n\n" << buffer << "\n\n\n";
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

