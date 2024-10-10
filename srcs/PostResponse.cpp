#include "../headers/response.hpp"

//TOFINISH
// Parser d'abord le header pour svoir si la methode post :
    //a un body destine a un cgi -> handle cgi
    //un upload vers un fichier non existant
    // un upload vers un fichier deja existant

void Response::handlePostResponse()
{
    std::cout << "Request Path = " << _request.getPath() << std::endl;
    std::cout << "Upload dir Path = " << _server.getUploadDir() << std::endl;

    switch (Post_Check()) {
        case -1:
            _status_code = "403 Forbidden";
            break;
        case -2:
            _status_code = "409 Conflict";
            break;
        case 0:
            _status_code = "201 Created";
            createFile();
            break;
    }
    std::cout << "STATUS CODE POST = " << _status_code << std::endl; //debug

    // buildPostResponse();
}

int Response::Post_Check()
{
    std::string dirPath = postParseDirPath();

    if (access(dirPath.c_str(), F_OK) && !access(dirPath.c_str(), W_OK))
        return -1;
    if (!access(_server.getUploadDir().c_str(), W_OK)) //no permissions to write
        return -1;
    else if (access(_request.getPath().c_str(), F_OK) == 0) //file already exists
        return -2;
    return 0;
}

std::string Response::postParseDirPath()
{
    std::string path = _request.getPath();
    size_t lastSlash = path.find_last_of('/');

    if (lastSlash != std::string::npos)
        return path.substr(0, lastSlash);

    return "";
}

std::string Response::postParseFilePath()
{
    std::string path = _request.getPath();
    size_t lastSlash = path.find_last_of('/');

    if (lastSlash != std::string::npos)
        return path.substr(lastSlash);
    return path;
}

void Response::createFile()
{
    std::string dirPath = postParseDirPath();
    std::string filePath = postParseFilePath();

    createDirectoryRecursive(dirPath);

    if (_contentType == "application/json")
        filePath += ".json";
    else if (_contentType == "text/plain")
        filePath += ".txt";
    else if (_contentType == "application/x-www-form-urlencoded")
        filePath += ".txt";
    else if (_contentType == "multipart/form-data")
        filePath += postHandleMultipart();

    std::ofstream file(filePath.c_str());
    if (!file)
        std::cerr << "Error creating file" << std::endl;

    // std::string body = extractRequestBody();

    file << _request.getBody();

    if (!file.good())
        std::cerr << "Error writing to file at path: " << filePath << std::endl;

    file.close();
}

std::string Response::postHandleMultipart()
{
    std::map<std::string, std::string> map = _request.getFormDataName();
    std::map<std::string, std::string>::iterator it = map.begin();

    while (it != map.end() && it->first != "filename")
        it++;
    if (it->first == "filename")
        return(extractExtension(it->second));
    return "";
}

std::string Response::extractExtension(std::string file)
{
    size_t pos = file.find(".");
    return file.substr(pos);
}

// std::string Response::extractRequestBody()
// {
//     size_t pos = _request.getBody().find("\r\n\r\n");
//     if (pos != std::string::npos)
//         return _request.getBody().substr(pos + 4);

//     return _request.getBody();
// }

void Response::createDirectoryRecursive(const std::string& path)
{
    std::string currentPath;
    std::istringstream pathStream(path);
    std::string buffer;

    while (std::getline(pathStream, buffer, '/')) {
        if (buffer.empty())
            continue;
        if (!currentPath.empty())
            currentPath += "/";
        currentPath += buffer;

        if (!createDirectory(currentPath)) {
            if (errno != EEXIST)
                std::cerr << "Failed to create directory " << currentPath << std::endl;
        }
    }
}

bool Response::createDirectory(const std::string& path)
{
    if (mkdir(path.c_str(), 0755) == 0)
        return true;
    if (errno == EEXIST)
        return true;
    std::cerr << "Error creating directory " << path << std::endl;
    return false;
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