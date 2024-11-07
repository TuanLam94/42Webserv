#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <dirent.h>
#include <cstdlib>
#include <fcntl.h>
#include <sys/epoll.h>
#include <cstring>
#include <unistd.h>
#include "../headers/server.hpp"
#include "../headers/request.hpp"
#include <sys/stat.h>
#include <sys/wait.h>


class Server;
class Request;

class Response
{
	private:
		std::string _method;
		std::string _path;
		std::string _version;
		Request		_request;
		std::string	_status_code;
		std::stringstream _response;
		std::string _contentType;
		std::string	_response_str;
		std::string _responseBody;
		std::string	_boundary_full;
		Server		_server;
		std::map<std::string, std::string>	_formDataName;
		std::map<std::string, std::string>	_FormDataFileName;
		std::map<std::string, std::string>	_jsonParam;
		std::map<std::string, std::string> _urlParam;
		std::vector<unsigned char> _bodyVector;
		int _cgi_type;
		unsigned long int	_contentLength;
		bool	_isRedirect;
		std::string _host;
	public:
		Response() {};
		Response(const Request& request);
		Response(const Response& copy);
		Response& operator=(const Response& other);
		~Response() {};
		//request handling
		void	handleRequest();
		bool 	isErrorResponse();
		void	sendResponse(int fd);
		void	setStatusCode(const Request& request);
		std::string loadErrorPage(const std::string& errorPage);
		int responseSetCgiType();
		//response build
			//get
		void	handleGetResponse();
		int		GET_CheckFile();
		bool	fileIsReg();
		void	buildGetResponse();
		void	buildRedirectResponse();
			//post
		void handlePostResponse();
		void handleDataSubmission();
		bool storeJsonData();
		void handleFormSubmission();
		bool storeFormData();
		void handleUploads();
		int Post_Check_Errors();
		bool createFile(/*std::string filename*/);
		void buildPostResponse();
			//delete
		void	handleDeleteResponse();
		int		DEL_CheckFile();
		int		DeleteFile();
		bool	isDirectoryEmpty();
		void	buildDelResponse();

			//cgi
		void runScript(std::string Lpath);
		void handleCGIGet();
		void handleCGIPost();
		void buildResponse();
		void handleErrorResponse();
		bool isErrorCode();

		// void buildCGIResponse();
		//NEEDED ONLY IF DIRECTORY IN PATH
		std::string postParseDirPath();
		std::string postParseFilePath();
		std::string postHandleMultipart();
		std::string extractExtension(std::string file);
		void createDirectoryRecursive(const std::string& path);
		bool createDirectory(const std::string& path);
			//getters
		std::string getMethod() const;
		std::string getPath() const;
		std::string getVersion() const;
		std::string getStatusCode() const;
		std::string getResponseStr() const;
		Request getRequest() const;
		std::string getContentType() const;
		std::vector<unsigned char> getMyVector() const;
		unsigned long int	getContentLength() const;
		std::map<std::string, std::string> getFormDataFileName() const;
		bool getIsRedirect() const;
			//setters
		void setCode(std::string statuscode);
		void setServer(Server& server);
			//utils
		void printResponse();

};

// class	Response
// {
// 	private: 
// 	// attributs remplis apres parsing de la requete 
// 	std::string _method;
// 	std::string _path;
// 	std::string _version;
// 	std::string _response;
// 	Request	_request;
// 	std::string	_status_code;

// 	// int	_client_fd;

// 	// attributs GET
// 	std::fstream	_input;


// 	public:
// 	Response(Request request);
// 	~Response() {};
// 	// Request(int client_fd);
// 	void	open_file_GET(Server i, const std::string& buffer);
// 	void	find_request();
// 	std::string	GET_method();
// 	// void	POST_method();
// 	// void	DELETE_method();
// 	std::string	build_response(const std::string& body, const std::string& content_type);
// 	std::string	getMethod();
// 	std::string	getPath();
// 	std::string	getVersion();
// 	std::string getStatusCode();

// };

#endif