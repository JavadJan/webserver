#include "../include/ResponseHandler.hpp"

void ResponseHandler::parseOutBufferCGI(const std::string& out)
{
    size_t pos = out.find("\r\n\r\n");
    size_t delim = 4;

    if (pos == std::string::npos)
    {
        pos = out.find("\n\n");
        delim = 2;
    }

    if (pos == std::string::npos)
    {
        // No headers found → entire output is body
        this->setHeaderCGI("");
        this->setCGIBody(out);
        return;
    }

    std::string headerPart = out.substr(0, pos);
    std::string bodyPart   = out.substr(pos + delim);

    this->setHeaderCGI(headerPart);
    this->setCGIBody(bodyPart);
}


// Convert std::string to pointer and alloc in heap
static char* dupString(const std::string& s)
{
    char* p = new char[s.size() + 1];
    std::strcpy(p, s.c_str());
    return p;
}

// Convert size_t to string
static std::string size_to_string(size_t len)
{
	std::stringstream ss;
	ss << len;
	return ss.str();
}

std::string ResponseHandler::scriptCGI()
{
    size_t dot = full_path.rfind('.');
    if (dot == std::string::npos)
        return "";

    std::string ext = full_path.substr(dot);

    std::map<std::string, std::vector<std::string> >::const_iterator it =
        this->loc.directive.find("cgi");

	if (it == this->loc.directive.end())
        return "";
		
		const std::vector<std::string>& v = it->second;
		
	for (size_t i = 0; i + 1 < v.size(); i += 2)
	{
		if (v[i] == ext)
		{
			std::cout << "CGI file to run: "  << v[i] << v[i + 1] << std::endl;
			this->setCGIScript(v[i + 1]); // /usr/bin/
            return v[i + 1];
		}
    }

    return "";
}


// create env variable for cgi
std::vector<char*> ResponseHandler::buildCGIEnv(const HttpRequest& req, const Config& server)
{
    std::vector<char*> env;

    // REQUEST_METHOD
    env.push_back(dupString("REQUEST_METHOD=" + req.getMethod()));

    // SCRIPT_FILENAME
    //size_t slash = req.getPath().find_last_of('/');
    //std::string filename = req.getPath().substr(slash + 1);
    //env.push_back(dupString("SCRIPT_FILENAME=" + filename));
	env.push_back(dupString("SCRIPT_FILENAME=" + full_path));


    //// QUERY_STRING
    env.push_back(dupString("QUERY_STRING=" + req.getQuery()));

    //// CONTENT_LENGTH
	std::string len = size_to_string(req.getContetnLen());
    env.push_back(dupString("CONTENT_LENGTH=" + len));

    //// CONTENT_TYPE
    env.push_back(dupString("CONTENT_TYPE=" + req.getContentType()));

    //// SERVER_PROTOCOL
    env.push_back(dupString("SERVER_PROTOCOL=HTTP/1.1"));

    //// SERVER_NAME
    env.push_back(dupString("SERVER_NAME=" + server.host)); // server is struct no getter

    //// SERVER_PORT
    env.push_back(dupString("SERVER_PORT=" + server.port));

    // GATEWAY_INTERFACE
    env.push_back(dupString("GATEWAY_INTERFACE=CGI/1.1"));

    // NULL terminator required by execve
    env.push_back(NULL);

    return env;
}


