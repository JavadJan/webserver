/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asemykin <asemykin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/21 10:57:07 by asemykin          #+#    #+#             */
/*   Updated: 2026/01/17 15:42:30 by asemykin         ###   ########.fr       */
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
    //std::string configFile;
    _config = "";
    while(1)
    {
        ssize_t bytes = read(fd, buffer, sizeof(buffer));
        if(bytes <= 0)
            break;
        _config.append(buffer, bytes);
    }

    close(fd);
    
    // std::cout << _config << std::endl;
    
}

std::string trim(const std::string &line)
{
    size_t start = line.find_first_not_of(" \t\r\n");
    if(start == std::string::npos)
        return "";
        
    size_t end = line.find_last_not_of(" \t\r\n");

    std::string newLine = line.substr(start, end-start+1);
    
    return newLine;        
}

std::vector<ConfigServer> Config::getServers()
{
    return _servers;
}
void Config::setServer(const std::vector<ConfigServer> &servers)
{
    _servers = servers;
}

std::vector<ConfigServer> Config::parseConfig()
{
    // trim and extract to save in vector
    std::string line;
    std::vector<std::string> lines;
    
    std::vector<ConfigServer> servers;
    
        
    
    if(_config.empty())
        return servers;
    
    size_t start = 0;
    while(1)
    {
        size_t end = _config.find("\n", start);
        
        if(end == std::string::npos)
            break;

        line = trim(_config.substr(start, end-start));
        size_t com = line.find("#");
        if(com != std::string::npos)
            line = line.substr(0, com);
        
        if(!line.empty())
        {
            lines.push_back(line);
        }
        
        start = end + 1;
    }
    
    if(lines.size() == 0)
        return servers;
    
    // for (size_t i = 0; i < lines.size(); i++)
    //     std::cout << lines[i] << std::endl;
    
    
    // extract servers
    
    
    size_t s = 0;
    
    for(size_t i = 0; i < lines.size(); i++)
    {
        if(lines[i] == "server" && i+1<=lines.size() && lines[i+1] == "{")
            i += 2;
        else
            continue;
        
        ConfigServer server;
        servers.push_back(server);
        int nested = 1;
        
        while(i < lines.size() && nested > 0)
        {
            if(lines[i] == "{")
                nested++;
            if(lines[i] == "}")
                nested--;
                
            if(nested > 0)
                servers[s].lines.push_back(lines[i]);
            
            i++;
        }
        s++;
        i--; // for loop will increment, so we need to decrement before
    }
    
    // std::string port;
    // std::string host;
    // std::string root;

    // std::map<std::string, std::string>  errorPages;
    // size_t                              maxBodySize;
    // std::vector<ConfigLocations>        locations;
    
    
    // parse servers // EDGE CASES NOT INCLUDED
    
    for (size_t x = 0; x < servers.size(); x++)
    {
        // std::vector<ConfigLocations> *locations;
        // servers[x].locations = &locations;
        for (size_t i = 0; i < servers[x].lines.size(); i++)
        {
            // std::cout << servers[x].lines[i] << std::endl;
            if(servers[x].lines[i].substr(0,6) == "listen")
            {
                // std::cout << "LISTEN" << std::endl;
                size_t first = servers[x].lines[i].find_first_not_of(" \t\r\n", 7);
                // std::istringstream ss(servers[x].lines[i].substr(first));
                size_t colon = servers[x].lines[i].find(":");
                size_t semic = servers[x].lines[i].find(";");
                size_t lineSize     = servers[x].lines[i].size();
                size_t space        = lineSize - (servers[x].lines[i].find_last_not_of(" \t\r\n", lineSize - 2)+2);
                
                servers[x].host = servers[x].lines[i].substr(first, colon-first);
                servers[x].port = std::atoi(servers[x].lines[i].substr(colon+1, semic-colon-1-space).c_str());
                // std::cout << "   :" << servers[x].host << ":" << std::endl;
                // std::cout << "   :" << servers[x].port << ":" << std::endl;
            }
            
            if(servers[x].lines[i].substr(0,4) == "root")
            {
                // std::cout << "ROOT" << std::endl;
                size_t first = servers[x].lines[i].find_first_not_of(" \t\r\n", 5);
                size_t semic = servers[x].lines[i].find(";");
                servers[x].root = servers[x].lines[i].substr(first, semic-first);
                // std::cout  << "   :" << servers[x].root << ":" << std::endl;
            }
            
            if(servers[x].lines[i].substr(0,10) == "error_page")
            {
                // std::cout << "ERROR_PAGE" << std::endl;
                size_t first = servers[x].lines[i].find_first_not_of(" \t\r\n", 11);
                if(first == std::string::npos)
                    continue;
                    
                std::istringstream iss(servers[x].lines[i].substr(first));
                std::string word;
                std::vector<std::string> meth;
                while(iss >> word)
                {
                    if(word != ";")
                        meth.push_back(word);
                }
                   
                if(meth.size() != 2)
                    continue;
                    
                int key             = std::atoi(meth[0].c_str()) ;
                std::string value   = meth[1];
                
                if(meth[0].size() != 3 || key < 100 || key > 599)
                    continue;

                size_t semic = meth[1].find(";");
                if(semic != std::string::npos)
                    value = value.substr(0, value.size()-1);
                    
                servers[x].errorPages[key] = value;
                
                // std::cout << "   :" << key << ":" << std::endl;
                // std::cout << "   :" << servers[x].errorPages[key] << ":" << std::endl;
            } 

            if(servers[x].lines[i].substr(0,13) == "max_body_size")
            {
                // std::cout << "max_body_size" << std::endl;
                size_t first = servers[x].lines[i].find_first_not_of(" \t\r\n", 14);
                size_t semic = servers[x].lines[i].find(";");
                servers[x].maxBodySize = std::atoi(servers[x].lines[i].substr(first, semic-first).c_str());
                // std::cout << "   :" << servers[x].maxBodySize << ":" << std::endl;
            }
            
            // std::string path;
            // std::vector<std::string> methods;
            // std::string index;
            // std::string uploadDir;
    
            // parse locations
            if(servers[x].lines[i].substr(0,8) == "location")
            {
                // std::cout << "LOCATION" << std::endl;
                size_t first = servers[x].lines[i].find("/");
                if(first == std::string::npos)
                    continue;

                std::string path = servers[x].lines[i].substr(first);

                ConfigLocations loca;
                loca.path = path;
                while(servers[x].lines[i] != "}")
                {
                    if(servers[x].lines[i].substr(0,7) == "methods")
                    {
                        // std::cout << "- methods" << std::endl;
                        size_t first = servers[x].lines[i].find_first_not_of(" \t\r\n", 8);
                        std::istringstream iss(servers[x].lines[i].substr(first));
                        std::string method;
                        while(iss >> method)
                        {
                            size_t semi = method.find(";");
                            std::string correctMethod;
                            if(semi == std::string::npos)
                            {
                                correctMethod = method;
                            }
                            else
                            {
                                std::string insert = trim(method.substr(0, semi));
                                correctMethod = insert;
                            }
                            if(correctMethod == "GET" || correctMethod == "POST" || correctMethod == "DELETE")
                            {
                                loca.methods.push_back(correctMethod);
                                // std::cout << "   :" << correctMethod << ":" << std::endl;
                            }
                            // else
                            //     std::cout << "   :" << correctMethod << ": WRONG METHODE" << std::endl;
                        }
                    }
                    
                    if(servers[x].lines[i].substr(0,5) == "index")
                    {
                        // std::cout << "- index" << std::endl;
                        size_t first = servers[x].lines[i].find_first_not_of(" \t\r\n", 6);
                        size_t semic = servers[x].lines[i].find(";");
                        if(semic == std::string::npos)
                            continue;
                        loca.index = trim(servers[x].lines[i].substr(first, semic-first));
                        // std::cout << "   :" << loca.index << ":" << std::endl;
                    }
                    
                    if(servers[x].lines[i].substr(0,10) == "upload_dir")
                    {
                        // std::cout << "- upload_dir" << std::endl;
                        size_t first = servers[x].lines[i].find_first_not_of(" \t\r\n", 11);
                        size_t semic = servers[x].lines[i].find(";");
                        if(semic == std::string::npos)
                            continue;
                        loca.uploadDir = trim(servers[x].lines[i].substr(first, semic-first));
                        // std::cout << "   :" << loca.uploadDir << ":" << std::endl;
                    }

                    if(servers[x].lines[i].substr(0, 9) == "autoindex")
                    {
                        size_t first = servers[x].lines[i].find_first_not_of(" \t\r\n", 10);
                        size_t semic = servers[x].lines[i].find(";");
                        if(semic == std::string::npos)
                            continue;
                        std::string value = trim(servers[x].lines[i].substr(first, semic-first));
                        if(value == "on")
                            loca.autoindex = true;
                        else
                            loca.autoindex = false;
                    }
                    
                    i++;
                }
                servers[x].locations.push_back(loca);
            }
        }
    }
    
    return servers;
}

