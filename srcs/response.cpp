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
        handleGetRequest();
    // else if (_method == "POST")
    //     handlePostRequest();
    // else if (_method == "DELETE")
    //     handleDeleteRequest();
    // else if (_method == "PUT")
    //     handlePutRequest();
}    

// void Response::handlePostRequest()
// {

// }

void Response::handleGetRequest()
{
    std::cout << "STATUS CODE 0 = " << _status_code << std::endl;
    std::cout << "PATH = " << _path << std::endl;
    switch(GET_CheckFile()) {
        case -1:
            _status_code = "404 Not found";
            break;
        case -2:
            _status_code = "403 Forbidden";
            break;
        case 0:
            _status_code = "200 OK";
            break;
    }
    std::cout << "STATUS CODE 1 = " << _status_code << std::endl;

    buildGetResponse();
}


int Response::GET_CheckFile()
{
    // First, check if the file exists
    if (access(_path.c_str(), F_OK) != 0) {
        return -1; // file does not exist
    }
    // Then, check if it is a regular file
    if (!fileIsReg()) {
        return -2; // file is not a regular file
    }
    // Finally, check if it has read permissions
    if (access(_path.c_str(), R_OK) != 0) {
        return -2; // file exists but no read permission
    }
    return 0; // file exists, is a regular file and has permission
}

bool Response::fileIsReg()
{
    struct stat fileStat;
    if (stat(_path.c_str(), &fileStat) == 0) {
        if (S_ISREG(fileStat.st_mode))
            return true;
    }
    return false;
}

void Response::buildGetResponse()
{
    std::string responseBody;

    std::cout << "STATUS CODE = " << _status_code << std::endl;

    _response << "HTTP/1.1 " << _status_code << std::endl;
    if (_status_code == "404 Not found") {
        responseBody = loadErrorPage("404.html");
        _response << "Content-Type: text/html\r\n"; 
    } 
    else if (_status_code == "403 Forbidden") {
        responseBody = loadErrorPage("403.html");
        _response << "Content-Type: text/html\r\n";
    } 
    else {
        responseBody = _request.getBody();
        _response << "Content-Type: " << _request.getContentType() << "\r\n";
    }

    _response << "Content-Length: " << responseBody.size() << "\r\n";
    _response << "Connection: close\r\n";
    _response << "\r\n";
    _response << responseBody;
    _response_str = _response.str();
}

std::string Response::loadErrorPage(const std::string &errorPage)
{
    std::ifstream file(errorPage.c_str());
    if (!file)
        return "<html><body><h1>Error</h1><p>Could not load error page.</p></body></html>";
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
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