#include "http_parser.hpp"
#include "webserver.hpp"

HttpParser::HttpParser(const std::string &request) {
    request_ = request;
    WebServer::log("Request: " + request_, info);

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

    size_t version_start = request.find("HTTP/", pos);
    if (version_start != std::string::npos) {
        size_t version_end = request.find("\r\n", version_start);
        if (version_end != std::string::npos)
            http_version_ = request.substr(version_start + 5,
                                           version_end - version_start - 5);
        else
            WebServer::log("Invalid HTTP request: no HTTP version", error);
    } else
        WebServer::log("Invalid HTTP request: no HTTP version", error);

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

HttpParser::~HttpParser() {}

std::string HttpParser::get_method() const { return (method_); }

std::string HttpParser::get_uri() const { return (uri_); }

std::string HttpParser::get_http_version() const { return (http_version_); }

std::string HttpParser::get_host() const { return (host_); }

std::string HttpParser::get_accept() const { return (accept_); }