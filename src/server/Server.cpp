#include "Server.hpp"

server::server()
{
	port_ = 80;
	host_ = "127.0.0.1";
	config_.setRoot("/");
	config_.setMaxClientBodySize("1m");
	config_.setIndex("index.html");
	config_.setAutoIndex(false);
}

server::~server() {};

void	server::set_host(const std::string &host)
{
	host_ = host;
}
void	server::set_port(const int &port)
{
	port_ = port;
}
void	server::set_server_name(const std::string &server_name)
{
	server_name_.push_back(server_name);
}

const int	&server::get_port() const
{
	return (port_);
}

const std::string	&server::get_host()
{
	return (host_);
}

const vector<std::string>	&server::get_server_name()
{
	return (server_name_);
}

ServerConfig	&server::get_config()
{
	return (config_);
}

vector<Location>	&server::getlocations()
{
	return (locations_);
}