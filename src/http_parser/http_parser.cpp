#include "http_parser.hpp"
#include "webserver.hpp"

void HttpParser::set_method_and_uri(const string &request) {
  size_t pos = request.find(' ');
  if (pos != string::npos)
    headers_["method"] = request.substr(0, pos);
  else
    WebServer::log("Invalid HTTP request: no method", error);

  size_t uri_start = pos + 1;
  pos = request.find(' ', uri_start);
  if (pos != string::npos)
    headers_["uri"] = request.substr(uri_start, pos - uri_start);
  else
    WebServer::log("Invalid HTTP request: no URI", error);
}

void HttpParser::set_host(const string &request) {
  size_t host_start = request.find("Host: ");
  if (host_start != string::npos) {
    size_t host_end = request.find("\r\n", host_start);
    if (host_end != string::npos)
      headers_["host"] =
          request.substr(host_start + 6, host_end - host_start - 6);
  }
}

void HttpParser::set_accept(const string &request) {
  size_t accept_start = request.find("Accept: ");
  if (accept_start != string::npos) {
    size_t accept_end = request.find("\r\n", accept_start);
    if (accept_end != string::npos)
      headers_["accept"] =
          request.substr(accept_start + 8, accept_end - accept_start - 8);
  }
}

void HttpParser::set_body(const string &request) {
  size_t body_start = request.find("\r\n\r\n");
  if (body_start != string::npos)
    headers_["body"] = request.substr(body_start + 4);
}

void HttpParser::set_content_type(const string &request) {
  size_t content_type_start = request.find("Content-Type: ");
  if (content_type_start != string::npos) {
    size_t content_type_end = request.find("\r\n", content_type_start);
    if (content_type_end != string::npos)
      headers_["content_type"] = request.substr(
          content_type_start + 14, content_type_end - content_type_start - 14);
  }
}

void HttpParser::set_content_length(const string &request) {
  size_t content_length_start = request.find("Content-Length: ");
  if (content_length_start != string::npos) {
    size_t content_length_end = request.find("\r\n", content_length_start);
    if (content_length_end != string::npos)
      headers_["content_length"] =
          request.substr(content_length_start + 16,
                         content_length_end - content_length_start - 16);
  }
}

HttpParser::HttpParser(const string &request) {
  set_method_and_uri(request);
  set_host(request);
  set_accept(request);
  set_body(request);
  set_content_type(request);
  set_content_length(request);
}

HttpParser::~HttpParser() {}

std::map<string, string> HttpParser::get_headers() const { return (headers_); }