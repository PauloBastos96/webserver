#ifndef HTTP_HANDLER_HPP
#define HTTP_HANDLER_HPP

#include "http_parser.hpp"
#include "error_page_handler.hpp"

#define IS_VALID_BUT_NOT_SUPPORTED(method)                                     \
    (method == "PUT" || method == "PATCH" || method == "TRACE" ||              \
     method == "CONNECT" || method == "OPTIONS" || method == "HEAD")

typedef struct stat Stat;

enum redirectionType {
    REDIRECT_307,
    REDIRECT_308
};

class HttpHandler {
  public:
    HttpHandler(const std::string &request, Server &server);

    ~HttpHandler();

    std::string process_request();

  private:
    Server *server_;
    std::map<std::string, std::string> headers_;
    ErrorPageHandler error_page_handler_;

    std::string process_get();

    std::string process_post();

    std::string process_delete();

    std::string create_redirection_response();

    std::string get_content_type(const std::string &file_path);

    std::string get_file_path(const std::string &uri);

    std::string get_location_path(const std::string &uri);

    const std::string create_autoindex(const std::string &path,
                                       const std::string &uri);

    bool is_method_allowed(const std::string &method);

    bool has_redirection(const std::string &uri);

    size_t get_max_size(const std::string &max_size);
};

#endif