#include "../headers/request.hpp"

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

void	Request::parserFormData_bis(const std::string& buff)
{
	size_t	pos_boundary;
	size_t	pos_info;
	unsigned long int	i = 0;

	std::cout << std::endl;
	pos_boundary = buff.find(_boundary);
	if (pos_boundary != std::string::npos)
		i = pos_boundary;
	// exit (1);
	while (buff[i] != '\n')
		i++;
	pos_boundary = buff.find(_boundary, i);
	i += _boundary.size() + 5;
	pos_info = buff.find("Content-Disposition: form-data; ");
	i = pos_info;
	std::cout << i << std::endl;
	while (buff[i] != ';')
		i++;
	while (i < buff.size())
	{
		std::cout << buff[i];
		i++;
	}
	exit (1);
}

std::string	Request::parserFormData(std::string second, const std::string& buff)
{
	std::string	new_second;
	unsigned long int	i = 0;

	while (i < second.size() && second[i] != 59)
	{
		new_second += second[i];
		i++;
	}
	i++;
	while (i < second.size() && second[i] != 45)
		i++;
	while (i < second.size())
	{
		_boundary += second[i];
		i++;
	}
	parserFormData_bis(buff);
	// std::cout << _boundary << std::endl; 
	// std::cout << new_second << std::endl;
	return (new_second);
}

int	Request::checkContentType()
{
	std::vector<std::pair<std::string, std::string> >::iterator it;
	std::vector<std::pair<std::string, std::string> >::iterator ite;

	it = _headersHttp.begin();
	ite = _headersHttp.end();

	while (it != ite)
	{
		if (it->first == "Content-Type:")
			return (1);
		it++;
	}
	return (0);
}

void	Request::parsingPOST(Server i, const std::string& buffer)
{
	std::vector<std::pair<std::string, std::string> >::iterator it;
	std::vector<std::pair<std::string, std::string> >::iterator ite;

	size_t	pos = _path.find("?");
	if (pos != std::string::npos)
		parsParamPath();
	parsPath(i);
	parsHeaders(buffer);
	checkHeaderName();
	if (!checkContentType())
	{
		std::cerr << "parsingPost Error 400: Bad Request\n";
		exit (1);
	}
	it = _headersHttp.begin();
	ite = _headersHttp.end();
	while (it != ite)
	{
		if (it->first == "Content-Type:")
		{
			// std::cout << it->first << std::endl;
			pos = it->second.std::string::find("multipart/form-data");
			if (it->second == "application/json") // json utiliser pour la creation de ressource
				parserJson();
			else if (it->second == "application/x-www-form-urlencoded")
				parserUrlencoded();
			else if (pos != std::string::npos)
			{
				it->second = parserFormData(it->second, buffer);
			}
			else
			{
				std::cerr << "body et content-type ";
				exit (1);
			}
		}
		it++;
	}
}
