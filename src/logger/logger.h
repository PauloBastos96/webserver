#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>

/**
 * @brief Enum for log levels.
 */
enum log_level {
    info, ///< Informational log level
    warning, ///< Warning log level
    error, ///< Error log level
};

#define RED "\033[0;91m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[0;94m"

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