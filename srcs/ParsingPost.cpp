#include "../headers/request.hpp"


void	Request::checkJsonAccolade()
{
	unsigned long int	i = 0;
	int	count =  0;

	while (i < _body.size())
	{
		if (_body[i] == '"')
			count++;
		i++;
	}
	if ((count % 2) != 0)
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

bool	Request::checkFirstAccolade(size_t pos)
{
	if (_buffer[pos] != _body[0])
		return (false);
	return (true);
}


bool	Request::checkLastAccolade(size_t pos)
{
	if (_buffer[pos] != _body[_body.size() - 1])
		return (false);
	return (true);	
}

int	Request::checkIsDigit(size_t pos_start)
{
	while (pos_start < _buffer.size())
	{
		if (!(_buffer[pos_start] >= 48 && _buffer[pos_start] <= 57)
			&& _buffer[pos_start] != ','
			&& _buffer[pos_start] != 32
			&& _buffer[pos_start] != '}')
			return (-1);
		if (_buffer[pos_start] == ',' || _buffer[pos_start] == '}' || _buffer[pos_start] == 32)
			break ;
		pos_start++;
	}
	return (pos_start);
}

bool	isDigit(char str)
{
	if (str >= 48 && str <= 57)
		return (true);
	return (false);
}

int	Request::parserJsonBis(size_t pos_start, size_t pos_comma)
{
	std::string	key;
	std::string	value;
	size_t	pos_points = 0;
	int	index_pts = 0;
	int	index_g = 0;

	pos_points = findPosition(":", _buffer, pos_start);
	if (pos_points == std::string::npos)
		return (-1);
	else
	{
		while (pos_start < pos_comma)
		{
			if (_buffer[pos_start] == ':')
			{
				if (index_pts == 1)
					return (-1);
				index_pts = 1;
			}
			else if (_buffer[pos_start] == '\"')
				index_g = 1;
			else if (index_pts == 0 && index_g == 1)
			{
				while (_buffer[pos_start] != '\"')
				{
					key += _buffer[pos_start];
					pos_start++;
				}
				index_g = 0;
			}
			else if (index_pts == 1 && isDigit(_buffer[pos_start]) == true)
			{
				size_t pos = pos_start;
				pos_start = checkIsDigit(pos_start);
				if (static_cast<int>(pos_start) == -1)
					return (-1);
				else
				{
					while (pos < pos_start)
					{
						value += _buffer[pos];
						pos++;
					}
				}
				index_pts = 0;
			}
			else if (index_pts == 1 && index_g == 1)
			{
				while (_buffer[pos_start] != '\"')
				{
					value += _buffer[pos_start];
					pos_start++;
				}
				index_pts = 0;
				index_g = 0;
			}
			else if (_buffer[pos_start] != 32 && _buffer[pos_start] != '{'
				&& _buffer[pos_start] != '}')
				return (-1);
			pos_start++;
		}
		std::cout << key << std::endl;
		std::cout << value << std::endl;
		if (checkMap(key, _jsonParam.begin(), _jsonParam.end()) == false
			&& key.empty() == false && value.empty() == false)
			_jsonParam.insert(std::pair<std::string, std::string>(key, value));
		else
			return (-1);
	}
	pos_start++;
	return (pos_start);
}

void	Request::parserJson()
{
	size_t	pos_start;
	size_t	pos_end;
	size_t	pos_comma;
	int	index = 0;

	checkJsonAccolade();
	if (checkStatusCode() == false)
		return ;	
	pos_start = findPosition("{", _buffer, 0);
	if (pos_start == std::string::npos)
		throw MyExcep();
	else
	{
		if (checkFirstAccolade(pos_start) == false)
			throw MyExcep();
	}
	pos_end = findPosition("}", _buffer, 0);
	if (pos_end == std::string::npos)
		throw MyExcep();
	else
	{
		if (checkLastAccolade(pos_end) == false)
			throw MyExcep();
	}
	while (pos_start != pos_end)
	{
		pos_comma = findPosition(",", _buffer, pos_start);
		if (pos_comma != std::string::npos)
		{
			pos_start = parserJsonBis(pos_start, pos_comma);
			if (static_cast<int>(pos_start) == -1)
			{
				_status_code = 400;
				std::cerr << "parserJson1 Error 400: Bad Request.\n";
				throw MyExcep();
			}
		}
		else if (index == 0)
		{
			index = 1;
			pos_start = parserJsonBis(pos_start, pos_end);
			if (static_cast<int>(pos_start) == -1)
			{
				_status_code = 400;
				std::cerr << "parserJson2 Error 400: Bad Request.\n";
				throw MyExcep();
			}
			else
				break ;
		}
	}
}

void	Request::checkKeyUrl(std::string key)
{
	// unsigned long int i = 0;

	key = trim(key);
	if (key.empty() == true)
	{
		_status_code = 400;
		std::cerr << "checkKey 1 Error 400: Bad Request\n";
		throw MyExcep();
	}
	// while (i < key.size())
	// {
	// 	std::cout << key[i] << std::endl;
	// 	if ((!(key[i] >= 48 && key[i] <= 57)
	// 		&& !(key[i] >= 65 && key[i] <= 90)
	// 		&& !(key[i] >= 97 && key[i] <= 122)
	// 		&& !(key[i] == 45)
	// 		&& !(key[i] == 46)
	// 		&& !(key[i] == 95)
	// 		&& !(key[i] == 126)))
	// 	{ 
	// 		_status_code = 400;
	// 		std::cerr << "checkKey 2 Error 400: Bad Request\n";
	// 		throw MyExcep();
	// 	}
	// 	i++;
	// }
}

void	Request::checkValueUrl(std::string value)
{
	value = trim(value);
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
			{
				value += new_body[i];
			}
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
			&& !(body[i] == 10) && !(body[i] == 13)
			&& !(body[i] == 37) && !(body[i] == 38)
			&& !(body[i] == 43) && !(body[i] == 45)
			&& !(body[i] == 46) && !(body[i] == 61)
			&& !(body[i] == 95) && !(body[i] == 126)))
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

	// std::cout << _boundary << std::endl;
	new_boundary = _boundary + "--";
	// std::cout << new_boundary << std::endl;
	while (j < new_boundary.size())
	{
		final_boundary += buff[i];
		i++;
		j++;
	}
	// std::cout << final_boundary << std::endl;
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
		// std::cout << "key : " << key << std::endl;
		// std::cout << "value : " << value << std::endl;
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
			std::cerr << "formDataGetFileName Error 400: Bad Request.\n";
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
			i = pos_info + 2;
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

