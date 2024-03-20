#ifndef HTTP_PARSER_HPP
# define HTTP_PARSER_HPP

#include <string>
#include <ostream>

class HttpParser{
private:
	//Request line
	std::string request_;
	std::string method_;
	std::string uri_;
	std::string http_version_;

	//Request header
	std::string host_;
	
public:
	HttpParser(const std::string &request);
	~HttpParser(void);
	
	std::string get_method() const;
	std::string get_uri() const;
	std::string get_http_version() const;
	std::string get_host() const;
};

#endif