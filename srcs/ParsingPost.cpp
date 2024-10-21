#include "../headers/request.hpp"


void	Request::checkJsonAccolade()
{
	unsigned long int	i = 0;
	int	count =  0;

	while (i < _contentLength && _body[i] != '}')
	{
		if (_body[i] == '"')
			count++;
		i++;
	}
	if (_body[0] != '{' || _body[i] != '}' || (count % 2) != 0)
	{
		_status_code = 400;
		std::cerr << "checkJsonFormat Error 400: Bad Request.\n";
		return ;
	}	
}

bool	Request::checkMap(std::string key, std::map<std::string,std::string>::iterator it, std::map<std::string, std::string>::iterator ite)
{
	while (it != ite)
	{
		if (key == it->first)
			return (true);
		it++;
	}
	return (false);
}

// void	Request::parserJson()
// {
// 	int	index = 0;
// 	int	index_comma = 0;
// 	std::string	key;
// 	std::string	value;
// 	unsigned long int	i = 1;
// 	size_t	pos;

// 	checkJsonAccolade();
// 	if (checkStatusCode() == false)
// 		return ;	
// 	pos = findPosition("{")
// }


void	Request::parserJson()
{
	int	index = 0;
	int	index_comma = 0;
	std::string	key;
	std::string	value;
	unsigned long int	i = 1;
	
	checkJsonAccolade();
	if (checkStatusCode() == false)
		return ;
	while (i < _contentLength && _body[i] != '}')
	{
		while (i < _contentLength)
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
				return ;
			}
			i++;
		}
		while (i < _contentLength && _body[i] >= 32)
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
		while (i < _contentLength)
		{
			if (_body[i] == ':')
				index++;
			if (_body[i] == '"' || (_body[i] >= 48 && _body[i] <= 57))
			{
				i++;
				break ;  
			}
			if ((_body[i] != 32
				&& !(_body[i] >= 48 && _body[i] <= 57)
				&& _body[i] != ':') 
				|| index > 1)
			{
				_status_code = 400;
				std::cerr << "checkJsonFormat 2 Error 400: Bad Request.\n";
				return ;
			}
			i++;
		}
		index = 0;
		while (i < _contentLength && _body[i] >= 32)
		{
			if (_body[i] == '"' || _body[i] == ',' || _body[i] == '}')
			{
				i++;
				break ;
			}
			else
				value += _body[i];
			i++;			
		}
		while (i < _contentLength && _body[i] != '"')
		{
			if (_body[i] == ',')
				index_comma++;
			// if (_body[i] == '"')
			// 	break ;
			if (_body[i] == '}' && index_comma == 1)
			{
				_status_code = 400;
				std::cerr << "checkJsonFormat 3 Error 400: Bad Request.\n";
				return ;
			}
			if ((_body[i] != 32 && _body[i] != '}') && index_comma != 1)
			{
				_status_code = 400;
				std::cerr << "checkJsonFormat 3 Error 400: Bad Request.\n";
				return ;
			}
			i++;
		}
		index_comma = 0;
		if (checkMap(key, _jsonParam.begin(), _jsonParam.end()) == false)
			_jsonParam.insert(std::pair<std::string, std::string>(key, value));
		else
		{
			_status_code = 400;
			std::cerr << "checkJsonFormat 4 Error 400: Bad Request.\n";
			throw MyExcep();
		}
		key.clear();
		value.clear();
	}
}

void	Request::checkKeyUrl(std::string key)
{
	unsigned long int i = 0;

	if (key.empty() == true)
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
			&& !(key[i] == 45)))
		{ 
			_status_code = 400;
			std::cerr << "checkKey 2 Error 400: Bad Request\n";
			throw MyExcep();
		}
		i++;
	}
}

