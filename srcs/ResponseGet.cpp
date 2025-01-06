#include "../headers/response.hpp"

void Response::handleGetResponse()
{
    switch(GET_CheckFile()) {
        case -1:
            _status_code = "404 Not Found";
            break;
        case -2:
            _status_code = "403 Forbidden";
            break;
        case -3:
            _status_code = "200 OK";
            break;
        case 0:
            _status_code = "200 OK";
            break;
        case 1:
            handleCGIGet();
            break;
    }
}


int Response::GET_CheckFile()
{
    if (_request.isCgi())
        return 1;
    if (access(_path.c_str(), F_OK) != 0)
        return -1; // file does not exist
    if (fileIsDir())
        return -3;
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

bool Response::fileIsDir()
{
    struct stat fileStat;

    if (stat(_path.c_str(), &fileStat) == 0) {
        if (S_ISDIR(fileStat.st_mode))
            return true;
    }
    return false;

}

void Response::buildGetResponse()
{
    _response << "HTTP/1.1 " << _status_code << "\r\n";
	_responseBody = _request.getBody();
	_response << "Content-Type: " << _request.getContentType() << "\r\n";
    _response << "Content-Length: " << _responseBody.size() << "\r\n";
    _response << "Connection: keep-alive\r\n";
    _response << "\r\n";
    _response << _responseBody;
    _response_str = _response.str();
}

std::string Response::loadErrorPage(const std::string &errorPage)
{
    std::string toDisplay = "config/errors/" + errorPage;

    std::ifstream file(toDisplay.c_str());
    if (!file)
        return "<html><body><h1>Error</h1><p>Could not load error page.</p></body></html>";
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void Response::handleCGIGet()
{       
	setenv("REQUEST_METHOD", "GET", 1);
	setenv("QUERY_STRING", _request.getQueryString().c_str(), 1);
	setenv("PATH_INFO", _request.getPathInfo().c_str(), 1);
	setenv("SCRIPT_NAME", _request.getScriptName().c_str(), 1);
	setenv("CONTENT_TYPE", _request.getContentType().c_str(), 1);
	setenv("SERVER_NAME", _request.getServerName().c_str(), 1);
	setenv("SERVER_PORT", _request.getServerPort().c_str(), 1);
	setenv("REMOTE_ADDR", _request.getRemoteAddr().c_str(), 1);

	int pipefd[2];
	if (pipe(pipefd) == -1) {
		std::cerr << "Failed to create pipe.\n";
		return;
	}

	pid_t pid = fork();
	if (pid == -1) {
		std::cerr << "Failed to fork process.\n";
		close(pipefd[0]);
		close(pipefd[1]);
		return ;
	}

	if (pid == 0) {
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[0]);
		close(pipefd[1]);

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
        close (pipefd[0]);
	}
}
