#include "../include/HttpRequest.hpp"

HttpRequest::HttpRequest()
{
}
HttpRequest::~HttpRequest()
{
}
HttpRequest::HttpRequest(const HttpRequest &other)
{
	*this = other;
}
HttpRequest& HttpRequest::operator=(const HttpRequest &other)
{
	if (this != &other)
	{
		this->method = other.method;
		this->path = other.path;
		this->protocol = other.protocol;
		this->body = other.body;
		this->header = other.header;
	}
	return (*this);
}

bool isMethod(std::string method)
{
	 std::string normalized = method;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                   (int(*)(int))std::toupper);
	if (method.find("GET") != std::string::npos && method.size() == 3)
		return true;
	else if (method.find("POST") != std::string::npos && method.size() == 4)
		return true;
	else if (method.find("DELETE") != std::string::npos && method.size() == 6)
		return true;
	return false;
}


bool isHTTP(const std::string& http) {
    // Normalize to uppercase
    std::string normalized = http;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                   (int(*)(int))std::toupper); // cast needed in C++98

    // Pool of valid HTTP versions
    static const char* httpPool[] = {
        "HTTP/0.9", "HTTP/1.0", "HTTP/1.1", "HTTP/2", "HTTP/3"
    };

    // Exact match check
    for (int i = 0; i < 5; ++i) {
        if (normalized == httpPool[i]) {
            return true;
        }
    }
    return false;
}


//--------------------------#
//		getter				#
//--------------------------#
const std::map<std::string, std::string>& HttpRequest::getHeader() const{
    return header;
}
std::map<std::string,std::string>& HttpRequest::getHeader()
{
	return header;
}
const std::string& HttpRequest::getMethod() const
{
	return method;
}
const std::string& HttpRequest::getPath() const
{
	return path;
}
const std::string& HttpRequest::getProtocol() const
{
	return protocol;
}
const std::string& HttpRequest::getBody() const{
	return body;
}
//--------------------------#
//		setter				#
//--------------------------#
void HttpRequest::setMethod(const std::string& x) {
    method = x;
}

void HttpRequest::setPath(const std::string& path) {
    this->path = path;
}

void HttpRequest::setProtocol(const std::string& proto) {
    protocol = proto;
}

void HttpRequest::setBody(const std::string& body) {
    this->body = body;
}

void HttpRequest::setHeader(const std::string& key, const std::string& value) {
    header[key] = value; // overwrite if exists
}

void HttpRequest::setHeader(const std::map<std::string, std::string>& hdr) {
    header = hdr; // replace entire header map
}



std::ostream& operator<<(std::ostream& out,const HttpRequest& http) {    // Request line
    out << http.getMethod() << " " << http.getPath() << " " << http.getProtocol() << "\n";

    // Headers
	const std::map<std::string, std::string>& headers = http.getHeader();
    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        out << it->first << ": " << it->second << "\n";
    }

    // Blank line before body (HTTP convention)
    out << "\n";

    // Body
    out << http.getBody();

    return out;
}