#ifndef HTTP_HANDLER_HPP
#define HTTP_HANDLER_HPP

#include "http_parser.hpp"
#include "server.hpp"

typedef struct stat Stat;

class HttpHandler {
  public:
    HttpHandler(const std::string &request, int client_socket,
                const Server &server);

    ~HttpHandler();

    void process_request();

  private:
    HttpParser request_;
    int client_socket_;
    Server server_;

    void process_get();
    void process_post();
    void process_delete();

    std::string response_builder(const std::string &status_code,
                                 const std::string &status_message,
                                 const std::string &content_type,
                                 const std::string &content_length = "0");
    void send_response(const std::string &response, const std::string &content);
    std::string read_file(const std::string &file_path);
    std::string get_content_type(const std::string &file_path);
    bool is_text_file(const std::string &file_path);
};

#endif