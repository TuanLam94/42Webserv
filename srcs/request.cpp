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

void	Request::open_file_GET(Server i, const std::string& buffer)
{
	std::stringstream ss(buffer);
	std::vector<std::string> routes;
	int j = 0;
	ss >> _method >> _path >> _version;
	routes = i.getRoutes();
	while (j < 3)
	{
		if (_path == "/" + routes[j])
			break ;
		j++;
	}
	_path = "./config/routes/" + routes[j];
	_input.open(_path.c_str());
	if (!_input.is_open())
	{
		std::cerr << "Can't open input\n";
		exit (1);
	}
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