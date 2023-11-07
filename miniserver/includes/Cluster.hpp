/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maricard <maricard@student.porto.com>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/13 12:41:18 by bsilva-c          #+#    #+#             */
/*   Updated: 2023/11/07 20:27:15 by maricard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Server.hpp"
#include "Request.hpp"
#include "Cgi.hpp"
#include <sys/select.h>
#include <unistd.h>
#include <fstream>

class Cluster
{
	std::vector<Server*> _serverList;

public:
	Cluster();
	Cluster(const Cluster&);
	Cluster& operator=(const Cluster&);
	~Cluster();

	int configure(const std::string& file_path);
	void run();
};
