#include "../include/Server.hpp"
#include "../include/HttpRequest.hpp"
#include "../include/Config.hpp"

/*	------------------ INFO ABOUT FUNCION HAS BEEN USED -----------------------
	******** parser the request line+ => <METHOD> <PATH> <HTPP VERSION>
	static int error_to_code(const std::string err);
	void Server::consume(size_t start, size_t end, int sock_fd);
	static std::vector<std::string> splitPath(const std::string& normalized);
	std::string normalPath(std::string path);
	bool Server::validateRequestLine(int fd);
	void Server::parseRequestLine(std::string buf, int sock_fd);
*/
/*
	******** parser http header; after first line start the header until \r\n
	bool Server::validateHeaders(int fd);
	void Server::parseHeader(std::string buf, int sock_fd);
*/

/*
	******** decode the path with /%2e%2e/a/
	static int hexValue(char c);
	static std::string urlDecode(const std::string& input);
*/

/*	
	******** read whole stream from recv, then parse in state machine
	void Server::fsm(int sock_fd);
*/


static int error_to_code(const std::string err)
{
	if (err == "OK") return 200;
	else if (err == "Not Found") return 404;
	else if (err == "Forbidden") return 403;
	else if (err == "Method Not Allowed") return 405;
	else if (err == "Not Implemented") return 501;
	else if (err == "Bad Request") return 400;
	else return -1;
}

void Server::consume(size_t start, size_t end, int sock_fd)
{
    http_req[sock_fd].eraseBuffer(start, end);
}

static std::vector<std::string> splitPath(const std::string& normalized)
{
    std::vector<std::string> parts;
    std::string current;
    for (size_t i = 0; i < normalized.size(); ++i)
    {
        if (normalized[i] == '/')
        {
            if (!current.empty())
            {
                parts.push_back(current);
                current.clear();
            }
        }
        else
        {
            current += normalized[i];
        }
    }

    if (!current.empty())
        parts.push_back(current);

    return parts;
}

/* remove all ///// to consider just one */
/* /////  or /form////text.txt */
std::string normalPath(std::string path)
{
	std::string normalized;
	
	
	// step 1: remove redundance ////
	bool prev_slash = false;
	for (size_t i = 0; i < path.size(); ++i)
	{
		if (path[i] == '/')
		{
			if (!prev_slash)
			{
				normalized += '/';
				prev_slash = true;
			}
			// else skip additional slashes
		}
		else
		{
			normalized += path[i];
			prev_slash = false;
		}
	}
	// STEP 2: split and resolve . and ..
    std::vector<std::string> parts = splitPath(normalized);
    std::vector<std::string> stack;
    for (size_t i = 0; i < parts.size(); ++i)
    {
        if (parts[i].empty() || parts[i] == ".")
            continue;
		
        if (parts[i] == "..")
        {
            if (stack.empty())
                throw std::runtime_error("Forbidden");
            stack.pop_back();
        }
        else
        {
            stack.push_back(parts[i]);
        }
    }

    // STEP 3: rebuild
    std::string resolved = "/";
    for (size_t i = 0; i < stack.size(); ++i)
    {
        resolved += stack[i];
        if (i + 1 < stack.size())
            resolved += "/";
    }
    return resolved;
}
//--------------------------#
//			validation		#
//--------------------------#
bool Server::validateRequestLine(int fd)
{
	// methos os case-sensitive?
    if (http_req[fd].getMethod() != "GET" && http_req[fd].getMethod() != "POST" && http_req[fd].getMethod() != "DELETE")
	{
		http_req[fd].setStatusCode(405); // bad request
        return false;
	}

	// both version HTTP/1.0 and HTTP/1.1
    if (http_req[fd].getProtocol() != "HTTP/1.1" && http_req[fd].getProtocol() != "HTTP/1.0")
	{
		http_req[fd].setStatusCode(505);
        return false;
	}

    if (http_req[fd].getPath().empty() || http_req[fd].getPath()[0] != '/')
	{
		http_req[fd].setStatusCode(400);
        return false;
	}

    return true;
}

