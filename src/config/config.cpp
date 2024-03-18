#include "config.hpp"
#include "config.tpp"
#include <iostream>
#include <location.hpp>
#include <algorithm>

#pragma region Constructors
config::config() {}

config::~config() {}

config::config(const config &other) {
    *this = other;
}

config &config::operator=(const config &other) {
    if (this == &other)
        return (*this);
    root_ = other.root_;
    index_ = other.index_;
    error_page_ = other.error_page_;
    max_client_body_size_ = other.max_client_body_size_;
    auto_index_ = other.auto_index_;
    return (*this);
}
#pragma endregion

#pragma region Setters
void config::set_index(const std::string &index) {
    index_.push_back(index);
}

void config::set_error_page(const uint &error_code, const std::string &error_page) {
    error_page_.insert(std::pair<uint, std::string>(error_code, error_page));
}

void config::set_max_client_body_size(const std::string &max_client_body_size) {
    max_client_body_size_ = max_client_body_size;
}

void config::set_auto_index(const bool &autoindex) {
    auto_index_ = autoindex;
}
#pragma endregion

#pragma region Getters
const std::string &config::get_root() {
    return (root_);
}

std::vector<std::string> &config::get_index() {
    return (index_);
}

std::map<uint, std::string> &config::get_error_page() {
    return (error_page_);
}

const std::string &config::get_max_client_body_size() {
    return (max_client_body_size_);
}

const bool &config::get_auto_index() const {
    return (auto_index_);
}
#pragma endregion

/// @brief Parse the IP address from the line from the config file and set it to the server object
/// @param line The line from the config file
/// @param server The server object
void config::host(const std::string &line, Server &server) {
    std::string host;
    if (line.at(line.size() - 1) != ';')
        WebServer::log(WARM_CFG_SEMICOLON, warning);
    if (line.find_first_of(':') != std::string::npos)
        host = line.substr(line.find_first_of(' ') + 1, line.find_first_of(':') - line.find_first_of(' ') - 1);
    else
        host = line.substr(line.find_first_of(' ') + 1, line.find_first_of(';') - line.find_first_of(' ') - 1);
    server.set_host(host);
}

/// @brief Parse the port from the line from the config file and set it to the server object
/// @param line The line from the config file
/// @param server The server object
void config::port(const std::string &line, Server &server) {
    int port = 80;
    if (line.at(line.size() - 1) != ';')
        WebServer::log(WARM_CFG_SEMICOLON, warning);
    if (line.find_first_of(':') != std::string::npos) {
        std::string port_str = line.substr(line.find_first_of(':') + 1);
        std::stringstream ss(port_str);
        ss >> port;
        if (ss.fail() || port < 0 || port > 65535) {
            WebServer::log(WARN_CFG_PORT, warning);
            port = 80;
        }
    } else
        WebServer::log(INFO_CFG_NOPORT, info);
    server.set_port(port);
}

/// @brief Parse location block from the config file and add it to the server object
/// @param line The line from the config file
/// @param server The server object
/// @param line_number The starting line number
/// @param file The config file stream
void config::location(std::string line, Server &server, std::ifstream &file) {
    Location location;
    location.set_path(line.substr(line.find_first_of(' ') + 1, line.find_last_of(' ') - line.find_first_of(' ') - 1));
    while (std::getline(file, line) && !file.eof() && line.find('}') == std::string::npos) {
        if (line.find("root") != std::string::npos)
            location.get_config().set_root(line.substr(line.find_first_of(' ') + 1, line.find_first_of(';') - line.find_first_of(' ') - 1));
        if (line.find("index") != std::string::npos && line.find("autoindex") == std::string::npos)
            location.get_config().set_index(line.substr(line.find_first_of(' ') + 1, line.find_first_of(';') - line.find_first_of(' ') - 1));
        if (line.find("error_page") != std::string::npos)
            parse_error_page(line, location);
        if (line.find("max_client_body_size") != std::string::npos)
            location.get_config().set_max_client_body_size(line.substr(line.find_first_of(' ') + 1, line.find_first_of(';') - line.find_first_of(' ') - 1));
        if (line.find("location") != std::string::npos && line.at(line.size() - 1) == '{')
            WebServer::log(ERR_CFG_MISSING_BRACKET, error);
        if (line.find("limit_except") != std::string::npos)
            limit_except(line, location);
        if (line.find("autoindex") != std::string::npos) {
            std::string autoindex = line.substr(line.find_first_of(' ') + 1);
            location.get_config().set_auto_index(autoindex != "off");
        }
    }
    server.get_locations().push_back(location);
}

/// @brief Parse the limit_except block from the config file and add it to the location object
/// @param line The line from the config file
/// @param location The location object
void config::limit_except(const std::string &line, Location &location) {
    std::stringstream ss(line);
    std::string word;
    while (ss >> word) {
        if (word != "limit_except" && word.find_first_of('{') == std::string::npos)
            location.set_allowed_methods(word);
    }
}

/// @brief Check line for ending semicolon and log a warning if it's missing
/// @param line The line from the config file
void config::check_semicolon(const std::string &line) {
    if (line.size() == 0)
        return;
    if (line.at(line.size() - 1) != '{' && line.at(line.size() - 1) != '}' && line.at(line.size() - 1) != ';')
        WebServer::log(WARM_CFG_SEMICOLON, warning);
}

