#include "../headers/response.hpp"

void Response::handlePostResponse()
{
    if (_contentType == "application/json")                         //json data submission
        handleDataSubmission();
    else if (_contentType == "application/x-www-form-urlencoded")   //form submission
        handleFormSubmission();
    else if (_contentType == "multipart/form-data")					//file upload
        handleCGIPost();
    // else if (_contentType == "text/plain")
    //     filePath += ".txt";
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

void Response::handleCGIPost()
{
    // std::string tmp = _boundary;
    // _boundary.clear();
    // size_t i = 2;
    // while (i < tmp.size())
    // {
    //     _boundary += tmp[i];
    //     i++;
    // }
    std::ostringstream oss;
    oss << _request.getBody().size();
    std::string content_length_str = oss.str();

	std::map<std::string, std::string>::const_iterator it = getFormDataFileName().begin();

    // std::cout << "CONTENT_LENGTH = " << content_length_str.c_str() << std::endl;
    // std::cout << "CONTENT_TYPE = " << _boundary_full.c_str() << std::endl;
    // std::cout << "QUERY_STRING = " <<  _request.getQueryString().c_str() << std::endl;
    // std::cout << "PATH_INFO = " <<  _request.getPathInfo().c_str() << std::endl;
	// std::cout << "FILE_NAME = " <<  it->first.c_str() << std::endl;
	// std::cout << "FILE_BODY = " <<  it->second.c_str() << std::endl;

	
    setenv("REQUEST_METHOD", "POST", 1);
    setenv("CONTENT_LENGTH", content_length_str.c_str(), 1);
    setenv("CONTENT_TYPE", _boundary_full.c_str(), 1);
    setenv("QUERY_STRING", _request.getQueryString().c_str(), 1);
    setenv("PATH_INFO", _request.getPathInfo().c_str(), 1);
	setenv("FILE_NAME", it->first.c_str(), 1);
	setenv("FILE_BODY", it->second.c_str(), 1);	//TO CHANGE FOR BINARY ?

    int pipefd[2], bodyPipe[2];
    if (pipe(pipefd) == -1 || pipe(bodyPipe) == -1) {
        std::cerr << "Failed to create pipes.\n";
        return;
    }

    pid_t pid = fork();
    if (pid == -1) {
        std::cerr << "Failed to fork process.\n";
        close(pipefd[0]); close(pipefd[1]);
        close(bodyPipe[0]); close(bodyPipe[1]);
        return;
    }

    if (pid == 0) {
        dup2(pipefd[1], STDOUT_FILENO);
        dup2(bodyPipe[0], STDIN_FILENO);
        close(pipefd[0]); close(pipefd[1]);
        close(bodyPipe[0]); close(bodyPipe[1]);

		if (_cgi_type == 1)
            runScript("/usr/bin/python3");
        else if (_cgi_type == 2)
            runScript("/bin/bash");
        else {
            std::cerr << "Unsupported CGI type\n";
            close(_server.getEpollFd());
            exit(415);
        }

        std::cerr << "execv failed\n";
        close(_server.getEpollFd());
        exit(500);
    }
	else {
		close(pipefd[1]);
        close(bodyPipe[0]);

        write(bodyPipe[1], _request.getBody().c_str(), _request.getBody().size());
        close(bodyPipe[1]);

		time_t start_time = time(NULL);
		const time_t timeout = 60;
		int status;
		pid_t result = 0;
		bool timedout = false;

        char buffer[1024];
        ssize_t bytesRead;
        _responseBody.clear();

		while (result == 0) {
			result = waitpid(pid, &status, WNOHANG);
			if (result == -1)
				break;
			if (result == 0) {
				if (time(NULL) - start_time >= timeout) {
					timedout = true;
					break;
				}
				usleep(100000);
			}

            if (WIFEXITED(status)) {
                int exitCode = WEXITSTATUS(status);
                if (exitCode == 500) {
                    _status_code = "500 Internal Server Error";
                    close(pipefd[0]);
                    buildResponse();
                    return ;
                }
                else if (exitCode == 415) {
                    _status_code = "415 Unsupported Media Type";
                    close(pipefd[0]);
                    buildResponse();
                    return ;
                }
            }
		}

		if (timedout) {
			kill(pid, SIGTERM);
			usleep(100000);
			kill(pid, SIGKILL);
			waitpid(pid, NULL, 0);

			std::cerr << "CGI Script timed out\n";
			_status_code = "504 Gateway Timeout";
			_responseBody = "CGI Script execution timed out after 60 seconds";
		}
		else if (result == -1) {
			std::cerr << "Error waiting for child process\n";
			_status_code = "500 Internal Server Error\n";
			_responseBody = "Failed to execute CGI Script";
		}
        else {
            while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0)
                _responseBody.append(buffer, bytesRead);
            if (bytesRead == -1) {
                std::cerr << "Error reading from pipe\n";
                _status_code = "500 Internal Server Error";
                _responseBody = "Error reading CGI Script output"; //?
            }
            else
                _status_code = "200 OK";
        }
        close(pipefd[0]);
        buildResponse();
	}
}

