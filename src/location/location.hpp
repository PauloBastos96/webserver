#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <config.hpp>
#include <string>

class Location {
public:
    Location();

    ~Location();

    const std::string &get_path();

    config &get_config();

    std::vector<std::string> &get_allowed_methods();

    void set_path(const std::string &path);

    void set_allowed_methods(const std::string &method);

private:
    std::string path_;
    config config_;
    std::vector<std::string> allowed_methods_;
};

#endif