/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asemykin <asemykin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/20 18:29:27 by asemykin          #+#    #+#             */
/*   Updated: 2026/01/15 19:17:22 by asemykin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <iostream>
# include <vector>
# include <map>
# include <cstdlib>
#include <sstream>

#define BUFFERSIZE 2048

struct ConfigLocations
{
    std::string path;
    std::vector<std::string> methods;
    std::string index;
    std::string uploadDir;
    bool autoindex;
    bool        isMethodAllowed(const std::string &method)const
                {
                    for (size_t i = 0; i < methods.size(); i++)
                    {
                        // std::cout << "METHOD IS:" << methods[i] << std::endl; 
                        if(methods[i] == method)
                            return true;
                    }
                    return false;
                }
};

struct ConfigServer
{
    std::vector<std::string> lines;
    
    int         port;
    std::string host;
    std::string root;

    std::map<int, std::string>      errorPages;
    size_t                          maxBodySize;
    std::vector<ConfigLocations>    locations;

    const ConfigLocations *getMatchingLocation(const std::string &path)const;
};

class Config
{
    private:
        int                         _ports;       // dynamic array
        std::string                 _root;
        std::map<int, std::string>  _errorPages;  // key value container
        std::string                 _config;
        // void parseFile(const std::string &filename);
        std::vector<ConfigServer>   _servers;
    public:    
        Config();
        Config(const Config &copy);
        Config &operator=(const Config &copy);
        ~Config();

        int         getPorts() const;
        std::string getRoot() const;
        std::string getErrorPage(int errorCode) const;
        
        std::vector<ConfigServer> getServers();
        ConfigServer getServer(int port);
        // const ConfigLocations  *getMatchingLocation(const ConfigServer &server, const std::string &path);
        
        void setServer(const std::vector<ConfigServer> &servers);
        
        void loadFile(const std::string &path);
        std::vector<ConfigServer> parseConfig();
        int checkServerExists(int port);
        // size_t singleMatchingLocation(const ConfigLocations &path_conf, const std::string &path_client);
};