std::vector<std::string> dirArray(const std::string &path, char c)
{
    std::vector<std::string> parts;
    std::istringstream iss(path);
    std::string word;

    while(std::getline(iss, word, c))
    {
        if(!word.empty())
            parts.push_back(word);
    }

    return parts;
}


size_t singleMatchingLocation(const ConfigLocations &path_conf, const std::string &path_client)
{
    std::vector<std::string> parts_conf;
    std::vector<std::string> parts_src;
    
    parts_conf = dirArray(path_conf.path, '/');
    parts_src = dirArray(path_client, '/');

    int size = 0;
    if(parts_conf.size() < parts_src.size())
        size = parts_conf.size();
    else
        size = parts_src.size();
            
    size_t depth = 0;

    for(int i = 0; i < size; i++)
    {
        if(parts_conf[i] == parts_src[i])
            depth++;
    }
    
    if(depth != parts_conf.size())
        return 0;
        
    return depth;
}


const ConfigLocations *ConfigServer::getMatchingLocation(const std::string &path)const
{
    const ConfigLocations *locaMatch;
    size_t best = 0;
    
    for(size_t i = 0; i < locations.size(); i++)
    {
        std::cout << "LOCA :" << locations[i].path << std::endl;
        size_t depth = singleMatchingLocation(locations[i], path);
        if(depth > best)
        {
            best = depth;
            locaMatch = &locations[i];
            std::cout << "FOUND BEST" << std::endl;
        }
    }
    
    if(best == 0)
        return NULL;
    return locaMatch;
}

