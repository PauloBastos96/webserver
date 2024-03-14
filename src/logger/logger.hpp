#ifndef WebServer_H
#define WebServer_H

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
# define RESET "\033[0m"

#endif