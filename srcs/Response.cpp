#include "../headers/response.hpp"
// #include "../headers/request.hpp"

Response::Response(const Request& request)
{
    _method = request.getMethod();
    _path = request.getPath();
    _version = request.getVersion();
    _request = request;
    _server = request.getServer();
    _contentType = request.getContentType();
    _jsonParam = request.getJsonParam();
    _urlParam = request.getUrlParam();
    _formDataName = request.getFormDataName();
}

void Response::handleRequest()
{
    if (isErrorResponse())
        handleErrorResponse();
    else {
        if (_method == "GET")
            handleGetResponse();
        else if (_method == "POST")
            handlePostResponse();
        else if (_method == "DELETE")
            handleDeleteRequest();
        }
}

bool Response::isErrorResponse()
{
    if (_request.getStatusCode() == 400 || _request.getStatusCode() == 405 || _request.getStatusCode() == 505)
        return true;
    return false;
}

void Response::handleErrorResponse()
{
    switch(_request.getStatusCode()) {
        case 400:
            _responseBody = loadErrorPage("400.html");
            break;
        case 405:
            _responseBody = loadErrorPage("405.html");
            break;
        case 505:
            _responseBody = loadErrorPage("505.html");
            break;
    }

    _response << "HTTP/1.1" << _status_code << "\r\n";
    _response << "Content-Type: text/html\r\n";
    _response << "Content-Length: " << _responseBody.size() << "\r\n";
    _response << "Connection: keep-alive\r\n";
    _response << "\r\n";
    _response << _responseBody;
    _response_str = _response.str();
}

void Response::sendResponse(int fd)
{
    write(fd, _response_str.c_str(), _response_str.size());
}


void Response::runScript(std::string Lpath)
{
    char* const args[] = {const_cast<char*>(Lpath.c_str()), (char*)_path.c_str(), NULL};

    if (execv(Lpath.c_str(), args) == -1) {
        std::cerr << "Failed to execute CGI script: " << _path << std::endl;
        exit(1);
    }
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