#ifndef LOGGER_HPP
#define LOGGER_HPP

#pragma region Log Level enum

/**
 * @brief Enum for log levels.
 */
enum log_level {
    debug, ///< Debug log level
    info, ///< Informational log level
    warning, ///< Warning log level
    error, ///< Error log level
};

#pragma endregion

#pragma region Color Codes

#define RED "\033[1;91m"
#define YELLOW "\033[1;93m"
#define BLUE "\033[1;94m"
#define PURPLE "\033[1;95m"
#define RESET "\033[0m"

#pragma endregion

#pragma region Log Messages

#define INFO_CFG_NOPORT "[CONFIG] No port number specified, defaulting to 80"
#define INFO_SVR_START "[SERVER] Server started"

#define WARN_CFG_PORT "[CONFIG] Invalid port number, defaulting to 80"
#define WARM_CFG_SEMICOLON "[CONFIG] Missing semicolon"

#define ERR_CFG_LISTEN "[CONFIG] Invalid listen directive"
#define ERR_CFG_MISSING_BRACKET "[CONFIG] Missing closing bracket"
#define ERR_CANT_OPEN_FILE "[CONFIG] Could not open file "
#define ERR_CFG_INVALID_ERROR_PAGE "[CONFIG] Invalid error page directive"
#define ERR_SOCKET "Failed to create socket for server: "
#define ERR_BIND "Failed to bind socket for server: "
#define ERR_LISTEN "Failed to listen on socket for server: "

#pragma endregion

//HTTP status codes

# define HTTP_200 "[200] OK |   URI:"
# define HTTP_404 "[404] Not Found  |   URI:"

#endif