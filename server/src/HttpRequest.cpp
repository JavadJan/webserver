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

HttpRequest ParseFSM(std::string bufr)
{
	HttpRequest req; // every time is constructed new

	std::string method;
	std::string path;
	std::string protocol;

	std::string header;
	std::string body;

	//STATE state = REQ_LINE;
	int i = 0;
	std::cout << "chunk: " << i++ << ": " << bufr << std::endl;
	
	//while (state != DONE && state != ERROR) // DONE == true break the loop, if ERROR break loop
	//{
	//	//bufr.append(buffer);
	//	switch (state)
	//	{
	//	case REQ_LINE:
	//	{
	//		// fill method, version, path
			size_t pos = bufr.find("\r\n"); // scan first line
			//if (pos == std::string::npos) break; // wait for more data
			
			// now first line has recived completly, start to prase
			std::string line = bufr.substr(0, pos);
			std::istringstream ss(line);
			ss >> method >> path >> protocol;


	//		//split the first line
	//		ss >> method >> path >> protocol; // split in three part with space delimiter

	//		std::cout << "parsing the req_line: \n"
	//				<<"method: "<< method << ", path: " 
	//				<< path << ", protocol: " 
	//				<< protocol << std::endl;

	//		if (!isMethod(method) || !isHTTP(path)) // check bad form: GE   T
	//			state = ERROR;
	//		else {
	//			req.setMethod(method);
	//			req.setPath(path);
	//			req.setProtocol(protocol);
	//			bufr.erase(0, pos + 2); // erase \r\n
	//			state = HEADER; // transition
	//		}
	//		break ;
	//	}
	//	case HEADER:
	//	{
	//		std::cout << "parsing the header\n";

	//		size_t pos = bufr.find("\r\n\r\n"); // before body start
	//		if (pos == std::string::npos) break; // stay at the same state

	//		std::string line = bufr.substr(0, pos); // 0 - 10
	//		std::istringstream ss(line);

	//		while (std::getline(ss, line)) {
    //            if (line.empty() || line == "\r") continue;
    //            size_t colon = line.find(':');
    //            if (colon != std::string::npos) {
    //                std::string key = line.substr(0, colon);
    //                std::string value = line.substr(colon + 1);
    //                // trim spaces if needed
    //                req.getHeader()[key] = value;
    //            }
    //        }
	//		bufr.erase(0, pos + 4);
    //        state = BODY;
    //        break;
	//	}
	//	 case BODY: {
    //        req.setBody(bufr);
    //        state = DONE;
    //        break;
    //    }
    //    default:
    //        state = ERROR;
    //        break;
    //    }
	//}
	req.setMethod(method);
	req.setPath(path);
	req.setProtocol(protocol);
	std::cout << req;
	return req;
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