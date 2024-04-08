#ifndef HTTP_PARSER_HPP
#define HTTP_PARSER_HPP

#include <map>
#include <ostream>
#include <string>

using std::string;

class HttpParser {
  private:
    std::map<string, string> headers_;

    // Setters
    void set_method_and_uri(const string &request);
    void set_host(const string &request);
    void set_accept(const string &request);
    void set_body(const string &request);
    void set_content_type(const string &request);
    void set_content_length(const string &request);

  public:
    HttpParser(const string &request);
    ~HttpParser(void);

    std::map<string, string> get_headers() const;
    void parse_request(const string &request);
};

#endif