void	Request::checkValueUrl(std::string value)
{
	if (value.empty() == true || value[0] < 32)
	{
		_status_code = 400;
		std::cerr << "chechValue Error 400: Bad Request\n";
		throw MyExcep();
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
	if (checkStatusCode() == true)
		checkKeyUrl(key);
	if (checkStatusCode() == true)
		checkValueUrl(value);
	else
		throw MyExcep();
	if (checkMap(key, _urlParam.begin(), _urlParam.end()) == false)
		_urlParam.insert(std::pair<std::string, std::string>(key, value));
	else
	{
		_status_code = 400;
		std::cerr << "parserUrlencoded Error 400: Bad Request\n";
		throw MyExcep();
	}	
	
}

int	checkUrlEncoded(std::string body)
{
	unsigned long int i = 0;

	while (i < body.size())
	{
		if ((!(body[i] >= 48 && body[i] <= 57)
			&& !(body[i] >= 65 && body[i] <= 90)
			&& !(body[i] >= 97 && body[i] <= 122)
			&& !(body[i] == 37)
			&& !(body[i] == 38)
			&& !(body[i] == 43)
			&& !(body[i] == 45)
			&& !(body[i] == 46)
			&& !(body[i] == 61)
			&& !(body[i] == 95)
			&& !(body[i] == 126)))
		{
			std::cerr << "checkUrlencoded Error 400: Bad Request.\n";
			return (400);
		}
		i++;
	}
	return (0);
}

void	Request::parserUrlencoded()
{
	std::string	new_body;
	unsigned long int	i = 0;
	
	_status_code = checkUrlEncoded(_body);
	if (checkStatusCode() == false)
		throw MyExcep();
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

bool	Request::parserFormData_help(const std::string& buff, size_t i)
{
	std::string	new_boundary;
	std::string	final_boundary;
	unsigned long int j = 0;

	new_boundary = _boundary + "--";
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

void	Request::formDataGetName(const std::string& buff, size_t pos)
{
	std::string	key;
	std::string	value;

	pos = pos + 6;
	while (buff[pos] != '\"')
	{
		key += buff[pos];
		pos++;
	}
	pos = findPosition("\r\n\r\n", buff, pos);
	if (pos != std::string::npos)
	{
		pos += 4;
		while (buff[pos] != '\r')
		{
			value += buff[pos];
			pos++;
		}
		if (key.empty() == false && value.empty() == false)
			_FormDataName.insert(std::pair<std::string, std::string>(key, value));
		else
		{
			_status_code = 400;
			std::cerr << "formDataGetName Error 400: Bad Request.\n";
			throw MyExcep();
		}
	}
}

void	Request::formDataGetFilename(const std::string& buff, size_t pos)
{
	std::string	key;
	std::string	value;

	pos = pos + 6;
	while (buff[pos] != '\"')
	{
		key += buff[pos];
		pos++;
	}
	pos = findPosition("\r\n\r\n", buff, pos);
	if (pos != std::string::npos)
	{
		pos += 4;
		while (buff[pos] != '\r')
		{
			value += buff[pos];
			pos++;
		}
		if (key.empty() == false && value.empty() == false)
			_FormDataFilename.insert(std::pair<std::string, std::string>(key, value));
		else
		{
			_status_code = 400;
			std::cerr << "formDataGetName Error 400: Bad Request.\n";
			throw MyExcep();
		}
	}
}

void	Request::parserFormData_bis(const std::string& buff, size_t pos)
{
	size_t	pos_b = 0;
	size_t	pos_info = 0;
	size_t	i = 0;

	pos_b = findPosition(_boundary, buff, pos);
	if (pos_b != std::string::npos)
		i = pos_b;
	while (true)
	{
		pos_info = findPosition("Content-Disposition: form-data; ", buff, i);
		if (pos_info != std::string::npos)
		{
			i = pos_info;
			pos_info = findPosition("name=\"", buff, i);
			if (pos_info != std::string::npos)
				formDataGetName(buff, pos_info);
			else
			{
				pos_info = findPosition("filename=\"", buff, i);
				if (pos_info != std::string::npos)
					formDataGetFilename(buff, pos_info);
			}
		}
		pos_info = findPosition("\r\n\r\n", buff, i);
		if (pos_info != std::string::npos)
			i = pos_info + 5;
		pos_info = findPosition("\r\n", buff, i);
		if (pos_info != std::string::npos)
			i = pos_info + 3;
		if (parserFormData_help(buff, i) == true)
			break ;
	}
}

void	Request::parserFormData(const std::string& buff)
{
	size_t	pos_b = 0;
	size_t	pos_end = 0;
	size_t	i = 0;
	size_t	j = 0;
	std::string	boundary = "boundary=";

	pos_b = findPosition(boundary, buff, 0);
	if (pos_b == std::string::npos)
	{
		_status_code = 400;
		std::cerr << "parserFormData Error 400: Bad request.\n";
		throw MyExcep();
	}
	j = boundary.size();
	while (i < j)
	{
		i++;
		pos_b++;
	}
	_boundary += "--";
	pos_end = findPosition("\r\n", buff, pos_b);
	while (pos_b < buff.size() && pos_b < pos_end)
	{
		_boundary += buff[pos_b];
		pos_b++;
	}
	parserFormData_bis(buff, pos_b);
}

void	Request::parsingPOST_v2(const std::string& buffer)
{
	size_t	pos;
	std::vector<std::pair<std::string, std::string> >::iterator it;
	std::vector<std::pair<std::string, std::string> >::iterator ite;

	it = _headersHttp.begin();
	ite = _headersHttp.end();
	while (it != ite)
	{
		if (it->first == "Transfer-Encoding:")
		{
			if (it->second == "chunked")
			{
				
				_isChunk = true;
				std::cout << "chunked request\n";
				exit (1);
			}
		}
		if (it->first == "Content-Type:")
		{
			pos = it->second.std::string::find("multipart/form-data");
			if (it->second == "application/json") // json utiliser pour la creation de ressource
			{
				_contentType = it->second;
				parserJson();
			}
			else if (it->second == "application/x-www-form-urlencoded")
			{
				_contentType = it->second;		
				parserUrlencoded();
			}
			else if (pos != std::string::npos)
			{
				it->second = "multipart/form-data";
				parserFormData(buffer);
				_contentType = it->second;
			}
			else if (it->second == "plain/text")
			{
				_contentType = it->second;
				parserTextPlain();
			}
			else
			{
				if (_cgiIsHere == false)
				{
					_status_code = 415;
					std::cerr << "parsingPOST Error 415: Unsupported Media Type.\n";
					throw MyExcep();
				}
			}
		}
		it++;
	}
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

void	Request::parserTextPlain()
{
	unsigned long int	i = 0;

	while (i < _contentLength)
	{
		_dataBrut += _body[i];
		i++;
	}
}


void	Request::initContentLength()
{
	std::vector<std::pair<std::string, std::string> >::iterator it;
	std::vector<std::pair<std::string, std::string> >::iterator ite;

	it = _headersHttp.begin();
	ite = _headersHttp.end();
	while (it != ite)
	{
		if (it->first == "Content-Length:")
		{
			std::istringstream ss(it->second);
			ss >> _contentLength;
			if (_contentLength != _body.size())
			{
				_status_code = 400;
				std::cerr << "initContentLength Error 400: Bad Request\n";
				throw MyExcep();
			}
		}
		it++;
	}
}

void	Request::parsingPOST_v1(Server i, const std::string& buffer)
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
		if (!checkContentType())
		{
			if (_cgiIsHere == false)
			{
				_status_code = 400;
				std::cerr << "parsingPost Error 400: Bad Request\n";
				throw MyExcep();
			}
		}
		initContentLength();
		parsingPOST_v2(buffer);
	}
	catch(std::exception &ex)
	{
		return ;
	}
}