//TO REDO
void Response::buildPostResponse()
{
    _response << "HTTP/1.1 " << _status_code << "\r\n";
	_responseBody = _request.getBody();// ?
	_response << "Content-Type: " << _request.getContentType() << "\r\n";
    _response << "Content-Length: " << _responseBody.size() << "\r\n";
    _response << "Connection: keep-alive\r\n";
    _response << "\r\n";
    _response << _responseBody;
    _response_str = _response.str();
}

// int Response::Post_Check_Errors()
// {
//     std::string dirPath = postParseDirPath();

//     if (access(dirPath.c_str(), F_OK) && !access(dirPath.c_str(), W_OK))//no permission to write in subdirectory
//         return -1;
//     if (!access(_server.getUploadDir().c_str(), W_OK)) //no permissions to write in directory
//         return -1;
//     else if (access(_request.getPath().c_str(), F_OK) == 0) //file already exists
//         return -2;
//     if (_request.getBody().size() > static_cast<size_t>(_request.getMaxBodySize()))
//         return -3;
//     return 0;
// }

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
// // }

// bool Response::createFile(/*std::string filename*/)
// {
//     std::string dirPath = postParseDirPath();
//     std::string filePath = postParseFilePath(); //or is it filename ? tocheck

//     createDirectoryRecursive(dirPath);

//     filePath += postHandleMultipart();

//     std::ofstream file(filePath.c_str() /*, std::ios::binary*/);
//     if (!file.is_open()) {
//         std::cerr << "Error creating file" << std::endl;
//         return false;
//     }

//     //std::string body = extractRequestBody();

//     file << _request.getBody();

//     if (!file.good())
//         std::cerr << "Error writing to file at path: " << filePath << std::endl;

//     file.close();
//     return true;
// }

// std::string Response::postParseDirPath()
// {
//     std::string path = _request.getPath();
//     size_t lastSlash = path.find_last_of('/');

//     if (lastSlash != std::string::npos)
//         return path.substr(0, lastSlash);

//     return "";
// }

// std::string Response::postParseFilePath()
// {
//     std::string path = _request.getPath();
//     size_t lastSlash = path.find_last_of('/');

//     if (lastSlash != std::string::npos)
//         return path.substr(lastSlash);
//     return path;
// }

// std::string Response::postHandleMultipart()
// {
//     std::map<std::string, std::string> map = _request.getFormDataName();
//     std::map<std::string, std::string>::iterator it = map.begin();

//     while (it != map.end() && it->first != "filename")
//         it++;
//     if (it->first == "filename")
//         return(extractExtension(it->second));
//     return "";
// }

// std::string Response::extractExtension(std::string file)
// {
//     size_t pos = file.find(".");
//     return file.substr(pos);
// }

// void Response::createDirectoryRecursive(const std::string& path)
// {
//     std::string currentPath;
//     std::istringstream pathStream(path);
//     std::string buffer;

//     while (std::getline(pathStream, buffer, '/')) {
//         if (buffer.empty())
//             continue;
//         if (!currentPath.empty())
//             currentPath += "/";
//         currentPath += buffer;

//         if (!createDirectory(currentPath)) {
//             if (errno != EEXIST)
//                 std::cerr << "Failed to create directory " << currentPath << std::endl;
//         }
//     }
// }

// bool Response::createDirectory(const std::string& path)
// {
//     if (mkdir(path.c_str(), 0755) == 0)
//         return true;
//     if (errno == EEXIST)
//         return true;
//     std::cerr << "Error creating directory " << path << std::endl;
//     return false;
// }
