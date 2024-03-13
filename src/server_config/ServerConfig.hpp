/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: paulorod <paulorod@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/19 14:01:35 by paulorod          #+#    #+#             */
/*   Updated: 2024/03/12 13:12:51 by paulorod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

#include <vector>
#include <map>
#include <string>

using std::vector;
using std::map;

typedef unsigned int uint;

class ServerConfig {
private:
    std::string _root;
    vector<std::string> _index;
    map<uint, std::string> _errorPage;
    std::string _maxClientBodySize;
    bool _autoIndex;

public:
    ServerConfig();

    ~ServerConfig();

    const std::string &getRoot(void);

    vector<std::string> &getIndex(void);

    map<uint, std::string> &getErrorPage(void);

    const std::string &getMaxClientBodySize(void);

    const bool &getAutoIndex(void);

    void setRoot(const std::string &root);

    void setIndex(const std::string &index);

    void setErrorPage(const uint &error_code, const std::string &error_page);

    void setMaxClientBodySize(const std::string &max_client_body_size);

    void setAutoIndex(const bool &auto_index);
};

#endif