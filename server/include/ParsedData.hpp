#ifndef ParsedData_hpp
# define ParsedData_hpp

#include "./server.hpp"
#include <iostream>
#include <map>
#include <sstream>
#include <cerrno>
#include <algorithm> // for std::transform
#include <cctype>    // for std::tolower

enum	STATE
{
	REQ_LINE,
	HEADER,
	BODY,
	DONE,
	ERROR
};

typedef struct data{
	std::string method;
	std::string path;
	std::string protocol;
	std::pair<std::string, std::pair<std::string, std::string> >	header; // host : "127.0.0.1:8080"
	std::string	body;
} temp_data;

class ParsedData
{
	private:
		temp_data data;
	public:
		ParsedData(temp_data data);
		~ParsedData();
		ParsedData(const ParsedData& other);
		ParsedData& operator=(const ParsedData& other);

		/* setter and getter */
		/*  */


};

enum STATE FSM(char buffer[], temp_data *data);
bool isMethod(std::string method);
#endif