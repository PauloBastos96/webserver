#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <fstream>
#include "../webserv.hpp"

/**
 * @brief Enum for log levels.
 */
enum log_level {
    info, ///< Informational log level
    warning, ///< Warning log level
    error, ///< Error log level
};

/**
 * @class logger
 * @brief This class is used for logging messages with different log levels.
 */
class logger {
public:
    logger();

    ~logger();

    static void log(const std::string &message, int log_level);

private:
    static std::ofstream log_file_;
};

#endif