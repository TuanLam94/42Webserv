#include "../headers/response.hpp"

void Response::handlePostResponse()
{
    std::cout << "Request Path = " << _request.getPath() << std::endl;
    std::cout << "Upload dir Path = " << _server.getUploadDir() << std::endl;

    // switch (Post_Check()) {
    //     case -1:
    //         _status_code = "403 Forbidden";
    //         break;
    //     case -2:
    //         _status_code = "409 Conflict";
    //         break;
    //     case 0:
    //         _status_code = "201 Created";
    //         createFile();
    //         break;
    // }
    // std::cout << "STATUS CODE POST = " << _status_code << std::endl; //debug

    // buildPostResponse();
}

// void Response::createFile()
// {

// }


int Response::Post_Check()
{
    std::string path = _server.getUploadDir() + _request.getPath();
    std::cout << "POST PATH = " << path << std::endl; //debug

    if (!access(_server.getUploadDir().c_str(), W_OK)) //no permissions to write
        return -1;
    else if (access(path.c_str(), F_OK) == 0) //file already exists
        return -2;
    return 0;
}

void Response::buildPostResponse()
{
    std::string responseBody;

    _response << "HTTP/1.1 " << _status_code << std::endl;
    if (_status_code == "409 Conflict") {
        responseBody = loadErrorPage("409.html");
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
    _response << "Connection: close\r\n"; //keep alive ?
    _response << "\r\n";
    _response << responseBody;
    _response_str = _response.str();
}