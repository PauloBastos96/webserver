#include "http_parser.hpp"
#include "webserver.hpp"

void HttpParser::set_method_and_uri(const std::string &request) {
  size_t pos = request.find(' ');
  if (pos != std::string::npos)
    method_ = request.substr(0, pos);
  else
    WebServer::log("Invalid HTTP request: no method", error);

  size_t uri_start = pos + 1;
  pos = request.find(' ', uri_start);
  if (pos != std::string::npos)
    uri_ = request.substr(uri_start, pos - uri_start);
  else
    WebServer::log("Invalid HTTP request: no URI", error);
}

void HttpParser::set_host(const std::string &request) {
  size_t host_start = request.find("Host: ");
  if (host_start != std::string::npos) {
    size_t host_end = request.find("\r\n", host_start);
    if (host_end != std::string::npos)
      host_ = request.substr(host_start + 6, host_end - host_start - 6);
  }
}

void HttpParser::set_accept(const std::string &request) {
  size_t accept_start = request.find("Accept: ");
  if (accept_start != std::string::npos) {
    size_t accept_end = request.find("\r\n", accept_start);
    if (accept_end != std::string::npos)
      accept_ =
          request.substr(accept_start + 8, accept_end - accept_start - 8);
  }
}

void HttpParser::set_body(const std::string &request) {
  size_t body_start = request.find("\r\n\r\n");
  if (body_start != std::string::npos)
    body_ = request.substr(body_start + 4);
}

void HttpParser::set_content_type(const std::string &request) {
  size_t content_type_start = request.find("Content-Type: ");
  if (content_type_start != std::string::npos) {
    size_t content_type_end = request.find("\r\n", content_type_start);
    if (content_type_end != std::string::npos)
      content_type_ = request.substr(
          content_type_start + 14, content_type_end - content_type_start - 14);
  }
}

void HttpParser::set_content_length(const std::string &request) {
  size_t content_length_start = request.find("Content-Length: ");
  if (content_length_start != std::string::npos) {
    size_t content_length_end = request.find("\r\n", content_length_start);
    if (content_length_end != std::string::npos)
      content_length_ =
          request.substr(content_length_start + 16,
                         content_length_end - content_length_start - 16);
  }
}

HttpParser::HttpParser(const std::string &request) {
  request_ = request;
  set_method_and_uri(request);
  set_host(request);
  set_accept(request);
  set_body(request);
  set_content_type(request);
  set_content_length(request);
}

HttpParser::~HttpParser() {}

std::string HttpParser::get_method() const { return (method_); }

std::string HttpParser::get_uri() const { return (uri_); }

std::string HttpParser::get_host() const { return (host_); }

std::string HttpParser::get_accept() const { return (accept_); }

std::string HttpParser::get_body() const { return (body_); }

std::string HttpParser::get_content_type() const { return (content_type_); }

std::string HttpParser::get_content_length() const { return (content_length_); }

const std::string HttpParser::get_request() const { return (request_); }