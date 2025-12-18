#include "../include/HttpRequest.hpp"

HttpRequest::HttpRequest(): state(REQ_LINE), conten_len(0)
{
	std::cout << "default constructor called" << state << std::endl;
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
		
		this->header = other.header; // 
		
		this->body = other.body;
		this->state = other.state;

		this->conten_len = other.conten_len;

		this->recvBuffer = other.recvBuffer;
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

void HttpRequest::eraseBuffer(size_t start, size_t end)
{
	this->recvBuffer.erase(start, end);
}

//--------------------------#
//		getter				#
//--------------------------#

int HttpRequest::getPortServer() const
{
	return portServer;
}
int HttpRequest::getCleintSocket() const
{
	return sock_fd_cleint;
}
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

const std::string& HttpRequest::getBuffer() const{
	return recvBuffer;
}

size_t HttpRequest::getContetn() const{
	return conten_len;
}

HttpRequest::STATE HttpRequest::getState() const
{
	return state;
}

void HttpRequest::clearBuffer(){recvBuffer.clear();}
//--------------------------#
//		setter				#
//--------------------------#
void HttpRequest::setState(enum STATE state){ this->state = state;}
void HttpRequest::setClientSocket(int sock_fd){ this->sock_fd_cleint = sock_fd;}
void HttpRequest::setPortServer(int port){ this->portServer = port;}
void HttpRequest::appendBuffer(std::string chunk, int bytes_read)
{
    if (bytes_read > static_cast<int>(chunk.size()))
        bytes_read = chunk.size();
    recvBuffer.append(chunk, 0, bytes_read);
    //std::cout << "in append: " << recvBuffer  << "-->|" << std::endl;
}

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

void HttpRequest::setContent(size_t len) {
    this->conten_len = len; // replace entire header map
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