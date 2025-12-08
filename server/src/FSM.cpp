#include "../include/Server.hpp"
#include "../include/HttpRequest.hpp"

/* 
	read whole stream from recv,
	then pass to the fsm
*/

void fsm(std::string recv, enum STATE *state)
{
	std::cout << "arived to fsm: " << recv << std::endl;
	switch (*state)
	{
	case REQ_LINE:
	{
		if (recv.find("\r\n") == std::string::npos) // not complted wait more
			break ;
		*state = HEADER;
		std::cout << "first line was completed\n";
		break;
	}
	case HEADER:
	{
		if (recv.find("\r\n\r\n") == std::string::npos) // not complted wait more
			break;
		if (recv.find("Content-Length") != std::string::npos) // found body
		{
			*state = BODY;
			break ;	
		}
		else
			*state = DONE;
		break;
	}
	case BODY:
	{
		int pos = recv.find("Content-Length"); // obtain the pos is so overload calculation
			//!= std::string::npos; // read the content-len;
		(void)pos;
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
