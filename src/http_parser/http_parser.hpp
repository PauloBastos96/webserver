#ifndef HTTP_PARSER_HPP
#define HTTP_PARSER_HPP

#include <map>
#include <ostream>
#include <string>

using std::string;

class HttpParser {
  private:
    std::map<string, string> headers_;
    void set_method_and_uri(const string &request);
    void parse_request(const string &request);

  public:
    HttpParser(const string &request);
    ~HttpParser();
    const std::map<string, string> &get_headers() const;
};

#endif