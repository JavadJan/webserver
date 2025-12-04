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