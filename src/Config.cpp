/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asemykin <asemykin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/21 10:57:07 by asemykin          #+#    #+#             */
/*   Updated: 2026/01/10 01:30:46 by asemykin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../includes/Config.hpp"

Config::Config()
{}

Config::Config(const Config &copy)
{
    *this = copy;
}

Config &Config::operator=(const Config &copy)
{
    if(this != &copy)
    {
           _ports = copy._ports;
           _root = copy._root;
           _errorPages = copy._errorPages;  
           _config = copy._config;
    }
    
    return *this;
}

Config::~Config()
{}

#include <fcntl.h>
#include <unistd.h>
#include <sstream>

void Config::loadFile(const std::string &path)
{
    int fd = open(path.c_str(), O_RDONLY);
    if(fd < 0)
    {
        // ERROR
    }

    char buffer[BUFFERSIZE];
    std::stringstream ss;
    // std::string configFile;
    _config = "";
    while(1)
    {
        ssize_t bytes = read(fd, buffer, sizeof(buffer));
        if(bytes <= 0)
            break;
        _config.append(buffer, bytes);
    }

    close(fd);
    
    std::cout << _config << std::endl;
    
}

std::string Config::parseConfig(const std::string &path)
{
    std::string line;

    return line;
}