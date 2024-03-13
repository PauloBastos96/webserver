#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <fstream>
#include <vector>
#include <server/Server.hpp>
#include <sstream>
#include <cstdlib>

class Config {
public:
    explicit Config(const std::string &config_file);

    ~Config();

    static void parse_config_file(const std::string &path, std::vector<server> &servers);

    static void display_configs(vector<server> &servers);


    static void host(std::string line, server &server);

    static void port(std::string line, server &server);

    static void server_name(std::string line, server &server);

    static void index(std::string line, server &server);

    static void location(std::string line, server &server, int &line_number, std::ifstream &file);

    static void limit_except(std::string line, Location &location);

    template<typename T>
    static int parseErrorPage(const std::string &line, T &obj) {
        std::stringstream ss(line);
        std::string word;
        while (ss >> word) {
            if (word != "error_page" && word.find_first_of('/') == std::string::npos) {
                if (word.find_first_not_of("0123456789") != std::string::npos)
                    return (1);
                obj.get_config().setErrorPage(std::atoi(word.c_str()), ss.str().substr(ss.str().find_first_of("/")));
            }
        }
        return (0);
    }
private :
    std::ofstream file;
};

#endif