bool Server::validateHeaders(int fd)
{
    const HttpRequest& req = http_req[fd];
    const std::map<std::string, std::string>& headers = req.getHeader();
		
    // Host header (HTTP/1.1)
    if (req.getProtocol() == "HTTP/1.1")
	{
		bool host_found = false;
		for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
		{
			std::string key = it->first;
			// check case-insensitive
			if (key.size() == 4 &&
				(key[0]=='H'||key[0]=='h') &&
				(key[1]=='o'||key[1]=='O') &&
				(key[2]=='s'||key[2]=='S') &&
				(key[3]=='t'||key[3]=='T'))
			{
				host_found = true;
				break;
			}
		}
		if (!host_found)
		{
			http_req[fd].setStatusCode(400); // Bad Request
			return false;
		}
	}


    // Content-Length checks
    std::map<std::string, std::string>::const_iterator it =
		headers.find("Content-Length");
	
    //if (req.getMethod() == "POST")
    //{
    //    if (it == headers.end()) // if method == post then req should include content-len
    //    {
    //        http_req[fd].setStatusCode(411); // Length Required
    //        return false;
    //    }
    //}

    if (it != headers.end())
    {
        // must be numeric
        const std::string& contetnLen = it->second;
        if (contetnLen.empty() || contetnLen.find_first_not_of("0123456789") != std::string::npos)
        {
            http_req[fd].setStatusCode(400);
            return false;
        }
		// must be positive first trim

        long long body_size = atoll(contetnLen.c_str());
        if (body_size < 0)
        {
            http_req[fd].setStatusCode(400);
            return false;
        }

        // Max body size
        const Config* conf = req.getServerConfig();
        if (conf)
        {
            std::map<std::string, std::vector<std::string> >::const_iterator mit =
                conf->directives.find("max_body_size");

            if (mit != conf->directives.end())
            {
                long long max_size = atoll(mit->second[0].c_str());
                if (body_size > max_size)
                {
                    http_req[fd].setStatusCode(413); // Payload Too Large
                    return false;
                }
            }
        }
    }

    return true;
}

static int hexValue(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

static std::string urlDecode(const std::string& input)
{
    std::string output;

    for (size_t i = 0; i < input.size(); ++i)
    {
        if (input[i] == '%' && i + 2 < input.size())
        {
            int hi = hexValue(input[i + 1]);
            int lo = hexValue(input[i + 2]);

            if (hi == -1 || lo == -1)
                throw std::runtime_error("Bad Request");

            char decoded = static_cast<char>((hi << 4) | lo);
            output += decoded;
            i += 2;
        }
        else
        {
            output += input[i];
        }
    }

    return output;
}

//--------------------------#
//			validation		#
//--------------------------#



void Server::parseRequestLine(std::string buf, int sock_fd)
{

    std::istringstream rl(buf);
    std::string method, path, protocol;
    rl >> method >> path >> protocol;
	
    http_req[sock_fd].setMethod(method); // not valid 405
	
	if (path[0] != '/') // before to normilize check "/doc" or "doc" 
	{
		http_req[sock_fd].setState(HttpRequest::ERROR);
		http_req[sock_fd].setStatusCode(400);
		throw std::runtime_error("Bad Request"); ;
	}
	std::string decoded  = urlDecode(path);
	path = normalPath(decoded);
    http_req[sock_fd].setPath(path); // error 400

    http_req[sock_fd].setProtocol(protocol); // error 505
}

void Server::parseHeader(std::string buf, int sock_fd)
{
	std::istringstream header_stream(buf);
	std::string line;

	while (std::getline(header_stream, line)) 
	{
		if (line.empty())
			continue;

		// Remove trailing \r
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);

		//if (line.size() > MAX_HEADER_LINE) // it can throw an exeption as well
		//{
		//	http_req[sock_fd].setStatusCode(431);
		//	http_req[sock_fd].setState(HttpRequest::ERROR);
		//	std::cout << "REACH MAX HEADER LINE\n";
		//	return;
		//}

		size_t colon = line.find(':');
		if (colon == std::string::npos)
			continue;

		std::string key = line.substr(0, colon);
		std::string value = line.substr(colon + 1);
		
		// trim leading space
		while (!value.empty() && value[0] == ' ')
			value.erase(0, 1);
		
		//check duplicate
		std::map<std::string, std::string> &headers = http_req[sock_fd].getHeader();
		if (headers.count(key) && key != "Cookie")
		{
			http_req[sock_fd].setStatusCode(400); // Bad Request
			http_req[sock_fd].setState(HttpRequest::ERROR); 
            return ;
		}
		// HOST to lower case
		//key = to_lower(key);
		http_req[sock_fd].setHeader(key, value);
	}
}


