#include "http_handler.hpp"
#include "webserver.hpp"
#include "error_page_handler.hpp"
#include "utils.hpp"
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

HttpHandler::HttpHandler(const std::string &request, Server &server)
    : server_(&server) {
  HttpParser parser(request);
  headers_ = parser.get_headers();
  error_page_handler_ = ErrorPageHandler(*server_, headers_);
}

HttpHandler::~HttpHandler() {}

#pragma region HTTP Methods

/// @brief Process the request
std::string HttpHandler::process_request() {
  if (!is_method_allowed(headers_.at("method")))
    return error_page_handler_.get_error_page(405);
  if (has_redirection(headers_.at("uri")))
    return create_redirection_response();
  if (headers_.at("method") == "GET")
    return process_get();
  else if (headers_.at("method") == "POST")
    return process_post();
  else if (headers_.at("method") == "DELETE")
    return process_delete();
  else if (IS_VALID_BUT_NOT_SUPPORTED(headers_.at("method")))
    return error_page_handler_.get_error_page(501);
  else
    return error_page_handler_.get_error_page(400);
  return "";
}

/// @brief Process a GET request
std::string HttpHandler::process_get() {
  std::fstream file;
  std::string content;
  std::string file_path;
  std::stringstream ss;
  Stat buffer;
  try {
    file_path = get_file_path(headers_.at("uri"));
    if (*(headers_.at("uri").end() - 1) == '/' &&
        server_->get_config().get_auto_index()) {
      file_path = server_->get_config().get_root() + headers_.at("uri");
      content = create_autoindex(file_path, headers_.at("uri"));
      if (content.empty())
        return error_page_handler_.get_error_page(404);
      ss << content.length();
      std::string response =
          Utils::response_builder("200", "OK", "text/html", ss.str());
      WebServer::log(std::string(HTTP_200) + headers_.at("uri"), info);
      return response + content;
    }
    if (stat(file_path.c_str(), &buffer) != 0 || S_ISDIR(buffer.st_mode))
      throw std::runtime_error("404");
    content = Utils::read_file(file_path);
    ss << content.length();
    std::string response =
        Utils::response_builder("200", "OK", get_content_type(file_path), ss.str());
    WebServer::log(std::string(HTTP_200) + headers_.at("uri"), info);
    return response + content;
  } catch (const std::runtime_error &e) {
    return error_page_handler_.get_error_page(404);
  }
}

/// @brief Process a POST request
std::string HttpHandler::process_post() {
  size_t max_size =
      get_max_size(server_->get_config().get_max_client_body_size());
  if (headers_.at("body").size() > max_size)
    return error_page_handler_.get_error_page(413);
  std::string response = "Received" + headers_.at("body");
  std::stringstream ss;
  ss << response.length();
  return Utils::response_builder("201", "Created", "text/plain", ss.str()) + response;
}

/// @brief Process a DELETE request
std::string HttpHandler::process_delete() {
  std::string file_path;
  Stat buffer;

  file_path = server_->get_config().get_root() + headers_.at("uri");
  if (stat(file_path.c_str(), &buffer) != 0)
    return error_page_handler_.get_error_page(404);
  if (!(buffer.st_mode & S_IWOTH))
    return error_page_handler_.get_error_page(403);
  if (std::remove(file_path.c_str()))
    return error_page_handler_.get_error_page(403);
  return Utils::response_builder("204", "No Content", "text/plain", "0");
}

#pragma endregion

#pragma region Utility Functions

/// @brief Check if the request has a redirection
/// @param uri The URI of the request
/// @return True if the request has a redirection, false otherwise
bool HttpHandler::has_redirection(const std::string &uri) {
  std::vector<Location> locations = server_->get_locations();
  for (size_t i = 0; i < locations.size(); i++) {
    if (uri == locations.at(i).get_path() &&
        !locations.at(i).get_config().get_redirection().empty())
      return true;
  }
  if (!server_->get_config().get_redirection().empty())
    return true;
  return false;
}

/// @brief Create a redirection response
/// @return The redirection response
std::string HttpHandler::create_redirection_response() {
  enum redirectionType type;
  std::string response;
  std::string redirection;

  redirection = server_->get_config().get_redirection();
  for (size_t i = 0; i < server_->get_locations().size(); i++) {
    if (headers_.at("uri") == server_->get_locations().at(i).get_path()) {
      redirection =
          server_->get_locations().at(i).get_config().get_redirection();
      break;
    }
  }
  if (redirection.substr(redirection.find_last_of(' ') + 1) == "redirect")
    type = REDIRECT_307;
  else
    type = REDIRECT_308;
  if (type == REDIRECT_307)
    response = "HTTP/1.1 307 Temporary Redirect\r\n";
  else
    response = "HTTP/1.1 308 Permanent Redirect\r\n";
  response +=
      "Location: " + redirection.substr(0, redirection.find_last_of(' ')) +
      "\r\nContent-Type: text/html\r\nContent-Length: 0\r\n\r\n";
  WebServer::log(std::string(type == REDIRECT_307 ? HTTP_307 : HTTP_308) +
                     headers_.at("uri"),
                 info);
  return response;
}

/// @brief Check if the method is allowed for the location
/// @param method The method to check
/// @return True if the method is allowed, false otherwise
bool HttpHandler::is_method_allowed(const std::string &method) {
  std::string location =
      headers_.at("uri").substr(0, headers_.at("uri").find_last_of('/'));
  std::vector<Location> locations = server_->get_locations();
  for (size_t i = 0; i < locations.size(); i++) {
    if (location == locations.at(i).get_path()) {
      for (size_t j = 0; j < locations.at(i).get_allowed_methods().size();
           j++) {
        if (method == locations.at(i).get_allowed_methods().at(j))
          return true;
      }
      return false;
    }
  }
  return true;
}

