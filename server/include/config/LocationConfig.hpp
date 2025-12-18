#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP

#include<string>
#include<vector>

class LocationConfig{
    private:
        std::string _path;
        std::string _root;
        std::vector<std::string> _index;
        bool _autoindex;
        std::vector<std::string> _allowedMethod;

    public:
        LocationConfig();
        ~LocationConfig();

        void setPath(const std::string& path);
        void setRoot(const std::string& root);
        void setAutoindex(bool value);

        const std::string& getPath() const;
        const std::string& getRoot() const;

        void addIndex(const std::string& index);
        void addAllowedMethod(const std::string& method);
        
        
        
};

#endif