#include "logger.h"

/**
 * @brief Logs a message with a specific log level.
 *
 * This method logs a message with a specific log level. The message is output to the console in a specific color
 * depending on the log level and is also written to the log file. If the log level is ERROR, a runtime error is thrown.
 *
 * @param message The message to log.
 * @param log_level The log level of the message (INFO, WARNING, ERROR).
 */
void Logger::log(const std::string &message, const int log_level) {
    std::string log_message;
    std::string color_message;
    switch (log_level) {
        case INFO:
            log_message = "[INFO]\t\t\t" + message;
            color_message = BLUE + log_message;
            break;
        case WARNING:
            log_message = "[WARNING]\t\t" + message;
            color_message = YELLOW + log_message;
            break;
        case ERROR:
            log_message = "[ERROR]\t\t\t" + message;
            color_message = RED + log_message;
            break;
        default:
            log_message = "[ERROR]\t\t\tInvalid log level passed";
            color_message = RED + log_message;
            break;
    }
    std::cout << color_message << std::endl;
    logFile << log_message << std::endl;
    if (log_level == ERROR)
        throw std::runtime_error(log_message);
}

/**
 * @brief Constructor for the Logger class.
 *
 * This constructor opens the log file in truncation mode. If the log file fails to open, a runtime error is thrown.
 */
Logger::Logger() {
    logFile.open("logs/webserv.log", std::ios::out | std::ios::trunc);
    if (!logFile) {
        throw std::runtime_error("Failed to open log file");
    }
}

/**
 * @brief Destructor for the Logger class.
 *
 * This destructor closes the log file.
 */
Logger::~Logger() {
    logFile.close();
}