#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "LocationConfig.hpp"

#include <string>
#include <vector>

class ServerConfig
{
    private:
        int _listenPort;
        std::string _root;
        std::string _index;
        std::vector<LocationConfig> _locations;

    public:
        ServerConfig();
        ~ServerConfig() {};

        void setListenPort(int port);
        int getListenPort() const;

        void setRoot(const std::string &root);
        void setIndex(const std::string &index);

        const std::string &getRoot() const;
        const std::string &getIndex() const;

        void addLocation(const LocationConfig &location);
        std::vector<LocationConfig> & getLocations();
};

#endif