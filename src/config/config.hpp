#ifndef CONFIG_HPP
# define CONFIG_HPP

#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <map>

class Server;
class Location;

class config {
    // Parse config file
public:
    static void parse_config_file(const std::string &path, std::vector<Server> &servers);

    static void display_configs(std::vector<Server> &servers);

    static void host(const std::string &line, Server &server);

    static void port(const std::string &line, Server &server);

    static void location(std::string line, Server &server, std::ifstream &file);

    static void limit_except(const std::string &line, Location &location);

    static void check_semicolon(const std::string &line);

    // Config
public:
    config();

    ~config();

    config(const config &other);

    config &operator=(const config &other);

    const std::string &get_root();

    std::vector<std::string> &get_index();

    std::map<uint, std::string> &get_error_page();

    const std::string &get_max_client_body_size();

    const bool &get_auto_index() const;

    void set_root(const std::string &root);

    void set_index(const std::string &index);

    void set_error_page(const uint &error_code, const std::string &error_page);

    void set_max_client_body_size(const std::string &max_client_body_size);

    void set_auto_index(const bool &autoindex);

private:
    std::string root_;
    std::vector<std::string> index_;
    std::map<uint, std::string> error_page_;
    std::string max_client_body_size_;
    bool auto_index_;
};

#endif