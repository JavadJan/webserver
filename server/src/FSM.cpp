#include "../include/Server.hpp"
#include "../include/HttpRequest.hpp"
#include "../include/Config.hpp"

/*	------------------ CONTETN: -----------------------

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
	
	if (path.empty() || path[0] != '/') // before to normilize check "/doc" or "doc" 
	{
		http_req[sock_fd].setState(HttpRequest::ERROR);
		http_req[sock_fd].setStatusCode(400);
		throw std::runtime_error("Bad Request"); ;
	}
	std::string decoded  = urlDecode(path);
	path = normalPath(decoded);
	std::string raw_path = path;

	size_t pos = raw_path.find('?');

	if (pos != std::string::npos)
	{
		std::string p = raw_path.substr(0, pos);
		std::string q = raw_path.substr(pos + 1);

		http_req[sock_fd].setPath(p);
		http_req[sock_fd].setQuery(q);
	}
	else
	{
		http_req[sock_fd].setPath(raw_path);
		http_req[sock_fd].setQuery("");
	}
    //http_req[sock_fd].setPath(path); // error 400

    http_req[sock_fd].setProtocol(protocol); // error 505
}

static std::string to_lower(std::string key)
{
	std::string key_to_lower;
	for (size_t i = 0; i < key.size(); i++)
	{
		key_to_lower += std::tolower(key[i]);
	}
	return key_to_lower;
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
		key = to_lower(key);
<<<<<<< HEAD
		//std::cout << "CONTENT_KEY " << key << ": " << "VALUE: " << value << std::endl;
		if (key == "content-type")
		{
			http_req[sock_fd].setContentType(value);
		}
=======
		if (key == "content-type")
			http_req[sock_fd].setContentType(value);
>>>>>>> 43a25c45afca9c9962f9fd388bbdbc5365af5109
		
		http_req[sock_fd].setHeader(key, value);
	}
}
<<<<<<< HEAD
std::string Server::unchunkedBody(const std::string& buf)
{
    std::string out = "";
    std::string buffer = buf;

    std::cout << "WHOLE BODY: " << buffer << "|" << std::endl;

    while (!buffer.empty())
    {
        // 1. Find the end of the HEX size line
        size_t line_end = buffer.find("\r\n");
        if (line_end == std::string::npos) 
            break; // Incomplete data

        // 2. Parse hex size
        std::string hex = buffer.substr(0, line_end);
        char* endPtr;
        long chunk_size = strtol(hex.c_str(), &endPtr, 16);
        
        std::cout << "Detected hex: [" << hex << "] size: " << chunk_size << std::endl;

        // 3. Move buffer past the size line (\r\n)
        buffer.erase(0, line_end + 2);

        // 4. Check for terminal chunk
        if (chunk_size == 0)
        {
            // Optional: The protocol expects one more \r\n after the 0
            break; 
        }

        // 5. Safety: Check if buffer actually has enough data + trailing \r\n
        if (buffer.size() < (size_t)chunk_size)
            break; 

        // 6. Extract the actual content
        std::string chunk = buffer.substr(0, chunk_size);
        out.append(chunk);
        
        std::cout << "Extracted Chunk: [" << chunk << "]" << std::endl;

        // 7. Erase the chunk PLUS the \r\n that follows the chunk
        // RFC: chunk-data + CRLF
        buffer.erase(0, chunk_size + 2); 
    }

    return out;
}
=======
>>>>>>> 43a25c45afca9c9962f9fd388bbdbc5365af5109



void Server::fsm(int sock_fd)
{
    std::cout << "arived to fsm [STATE]: " << http_req[sock_fd].getState() << std::endl;

    while (true)
    {
        HttpRequest &req = http_req[sock_fd];

        if (req.getState() == HttpRequest::DONE ||
            req.getState() == HttpRequest::ERROR)
            return;

        switch (req.getState())
        {
        case HttpRequest::REQ_LINE:
        {
            const std::string &buf = req.getBuffer();

            size_t pos = buf.find("\r\n");
            if (pos == std::string::npos)
                pos = buf.find("\n");

            if (pos == std::string::npos)
                return; // need more data

            std::string req_line = buf.substr(0, pos);
            std::cout << "request line: " << req_line << std::endl;

            try
            {
                parseRequestLine(req_line, sock_fd);
            }
            catch (const std::exception &e)
            {
                std::string s = e.what();
                std::cout << "🚨🚨🚨 ERROR in parse request line: "
                          << "method: " << req.getMethod()
                          << ", path: " << req.getPath()
                          << ", protocol: " << req.getProtocol() << std::endl;

                int code = error_to_code(s);
                if (code < 0) code = 400;
                req.setStatusCode(code);
                req.setState(HttpRequest::ERROR);
                return;
            }

            size_t newline_len = (buf[pos] == '\r') ? 2 : 1;
            consume(0, pos + newline_len, sock_fd);

            if (!validateRequestLine(sock_fd))
            {
                req.setState(HttpRequest::ERROR);
                std::cout << "🚨🚨🚨 ERROR in validation request line "
                          << req.getMethod() << " "
                          << req.getPath() << " "
                          << req.getProtocol() << std::endl;
                return;
            }

            req.setState(HttpRequest::HEADER);
            std::cout << "[REQ_LINE STATE] has completed, Transit to HEADER\n\n";
            continue;
        }

        case HttpRequest::HEADER:
        {
            const std::string &buf = req.getBuffer();
            std::cout << req.getHeaderSize() << "✴️✴️✴️ HEADER STATE ✴️✴️✴️\n";

            if (buf.size() > MAX_HEADER_SIZE)
            {
                req.setStatusCode(431);
                req.setState(HttpRequest::ERROR);
                std::cout << "🚨🚨🚨 ERROR in header size " << buf.size() << std::endl;
                return;
            }

            size_t header_end = buf.find("\r\n\r\n");
            size_t delim = 4;

            if (header_end == std::string::npos)
            {
                header_end = buf.find("\n\n");
                delim = 2;
            }

            if (header_end == std::string::npos)
                return; // need more data

            std::string header_block = buf.substr(0, header_end);
            parseHeader(header_block, sock_fd);

            if (req.getState() == HttpRequest::ERROR)
                return;

            if (!validateHeaders(sock_fd))
            {
                req.setState(HttpRequest::ERROR);
                std::cout << "🚨🚨🚨 ERROR: validation header "
                          << req.getStatusCode() << std::endl;
                return;
            }

            // Decide next state based on method + Content-Length
            const std::map<std::string, std::string> &headers = req.getHeader();
<<<<<<< HEAD
            std::map<std::string, std::string>::const_iterator itEN =
                headers.find("transfer-encoding"); // after lower case
=======
>>>>>>> 43a25c45afca9c9962f9fd388bbdbc5365af5109
            std::map<std::string, std::string>::const_iterator it =
                headers.find("content-length"); // after lower case

            if (it != headers.end())
            {
                long len = atoll(it->second.c_str());
                req.setContentLen(static_cast<size_t>(len));

                if (req.getMethod() == "POST" || 
                    (req.getMethod() == "DELETE" && len > 0))
                {
                    req.setState(HttpRequest::BODY);
                }
                else
                {
                    // GET/HEAD with Content-Length > 0 already rejected in validateHeaders()
                    req.setState(HttpRequest::DONE);
                }
            }
<<<<<<< HEAD
            else if (itEN != headers.end())
            {
                // Transfer-Encoding: chunked - for now, set DONE
                // TODO: implement chunked transfer encoding support
				body_chunked = true;
                req.setState(HttpRequest::BODY);
            }
            else
            {
                // No Content-Length → no body
				std::cout << "nobody, but might chunked\n";
=======
            else
            {
                // No Content-Length → no body
>>>>>>> 43a25c45afca9c9962f9fd388bbdbc5365af5109
                req.setState(HttpRequest::DONE);
            }

            consume(0, header_end + delim, sock_fd);
            continue;
        }

        case HttpRequest::BODY:
        {
<<<<<<< HEAD
			std::cout << "[LOG IN STATE BODY] getBuffer: " << req.getBuffer().size() << "\n";
            const std::string &buf = req.getBuffer();
			std::string body;
			size_t need = req.getContetnLen();
			if (!body_chunked)
			{
				if (need == 0) // added later
				{
					req.setState(HttpRequest::DONE);
					return;
				}			
				if (buf.size() < need)
					return; // wait for more				
				body = buf.substr(0, need);
				
				if (!validateBody(sock_fd, body))
				{
					std::cout << "ERROR IN BODY VALIDATION BODY \n"; 
					req.setState(HttpRequest::ERROR);
					req.shouldClose = true;
					return;
				}
				std::cout << "[LOG IN BODY STATE]  no error after validation? : " << body << "\n";
	
				req.setBody(body);
				consume(0, need, sock_fd);
			}
            else
            {
                size_t body_end = buf.find("0\r\n\r\n");
				size_t terminator_len = 5; // "0" + "\r\n" + "\r\n"

				// Fallback for tools like nc that might only send \n
				if (body_end == std::string::npos) {
					body_end = buf.find("0\n\n");
					terminator_len = 3;
				}

				if (body_end == std::string::npos)
					return; // Data is incomplete, wait for next poll

				// 2. Decode the FULL chunked block (from start to the end of terminator)
				std::string raw_chunked_area = buf.substr(0, body_end + terminator_len);
				std::string decoded = unchunkedBody(raw_chunked_area);

				// 3. Validate the final resulting body size
				if (!validateBody(sock_fd, decoded)) {
					req.setState(HttpRequest::ERROR);
					return;
				}
				
				req.setBody(decoded);
				
				// 4. Consume the raw data from the buffer so it doesn't leak into the next request
				consume(0, body_end + terminator_len, sock_fd);
            }

=======
            const std::string &buf = req.getBuffer();
            size_t need = req.getContetnLen();
			if (need == 0) // added later
			{
				req.setState(HttpRequest::DONE);
				return;
			}

            if (buf.size() < need)
                return; // wait for more

            std::string body = buf.substr(0, need);

            if (!validateBody(sock_fd, body))
            {
                req.setState(HttpRequest::ERROR);
                req.shouldClose = true;
                return;
            }

            req.setBody(body);
            consume(0, need, sock_fd);

>>>>>>> 43a25c45afca9c9962f9fd388bbdbc5365af5109
            req.setState(HttpRequest::DONE);
            return;
        }

        case HttpRequest::DONE:
        {
            return;
        }

        case HttpRequest::ERROR:
        {
            req.shouldClose = true;
            std::cout << "🚨🚨🚨 ERROR: final state, status "
                      << req.getStatusCode() << std::endl;
            return;
        }

        default:
            return;
        }
    }
}


/*------------------------------------------------------------------------------------------*/
/*																							*/
/*									validation												*/
/*																							*/
/*------------------------------------------------------------------------------------------*/
bool Server::validateRequestLine(int fd)
{
	//1.  methos os case-sensitive?
	HttpRequest &req = http_req[fd];
    if (req.getMethod() != "GET" && req.getMethod() != "POST" && req.getMethod() != "DELETE")
	{
		req.setStatusCode(405); // bad request
        return false;
	}

	//2.  both version HTTP/1.0 and HTTP/1.1
    if (req.getProtocol() != "HTTP/1.1" && req.getProtocol() != "HTTP/1.0")
	{
		req.setStatusCode(505);
        return false;
	}

	// 3. Path must start with /
	const std::string &path = req.getPath();
    if (path.empty() || path[0] != '/')
	{
		req.setStatusCode(400);
        return false;
	}

	// 4. Reject absolute urls
	if (path.find("://") != std::string::npos)
	{
		req.setStatusCode(400);
		return false;
	}

	// 5. Reject space or controll chars
	for (size_t i = 0; i < path.size(); i++)
	{
		if (path[i] <= 31 || path[i] == ' ')
		{
			req.setStatusCode(400);
			return false;
		}
	}
	
	// 6. Reject too-long path
	if (path.size() > 8192)
	{
		req.setStatusCode(414);
		return false;
	}
    return true;
}

