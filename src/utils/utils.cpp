#include "utils.hpp"
#include <fstream>

Utils::Utils(void) {}
Utils::~Utils() {}

#pragma region Public

/// @brief Read requested file
/// @param file_path The path of the file
/// @return The content of the file
std::string Utils::read_file(const std::string &file_path) {
  std::fstream file;
  std::string content;

  const bool text_file = is_text_file(file_path);
  if (!text_file)
    file.open(file_path.c_str(), std::ios::in | std::ios::binary);
  else
    file.open(file_path.c_str(), std::ios::in);
  if (file.is_open()) {
    char buffer[1024];
    if (!text_file) {
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

/// @brief Build the response header
/// @param status_code The status code
/// @param status_message The status message
/// @param content_type The content type
/// @param content_length The content length
/// @return The response header
std::string Utils::response_builder(const std::string &status_code,
                                    const std::string &status_message,
                                    const std::string &content_type,
                                    const std::string &content_length) {
  std::string response = "HTTP/1.1 " + status_code + " " + status_message +
                         "\r\n" + "Content-Type: " + content_type + "\r\n" +
                         "Content-Length:" + content_length + "\r\n\r\n";
  return (response);
}
#pragma endregion

#pragma region Private

/// @brief Check if a file is a text file
/// @param file_path The path of the file
/// @return True if the file is a text file, false otherwise
bool Utils::is_text_file(const std::string &file_path) {
  const std::string extension =
      file_path.substr(file_path.find_last_of('.') + 1);
  if (extension == "html" || extension == "css" || extension == "js" ||
      extension == "json" || extension == "xml" || extension == "svg")
    return (true);
  return (false);
}
#pragma endregion
