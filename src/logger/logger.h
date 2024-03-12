#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <iostream>

/**
 * @brief Enum for log levels.
 */
enum LogLevel {
    INFO, ///< Informational log level
    WARNING, ///< Warning log level
    ERROR, ///< Error log level
};

#define RED "\033[0;91m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[0;94m"

/**
 * @class Logger
 * @brief This class is used for logging messages with different log levels.
 */
class Logger {
public:
    Logger();

    ~Logger();

    void log(const std::string &message, int log_level);

private:
    std::ofstream logFile;
};

#endif