#ifndef WebServer_H
# define WebServer_H

# include <fstream>

/**
 * @brief Enum for log levels.
 */
enum log_level {
    info, ///< Informational log level
    warning, ///< Warning log level
    error, ///< Error log level
};

# define RED "\033[0;91m"
# define YELLOW "\033[1;33m"
# define BLUE "\033[0;94m"
# define RESET "\033[0m"

//Error messages

# define INFO_CFG_NOPORT "[CONFIG] No port number specified, defaulting to 80"

# define WARN_CFG_PORT "[CONFIG] Invalid port number, defaulting to 80"
# define WARM_CFG_SEMICOLON "[CONFIG] Missing semicolon"

# define ERR_CFG_LISTEN "[CONFIG] Invalid listen directive"
# define ERR_CFG_MISSING_BRACKET "[CONFIG] Missing closing bracket"
# define ERR_CANT_OPEN_FILE "[CONFIG] Could not open file "
# define ERR_CFG_INVALID_ERROR_PAGE "[CONFIG] Invalid error page directive" 

#endif