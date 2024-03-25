#include "http_parser.hpp"

HttpParser::HttpParser(const std::string &request)
{
	request_ = request;
	
	method_ = request.substr(0, request.find(' '));

	uri_ = request.substr(request.find(' ') + 1, request.find(' ', request.find(' ') + 1) - request.find(' ') - 1);

	http_version_ = request.substr(request.find("HTTP/") + 5, request.find("\r\n") - request.find("HTTP/") - 5);
	
	host_ = request.substr(request.find("Host: ") + 6, request.find("\r\n", request.find("Host: ")) - request.find("Host: ") - 6);

	accept_ = request.substr(request.find("Accept: ") + 8, request.find("\r\n", request.find("Accept: ")) - request.find("Accept: ") - 8);
	// Parse the request
	// ...
	// Extract the method, URI, and HTTP version
	// ...
	// Extract the headers
	// ...
	// Extract the body
	// ...
	// Process the request
	// ...
	// Generate the response
	// ...
	// Send the response
	// ...
	// End of the request
	// ...
}

HttpParser::~HttpParser(){}

std::string HttpParser::get_method() const
{
	return (method_);
}

std::string HttpParser::get_uri() const
{
	return (uri_);
}

std::string HttpParser::get_http_version() const
{
	return (http_version_);
}

std::string HttpParser::get_host() const
{
	return (host_);
}

std::string HttpParser::get_accept() const
{
	return (accept_);
}