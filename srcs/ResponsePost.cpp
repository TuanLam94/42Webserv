#include "../headers/response.hpp"

void Response::handlePostResponse()
{
    if (_contentType == "application/json")                         //json data submission
        handleDataSubmission();
    else if (_contentType == "application/x-www-form-urlencoded")   //form submission
        handleFormSubmission();
    else if (_contentType == "multipart/form-data")					//file upload
        handleCGIPost();
}

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
    std::string str;
    for (size_t i = 0; i < _bodyVector.size(); i++)
        str += _bodyVector[i];

    std::ostringstream oss;
    oss << getContentLength();
    std::string content_length_str = oss.str();

    std::map<std::string, std::string> tmp = getFormDataFileName();
    std::map<std::string, std::string>::const_iterator it = tmp.begin();
    std::map<std::string, std::string>::const_iterator ite = tmp.end();

	
    setenv("REQUEST_METHOD", "POST", 1);
    setenv("CONTENT_LENGTH", content_length_str.c_str(), 1);
    setenv("CONTENT_TYPE", _boundary_full.c_str(), 1);
    setenv("QUERY_STRING", _request.getQueryString().c_str(), 1);
    setenv("PATH_INFO", _request.getPathInfo().c_str(), 1);
    if (it != ite)
    {
	    setenv("FILE_NAME", it->first.c_str(), 1);
	    setenv("FILE_BODY", it->second.c_str(), 1);
    }
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

        write(bodyPipe[1], str.c_str(), str.size());
        close(bodyPipe[1]);

		time_t start_time = time(NULL);
		const time_t timeout = 30;
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
                _responseBody = "Error reading CGI Script output";
            }
            else
                _status_code = "200 OK";
        }
        close(pipefd[0]);
        buildResponse();
	}
}

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
