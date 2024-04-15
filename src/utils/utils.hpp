#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

class Utils {
private:
  Utils();
  ~Utils();

  static bool is_text_file(const std::string &file_path);

public:
  static std::string read_file(const std::string &file_path);
  static std::string response_builder(const std::string &status_code,
                                      const std::string &status_message,
                                      const std::string &content_type,
                                      const std::string &content_length = "0");
};

#endif