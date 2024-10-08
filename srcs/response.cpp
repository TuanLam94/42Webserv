#include "../headers/response.hpp"
// #include "../headers/request.hpp"

Response::Response(const Request& request)
{
    _method = request.getMethod();
    _path = request.getPath();
    _version = request.getVersion();
    _request = request;
    handleRequest();
}

void Response::handleRequest()
{
    if (_method == "GET")
        handleGetResponse();
    // else if (_method == "POST")
    //     handlePostRequest();
    // else if (_method == "DELETE")
    //     handleDeleteRequest();
    // else if (_method == "PUT")
    //     handlePutRequest();
}    

void Response::sendResponse(int fd)
{
    write(fd, _response_str.c_str(), _response_str.size());
}

//--------------------------------------------GETTERS--------------------------------------------

std::string Response::getMethod() const
{
    return _method;
}

std::string Response::getPath() const
{
    return _path;
}

std::string Response::getVersion() const
{
    return _version;
}

std::string Response::getStatusCode() const
{
    return _status_code;
}

std::string Response::getResponseStr() const
{
    return _response_str;
}

Request Response::getRequest() const
{
    return _request;
}

//-------------------------------------------SETTERS----------------------------------

void Response::setServer(Server& server)
{
    _server = server;
}


//---------------------------------------------UTILS-----------------------------------------------

Response::Response(const Response& copy)
{
	*this = copy;
}

Response& Response::operator=(const Response& other)
{
	if (this != &other) {
		_method = other.getMethod();
		_path = other.getPath();
		_version = other.getVersion();
		_request = other.getRequest();
        _status_code = other.getStatusCode();
        _response_str = other.getResponseStr();
	}
    return *this;
}

void Response::printResponse()
{
    std::cout << "----RESPONSE----" << std::endl;
    std::cout << "Method = " << getMethod() << std::endl;
    std::cout << "Path = " << getPath() << std::endl;
    std::cout << "Version = " << getVersion() << std::endl;
    std::cout << "Status Code = " << getStatusCode() << std::endl;
    std::cout << "Response string = " << getResponseStr() << std::endl;
}