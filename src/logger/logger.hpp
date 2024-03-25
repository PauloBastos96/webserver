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

#pragma region HTTP Responses

#define HTTP_RESPONSE "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 591\n\n<!DOCTYPE html>\n<html>\n<head>\n    <title>Test Page for Webserver</title>\n    <style>\n        body {\n            background-color: black;\n            color: white;\n        }\n    </style>\n</head>\n<body>\n    <h1>Welcome to Diogo's & Paulo's Webserver</h1>\n    <p>This is a test page for our webserver. If you can see this page, it means our webserver is working correctly.</p>\n    <a href=https://github.com/diogoforte style=\\\"color: white;\\\">Visit Diogo's GitHub Profile</a>\n    <a href=https://github.com/PauloBastos96 style=\\\"color: white;\\\">Visit Paulo's GitHub Profile</a>\n</body>\n</html>\n"

#define HTTP_RESPONSE2 "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 600\n\n<!DOCTYPE html>\n<html>\n<head>\n    <title>Webserver</title>\n    <meta charset=\"utf-8\">\n    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n    <style>\n        body {\n            font-family: Arial, sans-serif;\n            margin: 0;\n            padding: 0;\n            background-color: #1a1a1a;\n        }\n\n        h1 {\n            text-align: center;\n            margin-top: 50px;\n            color: #dad7ff;\n        }\n\n        p {\n            text-align: center;\n            margin-top: 20px;\n            color: #dad7ff;\n        }\n    </style>\n</head>\n<body>\n<h1>Webserver</h1>\n<p>🟢 Webserver is running</p>\n</body>\n</html>\n"

#pragma endregion

#endif