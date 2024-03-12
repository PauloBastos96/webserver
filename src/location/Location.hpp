/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: paulorod <paulorod@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/20 14:00:17 by paulorod          #+#    #+#             */
/*   Updated: 2024/03/12 13:00:09 by paulorod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

#include "../server_config/ServerConfig.hpp"

class Location
{
	private:
		string			_path;
		ServerConfig	_config;
		vector<string>	_allowedMethods;
	public:
		Location();
		~Location();
		
		const string	&getPath(void);
		ServerConfig	&getConfig(void);
		vector<string>	&getAllowedMethods(void);

		void			setPath(const string &path);
		void			setAllowedMethods(const string &method);
};

#endif