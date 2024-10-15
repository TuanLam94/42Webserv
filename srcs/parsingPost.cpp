#include "../headers/request.hpp"


























void	Request::checkJsonAccolade()
{
	unsigned long int	i = 0;
	int	count =  0;

	while (i < _body.size() && _body[i] != '}')
	{
		if (_body[i] == '"')
			count++;
		i++;
	}
	if (i < _body.size() && (_body[0] != '{' || _body[i] != '}' || (count % 2) != 0))
	{
		_status_code = 400;
		std::cerr << "checkJsonFormat Error 400: Bad Request.\n";
		exit (1);
	}	
}

void	Request::parserJson()
{
	std::string	key;
	std::string	value;
	unsigned long int	i = 1;
	
	checkJsonAccolade();

	while (i < _body.size() && _body[i] != '}')
	{
		while (i < _body.size())
		{
			if (_body[i] == '"')
			{
				i++;
				break ;
			}
			if (_body[i] != 32)
			{
				_status_code = 400;
				std::cerr << "checkJsonFormat 1 Error 400: Bad Request.\n";
				exit (1);	
			}
			i++;
		}
		while (i < _body.size() && _body[i] >= 32)
		{
			if (_body[i] == '"')
			{
				i++;
				break ;
			}
			else
				key += _body[i];
			i++;
		}
		// std::cout << "key : " << key << std::endl;
		while (i < _body.size())
		{
			if (_body[i] == '"')
			{
				i++;
				break ;
			}
			if (_body[i] != 32 && _body[i] != ':')
			{
				_status_code = 400;
				std::cerr << "checkJsonFormat 2 Error 400: Bad Request.\n";
				exit (1);
			}
			i++;
		}
		while (i < _body.size() && _body[i] >= 32)
		{
			if (_body[i] == '"')
			{
				i++;
				break ;
			}
			else
				value += _body[i];
			i++;			
		}
		// std::cout << "value : " << value << std::endl;
		while (i < _body.size())
		{
			if (_body[i] == '"')
				break ;
			if (_body[i] != 32 && _body[i] != ',' && _body[i] != '}')
			{
				_status_code = 400;
				std::cerr << "checkJsonFormat 3 Error 400: Bad Request.\n";
				exit (1);
			}
			i++;
		}
		_jsonParam.insert(std::pair<std::string, std::string>(key, value));
		key.clear();
		value.clear();
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

bool	Request::parserFormData_help(const std::string& buff, unsigned long int i)
{
	std::string	new_boundary;
	std::string	final_boundary;
	unsigned long int j = 0;

	new_boundary = "--" + _boundary + "--";
	while (j < new_boundary.size())
	{
		final_boundary += buff[i];
		i++;
		j++;
	}
	if (new_boundary == final_boundary)
		return (true);	
	return (false);
}

int	Request::parserFormData_ter(const std::string& buff, unsigned long int i)
{
	std::string	id;
	std::string	key;
	std::string	value;

	while (i < buff.size() && (id != "name" && id != "filename"))
	{
		id += buff[i];
		i++;
	}
	i += 2;
	while (i < buff.size() && buff[i] != '"')
	{
		key += buff[i];
		i++;
	}
	i += 5;
	while (i < buff.size() && buff[i] != '\n')
	{
		value += buff[i];
		i++;
	}
	i++;
	if (parserFormData_help(buff, i) == true)
		return (-1);
	else
	{
		if (id == "name")
			_FormDataName.insert(std::pair<std::string, std::string>(key, value));
		else
			_FormDataFilename.insert(std::pair<std::string, std::string>(key, value));
	}
	return (i);
}

void	Request::parserFormData_bis(const std::string& buff)
{
	size_t	pos_boundary;
	size_t	pos_info;
	unsigned long int	i = 0;

	pos_boundary = buff.find(_boundary);
	if (pos_boundary != std::string::npos)
		i = pos_boundary;
	while (buff[i] != '\n')
		i++;
	pos_boundary = buff.find(_boundary, i);
	i += _boundary.size() + 5;
	pos_info = buff.find("Content-Disposition: form-data; ");
	i = pos_info;
	while (i < buff.size())
	{
		while (buff[i] != 32)
			i++;
		i++;
		while (buff[i] != 32)
			i++;
		i++;
		i = parserFormData_ter(buff, i);
		if (i == -1)
			break ;
	}
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
		_status_code = 400;
		std::cerr << "parsingPost Error 400: Bad Request\n";
		exit (1);
	}
	initContentLength();
	it = _headersHttp.begin();
	ite = _headersHttp.end();
	while (it != ite)
	{
		if (it->first == "Content-Type:")
		{
			pos = it->second.std::string::find("multipart/form-data");
			if (it->second == "application/json") // json utiliser pour la creation de ressource
			{
				parserJson();
				// std::map<std::string, std::string>::iterator it;
				// std::map<std::string, std::string>::iterator ite;
				// it = _jsonParam.begin();
				// ite = _jsonParam.end();
				// while (it != ite)
				// {
				// 	std::cout << it->first << std::endl;
				// 	std::cout << it->second << std::endl;
				// 	it++;
				// }
			}
			else if (it->second == "application/x-www-form-urlencoded")
				parserUrlencoded();
			else if (pos != std::string::npos)
			{
				it->second = parserFormData(it->second, buffer);
				// std::map<std::string, std::string>::iterator it;
				// std::map<std::string, std::string>::iterator ite;
				// it = _FormDataName.begin();
				// ite = _FormDataName.end();
				// while (it != ite)
				// {
				// 	std::cout << it->first << std::endl;
				// 	std::cout << it->second << std::endl;
				// 	it++;
				// }
			}
			else
			{
				_status_code = 415;
				std::cerr << "parsingPOST Error 415: Unsupported Media Type.\n";
				exit (1);
			}
		}
		it++;
	}
}
