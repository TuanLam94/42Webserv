#include "../headers/response.hpp"

void Response::handleDeleteResponse()
{
    switch(DEL_CheckFile()) {
        case -1:
            _status_code = "404 Not found";
            break;
        case -2:
            _status_code = "403 Forbidden";
            break;
        case 0:
            int status = DeleteFile();
            if (status == 0) //Success
                _status_code = "204 No Content";
            else if (status == -1) //Could not delete
                _status_code = "500 Internal Server Error";
            else if (status == -2) //Directory is not empty
                _status_code = "409 Conflict";
            else if (status == -3) //Not a regular file nor directory
                _status_code = "415 Unsupported Media Type";
            else if (status == -4) // file does not exist. Not sure if really needed
                _status_code = "404 Not found";
            break;
    }

    buildDelResponse();
}


int Response::DEL_CheckFile()
{
    if (access(_path.c_str(), F_OK) != 0) //file does not exists
        return -1;
    if (access(_path.c_str(), W_OK) != 0) //file exists but no permission to write
        return -2;
    return 0;
}

int Response::DeleteFile()
{
    struct stat fileStat;
    if (stat(_path.c_str(), &fileStat) == 0) {
        if (S_ISREG(fileStat.st_mode)) {
            if (unlink(_path.c_str()) == 0)
                return 0;                           //success deleting file
            else
                return -1;                          //500 error deleting file
        }

        else if (S_ISDIR(fileStat.st_mode)) {
            if (isDirectoryEmpty())
                if (rmdir(_path.c_str()) == 0)
                    return 0;                       //success deleting directory
                else
                    return -1;                      //500 error deleting directory
            else
                return -2;                          //409 Directory is not empty
        }

        else
            return -3;                              // not a directory nor regular file
    }
    return -4;                                      // file does not exist
}

bool Response::isDirectoryEmpty()
{
    DIR* dir = opendir(_path.c_str());
    if (dir == NULL)
        return false;
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (std::strcmp(entry->d_name, ".") != 0 && std::strcmp(entry->d_name, "..") != 0) {
            closedir(dir);
            return false;
        }
    }
    closedir(dir);
    return true;
}

void Response::buildDelResponse()
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
    else if (_status_code == "500 Internal Server Error") {
        _responseBody = loadErrorPage("500.html");
        _response << "Content-Type: text/html\r\n";
    }
    else if (_status_code == "409 Conflict") {
        _responseBody = loadErrorPage("409.html");
        _response << "Content-Type: text/html\r\n";
    }
    else if (_status_code == "415 Unsupported Media Type") {
        _responseBody = loadErrorPage("415.html");
        _response << "Content-Type: text/html\r\n";
    }

    _response << "Content-Length: " << _responseBody.size() << "\r\n";
    _response << "Connection: keep-alive\r\n";
    _response << "\r\n";
    _response << _responseBody;
    _response_str = _response.str();
}
