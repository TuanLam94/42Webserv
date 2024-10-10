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
		Server		_server;
	public:
		// Response() {};
		Response(const Request& request);
		Response(const Response& copy);
		Response& operator=(const Response& other);
		~Response() {};
		//request handling
		void	handleRequest();
		bool 	isErrorResponse();
		void	handleErrorResponse();
		void	sendResponse(int fd);
		std::string loadErrorPage(const std::string& errorPage);
		//response build
			//get
		void	handleGetResponse();
		int		GET_CheckFile();
		bool	fileIsReg();
		void	buildGetResponse();
			//post
		void	handlePostResponse();
		int		Post_Check();
		void	buildPostResponse();
		std::string extractExtension(std::string file);
		std::string extractRequestBody();
		std::string postParseDirPath();
		std::string postParseFilePath();
		void	createFile();
		bool	createDirectory(const std::string& path);
		void	createDirectoryRecursive(const std::string& path);
		std::string postHandleMultipart();

		void	buildDeleteResponse();
		void	buildPutResponse();
		//getters
		std::string getMethod() const;
		std::string getPath() const;
		std::string getVersion() const;
		std::string getStatusCode() const;
		std::string getResponseStr() const;
		Request getRequest() const;
		//setters
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