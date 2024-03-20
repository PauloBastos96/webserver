#ifndef HTTP_HANDLER_HPP
# define HTTP_HANDLER_HPP

#include "http_parser.hpp"



class HttpHandler{
private:
	HttpParser	request_;
	int 		client_fd_;

	void processGet(void);
	void processPost(void);
	void processDelete(void);

	std::string responseBuilder(std::string status_code, std::string status_message, std::string content_type);
	std::string readFile(const std::string &file_path);
	
public:
	HttpHandler(const std::string &request, int client_fd);
	~HttpHandler(void);
	
	void processRequest(void);
};

#endif