void Server::fsm(int sock_fd)
{
	std::string len_body;
	std::string contetn;
	std::cout << "arived to fsm [STATE]: " << http_req[sock_fd].getState() << std::endl;
	while (true)
	{
		
		if (http_req[sock_fd].getState() == HttpRequest::DONE 
			|| http_req[sock_fd].getState() == HttpRequest::ERROR)
			return;
		switch (http_req[sock_fd].getState())
		{
		case HttpRequest::REQ_LINE:
		{
			std::cout << "arived to fsm [STATE]: " << http_req[sock_fd].getState() << std::endl;
			const std::string& buf = http_req[sock_fd].getBuffer();

			// end of request line
			size_t pos = buf.find("\r\n"); // nc end:\n curl end:\r\n
			if (pos == std::string::npos)
				pos = buf.find("\n");

			if (pos != std::string::npos)
			{
				// extract each portion of http request and set them
				std::string req_line = buf.substr(0, pos);
				std::cout << "request line: " << req_line << std::endl;
				try
				{
					parseRequestLine(req_line, sock_fd);
				}
				catch(const std::exception& e)
				{
					std::string s = e.what();
					std::cout << "🚨🚨🚨 ERROR in parse request line: " << "method: " << http_req[sock_fd].getMethod()
							<< ", path: " << http_req[sock_fd].getPath() << ", protocol: " 
							<< http_req[sock_fd].getProtocol() << std::endl;
					http_req[sock_fd].setStatusCode(error_to_code(s));
					http_req[sock_fd].setState(HttpRequest::ERROR);
					return;
				}
				

				size_t newline_len = (buf[pos] == '\r') ? 2 : 1;
				consume(0, pos + newline_len, sock_fd);

				// after fill validation the http request line
				if (!validateRequestLine(sock_fd))
				{
					// if find error stop the program to continue
					http_req[sock_fd].setState(HttpRequest::ERROR);
					std::cout << "🚨🚨🚨 ERROR in validation request line" << http_req[sock_fd].getMethod()
							<< " " << http_req[sock_fd].getPath() << " " 
							<< http_req[sock_fd].getProtocol() << std::endl;
					return ; 
				}
				http_req[sock_fd].setState(HttpRequest::HEADER);
				//http_req[sock_fd].setHeaderSize(http_req[sock_fd].getHeaderSize());
				std::cout << "[REQ_LINE STATE] has completed, Transit to HEADER\n\n";
				continue;
				//break;
			}
			else
				return;
		}
		case HttpRequest::HEADER:
		{	
			//std::cout << "✴️✴️✴️ HEADER STATE ✴️✴️✴️\n";
			const std::string& buf = http_req[sock_fd].getBuffer();	
			std::cout << http_req[sock_fd].getHeaderSize() << "✴️✴️✴️ HEADER STATE ✴️✴️✴️\n";
			if (http_req[sock_fd].getHeaderSize() > MAX_HEADER_SIZE) // grather than 16kb
			{
				http_req[sock_fd].setStatusCode(431);
				http_req[sock_fd].setState(HttpRequest::ERROR);
				std::cout << "🚨🚨🚨 ERROR in header size" << buf.size() << std::endl;
				return ;
			}		
			// end of headers
			size_t header_end = buf.find("\r\n\r\n");
			size_t delim = 4;
			
			if (header_end == std::string::npos) {
				header_end = buf.find("\n\n");
				delim = 2;
			}

			if (header_end != std::string::npos) {
				std::string header_block = buf.substr(0, header_end);
				parseHeader(header_block, sock_fd);

				std::map<std::string, std::string> &headers = http_req[sock_fd].getHeader();
				std::map<std::string, std::string>::const_iterator it = headers.find("Content-Length");
				if (it != headers.end() && http_req[sock_fd].getMethod() == "POST")
				{
					long len = atoi(headers["Content-Length"].c_str());	
					http_req[sock_fd].setContentLen(static_cast<size_t>(len));
					
					//clientState[sock_fd] = BODY;	
					http_req[sock_fd].setState(HttpRequest::BODY);
				}
				else if (it != headers.end() && (http_req[sock_fd].getMethod() != "POST") ) // in get() no body
				{
					http_req[sock_fd].setStatusCode(400);
					http_req[sock_fd].setState(HttpRequest::ERROR);
					std::cout << "🚨🚨🚨 ERROR: no Contetn-len" << 400 << std::endl;

				}
				else
					http_req[sock_fd].setState(HttpRequest::DONE);
				if(!validateHeaders(sock_fd))
				{
					http_req[sock_fd].setState(HttpRequest::ERROR);
					std::cout << "🚨🚨🚨 ERROR: validatoin header" << http_req[sock_fd].getStatusCode() << std::endl;
					return ;
				} 
					

				consume(0, header_end + delim, sock_fd);
				continue;
				//break;
			}
			// if arrive here it meand the header has completed	
			return;
		}
		case HttpRequest::BODY:
		{
			const std::string &buf = http_req[sock_fd].getBuffer();
			if (buf.size() < http_req[sock_fd].getContetnLen())
				return; // not complete

			std::string body = buf.substr(0, http_req[sock_fd].getContetnLen());
			http_req[sock_fd].setBody(body);

			consume(0,http_req[sock_fd].getContetnLen(), sock_fd);

			http_req[sock_fd].setState(HttpRequest::DONE);
			break;
		}
		case HttpRequest::DONE:
		{
			break ;
		}
		case HttpRequest::ERROR:
		{
			// transition goes to the sending, but send an error_page
			//http_req[sock_fd].setState(HttpRequest::SENDING);
			//
			std::cout << "🚨🚨🚨 ERROR: validatoin header" << http_req[sock_fd].getStatusCode() << std::endl;
			return;
		}
		default:
			break;
		}
	}
	
}
