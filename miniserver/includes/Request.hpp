/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maricard <maricard@student.porto.com>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/17 15:58:21 by maricard          #+#    #+#             */
/*   Updated: 2023/11/09 10:47:30 by maricard         ###   ########.fr       */
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
#include "Server.hpp"
#include "Response.hpp"
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <cstdio>

class Server;

class Request
{
	private:
		std::string _method;
		std::string _path;
		std::string _query;
		std::string _protocol;
		std::map<std::string, std::string> _header;
		std::vector<char> _body;
		u_int32_t 	_bodyLength;
		u_int32_t 	_maxBodySize;
		std::string _uploadStore;

	public:
		Request();
		Request(Server* server);
		Request(const Request& copy);
		~Request();
		Request& operator=(const Request& other);

		std::string getMethod() const;
		std::string getPath() const;
		std::string getQuery() const;
		std::string getProtocol() const;
		std::map<std::string, std::string>	getHeader() const;
		std::vector<char>	getBody() const;
		std::string getUploadStore() const;

		int			parseRequest(char* buffer, int64_t& bytesRead);
		int			parseBody(char* buffer, int64_t bytesRead);
		int 		checkErrors();
		void		displayVars();
		int			isValidRequest(Server& server, int& error);
};