/// @brief Check if the server should generate an autoindex page
/// @param uri The URI of the request
/// @param server A reference to the server
/// @return True if the server should generate an autoindex page, false
/// otherwise
bool should_generate_autoindex(const std::string &uri, Server &server) {
  if (uri == "/" && server.get_config().get_auto_index())
    return true;
  else if (*(uri.end() - 1) == '/') {
    std::vector<Location> locations = server.get_locations();
    if (locations.empty() && server.get_config().get_auto_index())
      return true;
    for (size_t i = 0; i < locations.size(); i++) {
      if (uri == locations.at(i).get_path() &&
          locations.at(i).get_config().get_auto_index())
        return true;
    }
  }
  return false;
}

/// @brief Get the path of the location
/// @param uri The URI of the request
/// @return The path of the location
std::string HttpHandler::get_location_path(const std::string &uri) {
  std::string path;
  std::vector<Location> locations = server_->get_locations();
  Stat buffer;

  for (size_t i = 0; i < locations.size(); i++) {
    if (uri == locations.at(i).get_path()) {
      std::vector<std::string> indexes =
          locations.at(i).get_config().get_indexes();
      for (size_t j = 0; j < indexes.size(); j++) {
        path = server_->get_config().get_root() +
               locations.at(i).get_config().get_root() + "/" + indexes.at(j);
        if (stat(path.c_str(), &buffer) == 0)
          return path;
      }
    } else {
      path = server_->get_config().get_root() + uri;
      if (stat(path.c_str(), &buffer) == 0)
        break;
    }
  }
  return path;
}

/// @brief Get the path of the requested file
/// @param uri The URI of the request
/// @return The path of the requested file
std::string HttpHandler::get_file_path(const std::string &uri) {
  std::string file_path;
  Stat buffer;

  if (uri == "/") {
    for (size_t i = 0; i < server_->get_config().get_indexes().size(); i++) {
      file_path = server_->get_config().get_root() + "/" +
                  server_->get_config().get_indexes().at(i);
      if (stat(file_path.c_str(), &buffer) == 0)
        break;
    }
  } else {
    if (server_->get_locations().empty())
      file_path = server_->get_config().get_root() + uri;
    else
      file_path = get_location_path(uri);
  }
  return file_path;
}

/// @brief Create an autoindex page
/// @param path The path of the directory
/// @param uri The URI of the directory
/// @return The autoindex page content
const std::string HttpHandler::create_autoindex(const std::string &path,
                                                const std::string &uri) {
  std::string autoindex;
  autoindex = "<!DOCTYPE html>\n<html>\n<head>\n<title>Index of " + uri +
              "</title>\n<link rel=\"stylesheet\" href=\"/styles.css\"> "
              "\n</head>\n<body>\n<h1 id=\"autoindex\">Index of " +
              uri + "</h1>\n";
  autoindex += "<table>\n<tr>\n<th>Name</th>\n<th>Last Modified</th>\n";
  autoindex += "<th>Size</th>\n</tr>\n";
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir(path.c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      struct stat st;
      std::stringstream ss;
      std::string file_path = path + "/" + std::string(ent->d_name);
      stat(file_path.c_str(), &st);
      ss << st.st_size;
      std::string itemName = S_ISDIR(st.st_mode)
                                 ? std::string(ent->d_name) + "/"
                                 : std::string(ent->d_name);
      autoindex +=
          "<tr>\n<td><a href=\"" + itemName + "\">" + itemName + "</a></td>\n";
      autoindex += "<td>" + std::string(ctime(&st.st_mtime)) + "</td>\n";
      autoindex += "<td>" + ss.str() + " B</td>\n</tr>\n";
    }
    closedir(dir);
  } else {
    return "";
  }
  autoindex += "</table>\n</body>\n</html>";
  return autoindex;
}

/// @brief Get the maximum size of the client body
/// @param max_size The maximum size set in the configuration file
/// @return The maximum size of the client body in bytes
size_t HttpHandler::get_max_size(const std::string &max_size) {
  size_t size = 0;
  char unit;

  if (isalpha(*(max_size.end() - 1)))
    unit = static_cast<char>(std::tolower(*(max_size.end() - 1)));
  else
    unit = 'b';
  switch (unit) {
  case 'k':
    size = std::atoi(max_size.c_str()) * 1024;
    break;
  case 'm':
    size = std::atoi(max_size.c_str()) * 1024 * 1024;
    break;
  case 'g':
    size = std::atoi(max_size.c_str()) * 1024 * 1024 * 1024;
    break;
  default:
    size = std::atoi(max_size.c_str());
    break;
  }
  return size;
}

/// @brief Get the content type of a file
/// @param file_path The path of the file
/// @return The content type of the file
std::string HttpHandler::get_content_type(const std::string &file_path) {
  const std::string extension =
      file_path.substr(file_path.find_last_of('.') + 1);
  if (extension == "html" || extension == "css")
    return ("text/" + extension);
  if (extension == "js")
    return ("text/javascript");
  if (extension == "jpeg" || extension == "jpg")
    return ("image/jpeg");
  if (extension == "png")
    return ("image/png");
  if (extension == "gif")
    return ("image/gif");
  if (extension == "svg")
    return ("image/svg+xml");
  if (extension == "json")
    return ("application/json");
  if (extension == "xml")
    return ("application/xml");
  if (extension == "form")
    return ("application/x-www-form-urlencoded");
  return ("text/plain");
}
#pragma endregion
