#include "../headers/response.hpp"

void Response::handleGetResponse()
{
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
    std::cout << "STATUS CODE GET = " << _status_code << std::endl;

    buildGetResponse();
}


int Response::GET_CheckFile()
{
    if (access(_path.c_str(), F_OK) != 0)
        return -1; // file does not exist
    if (!fileIsReg())
        return -2; // file is not a regular file
    if (access(_path.c_str(), R_OK) != 0)
        return -2; // file exists but no read permission
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
    _response.str("");
    _response.clear();

    _response << "HTTP/1.1 " << _status_code << "\r\n";
    if (_status_code == "404 Not found") {
        _responseBody = loadErrorPage("404.html");
        _response << "Content-Type: text/html\r\n"; 
    } 
    else if (_status_code == "403 Forbidden") {
        _responseBody = loadErrorPage("403.html");
        _response << "Content-Type: text/html\r\n";
    } 
    else {
        _responseBody = _request.getBody();
        _response << "Content-Type: " << _request.getContentType() << "\r\n";
    }

    _response << "Content-Length: " << _responseBody.size() << "\r\n";
    _response << "Connection: keep-alive\r\n"; //keep alive ?
    _response << "\r\n";
    _response << _responseBody;
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