#include "../headers/request.hpp"


void	Request::checkJsonAccolade()
{
	int	count =  0;

	for (size_t i = 0; i < _my_v.size(); i++)
	{
		if (_my_v[i] == '"')
			count++;
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


int	Request::checkIsDigit(size_t pos_start)
{
	while (pos_start < _my_v.size())
	{
		if (!(_my_v[pos_start] >= 48 && _my_v[pos_start] <= 57)
			&& _my_v[pos_start] != ','
			&& _my_v[pos_start] != 32
			&& _my_v[pos_start] != '}')
			return (-1);
		if (_my_v[pos_start] == ',' || _my_v[pos_start] == '}' || _my_v[pos_start] == 32)
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

	pos_points = findPositionVec(":", pos_start);
	if (static_cast<int>(pos_points) == -1)
	{
		return (-1);
	}
	while (pos_start < pos_comma)
	{
		if (_my_v[pos_start] == ':')
		{
			if (index_pts == 1)
			{
				return (-1);
			}
			index_pts = 1;
		}
		else if (_my_v[pos_start] == '\"')
			index_g = 1;
		else if (index_pts == 0 && index_g == 1)
		{
			while (_my_v[pos_start] != '\"')
			{
				key += _my_v[pos_start];
				pos_start++;
			}
			index_g = 0;
		}
		else if (index_pts == 1 && index_g == 1)
		{
			while (_my_v[pos_start] != '\"')
			{
				value += _my_v[pos_start];
				pos_start++;
			}
			index_pts = 0;
			index_g = 0;
		}
		else if (index_pts == 1 && isDigit(_my_v[pos_start]) == true)
		{
			size_t pos = pos_start;
			pos_start = checkIsDigit(pos_start);
			if (static_cast<int>(pos_start) == -1)
			{
				return (-1);
			}
			else
			{
				while (pos < pos_start)
				{
					value += _my_v[pos];
					pos++;
				}
			}
			index_pts = 0;
		}
		else if (_my_v[pos_start] != 32 && _my_v[pos_start] != '{'
			&& _my_v[pos_start] != '}')
		{
			// std::cout << _my_v[pos_start] << std::endl;
			return (-1);
		}
		pos_start++;
	}
	// std::cout << key << std::endl;
	// std::cout << value << std::endl;
	if (checkMap(key, _jsonParam.begin(), _jsonParam.end()) == false
		&& key.empty() == false && value.empty() == false)
		_jsonParam.insert(std::pair<std::string, std::string>(key, value));
	else
	{
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
	pos_start = findPositionVec("\r\n\r\n", 0);
	if (pos_start == -1)
	{
		std::cerr << "parserJson19 Error 400: Bad Request.\n";
		throw MyExcep();
	}
	pos_start += 4;
	pos_start = findPositionVec("{", pos_start);
	if (pos_start == -1)
	{
		_status_code = 400;
		std::cerr << "parserJson4 Error 400: Bad Request.\n";
		throw MyExcep();
	}
	pos_end = findPositionVec("}", 0);
	if (static_cast<int>(pos_end) == -1)
	{
		_status_code = 400;
		std::cerr << "parserJson6 Error 400: Bad Request.\n";
		throw MyExcep();
	}
	size_t pos_tmp = pos_end + 1;
	while (pos_tmp < _my_v.size())
	{
		if (_my_v[pos_tmp] != '\r' && _my_v[pos_tmp] != '\n' && _my_v[pos_tmp] != 32)
		{
			_status_code = 400;
			std::cerr << "parserJson6 Error 400: Bad Request.\n";
			throw MyExcep();				
		}
		pos_tmp++;
	}
	while (pos_start != pos_end)
	{
		pos_comma = findPositionVec(",", pos_start);
		if (static_cast<int>(pos_comma) != -1)
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
	// std::cout << "key : " << key << std::endl;
	// std::cout << "value : " << value << std::endl;
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

int	Request::checkUrlEncoded()
{
	unsigned long int i = 0;
	
	i = findPositionVec("\r\n\r\n", 0) + 4;
	while (i < _my_v.size())
	{
		if ((!(_my_v[i] >= 48 && _my_v[i] <= 57)
			&& !(_my_v[i] >= 65 && _my_v[i] <= 90)
			&& !(_my_v[i] >= 97 && _my_v[i] <= 122)
			&& !(_my_v[i] == 10) && !(_my_v[i] == 13)
			&& !(_my_v[i] == 37) && !(_my_v[i] == 38)
			&& !(_my_v[i] == 43) && !(_my_v[i] == 45)
			&& !(_my_v[i] == 46) && !(_my_v[i] == 61)
			&& !(_my_v[i] == 95) && !(_my_v[i] == 126)))
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
	
	_status_code = checkUrlEncoded();
	if (checkStatusCode() == false)
		throw MyExcep();
	while (i < _my_v.size())
	{
		if (_my_v[i] == 38) // &
		{
			parserUrlencoded_bis(new_body);
			new_body.clear();
		}
		else
			new_body += _my_v[i];
		i++;
	}
	parserUrlencoded_bis(new_body);
}

bool	Request::parserFormData_help(size_t i)
{
	std::string	new_boundary;
	std::string	final_boundary;
	unsigned long int j = 0;

	new_boundary = _boundary + "--";
	while (j < new_boundary.size() && i < _my_v.size())
	{
		final_boundary += _my_v[i];
		i++;
		j++;
	}
	// std::cout << new_boundary << std::endl;
	// std::cout << final_boundary << std::endl;
	if (new_boundary == final_boundary)
		return (true);	
	return (false);
}

void	Request::formDataGetName(size_t pos)
{
	std::string	key;
	std::string	value;

	pos = pos + 6;
	while (_my_v[pos] != '\"')
	{
		key += _my_v[pos];
		pos++;
	}
	pos = findPositionVec("\r\n\r\n", pos);
	if (static_cast<int>(pos) == -1)
	{
		_status_code = 400;
		std::cerr << "parserJson6 Error 400: Bad Request.\n";
		throw MyExcep();
	}
	if (static_cast<int>(pos) != -1)
	{
		pos += 4;
		size_t pos1 = findPositionVec("\r\n", pos);
		if (static_cast<int>(pos1) == -1)
		{
			_status_code = 400;
			std::cerr << "formDataGetName Error 400: Bad Request.\n";
			throw MyExcep();
		}
		else
		{
			while (pos < _my_v.size() && pos < pos1)
			{
				value += _my_v[pos];
				pos++;
			}
		}
		// std::cout << "Name key : " << key << std::endl;
		// std::cout << "Name value : " << value << std::endl;
		_FormDataName.insert(std::pair<std::string, std::string>(key, value));
	}
}

void	Request::formDataGetFilename(size_t pos)
{
	std::string	key;
	std::string	value;

	pos = pos + 10;
	while (_my_v[pos] != '\"')
	{
		key += _my_v[pos];
		pos++;
	}
	pos = findPositionVec("\r\n\r\n", pos);
	if (static_cast<int>(pos) == -1)
	{
		_status_code = 400;
		std::cerr << "parserJson6 Error 400: Bad Request.\n";
		throw MyExcep();
	}
	if (static_cast<int>(pos) != -1)
	{
		pos += 4;
		size_t pos1 = findPositionVec("\r\n", pos);
		if (static_cast<int>(pos1) == -1)
		{
			_status_code = 400;
			std::cerr << "formDataGetFileName Error 400: Bad Request.\n";
			throw MyExcep();
		}
		else
		{
			while (pos < _my_v.size() && pos < pos1)
			{
				value += _my_v[pos];
				pos++;
			}
		}
		// std::cout << "FileName key : " << key << std::endl;
		// std::cout << "FileName value : " << value << std::endl;
		_FormDataFilename.insert(std::pair<std::string, std::string>(key, value));
	}
}

bool	Request::checkIfNext(size_t i)
{
	size_t pos;

	pos = findPositionVec(_boundary, i);
	if (static_cast<int>(pos) == -1)
		return (false);
	return (true);
}

void	Request::parserFormData_bis(size_t pos)
{
	std::string	boundary;
	size_t	pos_b = 0;
	size_t	pos_info = 0;
	size_t	i = 0;

	boundary = _boundary + "--";
	while (true)
	{
		pos_b = findPositionVec(_boundary, i);
		if (static_cast<int>(pos_b) != -1)
			i = pos_b;
		else
		{
			std::cout << "parserFormData_bis1 Error 400 : Bad Request.\n";
			throw MyExcep();
		}
		pos_info = findPositionVec("Content-Disposition: form-data; ", i);
		if (static_cast<int>(pos_info) == -1)
		{
			_status_code = 400;
			std::cerr << "parserFormData_Bis Error 400: Bad Request.\n";
			throw MyExcep();
		}
		i = pos_info;
		pos_info = findPositionVec("name=\"", i);
		if (static_cast<int>(pos_info) != -1)
			formDataGetName(pos_info);
		pos_info = findPositionVec("filename=\"", i);
		if (static_cast<int>(pos_info) != -1)
			formDataGetFilename(pos_info);
		pos_info = findPositionVec("\r\n\r\n", i);
		if (static_cast<int>(pos_info) != -1)
			i = pos_info + 5;
		pos_info = findPositionVec(_boundary, i);
		if (static_cast<int>(pos_info) != -1)
			i = pos_info + _boundary.size();
		if (checkIfNext(i) == false)
			break ;
	}
}


void	Request::parserFormData()
{
	size_t	pos_b = 0;
	size_t	pos_end = 0;
	size_t	i = 0;
	size_t	j = 0;
	std::string	boundary = "boundary=";

	pos_b = findPositionVec(boundary, 0);
	if (pos_b == -1)
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
	pos_end = findPositionVec("\r\n", pos_b);
	while (pos_b < _my_v.size() && pos_b < pos_end)
	{
		_boundary += _my_v[pos_b];
		pos_b++;
	}
	parserFormData_bis(pos_b);
}

void	Request::constructBody()
{
	size_t	pos1 = 0;
	size_t	pos2 = 0;
	size_t	pos3 = 0;
	std::string	str;
	size_t	hexa = 0;
	char	*end;
	std::string	strFinal;

	pos1 = findPositionVec("\r\n", pos2);
	if (static_cast<int>(pos1) != -1)
	{
		for (size_t i = 0; i < pos1; i++)
			str += _my_v[i];
		pos1 += 2;
	}
	else
		throw MyExcep();
	pos2 = pos1;
	hexa = strtol(str.c_str(), &end, 16);
	str.clear();
	pos3 = findPositionVec("0\r\n\r\n", 0);
	if (static_cast<int>(pos3) == -1)
	{
		_status_code = 400;
		std::cout << "constructBody Error 400: Bad Request.\n";
		throw MyExcep();
	}
	while (pos2 < pos3)
	{
		size_t i = 0;
		while (i < hexa && pos2 < pos3)
		{
			strFinal += _my_v[pos2];
			i++;
			pos2++;
		}
		pos2 += 2;
		pos1 = findPositionVec("\r\n", pos2);
		if (static_cast<int>(pos1) != -1)
		{
			for (size_t i = pos2; i < pos1; i++)
				str += _my_v[i];
			pos1 += 2;
		}
		else
			throw MyExcep();
		hexa = strtol(str.c_str(), &end, 16);
		if (hexa == 0)
			break ;
		str.clear();
		pos2 = pos1;
	}
	_my_v.clear();
	for (size_t i = 0; i < strFinal.size(); i++)
		_my_v.push_back(strFinal[i]);
}

void	Request::setBoundaryFull()
{
	size_t	pos1;
	size_t	pos2;

	pos1 = findPositionVec("Content-Type:", 0);
	if (pos1 != -1)
	{
		pos2 = findPositionVec("\r\n", pos1);
		if (pos2 != -1)
		{
			size_t i = pos1;
			while (i < pos2)
			{
				_boundary_full += _my_v[i];
				i++;
			}
		}
	}
}

void	Request::parsingPOST_v2()
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
				constructBody(); // reiquete chunk reconstruire _my_v
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
			
			pos = it->second.std::string::find("multipart/form-data;");
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
				_contentType = it->second;
				setBoundaryFull();
				parserFormData();
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

	while (i < _contentLength && i < _my_v.size())
	{
		_dataBrut += _my_v[i];
		i++;
	}
	std::cout << _dataBrut << std::endl;
	
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
	size_t i = 0;

	size_t pos = findPositionVec("\r\n\r\n", 0);
	if (pos == -1)
	{
		std::cerr << "InitContentLength4 Error 400: Bad Request.\n";
		throw MyExcep();
	}
	else
	{
		pos += 4;
		for (; pos < _my_v.size(); pos++, i++);
	}
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
			std::cout << _contentLength << std::endl;
			std::cout << i << std::endl;
			if (_contentLength != i)
			{
				_status_code = 400;
				std::cerr << "initContentLength2 Error 400: Bad Request\n";
				throw MyExcep();
			}
			if (static_cast<int>(_contentLength) > _max_client_body_size)
			{
 				_status_code = 413;
				std::cerr << "initContentLength3 Error 413 : Content Too Large.\n";
				throw MyExcep();
			}
		}
		it++;
	}
}

void	Request::parsingPOST_v1(Server i)
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
		parsingPOST_v2();
	}
	catch(std::exception &ex)
	{
		return ;
	}
}
