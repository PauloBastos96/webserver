#include <config.hpp>
#include <iostream>
#include <server.hpp>
#include <webserver.hpp>
#include <location.hpp>

config::config() {
}

config::~config() {
}

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

void config::set_index(const std::string &index) {
    index_.push_back(index);
}

void config::set_error_page(const uint &error_code, const std::string &error_page) {
    error_page_.insert(std::pair<uint, std::string>(error_code, error_page));
}

void config::set_max_client_body_size(const std::string &max_client_body_size) {
    max_client_body_size_ = max_client_body_size;
}

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

void config::set_auto_index(const bool &autoindex) {
    auto_index_ = autoindex;
}

const bool &config::get_auto_index() const {
    return (auto_index_);
}

void config::parse_config_file(const std::string &path, std::vector<server> &servers) {
    std::ifstream file(path.c_str());
    if (!file.is_open())
        WebServer::log("[CONFIG] Could not open file " + path, error);
    std::string line;
    int line_number = 0;
    while (std::getline(file, line) && !file.eof()) {
        line_number++;
        if (line.size() >= 2 && line.find("server") != std::string::npos && line.at(line.size() - 2) == '{') {
            server server;
            while (std::getline(file, line) && !file.eof() && line.find('}') == std::string::npos) {
                line_number++;
                if (line.find("listen") != std::string::npos) {
                    host(line, server);
                    port(line, server);
                }
                if (line.find("server_name") != std::string::npos)
                    server_name(line, server);
                if (line.find("root") != std::string::npos)
                    server.get_config().set_root(line.substr(line.find_first_of(' ') + 1));
                if (line.find("index") != std::string::npos)
                    index(line, server);
                if (line.find("error_page") != std::string::npos) {
                    if (parse_error_page(line, server))
                        WebServer::log("[CONFIG] Invalid error page directive", error);
                }
                if (line.find("max_client_body_size") != std::string::npos)
                    server.get_config().set_max_client_body_size(line.substr(line.find_first_of(' ') + 1));
                if (line.find("location") != std::string::npos && line.at(line.size() - 2) == '{')
                    location(line, server, line_number, file);
                if (line.find("autoindex") != std::string::npos) {
                    std::string autoindex = line.substr(line.find_first_of(' ') + 1);
                    server.get_config().set_auto_index(autoindex != "off");
                }
            }
            if (line.find('}') == std::string::npos) {
                WebServer::log("[CONFIG] Missing closing bracket", error);
            }
            WebServer::log("[CONFIG] Server configuration parsed", info);
            servers.push_back(server);
        }
    }
    file.close();
}

void config::display_configs(std::vector<server> &servers) {
    for (size_t i = 0; i < servers.size(); i++) {
        std::cout << BLUE << "Host " << servers[i].get_host() << ":" << servers[i].get_port() << std::endl;
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

/// @brief Parse the IP address from the line from the config file and set it to the server object
/// @param line The line from the config file
/// @param server The server object
void config::host(const std::string &line, server &server) {
    std::string host;
    if (line.find_first_of(':') != std::string::npos)
        host = line.substr(line.find_first_of(' ') + 1, line.find_first_of(':') - line.find_first_of(' ') - 1);
    else
        host = line.substr(line.find_first_of(' ') + 1);
    server.set_host(host);
}

/// @brief Parse the port from the line from the config file and set it to the server object
/// @param line The line from the config file
/// @param server The server object
void config::port(const std::string &line, server &server) {
    char *end;
    long int port = 80;
    if (line.find_first_of(':') != std::string::npos) {
        port = std::strtol(line.substr(line.find_first_of(':') + 1).c_str(), &end, 10);
        if (end == line.c_str() || *end != '\0' || port < 0 || port > 65535) {
            WebServer::log("[CONFIG] Invalid port number", warning);
            port = 80;
        }
    } else
        WebServer::log("[CONFIG] Port number not found", warning);
    server.set_port(static_cast<int>(port));
}

/// @brief Parse the server name from the line from the config file and set it to the server object
/// @param line The line from the config file
/// @param server The server object
void config::server_name(const std::string &line, server &server) {
    std::stringstream ss(line);
    std::string word;
    while (ss >> word) {
        if (word != "server_name")
            server.set_server_name(word);
    }
}

/// @brief Parse the index from the line from the config file and set it to the server object
/// @param line The line from the config file
/// @param server The server object
void config::index(const std::string &line, server &server) {
    server.get_config().get_index().clear();
    std::stringstream ss(line);
    std::string word;
    while (ss >> word) {
        if (word != "index")
            server.get_config().set_index(word);
    }
}

/// @brief Parse location block from the config file and add it to the server object
/// @param line The line from the config file
/// @param server The server object
/// @param line_number The starting line number
/// @param file The config file stream
void config::location(std::string line, server &server, int &line_number, std::ifstream &file) {
    Location location;
    location.set_path(line.substr(line.find_first_of(' ') + 1, line.find_last_of(' ') - line.find_first_of(' ') - 1));
    while (std::getline(file, line) && !file.eof() && line.find('}') == std::string::npos) {
        line_number++;
        if (line.find("root") != std::string::npos)
            location.get_config().set_root(line.substr(line.find_first_of(' ') + 1));
        if (line.find("index") != std::string::npos)
            location.get_config().set_index(line.substr(line.find_first_of(' ') + 1));
        if (line.find("error_page") != std::string::npos)
            if (parse_error_page(line, location))
                WebServer::log("[CONFIG] Invalid error page directive", error);
        if (line.find("max_client_body_size") != std::string::npos)
            location.get_config().set_max_client_body_size(line.substr(line.find_first_of(' ') + 1));
        if (line.find("location") != std::string::npos && line.at(line.size() - 2) == '{')
            WebServer::log("[CONFIG] Missing closing bracket", error);
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