#ifndef HTTP_HANDLER_HPP
#define HTTP_HANDLER_HPP

#include "http_parser.hpp"
#include "server.hpp"

typedef struct stat Stat;

class HttpHandler {
  public:
    HttpHandler(const std::string &request, Server &server);

    ~HttpHandler();

    const std::string process_request();

  private:
    HttpParser request_;
    Server *server_;

    const std::string process_get();

    const std::string process_post();

    const std::string process_delete();

    std::string response_builder(const std::string &status_code,
                                 const std::string &status_message,
                                 const std::string &content_type,
                                 const std::string &content_length = "0");

    std::string read_file(const std::string &file_path);

    std::string get_content_type(const std::string &file_path);

    std::string get_error_page(const int status_code);

    bool is_text_file(const std::string &file_path);
};

#endif