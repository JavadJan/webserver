#include "../include/Config.hpp"

//Config::Config():port(4545), host("127.0.0.1")
//{

//}

static std::string trim(std::string s)
{
	//find the first non‑whitespace character
	size_t start = s.find_first_not_of(" \t");
	//find the last non‑whitespace character
	size_t end = s.find_last_not_of(" \t");
	if (start == std::string::npos)
		return "";
	return s.substr(start, end -start + 1);
}

static std::string removeSemicolon(std::string s)
{
	if (!s.empty() && s[s.size() - 1] == ';')
		s.erase(s.size() - 1);
	return s;
}
std::vector<struct Config> parseConfig(const char* conf)
{
	std::ifstream file(conf);
	if (!file)
		throw std::runtime_error("failed to open config file");

	// create servers
	std::vector<struct Config> servers;

	// define location
	struct Location currentLocation;

	//define config
	struct Config currentServer;

	enum STATE {NONE, SERVER, LOCATOIN} state = NONE;

	// iteration line by line on file
	std::string line;
	while (std::getline(file, line))
	{
		// trim space and tab
		line = trim(line);
		if (line.empty())
			continue; // jump next iteration

		// erase comment
		size_t comment = line.find('#');
		if(comment != std::string::npos)
			line.erase(comment);

		// if was server is in server block-> state == SERVER
		if (line == "server")
		{
			state = SERVER;
			currentServer = Config();
			continue;
		}

		// single { skip line
		if (line == "{")
			continue;
		
		if (line == "}")
		{
			if (state == LOCATOIN)
			{
				currentServer.locations.push_back(currentLocation);
				currentLocation = Location();
				state = SERVER;
			}
			else if (state == SERVER)
			{
				servers.push_back(currentServer);
				// create new config server struct and change state
                currentServer = Config();
                state = NONE;
			}
			continue;
		}

		// if was location is in block location->satat = LOCAION
		std::istringstream ss(line);
		if (line.find("location") == 0)
		{
			std::string keyword, path;
			ss >> keyword >> path;

			currentLocation = Location();
			currentLocation.path = path;
			state = LOCATOIN;
			continue;
		}
			
		std::string key, value;
        ss >> key >> value;
		// remove ; from end
        value = removeSemicolon(value);

		if (state == SERVER)
        {
			// in server state has first line and rest
            if (key == "listen")
            {
                size_t pos = value.find(':');
                if (pos == std::string::npos)
                    throw std::runtime_error("Invalid listen directive");

                currentServer.host = value.substr(0, pos);
				std::cout << "port: " << value.substr(pos + 1) << std::endl;
                currentServer.port = atoi(value.substr(pos + 1).c_str());
            }
            else
            {
                currentServer.directives[key] = value;
            }
        }
        else if (state == LOCATOIN)
        {
            currentLocation.directive[key] = value;
        }
	}
	return servers;
}