void	Request::constructBody()
{
	size_t	pos1 = 0;
	size_t	pos2 = 0;
	std::string	str;
	int	hexa = 0;
	char	*end;
	std::string	strFinal;
	unsigned long int	i = 0;

	while (true)
	{
		pos1 = findPosition("\r\n", _body, pos2);
		if (pos1 != std::string::npos)
		{
			for (; i < pos1; i++)
				str += _body[i];
			pos1 += 2;
			i += 2;
		}
		else
			throw MyExcep();
		hexa = strtol(str.c_str(), &end, 16);
		if (hexa == 0)
			break ;
		str.clear();
		pos2 = findPosition("\r\n", _body, pos1);
		if (pos2 != std::string::npos)
		{
			for (int j = 0; j < hexa; j++, i++)
				strFinal += _body[i];
			
			pos2 += 2;
			i += 2;
		}
		else
			throw MyExcep();
	}
	_body.clear();
	_body += strFinal;
}

void	Request::parsingPOST_v2(const std::string& buffer)
{
	size_t	pos;
	std::vector<std::pair<std::string, std::string> >::iterator it;
	std::vector<std::pair<std::string, std::string> >::iterator ite;
	std::vector<std::pair<std::string, std::string> >::iterator it1;
	std::vector<std::pair<std::string, std::string> >::iterator ite1;

	it1 = _headersHttp.begin();
	ite1 = _headersHttp.end();
	while (it1 != ite1)
	{
		if (it1->first == "Transfer-Encoding:")
		{
			if (it1->second == "chunked")
			{				
				_isChunk = true;
				constructBody();
				std::cout << _body << std::endl;
			}
		}
		it1++;
	}
	it = _headersHttp.begin();
	ite = _headersHttp.end();
	while (it != ite)
	{
		if (it->first == "Content-Type:")
		{
			pos = it->second.std::string::find("multipart/form-data");
			if (it->second == "application/json") // json utiliser pour la creation de ressource
			{
				_contentType = it->second;
				std::cout << it->second << std::endl;
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
			else if (it->second == "text/plain")
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


bool	checkLength(std::string len)
{
	size_t	i = 0;

	while (i < len.size())
	{
		if (!(len[i] >= 48 && len[i] <= 57))
		{
			return (true);
		}
		i++;
	}
	return (false);
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
			if (checkLength(it->second) == true)
			{
				_status_code = 400;
				std::cerr << "initContentLength1 Error 400: Bad Request\n";
				throw MyExcep();
			}
			std::istringstream ss(it->second);
			ss >> _contentLength;
			if (_contentLength != _body.size())
			{
				_status_code = 400;
				std::cerr << "initContentLength2 Error 400: Bad Request\n";
				throw MyExcep();
			}
			if (static_cast<int>(_contentLength) > _max_client_body_size)
			{
				std::cout << _contentLength << std::endl;
				std::cout << _max_client_body_size << std::endl;
 				_status_code = 413;
				std::cerr << "initContentLength3 Error 413 : Payload Too Large.\n";
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
