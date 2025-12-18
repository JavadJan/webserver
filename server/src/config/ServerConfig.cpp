#include "config/ServerConfig.hpp"
#include "config/LocationConfig.hpp"

ServerConfig :: ServerConfig(): _listenPort(-1){}

void ServerConfig:: setListenPort(int port){
    _listenPort = port;
}

int ServerConfig :: getListenPort() const{
    return _listenPort;
}

void  ServerConfig::addLocation(const LocationConfig& location){
    _locations.push_back(location);
}
std::vector<LocationConfig>& ServerConfig :: getLocations(){
    return _locations;
}



void ServerConfig::setRoot(const std::string& root) {
    _root = root;
}

void ServerConfig::setIndex(const std::string& index) {
    _index = index;
}

const std::string& ServerConfig::getRoot() const {
    return _root;
}

const std::string& ServerConfig::getIndex() const {
    return _index;
}