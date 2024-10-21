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
        case 1:
            handleCGI(1);
            break;
    }
    // std::cout << "STATUS CODE GET = " << _status_code << std::endl;

    buildGetResponse();
}


int Response::GET_CheckFile()
{
    if (_request.isCgi())
        return 1;
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

void Response::handleCGI(int type)
{

    if (type == 1) {
        setenv("REQUEST_METHOD", "GET", 1);
        setenv("QUERY_STRING", _request.getQueryString().c_str(), 1);
        setenv("PATH_INFO", _request.getPathInfo().c_str(), 1);
        setenv("SCRIPT_NAME", _request.getScriptName().c_str(), 1);
        setenv("CONTENT_TYPE", _request.getContentType().c_str(), 1);
        setenv("SERVER_NAME", _request.getServerName().c_str(), 1);
        setenv("SERVER_PORT", _request.getServerPort().c_str(), 1);
        setenv("REMOTE_ADDR", _request.getRemoteAddr().c_str(), 1);
        // setenv("REMOTE_PORT", _request.getRemotePort().c_str(), 1);
    }
    else if (type == 2) {
        std::ostringstream oss;
        oss << _request.getBody().size();
        std::string content_length_str = oss.str();

        setenv("REQUEST_METHOD", "POST", 1);
        setenv("CONTENT_LENGTH", content_length_str.c_str(), 1);
        setenv("CONTENT_TYPE", _request.getContentType().c_str(), 1);
        setenv("QUERY_STRING", _request.getQueryString().c_str(), 1);
        setenv("PATH_INFO", _request.getPathInfo().c_str(), 1);
    }

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        std::cerr << "Failed to create pipe.\n";
        return ;
    }

    pid_t pid = fork();
    if (pid == -1) {
        std::cerr << "Failed to fork process.\n";
        return ;
    }

    if (pid == 0) {
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);

        if (_cgi_type == 1)
            runScript("/usr/bin/python3");
        else if (_cgi_type == 2)
            runScript("/bin/bash");
        else {
            std::cerr << "Unsupported CGI type\n";
            exit(1);
        }

        std::cerr << "execv failed\n";
        exit(1);
    }
    else {
        close(pipefd[1]);

        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            _status_code = "500 Internal Server Error";
            _responseBody = "Failed to execute CGI Script.";
        }
        else {
            char buffer[4096];
            ssize_t bytesRead;
            while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
                buffer[bytesRead] = '\0';
                _responseBody += buffer;
            }
            if (bytesRead < 0) {
                std::cerr << "Failed to read from pipe.\n";
                _status_code = "500 Internal Server Error";
                _responseBody = "Failed to read from pipe.\n";
            }
            else
                _status_code = "200 OK";
        }

        close (pipefd[0]);
        buildCGIResponse();
    }
}

void Response::buildCGIResponse()
{
    _response.str("");
    _response.clear();

    _response << "HTTP/1.1 " << _status_code << "\r\n";
    _response << "Content-Type: text/html\r\n";
    _response << "Content-Length: " << _responseBody.size() << "\r\n";
    _response << "Connection: keep-alive\r\n"; //keep alive ?
    _response << "\r\n";
    _response << _responseBody;
    _response_str = _response.str();
}