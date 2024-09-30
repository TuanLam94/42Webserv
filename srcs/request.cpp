#include "../headers/request.hpp"
#include <sstream>

// void	Request::Request(int client_fd)
// {
// 	_client_fd = client_fd;
// }

std::string	Request::build_response(const std::string& body, const std::string& content_type)
{
    std::stringstream response;
    
    response << "HTTP/1.1 200 OK\r\n";
    response << "Content-Length: " << body.size() << "\r\n";
    response << "Content-Type: " << content_type << "\r\n";
    response << "\r\n";
    
    response << body;

    return response.str();
}

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
			_path = path + "/index.html";
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
		while (buff[i] != 10)
		{
			if (buff[i] == 58)
			{
				index = false;
				i++;
			}
			if (index == true)
				key += buff[i];
			else
				value += buff[i];
			i++;
		}
		_headersHttp.push_back(std::pair<std::string, std::string>(key, value));
		key.clear();
		value.clear();
		index = true;
		i++;
	}
	// std::vector<std::pair<std::string, std::string> >::iterator	it;
	// std::vector<std::pair<std::string, std::string> >::iterator	ite;
	// it = _headersHttp.begin();
	// ite = _headersHttp.end();
	// while (it != ite)
	// {
	// 	std::cout << it->first << " " << it->second << std::endl;
	// 	it++;
	// }
}

void	Request::parsRequest(Server i, const std::string& buffer)
{
	std::stringstream ss(buffer);

	// std::cout << buffer << std::endl;
	ss >> _method >> _path >> _version;
	// std::cout << _method << "\n" << _path << "\n" << _version << std::endl; 
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
}

// partie reponse --> recuperation de la page et ecriture dans le FD

std::string	Request::GET_method()
{
	std::string	response;
	std::string	line;
	std::string	file;

	while (std::getline(_input, line))
	{
		file += line;
		file += "\n";
	}
	response = build_response(file, "\0");
	return (response);
}

void	Request::find_request()
{
	if (_method == "GET")
		GET_method();
	// else if (_method == "POST")
	// 	POST_method();
	// else if (_method == "DELETE")
	// 	DELETE_method();
	// else
	// 	throw 405_Method_Not_Allowed();
}

// void	Request::POST_method()
// {
// 	std::cout <<  "POST" << std::endl;
// 	return ;
// }

//-----------------------------GETTERS-----------------------------//

std::string	Request::getMethod()
{
	return (_method);
}

std::string	Request::getPath()
{
	return (_path);
}

std::string	Request::getVersion()
{
	return (_version);
}