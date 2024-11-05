#include "../headers/response.hpp"
// #include "../headers/request.hpp"

Response::Response(const Request& request)
{
    std::cout << "CREATING RESPONSE\n";
    _method = request.getMethod();
    _path = request.getPath();
    _version = request.getVersion();
    _boundary_full = request.getBoundary();
    setStatusCode(request);
    _contentType = request.getContentType();
    _server = request.getServer();
    _formDataName = request.getFormDataName();
	_FormDataFileName = request.getFormDataFileName();
    _jsonParam = request.getJsonParam();
    _urlParam = request.getUrlParam();
    _cgi_type = responseSetCgiType();
    _contentLength = request.getContentLength();
    _bodyVector = request.getMyV();
    _request = request;
}

void Response::setStatusCode(const Request& request)
{
	switch (request.getStatusCode()) {
		case 400:
			_status_code = "400 Bad Request";
			break;
		case 404:
			_status_code = "404 Not Found";
			break;
		case 415:
			_status_code = "415 Unsupported Media Type";
			break;
		case 409:
			_status_code = "409 Conflict";
			break;
		case 403:
			_status_code = "403 Forbidden";
			break;
		case 504:
			_status_code = "504 Gateway Timeout";
			break;
		case 500:
			_status_code = "500 Internal Server Error";
			break;
		case 413:
			_status_code = "413 Content Too Large";
			break;
		case 414:
			_status_code = "414 URI Too Long";
			break;
		case 405:
			_status_code = "405 Method Not Allowed";
			break;
		case 505:
			_status_code = "505 HTTP Version Not Supported";
			break;
	}
}

void Response::handleRequest()
{
    if (isErrorResponse())
        return ;
    else
    {
        if (_method == "GET")
            handleGetResponse();
        else if (_method == "POST")
            handlePostResponse();
        else if (_method == "DELETE")
            handleDeleteResponse();
        }
}

bool Response::isErrorResponse()
{
    if (_request.getStatusCode() == 400 || _request.getStatusCode() == 405
        || _request.getStatusCode() == 413 || _request.getStatusCode() == 414
        || _request.getStatusCode() == 500 || _request.getStatusCode() == 505
        || _request.getStatusCode() == 404 || _request.getStatusCode() == 415
        || _request.getStatusCode() == 409 || _request.getStatusCode() == 403
        || _request.getStatusCode() == 504 || _request.getStatusCode() == 431)
        return true;
    return false;
}

void Response::sendResponse(int fd)
{
    write(fd, _response_str.c_str(), _response_str.size());
}


void Response::runScript(std::string Lpath)
{
    char* const args[] = {const_cast<char*>(Lpath.c_str()), (char*)_path.c_str(), NULL};

    close(_server.getEpollFd());
    if (execv(Lpath.c_str(), args) == -1) {
        std::cerr << "Failed to execute CGI script: " << _path << std::endl;
        close(_server.getEpollFd());
        exit(500);
    }
}

int Response::responseSetCgiType()
{
    size_t lastSlash = _path.find_last_of('/');

    if (lastSlash != std::string::npos) {
        std::string filename = _path.substr(0, lastSlash);
        if (_path.find(".py") != std::string::npos)
            return (1);
        else if (_path.find(".sh") != std::string::npos)
            return (2);
    }

    return 0;
}


void Response::buildResponse()
{
    _response.str("");
    _response.clear();

    // _response << "HTTP/1.1 " << _status_code << "/r/n";
    if (isErrorCode())
        handleErrorResponse();
    else if (_method == "GET")
        buildGetResponse();
    else if (_method == "POST")
        buildPostResponse();
    else if (_method == "DELETE")
        buildDelResponse();
}

bool Response::isErrorCode()
{
	// std::cout << "STATUS CODE = " << _status_code << std::endl;

    if (_status_code == "404 Not Found" || _status_code == "415 Unsupported Media Type" || _status_code == "409 Conflict"
        || _status_code == "403 Forbidden" || _status_code == "504 Gateway Timeout" || _status_code == "400 Bad Request"
        || _status_code == "500 Internal Server Error" || _status_code == "413 Content Too Large" || _status_code == "414 URI Too Long"
        || _status_code == "405 Method Not Allowed" || _status_code == "505 HTTP Version Not Supported")
        return true;
    return false;
}

void Response::handleErrorResponse()
{
    char *end;

    int errorCode = strtol(_status_code.substr(0, 3).c_str(), &end, 10);
	_response << "HTTP/1.1 " << _status_code << "\r\n";
    switch (errorCode) {
        case 404:
            _responseBody = loadErrorPage("404.html");
            _response << "Content-Type: text/html\r\n";
            break;
        case 415:
            _responseBody = loadErrorPage("415.html");
            _response << "Content-Type: text/html\r\n";
            break;
        case 409:
            _responseBody = loadErrorPage("409.html");
            _response << "Content-Type: text/html\r\n";
            break;
        case 403:
            _responseBody = loadErrorPage("403.html");
            _response << "Content-Type: text/html\r\n";
            break;
        case 504:
            _responseBody = loadErrorPage("504.html");
            _response << "Content-Type: text/html\r\n";
            break;
        case 400:
            _responseBody = loadErrorPage("400.html");
            _response << "Content-Type: text/html\r\n";
            break;
        case 500:
            _responseBody = loadErrorPage("500.html");
            _response << "Content-Type: text/html\r\n";
            break;
        case 413:
            _responseBody = loadErrorPage("413.html");
            _response << "Content-Type: text/html\r\n";
            break;
        case 414:
            _responseBody = loadErrorPage("414.html");
            _response << "Content-Type: text/html\r\n";
            break;
        case 405:
            _responseBody = loadErrorPage("405.html");
            _response << "Content-Type: text/html\r\n";
            break;
        case 505:
            _responseBody = loadErrorPage("505.html");
            _response << "Content-Type: text/html\r\n";
            break;
        case 431:
            _responseBody = loadErrorPage("431.html");
            _response << "Content-Type: text/html\r\n";
            break;
    }
    _response << "Content-Length: " << _responseBody.size() << "\r\n";
    _response << "Connection: keep-alive\r\n";
    _response << "\r\n";
    _response << _responseBody;
    _response_str = _response.str();
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

std::string Response::getContentType() const
{
    return _contentType;
}

std::map<std::string, std::string> Response::getFormDataFileName() const
{
	return _FormDataFileName;
}

unsigned long int   Response::getContentLength()const
{
    return (_contentLength);
}

//-------------------------------------------SETTERS----------------------------------

void Response::setServer(Server& server)
{
    _server = server;
}

void Response::setStatusCode(std::string statuscode)
{
    _status_code = statuscode;
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