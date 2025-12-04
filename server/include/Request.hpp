#ifndef Request_hpp
# define Request_hpp
#include <iostream>
#include <map>

class Request {
private:
    std::string method;
    std::string target;
    std::string httpVersion;
    std::map<std::string, std::string> headers;
    std::string body;

public:
    // getters, setters, and clear() function
};

#endif