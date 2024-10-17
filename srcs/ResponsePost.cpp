#include "../headers/response.hpp"

void Response::handlePostResponse()
{
    std::cout << "\nCOMING HERE2\n";

    if (_contentType == "application/json")                         //json data submission
        handleDataSubmission();
    else if (_contentType == "application/x-www-form-urlencoded")   //form submission
        handleFormSubmission();
    else if (_contentType == "multipart/form-data")                 //file upload
        handleCGI(2);
    // else if (_contentType == "text/plain")
    //     filePath += ".txt";

    buildPostResponse();
}

//what to do with the data now ? 
void Response::handleDataSubmission()
{
    if (!storeJsonData()) {
        _status_code = "500 Internal Server Error";
        _responseBody = "Failed to store data";
        return ;
    }

    _status_code = "200 OK";
    _responseBody = "Data received succesfully";
}

bool Response::storeJsonData()
{
    std::ofstream outfile("config/data/data.json", std::ios::app);
    if (!outfile.is_open())
        return false;

    outfile << "{\n";
    for (std::map<std::string, std::string>::const_iterator it = _jsonParam.begin(); it !=_jsonParam.end(); it++) {
        outfile << " \"" << it->first << "\": \"" << it->second << "\"";
        if (it != --_jsonParam.end())
            outfile << ",";
        outfile << "\n";
    }
    outfile << "}\n";

    outfile.close();
    return true;
}

void Response::handleFormSubmission()
{
    if (!storeFormData()) {
        _status_code = "500 Internal Server Error";
        _responseBody = "Failed to store data";
        return ;
    }

    _status_code = "200 OK";
    _responseBody = "Data received succesfully";
}

bool Response::storeFormData()
{
    std::ofstream outfile("config/data/formdata.txt", std::ios::app);
    if (!outfile.is_open())
        return false;

    for (std::map<std::string, std::string>::const_iterator it = _urlParam.begin(); it != _urlParam.end(); it++) {
        outfile << it->first << ": " << it->second << "\n";
    }

    outfile.close();
    return true;
}

int Response::Post_Check_Errors()
{
    std::string dirPath = postParseDirPath();

    if (access(dirPath.c_str(), F_OK) && !access(dirPath.c_str(), W_OK))//no permission to write in subdirectory
        return -1;
    if (!access(_server.getUploadDir().c_str(), W_OK)) //no permissions to write in directory
        return -1;
    else if (access(_request.getPath().c_str(), F_OK) == 0) //file already exists
        return -2;
    if (_request.getBody().size() > static_cast<size_t>(_request.getMaxBodySize()))
        return -3;
    return 0;
}

// void Response::handleUploads()
// {
//     std::map<std::string, std::string>::const_iterator it = _formDataName.begin();
    
//     while (it != _formDataName.end() && it->first != "filename")
//         it++;
//     if (it->first == "filename") {
//         switch(Post_Check_Errors()) {
//             case -1:
//                 _status_code = "403 Forbidden";
//                 _responseBody = "Permission denied";
//                 break;
//             case -2:
//                 _status_code = "409 Conflict";
//                 _responseBody = "File already exists";
//                 break;
//             case -3:
//                 _status_code = "413 Payload Too Large";
//                 _responseBody = "File size exceeds the maximum allowed limit";
//                 break;
//             case 0:
//                 if (!createFile(/*it->second*/)) {
//                     _status_code = "500 Internal Server Error";
//                     _responseBody = "Failed to save file";
//                     break;
//                 }
//                 else {
//                     _status_code = "201 Created";
//                     _responseBody = "File uploaded succesfully";
//                     break;
//                 }
//         }
//     }
// }

// bool Response::createFile(std::string filename) //smaller one
// {
//     std::ofstream file(filename.c_str(), std::ios::binary);
//     if (!file.is_open()) {
//         std::cerr << "Error creating file " << filename << std::endl;
//         return false; 
//     }

//     file << _request.getBody();
//     if (!file.good()) {
//         std::cerr << "Error writing to file " << filename << std::endl;
//         return false;
//     }

//     file.close();
//     return true;
// }

bool Response::createFile(/*std::string filename*/)
{
    std::string dirPath = postParseDirPath();
    std::string filePath = postParseFilePath(); //or is it filename ? tocheck

    createDirectoryRecursive(dirPath);

    filePath += postHandleMultipart();

    std::ofstream file(filePath.c_str() /*, std::ios::binary*/);
    if (!file.is_open()) {
        std::cerr << "Error creating file" << std::endl;
        return false;
    }

    //std::string body = extractRequestBody();

    file << _request.getBody();

    if (!file.good())
        std::cerr << "Error writing to file at path: " << filePath << std::endl;

    file.close();
    return true;
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

//TO REDO
void Response::buildPostResponse()
{
    _response.str("");
    _response.clear();

    _response << "HTTP/1.1 " << _status_code << "\r\n";
    if (_status_code == "409 Conflict") {
        _responseBody = loadErrorPage("409.html");
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
