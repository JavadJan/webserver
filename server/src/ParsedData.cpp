#include "../include/ParsedData.hpp"

// ------------------ Helpers ------------------

/* bool isMethod(std::string method)
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
} */



// ------------------ Helpers ------------------
static bool isMethod(const std::string &m)
{
    if (m == "GET" || m == "POST" || m == "DELETE" || m == "PUT" || m == "HEAD" || m == "OPTIONS")
		return true;
	return false;
}

static bool isHTTPVersion (const std::string &v)
{	
	if (v == "HTTP/1.0" || v == "HTTP/1.1" || v == "HTTP/2" || v == "HTTP/3" || v == "HTTP/0.9")
		return true;
	return false;
}




// ------------------ FSM parser ------------------

/*
 * The FSM works in these phases:
 * 1) request-line: look for "\r\n". If absent -> need more bytes.
 * 2) headers: look for "\r\n\r\n". If absent -> need more bytes.
 *    When found, parse each header line "Key: value".
 * 3) body: if Content-Length header present -> read exact number of bytes.
 *    If buffer contains fewer bytes than Content-Length -> return BODY (need more).
 *    When body fully available -> set data.body and return DONE.
 *
 * The buffer is modified (consumed) as we parse.
 */

STATE FSM_parse(std::string &buffer, temp_data &data)
{
	
	// Find end of request-line
	size_t pos_line = buffer.find("\r\n");
	if (pos_line == std::string::npos) {
		// We don't even have a full request-line yet
		return REQ_LINE;
	}


	// Extract and parse request-line
	std::string request_line = buffer.substr(0, pos_line);
	std::istringstream rl(request_line);
	std::string method, path, version;
	if (!(rl >> method >> path >> version))
	{
		//malformed request_line
		return ERROR;
	}


	// Validate method and version
	if (!isMethod( data.method)) return ERROR;
	if (!isHTTPVersion(version)) return ERROR;

	// store parsed values

	// consume request-line + CRLF from buffer


	// -------------- HEADER --------------
    // Search for header end marker



	// Extract raw headers block


	// consume headers + CRLFCRLF

	// -------------- BODY --------------
    // Check for Content-Length header


	// If Content-Length > 0, ensure we have that many bytes in buffer

	// Everything parsed successfully
	return DONE;
}