#include "../include/ParsedData.hpp"

ParsedData::ParsedData(temp_data data)
{
	(void)data;
}
ParsedData::~ParsedData()
{
}
ParsedData::ParsedData(const ParsedData &other)
{
	*this = other;
}
ParsedData& ParsedData::operator=(const ParsedData &other)
{
	if (this != &other)
	{
		*this = other;
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

enum STATE FSM(char buffer[], temp_data *data)
{
	(void)buffer;
	(void)data;
	std::string bufr;
	
	STATE state = REQ_LINE;
	while (state != DONE)
	{
		bufr.append(buffer); // because tcp is stream it should wait untill end up with first line
		//bufr.append(buffer);
		switch (state)
		{
		case REQ_LINE:
		{
			// fill method, version, path
			size_t pos = bufr.find("\r\n");
			if (pos != std::string::npos)
			{
				// now first line has recived completly, start to prase
				std::istringstream ss(bufr);

				//split the first line
				ss >> data->method >> data->path >> data->protocol;

				std::cout << "parsing the req_line: " << data->method << ", " << data->path << ", " << data->protocol << std::endl;
				if (!isMethod(data->method) || !isHTTP(data->path))
					state = ERROR;
				else {
					bufr.erase(0, pos + 2); // erase \r\n
					state = HEADER;
				}
				//state = HEADER;
				break ;

			}
			break; // stay in current state if not complate then break and wait
		}
		case HEADER:
		{
			std::cout << "parsing the header\n";
			std::istringstream ss(bufr);
			size_t pos = bufr.find("\r\n\r\n");

			if (pos != std::string::npos)
			{
				// parse header
				ss >> data->header.first >> data->header.second.first; // check it might lost the port host: 127.0.0.1:8080
				if (data->header.first != "127.0.0.1" || data->header.first != "localhost")
					state = ERROR;
				else
					state = BODY;
				bufr.erase(0, pos + 4); // earse\r\n\r\n
				state = BODY; // go next state
				break;
			}
			// if exist key value it is header
			break ; //break and stay in current state
		}
		case BODY:
		{
			std::cout << "parsing the body\n";
			//state = DONE;
			break;
		}
		case DONE:
		{
			std::cout << "parsing the DONE\n";
			state = ERROR;
			break;
		}
		case ERROR:
		{
			std::cout << "ERROR\n";
			return ERROR;
			break;
		}
		default:
			break ;
		}
	}
	return DONE;
}