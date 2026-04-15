/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkhavari <mkhavari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 17:53:56 by asemykin          #+#    #+#             */
/*   Updated: 2026/04/15 21:34:29 by mkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/HttpRequest.hpp"
#include "../include/Config.hpp"

HttpRequest::HttpRequest()
:sendBuffer() , sendOffset(0),
shouldClose(false) , header_done(false),
state(REQ_LINE),
recvBuffer(),
method() , path(),
query(),
protocol(),
sock_fd_cleint(-1),
header(),
conten_len(0),
body(),
server(NULL),
connection_close(false), 
statusCode(0),
header_size(0)
{
    if(BUG)
	{std::cout << "default constructor called" << state << std::endl;}
}
HttpRequest::~HttpRequest()
{
}
HttpRequest::HttpRequest(const HttpRequest &other)
{
	*this = other;
}
//HttpRequest& HttpRequest::operator=(const HttpRequest &other)
//{
//	if (this != &other)
//	{
//		this->method = other.method;
//		this->path = other.path;
//		this->protocol = other.protocol;
		
//		this->header = other.header; // 
		
//		this->body = other.body;
//		this->state = other.state;

//		this->conten_len = other.conten_len;

//		this->recvBuffer = other.recvBuffer;

//		this->header_size = other.header_size;
//		this->header_done = other.header_done;

//		this->sendBuffer = other.sendBuffer;
//		this->sendOffset = other.sendOffset;
//	}
//	return (*this);
//}

HttpRequest& HttpRequest::operator=(const HttpRequest &other)
{
    if (this != &other)
    {
        this->sendBuffer       = other.sendBuffer;
        this->sendOffset       = other.sendOffset;

        this->state            = other.state;
        this->recvBuffer       = other.recvBuffer;

        this->method           = other.method;
        this->path             = other.path;
        this->protocol         = other.protocol;

        this->sock_fd_cleint   = other.sock_fd_cleint;
        this->header           = other.header;
        this->conten_len       = other.conten_len;
        this->body             = other.body;

        this->server           = other.server;
        this->connection_close = other.connection_close;
        this->statusCode       = other.statusCode;
        this->header_size      = other.header_size;
        this->shouldClose      = other.shouldClose;
        this->header_done      = other.header_done;
        this->contentType      = other.contentType;
		this->query			   = other.query;
    }
    return *this;
}


//void HttpRequest::resetForNextRequest()
//{
//    path.clear();
//    header.clear();
//    method.clear();
//    recvBuffer.clear();
//    protocol.clear();

//    connection_close = false; // reset default
//    state = REQ_LINE;
//	body.clear();
//	conten_len = 0;

//	shouldClose = false;
//	header_size = 0;
//	sendBuffer.clear();
//	sendOffset =0;
//	statusCode = 0;
//}
void HttpRequest::resetForNextRequest()
{
    path.clear();
    header.clear();
    method.clear();
    recvBuffer.clear();
    protocol.clear();

    connection_close = false;
    state = REQ_LINE;
    body.clear();
    conten_len = 0;

    shouldClose = false;
    header_size = 0;
    sendBuffer.clear();
    sendOffset = 0;
    statusCode = 0;
    header_done = false;   // IMPORTANT
}


void HttpRequest::eraseBuffer(size_t start, size_t end)
{
	this->recvBuffer.erase(start, end);
}

//------------------------------------------------------#
//														#
//						getter							#
//														#
//------------------------------------------------------#
const std::string& HttpRequest::getContentType() const
{
	return this->contentType;
}
const std::string& HttpRequest::getQuery() const
{
	return this->query;
}
int HttpRequest::getStatusCode() const
{
	return this->statusCode;
}
bool HttpRequest::getConnectionState()
{
	return connection_close;
}
//std::string HttpRequest::getPortServer() const
//{
//	return portServer;
//}
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

size_t HttpRequest::getContetnLen() const{
	return conten_len;
}

HttpRequest::STATE HttpRequest::getState() const
{
	return state;
}

const Config* HttpRequest::getServerConfig() const{
	return server;
}

void HttpRequest::clearBuffer(){recvBuffer.clear();}
//------------------------------------------------------#
//														#
//						setter							#
//														#
//------------------------------------------------------#
void HttpRequest::setQuery(const std::string &_query)
{
	this->query = _query;
}
void HttpRequest::setContentType(std::string content)
{
	this->contentType = content;
}
void HttpRequest::setServerConfig(Config *server)
{
	this->server = server;
}
void HttpRequest::setCloseConnection(bool cstate)
{
	this->connection_close = cstate;
}

void HttpRequest::setHeaderSize(size_t h_size)
{
	this->header_size = h_size;
}
size_t HttpRequest::getHeaderSize()
{
	return this->header_size;
}

void HttpRequest::setStatusCode(int status_code){this->statusCode = status_code;}
void HttpRequest::setState(enum STATE state){ this->state = state;}
void HttpRequest::setClientSocket(int sock_fd){ this->sock_fd_cleint = sock_fd;}
//void HttpRequest::setPortServer(std::string port){ this->portServer = port;}
//void HttpRequest::appendBuffer(std::string chunk, int bytes_read)
//{
//    if (bytes_read > static_cast<int>(chunk.size()))
//        bytes_read = chunk.size();
//    recvBuffer.append(chunk, 0, bytes_read);

//	if (!header_done)
//    {
//		header_size += bytes_read;
//		if (header_size > 16384)
//        {
//            state = ERROR;
//            this->statusCode = 431;
//			shouldClose = true;
//            return;
//        }
//        size_t pos = recvBuffer.find("\r\n\r\n");
//        if (pos != std::string::npos)
//        {
//            header_size = (pos + 4); // recvBuffer already has whole header
//			std::cout << "HEADER SIZE: " << header_size << std::endl;
//            header_done = true;
//        }
//    }
//}
void HttpRequest::appendBuffer(const char* data, int bytes_read)
{
    recvBuffer.append(data, bytes_read);

    if (!header_done)
    {
        header_size += bytes_read;

        if (header_size > 16384)
        {
            state = ERROR;
            statusCode = 431;
            shouldClose = true;
            return;
        }

        size_t pos = recvBuffer.find("\r\n\r\n");
        if (pos != std::string::npos)
        {
            header_size = pos + 4;
            header_done = true;
            if(BUG)
            {std::cout << "HEADER SIZE: " << header_size << std::endl;}
        }
    }
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

void HttpRequest::setContentLen(size_t len) {
    this->conten_len = len; // replace entire header map
}




/* ----------------------------- */
/* 			operator			 */
/* ----------------------------- */
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

