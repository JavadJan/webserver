#include "config/LocationConfig.hpp"


LocationConfig::LocationConfig(){}
LocationConfig::~LocationConfig(){}

void LocationConfig::setPath(const std::string& path){
    _path = path;
}

void LocationConfig :: setRoot(const std::string& root){
    _root = root;
}

void LocationConfig::setAutoindex(bool value){
    _autoindex = value;
}

void LocationConfig :: addIndex(const std::string& index){
    _index.push_back(index);
}

const std::string& LocationConfig :: getPath() const{
    return _path;
}

const std::string& LocationConfig :: getRoot() const {
    return _root;
}

void LocationConfig :: addAllowedMethod(const std::string& method){
    _allowedMethod.push_back(method);
}
