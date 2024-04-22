#ifndef HTTP_HANDLER_HPP
#define HTTP_HANDLER_HPP

#include "error_page_handler.hpp"
#include "http_parser.hpp"

#define IS_VALID_BUT_NOT_SUPPORTED(method)                                     \
  (method == "PUT" || method == "PATCH" || method == "TRACE" ||                \
   method == "CONNECT" || method == "OPTIONS" || method == "HEAD")

typedef struct stat Stat;

enum redirectionType { REDIRECT_307, REDIRECT_308 };

class HttpHandler {
  public:
    HttpHandler(const std::string &request, Server &server, Client &client);

  ~HttpHandler();

  std::string process_request();

private:
  Server *server_;
  std::map<std::string, std::string> headers_;
  ErrorPageHandler error_page_handler_;
    Client *client_;

  std::string process_get();

  std::string process_post();

  std::string process_delete();

  std::string create_redirection_response();

  std::string get_file_path(const std::string &uri);

  std::string get_location_path(const std::string &uri);

  bool is_method_allowed(const std::string &method);

  bool has_redirection(const std::string &uri);
};

#endif