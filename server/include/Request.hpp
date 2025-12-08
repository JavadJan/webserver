#ifndef Request_hpp
#define Request_hpp
#include <iostream>
#include <map>

class Request
{
private:
    std::string method;
    std::string target;
    std::string httpVersion;
    std::map<std::string, std::string> headers;
    std::string body;

public:
    // ---------------------------
    //       Setters
    // ---------------------------

    void setMethod(const std::string &m) { method = m; }
    void setTarget(const std::string &s) { target = s; }
    void setHttpVersion(const std::string &v) { httpVersion = v; }
    void addHeader(const std::string &k, const std::string &v)
    {
        headers[k] = v;
    }
    void appendBody(const std::string &b) { body += b; }

    // ---------------------------
    //       Getters
    // ---------------------------

    const std::string &getMethod() const { return method; }
    const std::string &getTarget() const { return target; }
    const std::string &getHttpVersion() const { return httpVersion; }
    const std::map<std::string, std::string> &getHeader() const
    {
        return headers;
    }
    const std::string &getBody() const { return body; }
};

    // -----------------------------------
    // Parse the start-line:
    // Example: "GET /index.html HTTP/1.1"
    // -----------------------------------

#endif