/// @brief Parse the config file and create server objects
/// @param path The path to the config file
/// @param servers The vector of server objects
void config::parse_config_file(const std::string &path, std::vector<Server> &servers) {
    std::ifstream file(path.c_str());
    if (!file.is_open())
        WebServer::log(ERR_CANT_OPEN_FILE + path, error);
    std::string line;
    while (std::getline(file, line) && !file.eof()) {
        if (line.size() >= 2 && line.find("server") != std::string::npos && line.at(line.size() - 1) == '{') {
            Server server;
            while (std::getline(file, line) && !file.eof() && line.find('}') == std::string::npos) {
                config::check_semicolon(line);
                if (line.find("listen") != std::string::npos) {
                    if (std::count(line.begin(), line.end(), ':') > 1)
                        WebServer::log(ERR_CFG_LISTEN, error);
                    host(line, server);
                    port(line, server);
                }
                if (line.find("server_name") != std::string::npos)
                    multi_value(line, server, std::string("server_name"));
                if (line.find("root") != std::string::npos)
                {
                    if (line.find_first_of(' ') != std::string::npos)
                        server.get_config().set_root(line.substr(line.find_first_of(' ') + 1, line.find_first_of(';') - line.find_first_of(' ') - 1));
                }
                if (line.find("index") != std::string::npos && line.find("autoindex") == std::string::npos)
                    multi_value(line, server, "index");
                if (line.find("error_page") != std::string::npos)
                    parse_error_page(line, server);
                if (line.find("max_client_body_size") != std::string::npos)
                    server.get_config().set_max_client_body_size(line.substr(line.find_first_of(' ') + 1, line.find_first_of(';') - line.find_first_of(' ') - 1));
                if (line.find("location") != std::string::npos){
                    if (line.at(line.size() - 1) == '{')
                        location(line, server, file);
                    else
                        WebServer::log(ERR_CFG_MISSING_BRACKET, error);
                }
                if (line.find("autoindex") != std::string::npos) {
                    std::string autoindex = line.substr(line.find_first_of(' ') + 1, line.find_first_of(';') - line.find_first_of(' ') - 1);
                    server.get_config().set_auto_index(autoindex != "off");
                }
                if (line.find("server") != std::string::npos && line.at(line.size() - 1) == '{')
                    WebServer::log(ERR_CFG_MISSING_BRACKET, error);
            }
            if (line.find('}') == std::string::npos) {
                WebServer::log(ERR_CFG_MISSING_BRACKET, error);
            }
            servers.push_back(server);
        }
    }
    file.close();
}

void config::display_configs(std::vector<Server> &servers) {
    for (size_t i = 0; i < servers.size(); i++) {
        std::cout << BLUE << "Host " << servers[i].get_host() << ":" << servers[i].get_port() << RESET << std::endl;
        std::cout << "Server name: ";
        for (size_t j = 0; j < servers[i].get_server_name().size(); j++) {
            std::cout << servers[i].get_server_name()[j] << " ";
        }
        std::cout << std::endl;
        std::cout << "Root: " << servers[i].get_config().get_root() << std::endl;
        std::cout << "Index: ";
        for (size_t j = 0; j < servers[i].get_config().get_index().size(); j++) {
            std::cout << servers[i].get_config().get_index()[j] << " ";
        }
        std::cout << std::endl;
        std::cout << "Error pages: ";
        for (std::map<uint, std::string>::iterator it = servers[i].get_config().get_error_page().begin();
             it != servers[i].get_config().get_error_page().end(); it++) {
            std::cout << it->first << "=>" << it->second << " ";
        }
        std::cout << std::endl;
        std::cout << "Max client body size: " << servers[i].get_config().get_max_client_body_size() << std::endl;
        std::cout << "Autoindex: " << (servers[i].get_config().get_auto_index() ? "on" : "off") << std::endl;
        std::cout << "Locations: " << std::endl;
        for (size_t j = 0; j < servers[i].get_locations().size(); j++) {
            std::cout << "	" << servers[i].get_locations()[j].get_path() << std::endl;
            std::cout << "	" << "Root: " << servers[i].get_locations()[j].get_config().get_root() << std::endl;
            std::cout << "	" << "Index: ";
            for (size_t k = 0; k < servers[i].get_locations()[j].get_config().get_index().size(); k++) {
                std::cout << servers[i].get_locations()[j].get_config().get_index()[k];
            }
            std::cout << std::endl;
            std::cout << "	" << "Error pages: ";
            for (std::map<uint, std::string>::iterator it = servers[i].get_locations()[j].get_config().get_error_page().
                         begin();
                 it != servers[i].get_locations()[j].get_config().get_error_page().end(); it++) {
                std::cout << "	" << it->first << " " << it->second << " ";
            }
            std::cout << std::endl;
            std::cout << "	" << "Max client body size: " << servers[i].get_locations()[j].get_config().
                    get_max_client_body_size()
                    << std::endl;
            std::cout << "	" << "Allowed methods: ";
            for (size_t k = 0; k < servers[i].get_locations()[j].get_allowed_methods().size(); k++) {
                std::cout << servers[i].get_locations()[j].get_allowed_methods()[k] << " ";
            }
            std::cout << std::endl;
            std::cout << "	" << "Autoindex: " << (
                        servers[i].get_locations()[j].get_config().get_auto_index() ? "on" : "off") <<
                    std::endl;
            std::cout << std::endl;
        }
    }
}