int Config::checkServerExists(int port)
{
    for(size_t i = 0; i < _servers.size(); i++)
    {
        std::cout << "Server Post :" << _servers[i].port << std::endl;
        if(_servers[i].port == port)
            return i;
    }
    return -1;
}

ConfigServer Config::getServer(int port)
{
    return _servers[port];
}

// std::vector<std::string> dirArray(const std::string &path, char c)
// {
//     std::vector<std::string> parts;
//     std::istringstream iss(path);
//     std::string word;

//     while(std::getline(iss, word, c))
//     {
//         if(!word.empty())
//             parts.push_back(word);
//     }

//     return parts;
// }

// size_t Config::singleMatchingLocation(const ConfigLocations &path_conf, const std::string &path_client)
// {
//     std::vector<std::string> parts_conf;
//     std::vector<std::string> parts_src;
    
//     parts_conf = dirArray(path_conf.path, '/');
//     parts_src = dirArray(path_client, '/');

//     int size = 0;
//     if(parts_conf.size() < parts_src.size())
//         size = parts_conf.size();
//     else
//         size = parts_src.size();
            
//     size_t depth = 0;

//     for(int i = 0; i < size; i++)
//     {
//         if(parts_conf[i] == parts_src[i])
//             depth++;
//     }

//     // std::cout << depth << std::endl;
    
//     if(depth != parts_conf.size())
//         return 0;
        
//     return depth;
// }

// const ConfigLocations *Config::getMatchingLocation(const ConfigServer &server, const std::string &path)
// {
//     const ConfigLocations *locaMatch;
//     size_t best = 0;
    
//     for(size_t j = 0; j < server.locations.size(); j++)
//     {
//         size_t depth = singleMatchingLocation(server.locations[j], path);
//         if(depth > best)
//         {
//             best = depth;
//             locaMatch = &server.locations[j];
//         }
//     }
    
//     if(best == 0)
//         return NULL;
//     return locaMatch;
    
//     // for(size_t i = 0; i < _servers.size(); i++)
//     // {
//     //     for(size_t j = 0; j < _servers[i].locations.size(); j++)
//     //     {
//     //         size_t depth = singleMatchingLocation(_servers[i].locations[j], path);
//     //         if(depth > best)
//     //         {
//     //             best = depth;
//     //             locaMatch = &_servers[i].locations[j];
//     //         }
//     //     }
//     // }
// }