bool Server::validateHeaders(int fd)
{
    HttpRequest &req = http_req[fd];
    const std::map<std::string, std::string> &headers = req.getHeader();

    // 1. Host header required for HTTP/1.1
    if (req.getProtocol() == "HTTP/1.1")
    {
        if (headers.count("host") == 0)
        {
            req.setStatusCode(400);
            return false;
        }
    }

    // 2. Reject Transfer-Encoding (we don't support chunked)
<<<<<<< HEAD
	// now here I should iimplement this
    //if (headers.count("transfer-encoding"))
    //{
    //    req.setStatusCode(501); // Not implemented
    //    return false;
    //}
=======
    if (headers.count("transfer-encoding"))
    {
        req.setStatusCode(501); // Not implemented
        return false;
    }
>>>>>>> 43a25c45afca9c9962f9fd388bbdbc5365af5109

    // 3. Content-Length validation
    std::map<std::string, std::string>::const_iterator it =
        headers.find("content-length");
<<<<<<< HEAD
    
    // POST must have Content-Length
    if (req.getMethod() == "POST")
    {
        if (it == headers.end() && !headers.count("transfer-encoding"))
=======
	
    // POST must have Content-Length
    if (req.getMethod() == "POST")
    {
        if (it == headers.end())
>>>>>>> 43a25c45afca9c9962f9fd388bbdbc5365af5109
        {
            req.setStatusCode(411); // Length Required
            return false;
        }
    }

    // GET/HEAD must not have body
    if ((req.getMethod() == "GET" || req.getMethod() == "HEAD") &&
        it != headers.end() &&
        atoll(it->second.c_str()) > 0)
    {
        req.setStatusCode(400);
        return false;
    }

    // DELETE: body allowed only if allow_upload on
    if (req.getMethod() == "DELETE" &&
        it != headers.end() &&
        atoll(it->second.c_str()) > 0)
    {
        const Config *conf = req.getServerConfig();
        bool allow_upload = false;

        if (conf && conf->directives.count("allow_upload"))
            allow_upload = true;

        if (!allow_upload)
        {
            req.setStatusCode(400);
            return false;
        }
    }

    // 4. Validate Content-Length numeric
    if (it != headers.end())
    {
        const std::string &cl = it->second;

        // numeric only
        if (cl.empty() || cl.find_first_not_of("0123456789") != std::string::npos)
        {
            req.setStatusCode(400);
            return false;
        }

        long long body_size = atoll(cl.c_str());
        if (body_size < 0)
        {
            req.setStatusCode(400);
            return false;
        }

        // 5. Check max_body_size
        const Config *conf = req.getServerConfig();
        if (conf)
        {
            std::map<std::string, std::vector<std::string> >::const_iterator mit =
                conf->directives.find("max_body_size");

            if (mit != conf->directives.end())
            {
                long long max_size = atoll(mit->second[0].c_str());
                if (body_size > max_size)
                {
                    req.setStatusCode(413);
                    return false;
                }
            }
        }
    }

    return true;
}

bool Server::validateBody(int fd, const std::string &body)
{
    HttpRequest &req = http_req[fd];
    size_t len = req.getContetnLen();

<<<<<<< HEAD
    // 1. Length mismatch (only validate if not using chunked encoding)
    if (!body_chunked && body.size() != len)
=======
    // 1. Length mismatch
    if (body.size() != len)
>>>>>>> 43a25c45afca9c9962f9fd388bbdbc5365af5109
        return false;

    // 2. Too large (config)
    const Config* config = req.getServerConfig();
    size_t max_body_size = 0;

    if (config)
	{
        std::map<std::string, std::vector<std::string> >::const_iterator it =
            config->directives.find("max_body_size");

        if (it != config->directives.end() && !it->second.empty())
            max_body_size = static_cast<size_t>(atoll(it->second[0].c_str()));
    }

    if (max_body_size > 0 && len > max_body_size)
    {
        req.setStatusCode(413);
        return false;
    }

    // 3. GET/HEAD must not have a body
    if ((req.getMethod() == "GET") && len > 0)
    {
        req.setStatusCode(400);
        return false;
    }

    // 4. DELETE must not have a body unless allow_upload is on
    if (req.getMethod() == "DELETE" && len > 0)
    {
        const Config* conf = req.getServerConfig();
        bool allow_upload = false;

        if (conf) {
            std::map<std::string, std::vector<std::string> >::const_iterator it =
                conf->directives.find("allow_upload");

            if (it != conf->directives.end())
                allow_upload = true;
        }

        if (!allow_upload) {
            req.setStatusCode(400);
            return false;
        }
    }

    // 5. POST must have Content-Type
    if (req.getMethod() == "POST")
    {
        if (req.getHeader().count("content-type") == 0)
        {
            req.setStatusCode(400);
            return false;
        }
    }

    return true;
}



//void Server::fsm(int sock_fd)
//{
//	std::string len_body;
//	std::string contetn;
//	while (true)
//	{
//		HttpRequest &req = http_req[sock_fd];
//		if (req.getState() == HttpRequest::DONE || req.getState() == HttpRequest::ERROR)
//			return;
//		switch (req.getState())
//		{
//		case HttpRequest::REQ_LINE:
//		{
//			std::cout << "arived to fsm [STATE]: " << http_req[sock_fd].getState() << std::endl;
//			const std::string& buf = http_req[sock_fd].getBuffer();

//			// end of request line
//			size_t pos = buf.find("\r\n"); // nc end:\n curl end:\r\n
//			if (pos == std::string::npos)
//				pos = buf.find("\n");

//			//if (pos == std::string::npos)
//			//	return; // need more data

//			if (pos != std::string::npos)
//			{
//				// extract each portion of http request and set them
//				std::string req_line = buf.substr(0, pos);
//				std::cout << "request line: " << req_line << std::endl;
//				try
//				{
//					parseRequestLine(req_line, sock_fd);
//				}
//				catch(const std::exception& e)
//				{
//					std::string s = e.what();
//					std::cout << "🚨🚨🚨 ERROR in parse request line: " << "method: " << http_req[sock_fd].getMethod()
//							<< ", path: " << http_req[sock_fd].getPath() << ", protocol: " 
//							<< http_req[sock_fd].getProtocol() << std::endl;
//					http_req[sock_fd].setStatusCode(error_to_code(s));
//					http_req[sock_fd].setState(HttpRequest::ERROR);
//					return;
//				}
				

//				size_t newline_len = (buf[pos] == '\r') ? 2 : 1;
//				consume(0, pos + newline_len, sock_fd);

//				// after fill validation the http request line
//				if (!validateRequestLine(sock_fd))
//				{
//					// if find error stop the program to continue
//					http_req[sock_fd].setState(HttpRequest::ERROR);
//					std::cout << "🚨🚨🚨 ERROR in validation request line" << http_req[sock_fd].getMethod()
//							<< " " << http_req[sock_fd].getPath() << " " 
//							<< http_req[sock_fd].getProtocol() << std::endl;
//					return ; 
//				}
//				http_req[sock_fd].setState(HttpRequest::HEADER);
//				//http_req[sock_fd].setHeaderSize(http_req[sock_fd].getHeaderSize());
//				std::cout << "[REQ_LINE STATE] has completed, Transit to HEADER\n\n";
//				continue;
//				//break;
//			}
//			else
//				return;
//		}
//		case HttpRequest::HEADER:
//		{	
//			//std::cout << "✴️✴️✴️ HEADER STATE ✴️✴️✴️\n";
//			const std::string& buf = http_req[sock_fd].getBuffer();	
//			std::cout << http_req[sock_fd].getHeaderSize() << "✴️✴️✴️ HEADER STATE ✴️✴️✴️\n";

//			// if header size was too large > 16kb
//			if (buf.size() > MAX_HEADER_SIZE)
//			{
//				http_req[sock_fd].setStatusCode(431);
//				http_req[sock_fd].setState(HttpRequest::ERROR);
				
//				std::cout << "🚨🚨🚨 ERROR in header size" << buf.size() << std::endl;
//				return ;
//			}		
//			// end of headers
//			size_t header_end = buf.find("\r\n\r\n");
//			size_t delim = 4;
			
//			if (header_end == std::string::npos) {
//				header_end = buf.find("\n\n");
//				delim = 2;
//			}

//			if (header_end != std::string::npos) {
//				std::string header_block = buf.substr(0, header_end);
//				parseHeader(header_block, sock_fd);

//				std::map<std::string, std::string> &headers = http_req[sock_fd].getHeader();
//				std::map<std::string, std::string>::const_iterator it = headers.find("Content-Length");
//				if (it != headers.end() && http_req[sock_fd].getMethod() == "POST")
//				{
//					long len = atoi(headers["Content-Length"].c_str());	
//					http_req[sock_fd].setContentLen(static_cast<size_t>(len));
					
//					//clientState[sock_fd] = BODY;	
//					http_req[sock_fd].setState(HttpRequest::BODY);
//				}
//				else if (it != headers.end() && (http_req[sock_fd].getMethod() != "POST") ) // in get() no body
//				{
//					http_req[sock_fd].setStatusCode(400);
//					http_req[sock_fd].setState(HttpRequest::ERROR);
//					std::cout << "🚨🚨🚨 ERROR: no Contetn-len" << 400 << std::endl;

//				}
//				else
//					http_req[sock_fd].setState(HttpRequest::DONE);
//				if(!validateHeaders(sock_fd))
//				{
//					http_req[sock_fd].setState(HttpRequest::ERROR);
//					std::cout << "🚨🚨🚨 ERROR: validatoin header" << http_req[sock_fd].getStatusCode() << std::endl;
//					return ;
//				} 
					

//				consume(0, header_end + delim, sock_fd);
//				continue;
//				//break;
//			}
//			// if arrive here it meand the header has completed	
//			return;
//		}
//		case HttpRequest::BODY:
//		{
//			HttpRequest &req = http_req[sock_fd];
//			const std::string &buf = req.getBuffer();
//			size_t need = req.getContetnLen();

//			// Not enough data yet
//			if (buf.size() < need)
//				return;

//			// Extract body
//			std::string body = buf.substr(0, need);

//			// Validate body BEFORE consuming
//			if (!validateBody(sock_fd, body))
//			{
//				req.setState(HttpRequest::ERROR);
//				req.shouldClose = true;
//				return;
//			}

//			// Store body
//			req.setBody(body);

//			// Remove consumed bytes
//			consume(0, need, sock_fd);

//			req.setState(HttpRequest::DONE);
//			return;
//		}

//		case HttpRequest::DONE:
//		{
//			break ;
//		}
//		case HttpRequest::ERROR:
//		{
//			// transition goes to the sending, but send an error_page
//			//http_req[sock_fd].setState(HttpRequest::SENDING);
//			//
//			http_req[sock_fd].shouldClose = true;
//			std::cout << "🚨🚨🚨 ERROR: validatoin header" << http_req[sock_fd].getStatusCode() << std::endl;
//			return;
//		}
//		default:
//			break;
//		}
//	}
	
//}