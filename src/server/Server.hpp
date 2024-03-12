/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: paulorod <paulorod@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/20 12:31:24 by paulorod          #+#    #+#             */
/*   Updated: 2024/03/12 13:04:33 by paulorod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include "../server_config/ServerConfig.hpp"
#include "../location/Location.hpp"
#include <vector>

class Server {
public:
    Server();

    ~Server();

    const int &getPort();

    const string &getHost();

    const vector<string> &getServerName();

    ServerConfig &getConfig();

    vector<Location> &getLocations();

    void setHost(const string &host);

    void setPort(const int &port);

    void setServerName(const string &server_name);

private:
    std::string _host;
    int _port;
    vector<string> _serverName;
    ServerConfig _config;
    vector<Location> _locations;
};

#endif