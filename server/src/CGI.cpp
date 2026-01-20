#include "../include/ResponseHandler.hpp"

bool ResponseHandler::isCGI()
{
	// extract extention *.*
	size_t dot = full_path.find(".");
	if (dot == std::string::npos)
		return false;

	std::string ext = full_path.substr(dot); // .py and .php

	// the server block has configured for cgi at all?
	std::map<std::string, std::vector<std::string> >::const_iterator it = this->loc->directive.find("cgi");
	if (it == this->loc->directive.end())
		return false;
	
	// find the specific extention cgi e.g., .py or .php
	for (size_t i = 0 ; i < it->second.size(); ++i)
	{
		if (it->second[i] == ext)
			return true;
	}
	return false;
}

static char* dupString(const std::string& s)
{
    char* p = new char[s.size() + 1];
    std::strcpy(p, s.c_str());
    return p;
}


// create env variable for cgi
std::vector<char*> ResponseHandler::buildCGIEnv(
    const HttpRequest& req,
    const Config& server)
{
    std::vector<char*> env;

    // REQUEST_METHOD
    env.push_back(dupString("REQUEST_METHOD=" + req.getMethod()));

    // SCRIPT_FILENAME
    size_t slash = req.getPath().find_last_of('/');
    std::string filename = req.getPath().substr(slash + 1);
    env.push_back(dupString("SCRIPT_FILENAME=" + filename));

    //// QUERY_STRING
    //env.push_back(dupString("QUERY_STRING=" + req.getQuery()));

    //// CONTENT_LENGTH
    //env.push_back(dupString("CONTENT_LENGTH=" + req.getContetnLen()));

    //// CONTENT_TYPE
    //env.push_back(dupString("CONTENT_TYPE=" + req.getContetnType()));

    //// SERVER_PROTOCOL
    //env.push_back(dupString("SERVER_PROTOCOL=HTTP/1.1"));

    //// SERVER_NAME
    //env.push_back(dupString("SERVER_NAME=" + server.getHost()));

    //// SERVER_PORT
    //env.push_back(dupString("SERVER_PORT=" + server.getPort()));

    // GATEWAY_INTERFACE
    env.push_back(dupString("GATEWAY_INTERFACE=CGI/1.1"));

    // NULL terminator required by execve
    env.push_back(NULL);

    return env;
}


void ResponseHandler::handleCGI(const HttpRequest &req, const Config &server)
{
	(void)req;
	(void)server;
	pid_t pid;
	pid = fork();
	if (pid < 0)
	{
		std::cout << "Failed to create child process: " << strerror(errno) << std::endl;
		return ;
	}
	else{
		
	}
}