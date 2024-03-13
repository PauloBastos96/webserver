#include "Location.hpp"

Location::Location()
{
	config_.setAutoIndex(false);
}
Location::~Location(){}

const std::string	&Location::get_path(void)
{
	return (path_);
}

ServerConfig	&Location::get_config(void)
{
	return (config_);
}

void	Location::set_path(const std::string &path)
{
	path_ = path;
}

vector<std::string>	&Location::get_allowed_methods(void)
{
	return (allowed_methods_);
}

void	Location::set_allowed_methods(const std::string &method)
{
	allowed_methods_.push_back(method);
}