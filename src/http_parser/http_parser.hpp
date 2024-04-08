#ifndef HTTP_PARSER_HPP
#define HTTP_PARSER_HPP

#include <ostream>
#include <string>

class HttpParser {
  private:
    // Request line
    std::string request_;
    std::string method_;
    std::string uri_;

    // Request header
    std::string host_;
    std::string accept_;
    std::string body_;
    std::string content_type_;
    std::string content_length_;

    // Setters
    void set_method_and_uri(const std::string &request);
    void set_host(const std::string &request);
    void set_accept(const std::string &request);
    void set_body(const std::string &request);
    void set_content_type(const std::string &request);
    void set_content_length(const std::string &request);

  public:
    HttpParser(const std::string &request);
    ~HttpParser(void);

    std::string get_method() const;
    std::string get_uri() const;
    std::string get_http_version() const;
    std::string get_host() const;
    std::string get_accept() const;
    std::string get_body() const;
    std::string get_content_type() const;
    std::string get_content_length() const;
    const std::string get_request() const;
};

#endif