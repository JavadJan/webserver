#include "../include/Server.hpp"
#include "../include/HttpRequest.hpp"
#include "../include/Config.hpp"

/* 
	read whole stream from recv,
	then pass to the fsm
*/

void Server::consume(size_t start, size_t end, int sock_fd)
{
    http_req[sock_fd].eraseBuffer(start, end);
}
/* remove all ///// to consider just one */
/* /////  or /form////text.txt */
std::string normalPath(std::string path)
{
	std::string normalized;
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
	return normalized;
}
//--------------------------#
//			validation		#
//--------------------------#
bool Server::validateRequestLine(int fd)
{
	// methos os case-sensitive?
    if (http_req[fd].getMethod() != "GET" && http_req[fd].getMethod() != "POST" && http_req[fd].getMethod() != "DELETE")
	{
		http_req[fd].setStatusCode(400); // bad request
        return false;
	}

    if (http_req[fd].getProtocol() != "HTTP/1.1")
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
        if (headers.find("Host") == headers.end())
        {
            http_req[fd].setStatusCode(400); // Bad Request
            return false;
        }
    }

    // Content-Length checks
    std::map<std::string, std::string>::const_iterator it =
        headers.find("Content-Length");

    if (req.getMethod() == "POST")
    {
        if (it == headers.end())
        {
            http_req[fd].setStatusCode(411); // Length Required
            return false;
        }
    }

    if (it != headers.end())
    {
        // must be numeric
        const std::string& value = it->second;
        if (value.empty() || value.find_first_not_of("0123456789") != std::string::npos)
        {
            http_req[fd].setStatusCode(400);
            return false;
        }

        long long body_size = atoll(value.c_str());
        if (body_size < 0)
        {
            http_req[fd].setStatusCode(400);
            return false;
        }

        // -----------------------
        // Max body size
        // -----------------------
        const Config* cfg = req.getServerConfig();
        if (cfg)
        {
            std::map<std::string, std::vector<std::string> >::const_iterator mit =
                cfg->directives.find("max_body_size");

            if (mit != cfg->directives.end())
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

//--------------------------#
//			validation		#
//--------------------------#



void Server::parseRequestLine(std::string buf, int sock_fd)
{

    std::istringstream rl(buf);
    std::string method, path, protocol;
    rl >> method >> path >> protocol;
	path = normalPath(path);

    http_req[sock_fd].setMethod(method); // not valid 405

    http_req[sock_fd].setPath(path); // error 400
	
    http_req[sock_fd].setProtocol(protocol); // error 505

	//std::string extra;
	//if (rl >> extra)
	//{
	//	http_req[sock_fd].setState(HttpRequest::ERROR);
	//	http_req[sock_fd].setStatusCode(400);
	//	return ;
	//}
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

		http_req[sock_fd].setHeader(key, value);
	}
}


int len_of_body(int pos, std::string recv)
{
	long long len = 0;

	// skip contetn len ->|:
	while (recv[pos] != ':')
		pos++;
	
	while (recv[pos] == ' ' || recv[pos] == ':') pos++;

	int start = pos;
	// until end; count letter in num
	while (recv[pos] != '\n')
	{
		len++;
		pos++;
	}
	
	len =  atoll(recv.substr(start, len).c_str());
	
	return len;
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
			const std::string& buf = http_req[sock_fd].getBuffer();

			size_t pos = buf.find("\r\n"); // nc end:\n curl end:\r\n
			if (pos == std::string::npos)
				pos = buf.find("\n");

			if (pos != std::string::npos)
			{
				// extract each portion of http request and set them
				std::string req_line = buf.substr(0, pos);
				parseRequestLine(req_line, sock_fd);

				size_t newline_len = (buf[pos] == '\r') ? 2 : 1;
				consume(0, pos + newline_len, sock_fd);

				// after fill validation the http request line
				if (!validateRequestLine(sock_fd))
				{
					// if find error stop the program to continue
					http_req[sock_fd].setState(HttpRequest::ERROR);
					std::cout << "transit to error state\n";
					return ; 
				}
				http_req[sock_fd].setState(HttpRequest::HEADER);
				std::cout << "[REQ_LINE STATE] has completed, parse the req_line then go header\n\n";
				continue;
				//break;
			}
			else
				return;
		}
		case HttpRequest::HEADER:
		{	
			const std::string& buf = http_req[sock_fd].getBuffer();

			size_t header_end = buf.find("\r\n\r\n");
			size_t delim = 4;

			if (header_end == std::string::npos) {
				header_end = buf.find("\n\n");
				delim = 2;
			}

			if (header_end != std::string::npos) {
				std::string header_block = buf.substr(0, header_end);
				parseHeader(header_block, sock_fd);

				if (http_req[sock_fd].getBuffer().find("Content-Length") != std::string::npos) // found body
				{
					int len = len_of_body(http_req[sock_fd].getBuffer().find("Content-Length"), http_req[sock_fd].getBuffer());					
					http_req[sock_fd].setContent(static_cast<size_t>(len));
					
					//clientState[sock_fd] = BODY;	
					http_req[sock_fd].setState(HttpRequest::BODY);
				}
				else
					http_req[sock_fd].setState(HttpRequest::DONE);
				validateHeaders(sock_fd);

				consume(0, header_end + delim, sock_fd);
				continue;
				//break;
			}
			// if arrive here it meand the header has completed	
			break;
		}
		case HttpRequest::BODY:
		{
			const std::string &buf = http_req[sock_fd].getBuffer();
			if (buf.size() < http_req[sock_fd].getContetn())
				return; // not complete

			std::string body = buf.substr(0, http_req[sock_fd].getContetn());
			http_req[sock_fd].setBody(body);

			consume(0,http_req[sock_fd].getContetn(), sock_fd);

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
			return;
		}
		default:
			break;
		}
	}
	
}
