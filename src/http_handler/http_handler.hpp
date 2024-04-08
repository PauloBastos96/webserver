#ifndef HTTP_HANDLER_HPP
#define HTTP_HANDLER_HPP

#include "http_parser.hpp"
#include "server.hpp"

#define IS_VALID_BUT_NOT_SUPPORTED(method)                                     \
  (method == "PUT" || method == "PATCH" || method == "TRACE" ||                \
   method == "CONNECT" || method == "OPTIONS" || method == "HEAD")

typedef struct stat Stat;

class HttpHandler {
public:
  HttpHandler(const std::string &request, Server &server);

  ~HttpHandler();

    std::string process_request();

private:
  HttpParser request_;
  Server *server_;

    std::string process_get();

    std::string process_post();

    std::string process_delete();

  std::string response_builder(const std::string &status_code,
                               const std::string &status_message,
                               const std::string &content_type,
                               const std::string &content_length = "0");

  std::string read_file(const std::string &file_path);

  std::string get_content_type(const std::string &file_path);

  std::string get_error_page(const int status_code);

  std::string get_error_page_path(const int status_code);

  void get_route_error_page(bool &hasCustomErrorPage, const int status_code, std::string &path);

  std::string get_file_path(const std::string &uri);

  std::string get_location_path(const std::string &uri);

  const std::string create_autoindex(const std::string &path,
									 const std::string &uri);

  bool is_text_file(const std::string &file_path);

  size_t get_max_size(const std::string &max_size);
};

#endif