void ResponseHandler::handleCGI(const HttpRequest &req, const Config &server)
{
	std::vector<char *> env = buildCGIEnv(req, server);
	int out_fd[2]; // for run script to parent
	int in_fd[2];	// for reading body post
	//int statusChild = -1;
	char* script = dupString(scriptCGI()); // or getCGIScript()
	char* script_path = dupString(full_path); // or getCGIScript()
	char* argv[] = {
		script,                 // interpreter
		script_path,   // script file
		NULL
	};

	std::cout << "full_path to run script: " << full_path << std::endl;
	std::string outBuffer;
	int state = pipe(out_fd);
	int stateIn = pipe(in_fd);
	if (state < 0 || stateIn < 0)
	{
		std::cout << "Failed in CGI, [creation pipe]" << strerror(errno) << std::endl;
		delete[] script;
		for (size_t i = 0; i < env.size(); ++i)
			delete[] env[i];
		return;
	}

	pid_t pid = fork();
	if (pid < 0)
	{
		std::cout << "Failed to create child process: " << strerror(errno) << std::endl;
		delete[] script;
		for (size_t i = 0; i < env.size(); ++i)
			delete[] env[i];
		close(out_fd[0]);
		close(in_fd[0]);
		close(out_fd[1]);
		close(in_fd[1]);
		return;
	}
	if (pid == 0)
	{
		// Child process
		dup2(in_fd[0], STDIN_FILENO);   // read POST body
		dup2(out_fd[1], STDOUT_FILENO); // send script output
		close(in_fd[1]);
		close(out_fd[0]);
		if (execve(script, argv, &env[0]) == -1)
		{
			std::cout << "Failed to execve: " << strerror(errno) << std::endl;
			_exit(1);
			//return ;
		}
	}
	else
	{
		close(in_fd[0]); // here write no nead to input
		if (req.getMethod() == "POST")
		{
			write(in_fd[1], req.getBody().c_str(), req.getBody().size());
		}
		// Parent process
		close(out_fd[1]);

		int flags = fcntl(out_fd[0], F_GETFL, 0); // the read should not be blocking
		fcntl(out_fd[0], F_SETFL, flags | O_NONBLOCK);

		char buffer[4096];
		std::string outBuffer;
		int statusChild = -1;

		int waited = 0;
		bool timedOut = false;
		bool interrupted = false;

		while (true)
		{
			// 1) Check for shutdown (Ctrl+C)
			if (Server::stop_flag)
			{
				interrupted = true;
				break;
			}

			// 2) Try to read any available CGI output (non-blocking)
			ssize_t bytesRead = read(out_fd[0], buffer, sizeof(buffer));
			if (bytesRead > 0)
			{
				outBuffer.append(buffer, bytesRead);
			}

			// 3) Check if child has exited
			pid_t result = waitpid(pid, &statusChild, WNOHANG);
			if (result == pid)
			{
				// child finished
				break;
			}
			if (result == -1 && errno == EINTR)
			{
				// interrupted by signal, retry
				continue;
			}

			// 4) Timeout logic
			if (waited >= 3000) // e.g. 3000 * 1ms = 3 seconds
			{
				timedOut = true;
				kill(pid, SIGKILL);
				break;
			}

			usleep(1000); // sleep 1ms
			waited++;
		}

		close(out_fd[0]);

		if (interrupted)
		{
			res.setStatusCode(503); // Service Unavailable
		}
		else if (timedOut)
		{
			res.setStatusCode(504); // Gateway Timeout
		}
		else if (WIFEXITED(statusChild))
		{
			int code = WEXITSTATUS(statusChild);
			res.setStatusCode(code == 0 ? 200 : 500);
		}
		else if (WIFSIGNALED(statusChild))
		{
			res.setStatusCode(500);
		}

		parseOutBufferCGI(outBuffer);
		res.setBody(getBodyCGI());
	}
	delete[] script_path;
	delete[] script;
	for (size_t i = 0; i < env.size(); ++i)
		if (env[i] != NULL)
			delete[] env[i];
}

// CGI

//bool ResponseHandler::isCGI()
//{
//	// extract extention *.ext
//	// the request_line should be have .py? e.g., POST /cgi/file.py HTTP/1.1 ?
//    size_t dot = full_path.rfind('.');
//    if (dot == std::string::npos)
//        return false;

//	// .py and .php extention
//    std::string ext = full_path.substr(dot);

//	// the sever block has configured for cgi at all?
//    std::map<std::string, std::vector<std::string> >::const_iterator it =
//        this->loc->directive.find("cgi");

//    if (it == this->loc->directive.end())
//        return false;

//	// find the specific extention cgi e.g., .py or .php: path==cgi .py
//    const std::vector<std::string>& v = it->second;
//    for (size_t i = 0; i + 1 < v.size(); i += 2)
//    {
//        if (v[i] == ext)
//            return true;
//    }

//    return false;
//}

bool ResponseHandler::isCGI()
{
    if (full_path.empty())
        return false;

    size_t dot = full_path.rfind('.');
    if (dot == std::string::npos)
        return false;

    std::string ext = full_path.substr(dot);

    if (!has_loc)
        return false;

    std::map<std::string, std::vector<std::string> >::const_iterator it =
        loc.directive.find("cgi");

    if (it == loc.directive.end())
        return false;

    const std::vector<std::string>& v = it->second;
    for (size_t i = 0; i + 1 < v.size(); i += 2)
    {
        if (v[i] == ext)
            return true;
    }

    return false;
}


#pragma region 
/* ----------------------------------------- */
/* 											 */
/* 			SETTER AND GETTER				 */
/* 											 */
/* ----------------------------------------- */

void ResponseHandler::setHeaderCGI(std::string _header)
{
	this->headerCGI = _header;
}

void ResponseHandler::setCGIBody(std::string cgiBod)
{
	this->bodyCGI = cgiBod;
}
const std::string& ResponseHandler::getHeaderCGI() const
{
	return headerCGI;
}

const std::string& ResponseHandler::getBodyCGI() const
{
	return bodyCGI;
}
#pragma endregion