/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asemykin <asemykin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/20 18:29:27 by asemykin          #+#    #+#             */
/*   Updated: 2026/01/10 01:30:15 by asemykin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <iostream>
# include <vector>
# include <map>

#define BUFFERSIZE 2048

struct ConfigLocations
{
    std::string path;
    std::vector<std::string> methods;
    std::string index;
    std::string uploadDir;
};

struct ConfigServer
{
    std::string port;
    std::string host;
    std::string root;

    std::map<std::string, std::string>  errorPages;
    size_t                              maxBodySize;
    std::vector<ConfigLocations>        locations;
};

class Config
{
    private:
        std::vector<int>            _ports;       // dynamic array
        std::string                 _root;
        std::map<int, std::string>  _errorPages;  // key value container
        std::string                 _config;
        void parseFile(const std::string &filename);
        
    public:    
        Config();
        Config(const Config &copy);
        Config &operator=(const Config &copy);
        ~Config();

        std::vector<int>    getPorts() const;
        std::string         getRoot() const;
        std::string         getErrorPage(int errorCode) const;
        
        void loadFile(const std::string &path);
        std::string parseConfig(const std::string &path);
};
