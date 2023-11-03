/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maricard <maricard@student.porto.com>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/17 17:14:44 by maricard          #+#    #+#             */
/*   Updated: 2023/10/27 17:43:20 by bsilva-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request()
{

}

Request::Request(char* buffer) : _buffer(buffer), _bodyLength(-1)
{

}

Request::Request(const Request& copy)
{
	*this = copy;
}

Request::~Request()
{
	_header.clear();
	_body.clear();
}

Request& Request::operator=(const Request& other)
{
	if (this == &other)
		return *this;

	_method = other._method;
	_path = other._path;
	_query = other._query;
	_protocol = other._protocol;
	_header = other._header;
	_body = other._body;
	_argv = other._argv;
	_envp = other._envp;
	_bodyLength = other._bodyLength;
	return *this;
}

std::string Request::getMethod() const
{
	return _method;
}

std::string Request::getPath() const
{
	return _path;
}

std::string Request::getQuery() const
{
	return _query;
}

std::string Request::getProtocol() const
{
	return _protocol;
}

int	Request::handleRequest(char* header_buffer, int bytesRead)
{
	int bytesToRead = parseRequest(header_buffer, bytesRead);
	
	return bytesToRead;
}

void	Request::handleBody(char* body_buffer, int bytesRead)
{
	for (int i = 0; i < bytesRead; i++)
		_body.push_back(body_buffer[i]);
}

int	Request::parseRequest(char* header_buffer, int bytesRead)
{
	(void)bytesRead;
	std::string request = header_buffer;
	std::stringstream ss(request);
	std::string line;

	ss >> _method >> _path >> _protocol;

	if (_path.find("?") != std::string::npos)
	{
		_query = _path.substr(_path.find("?") + 1, _path.length());
		_path = _path.substr(0, _path.find("?"));
	}

	std::getline(ss, line);
	while (std::getline(ss, line) && line != "\r")
	{
    	size_t pos = line.find(':');
    
		if (pos != std::string::npos)
		{
			std::string first = line.substr(0, pos);
			std::string second = line.substr(pos + 2, line.length());
			_header[first] = second;
    	}
    }

	if (line != "\r")
	{
		//! error 413 Entity to large
		MESSAGE("413 ENTITY TO LARGE", ERROR);
		return 0;
	}

	if (checkErrors() == true)
		return 0;

	size_t pos = request.find("\r\n\r\n") + 4;
	int k = 0;

	for(int i = pos; i < bytesRead; i++)
	{
		_body.push_back(header_buffer[i]);
		k++;
	}

	if (k < _bodyLength)
		return _bodyLength - k;
	
	return 0;
}

//! verify config max body!
void	Request::runCGI()
{
	setArgv();
	setEnvp();

	std::string filename = ".tmp";

	{
		std::ofstream file(filename.c_str());
		if (file.is_open())
		{
			for (unsigned i = 0; i < _body.size(); i++)
				file <<  _body[i];
			file.close();
		}
		else
		{
			MESSAGE("CREATE FILE ERROR", ERROR)
			return ;
		}
	}

    FILE* file = std::fopen(filename.c_str(), "r");
	int file_fd = -1;
	
	if (file != NULL)
        file_fd = fileno(file);
	else
	{
		MESSAGE("file fd", ERROR)
		return ;
	}

	int pipe_write[2];
	if (pipe(pipe_write) == -1)
	{
		MESSAGE("PIPE ERROR", ERROR);
		return ;
	}

	//for (unsigned i = 0; i < _body.size(); i++)
	//	write(file_fd, &_body[i], 1);
	
	int pid = fork();
	if (pid == 0)
	{
		// child process
		dup2(file_fd, STDIN_FILENO);
		close(file_fd);

    	dup2(pipe_write[WRITE], STDOUT_FILENO);
    	close(pipe_write[WRITE]);
		close(pipe_write[READ]);

		execve(_argv[0], _argv, _envp);
		MESSAGE("EXECVE ERROR", ERROR);
		exit(0);
	}
	else
	{
		// parent process
		close(file_fd);
		waitpid(pid, NULL, 0);
	}

	char buffer[4096];

	read(pipe_write[READ], buffer, 4096);

	std::cout << F_RED "OUTPUT: " RESET 
			  << std::endl 
			  << buffer
			  << std::endl;

	if (std::remove(filename.c_str()) != 0)
	{
       MESSAGE("REMOVE FILE ERROR", ERROR)
    }

	close(pipe_write[READ]);
    std::fclose(file);
}

void	Request::setArgv()
{
	_argv[0] = argv0.c_str();
	_argv[1] = argv1.c_str();
	_argv[2] = NULL;
}

void	Request::setEnvp()
{
	int i = 0;

	if (!(_method.empty()))
	{
		std::string string = "REQUEST_METHOD=" + _method;
		_envp[i++] = string.c_str();
	}
	if (!(_query.empty()))
	{
		std::string string = "QUERY_STRING=" + _query;
		_envp[i++] = string.c_str();
	}
	if (!(_header["Content-Length"].empty()))
	{
		std::string string = "CONTENT_LENGTH=" + _header["Content-Length"];
		_envp[i++] = string.c_str();
	}
	if (!(_header["Content-Type"].empty()))
	{
		std::string string = "CONTENT_TYPE=" + _header["Content-Type"];
		_envp[i++] = string.c_str();
	}
	
	_envp[i] = NULL;
}

bool Request::hasCGI()
{
	//! temporary solution
	if (getMethod() == "GET" && getPath() == "/getDateTime")
		return true;
	else if (getMethod() == "POST" && getPath() == "/uploadFile")
		return true;
	return false;
}

bool	Request::checkErrors()
{
	if (_method == "POST" && (_header["Content-Type"].empty() ||
		_header["Content-Type"].find("multipart/form-data") == std::string::npos))
	{
		//! error 415 Unsupported Media Type
		MESSAGE("415 POST request without Content-Type", ERROR);
		return true;
	}
	if (_method == "POST" && _header["Content-Length"].empty())
	{
		//! error 411 Length Required
		MESSAGE("411 POST request without Content-Length", ERROR);
		return true;
	}
	else
	{
		std::istringstream ss(_header["Content-Length"]);
		ss >> _bodyLength;
	}

	return false;
}

void	Request::displayVars()
{
	std::cout << F_YELLOW "Method: " RESET + _method << std::endl;
	std::cout << F_YELLOW "Path: " RESET + _path << std::endl;
	std::cout << F_YELLOW "Query: " RESET + _query << std::endl;
	std::cout << F_YELLOW "Protocol: " RESET + _protocol << std::endl;

	if (_header.size() > 0)
		std::cout << F_YELLOW "Header" RESET << std::endl;
	
	std::map<std::string, std::string>::iterator it = _header.begin();
	for (; it != _header.end(); it++)
		std::cout << it->first + ": " << it->second << std::endl;

	if (_body.size() > 0)
		std::cout << F_YELLOW "Body: " RESET << _body.size() << std::endl;

	//for (unsigned i = 0; i < _body.size(); i++)
	//	std::cout << _body[i];
}
