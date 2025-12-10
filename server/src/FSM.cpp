#include "../include/Server.hpp"
#include "../include/HttpRequest.hpp"

/* 
	read whole stream from recv,
	then pass to the fsm
*/

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
		/* code */
		if (http_req[sock_fd].getState() == HttpRequest::DONE 
			|| http_req[sock_fd].getState() == HttpRequest::ERROR)
			return;
		switch (http_req[sock_fd].getState())
		{
		case HttpRequest::REQ_LINE:
		{
			if (http_req[sock_fd].getBuffer().find("\r\n") == std::string::npos) // not complted wait more
				break ;
			http_req[sock_fd].setState(HttpRequest::HEADER);
			std::cout << "[REQ_LINE STATE] has completed\n\n";
			continue;
			break;
		}
		case HttpRequest::HEADER:
		{
			if (http_req[sock_fd].getBuffer().find("\r\n\r\n") == std::string::npos) // not complted wait more
				break;
			// if arrive here it meand the header has completed	
			if (http_req[sock_fd].getBuffer().find("Content-Length") != std::string::npos) // found body
			{
				int len = len_of_body(http_req[sock_fd].getBuffer().find("Content-Length"), http_req[sock_fd].getBuffer());
				std::cout << "content len: " << len << std::endl;
				http_req[sock_fd].setContent(static_cast<size_t>(len));
				std::cout << "[HEADER STATE] has completed\n\n";
				http_req[sock_fd].setState(HttpRequest::BODY);
				//clientState[sock_fd] = BODY;	
				break ;	
			}
			else
			{
				http_req[sock_fd].setState(HttpRequest::DONE);
				//clientState[sock_fd] = DONE;
			}
			break;
		}
		case HttpRequest::BODY:
		{
			if (http_req[sock_fd].getBuffer().length() - start_body < http_req[sock_fd].getContetn())
			{
				// body incomplete
				break;
			}
			else
			{
				http_req[sock_fd].setState(HttpRequest::DONE);
				std::cout << "[BODY STATE] has completed\n\n";
			}
	
		}
		case HttpRequest::DONE:
		{
			break ;
		}
		case HttpRequest::ERROR:
		{
	
			break;
		}
		default:
			break;
		}
	}
	
}

void Server::ParseFSM(int sock_fd)
{
    const std::string& buf = http_req[sock_fd].getBuffer();

    // 1) Parse request line
    size_t pos_reqline_end = buf.find("\r\n");
    std::string reqline = buf.substr(0, pos_reqline_end);

    std::istringstream rl(reqline);
    std::string method, path, protocol;
    rl >> method >> path >> protocol;

    http_req[sock_fd].setMethod(method);
    http_req[sock_fd].setPath(path);
    http_req[sock_fd].setProtocol(protocol);

    // 2) Parse headers
    size_t header_start = pos_reqline_end + 2;                // skip "\r\n"
    size_t headers_end = buf.find("\r\n\r\n");                // end of headers block
    size_t headers_len = headers_end - header_start;

    std::string header_content = buf.substr(header_start, headers_len);
	std::istringstream header_stream(header_content);
	std::string line;

	while (std::getline(header_stream, line)) 
	{
		if (line.empty())
			continue;

		// Remove trailing \r (C++98 doesn't have pop_back)
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


    // 3) Parse body
    size_t body_start = headers_end + 4;          // skip "\r\n\r\n"
    std::string body = buf.substr(body_start);

    http_req[sock_fd].setBody(body);

    //std::cout << "Parsed method:   " << method << std::endl;
    //std::cout << "Parsed path:     " << path << std::endl;
    //std::cout << "Parsed protocol: " << protocol << std::endl;

    //const std::map<std::string, std::string>& headers = http_req[sock_fd].getHeader();

	//for (std::map<std::string, std::string>::const_iterator it = headers.begin();
	//	it != headers.end();
	//	++it)
	//{
	//	std::cout << "Header[" << it->first << "] = " << it->second << std::endl;
	//}

    //std::cout << "Body: " << body << std::endl;
}
