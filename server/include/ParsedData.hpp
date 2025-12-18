#ifndef ParsedData_hpp
#define ParsedData_hpp

// #include "./server.hpp"
#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <cerrno>
#include <algorithm> // for std::transform
#include <cctype>	 // for std::tolower

/*
 * Parser STATES:
 * - REQ_LINE : need the request-line (first line)
 * - HEADER   : parsing headers
 * - BODY     : expecting body bytes (Content-Length)
 * - DONE     : complete request parsed
 * - ERROR    : parsing error (malformed request)
 */
enum STATE
{
	REQ_LINE,
	HEADER,
	BODY,
	DONE,
	ERROR
};
/*
 * temp_data holds parsed pieces of a single HTTP request:
 * - method: "GET", "POST", etc.
 * - path: request target ("/", "/index.html", ...)
 * - protocol: HTTP version "HTTP/1.1"
 * - headers: a map of header -> value
 * - body: message body (for POST/PUT)
 */
typedef struct data
{
	std::string method;
	std::string path;
	std::string protocol;
	std::map<std::string, std::string> headers;
	std::string body;
} temp_data;

class ParsedData
{
private:
	temp_data data;

public:
	ParsedData() { clear(); }
	~ParsedData() {}

	/* setter and getter */
	void setMethod(const std::string &m) { data.method = m; }
	void setPath(const std::string &p) { data.path = p; }
	void setProtocol(const std::string &p) { data.protocol = p; }
	void addHeader(const std::string &k, const std::string &v) { data.headers[k] = v; }
	void appendBody(const std::string &chunk) { data.body += chunk; }

	void ParsedData::clear()
	{
		data.method.clear();
		data.path.clear();
		data.protocol.clear();
		data.headers.clear();
		data.body.clear();
	}
	const temp_data &getData() const { return data; }
	temp_data &getDataRef() { return data; }

/*
 * FSM:
 * - buffer: the per-connection accumulated raw bytes (std::string)
 * - data: temp_data to fill while parsing
 * Returns one of STATE:
 * - REQ_LINE  : request-line still incomplete (need more bytes)
 * - HEADER    : headers not finished (need more bytes)
 * - BODY      : waiting for full body bytes (need more bytes)
 * - DONE      : complete request parsed (data contains full request)
 * - ERROR     : malformed request
 *
 * IMPORTANT: the function WILL MODIFY buffer (erase data it consumed).
 * It is designed to be called repeatedly as new bytes arrive.
 */

STATE FSM_parse(std::string &buffer, temp_data &data);
};

#endif