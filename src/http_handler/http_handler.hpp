#ifndef HTTP_HANDLER_HPP
# define HTTP_HANDLER_HPP

#include "http_parser.hpp"
#include "server.hpp"

class HttpHandler{
private:
	HttpParser	request_;
	Server		*server_;
	int 		client_fd_;

	void processGet(void);
	void processPost(void);
	void processDelete(void);

	std::string responseBuilder(std::string status_code, std::string status_message, std::string content_type);
	void sendResponse(const std::string &response, const std::string &content);
	std::string readFile(const std::string &file_path);
	std::string getContentType(const std::string &file_path);
	
public:
	HttpHandler(const std::string &request, const int client_fd, Server *server);
	~HttpHandler(void);
	
	void processRequest(void);
};

#endif