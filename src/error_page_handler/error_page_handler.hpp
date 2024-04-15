#ifndef ERROR_PAGE_HANDLER_HPP
#define ERROR_PAGE_HANDLER_HPP

#include "server.hpp"
#include <map>
#include <string>
#include <sys/stat.h>

typedef struct stat Stat;

class ErrorPageHandler {
private:
  std::map<std::string, std::string> headers_;
  Server *server_;

  std::string get_error_page_path(const int status_code);
  void get_route_error_page(bool &hasCustomErrorPage, const int status_code,
                            std::string &path);

public:
  ErrorPageHandler(void);
  ErrorPageHandler(Server &server, std::map<std::string, std::string> &headers);
  ~ErrorPageHandler();

  std::string get_error_page(const int status_code);
};

#endif