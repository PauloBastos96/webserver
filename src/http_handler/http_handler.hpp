#ifndef HTTP_HANDLER_HPP
#define HTTP_HANDLER_HPP

#include "http_parser.hpp"
#include "server.hpp"

class HttpHandler {
public:
  HttpHandler(const std::string &request, const int client_fd,
              std::vector<Server> &server);
  ~HttpHandler(void);

  void processRequest(void);

private:
  HttpParser request_;
  int client_fd_;
  Server server_;

  void processGet(void);
  void processPost(void);
  void processDelete(void);

  std::string responseBuilder(std::string status_code,
                              std::string status_message,
                              std::string content_type,
                              std::string content_length = "0");
  void sendResponse(const std::string &response, const std::string &content);
  std::string readFile(const std::string &file_path);
  std::string getContentType(const std::string &file_path);
  bool isTextFile(const std::string &file_path);
};

#endif