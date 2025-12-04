#ifndef ParsedData_hpp
# define ParsedData_hpp

#include "./server.hpp"
#include <iostream>
#include <map>

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
	std::map<std::string, std::string>	header;
	std::map<std::string, std::string>	body;
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

#endif