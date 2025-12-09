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


void Server::fsm(std::string recv, int sock_fd)
{
	std::string len_body;
	std::string contetn;
	//std::cout << "arived to fsm: " << recv << std::endl;
	switch (clientState[sock_fd])
	{
	case REQ_LINE:
	{
		if (recv.find("\r\n") == std::string::npos) // not complted wait more
			break ;
		clientState[sock_fd] = HEADER;
		std::cout << "first line was completed\n";
		break;
	}
	case HEADER:
	{
		if (recv.find("\r\n\r\n") == std::string::npos) // not complted wait more
			break;
		if (recv.find("Content-Length") != std::string::npos) // found body
		{
			int len = len_of_body(recv.find("Content-Length"), recv);
			std::cout << "content len: " << len << std::endl;
			conten_len[sock_fd] = len;

			clientState[sock_fd] = BODY;
			break ;	
		}
		else
			clientState[sock_fd] = DONE;
		break;
	}
	case BODY:
	{
		//int pos = recv.find("Content-Length"); // obtain the pos is so overload calculation
			//!= std::string::npos; // read the content-len;
		// contetn_len is already completed in header
		//int len = len_of_body(pos, recv);
		//std::string temp = recv.substr(pos, len);
		
		/* start body */
		int start_body = recv.find("\r\n\r\n") + 4;
		//std::istringstream ss(temp);
		//ss >> contetn >> len_body ; 
		if (conten_len[sock_fd] >= recv.substr(start_body).length()) 
			break ; // stay in body
		//std::cout << "len body: " << contetn << " len:" << len_body << std::endl << std::endl;
		clientState[sock_fd] = DONE;
		break;
	}
	case DONE:
	{
		break ;
	}
	case ERROR:
	{

		break;
	}
	default:
		break;
	}
}


HttpRequest Server::ParseFSM(std::string bufr)
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