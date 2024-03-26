#include "http_handler.hpp"
#include "webserver.hpp"
#include <fstream>
#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

HttpHandler::HttpHandler(const std::string &request, const int client_fd,
                         std::vector<Server> &servers)
    : request_(request), client_fd_(client_fd) {
  for (std::vector<Server>::iterator server_it = servers.begin();
       server_it != servers.end(); ++server_it)
    for (std::vector<int>::iterator fd_it =
             server_it->get_connected_clients().begin();
         fd_it != server_it->get_connected_clients().end(); ++fd_it)
      if (*fd_it == client_fd_)
        server_ = *server_it;
}

HttpHandler::~HttpHandler() {}

/// @brief Process the request
void HttpHandler::processRequest(void) {
  if (request_.get_method() == "GET")
    processGet();
  else if (request_.get_method() == "POST")
    processPost();
  else if (request_.get_method() == "DELETE")
    processDelete();
}

/// @brief Send the response to the client
/// @param response The response header to send
/// @param content The content to send
void HttpHandler::sendResponse(const std::string &response,
                               const std::string &content) {
  send(client_fd_, response.c_str(), response.length(), 0);
  send(client_fd_, content.c_str(), content.length(), 0);
}

/// @brief Process a GET request
void HttpHandler::processGet(void) {
  std::fstream file;
  std::string content;
  std::string file_path;
  struct stat buffer;
  std::stringstream ss;

  try {
    if (request_.get_uri() == "/") {
      for (size_t i = 0; i < server_.get_config().get_index().size(); i++) {
        file_path = server_.get_config().get_root() + "/" +
                    server_.get_config().get_index().at(i);
        if (stat(file_path.c_str(), &buffer) == 0)
          break;
      }
    } else
      file_path = server_.get_config().get_root() + "/" + request_.get_uri();
    content = readFile(file_path);
    ss << content.length();
    std::string response =
        responseBuilder("200", "OK", getContentType(file_path), ss.str());
    sendResponse(response, content);
    WebServer::log(std::string(HTTP_200) + request_.get_uri(), info);
  } catch (const std::runtime_error &e) {
    content = readFile("default_pages/not_found.html");
    ss << content.length();
    std::string response =
        responseBuilder("404", "Not Found", "text/html", ss.str());
    sendResponse(response, content);
    WebServer::log(std::string(HTTP_404) + request_.get_uri(), warning);
  }
}

void HttpHandler::processPost() {}
void HttpHandler::processDelete() {}

/// @brief Build the response header
/// @param status_code The status code
/// @param status_message The status message
/// @param content_type The content type
/// @return The response header
std::string HttpHandler::responseBuilder(std::string status_code,
                                         std::string status_message,
                                         std::string content_type,
                                         std::string content_length) {
  std::string response = "HTTP/1.1 " + status_code + " " + status_message +
                         "\n" + "Content-Type: " + content_type + "\n" +
                         "Content-Length:" + content_length + "\n\r\n";
  return (response);
}

/// @brief Read requested file
/// @param file_path The path of the file
/// @return The content of the file
std::string HttpHandler::readFile(const std::string &file_path) {
  std::fstream file;
  std::string content;
  bool textFile;
  char buffer[1024];

  textFile = isTextFile(file_path);
  if (!textFile)
    file.open(file_path.c_str(), std::ios::in | std::ios::binary);
  else
    file.open(file_path.c_str(), std::ios::in);
  if (file.is_open()) {
    if (!textFile) {
      while (file.read(buffer, 1024))
        content.append(buffer, 1024);
    } else {
      while (file.getline(buffer, 1024))
        content += buffer;
    }
    file.close();
  } else
    throw std::runtime_error("404");
  return (content);
}

/// @brief Check if a file is a text file
/// @param file_path The path of the file
/// @return True if the file is a text file, false otherwise
bool HttpHandler::isTextFile(const std::string &file_path) {
  std::string extension = file_path.substr(file_path.find_last_of(".") + 1);
  if (extension == "html" || extension == "css" || extension == "js" ||
      extension == "json" || extension == "xml" || extension == "svg")
    return (true);
  return (false);
}

/// @brief Get the content type of a file
/// @param file_path The path of the file
/// @return The content type of the file
std::string HttpHandler::getContentType(const std::string &file_path) {
  std::string extension = file_path.substr(file_path.find_last_of(".") + 1);
  if (extension == "html" || extension == "css")
    return ("text/" + extension);
  else if (extension == "js")
    return ("text/javascript");
  else if (extension == "jpeg" || extension == "jpg")
    return ("image/jpeg");
  else if (extension == "png")
    return ("image/png");
  else if (extension == "gif")
    return ("image/gif");
  else if (extension == "svg")
    return ("image/svg+xml");
  else if (extension == "json")
    return ("application/json");
  else if (extension == "xml")
    return ("application/xml");
  else if (extension == "form")
    return ("application/x-www-form-urlencoded");
  else
    return ("text/plain");
}