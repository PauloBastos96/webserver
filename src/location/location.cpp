#include <location.hpp>

Location::Location() {
    config_.set_auto_index(false);
}

Location::~Location() {
}

const std::string &Location::get_path() {
    return (path_);
}

config &Location::get_config() {
    return (config_);
}

void Location::set_path(const std::string &path) {
    path_ = path;
}

std::vector<std::string> &Location::get_allowed_methods() {
    return (allowed_methods_);
}

void Location::set_allowed_methods(const std::string &method) {
    allowed_methods_.push_back(method);
}