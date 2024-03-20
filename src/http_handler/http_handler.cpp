#include "http_handler.hpp"
#include <fstream>
#include <unistd.h>
#include <string.h>

HttpHandler::HttpHandler(const std::string &request, int client_fd) : request_(request), client_fd_(client_fd){}

HttpHandler::~HttpHandler(){}

void HttpHandler::processRequest(void)
{
	if (request_.get_method() == "GET")
		processGet();
	else if (request_.get_method() == "POST")
		processPost();
	else if (request_.get_method() == "DELETE")
		processDelete();
}

void HttpHandler::processGet(void)
{
	std::fstream file;
	std::string content;

	if (request_.get_uri() == "/") {
		try {
			content = readFile("test_pages/index.html");
			std::string response = responseBuilder("200", "OK", "text/html");
			write(client_fd_, response.c_str(), response.length());
			write(client_fd_, content.c_str(), content.length());
		}
		catch(const std::runtime_error& e) {
			content = readFile("test_pages/not_found.html");
			std::string response = responseBuilder("404", "Not Found", "text/html");
			write(client_fd_, response.c_str(), response.length());
			write(client_fd_, content.c_str(), content.length());
		}
	} else {
		try {
			content = readFile("test_pages" + request_.get_uri());
			std::string response = responseBuilder("200", "OK", "text/html");
			write(client_fd_, response.c_str(), response.length());
			write(client_fd_, content.c_str(), content.length());
		}
		catch(const std::exception& e) {
			std::string content = readFile("test_pages/not_found.html");
			std::string response = responseBuilder("404", "Not Found", "text/html");
			write(client_fd_, response.c_str(), response.length());
			write(client_fd_, content.c_str(), content.length());
		}
	}
}

void HttpHandler::processPost(){}
void HttpHandler::processDelete(){}

std::string HttpHandler::responseBuilder(std::string status_code, std::string status_message, std::string content_type)
{
	std::string response = "HTTP/1.1 " + status_code + " " + status_message + "\n" + "Content-Type: " + content_type + "\n\n";
	return (response);
}

std::string HttpHandler::readFile(const std::string &file_path)
{
	std::fstream file;
	std::string content;
	char buffer[1024];

	file.open(file_path.c_str(), std::ios::in);
	if (file.is_open())
	{
		while (file.getline(buffer, 1024))
			content += buffer;
		file.close();
	}
	else
		throw std::runtime_error("File not found");
	return (content);
}