/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maricard <maricard@student.porto.com>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/17 15:58:21 by maricard          #+#    #+#             */
/*   Updated: 2023/10/20 10:52:03 by maricard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include <stdio.h>
#include <fstream>
#include "macros.hpp"

class Request
{
	private:
		std::string _method;
		std::string _path;
		std::string _protocol;
		std::map<std::string, std::string> _header;
		std::vector<std::string> _body;

	public:
		Request();
		Request(std::string request);
		Request(const Request& copy);
		~Request();
		Request& operator=(const Request& other);

		std::string getMethod() const;
		std::string getPath() const;
		std::string getProtocol() const;

		void	parseRequest(std::string request);
		void	displayVars();
};
