#include "../include/Config.hpp"

// Helper function to trim whitespace from start and end
static std::string trim(const std::string &s)
{
    size_t start = 0;
    while (start < s.length() && std::isspace(s[start]))
        start++;

    size_t end = s.length();
    while (end > start && std::isspace(s[end - 1]))
        end--;

    return s.substr(start, end - start);
}

// default constructor
Config::Config() : 
	port(4545),
	host("127.0.0.1")
{
}

// Constructor that parses a config file
Config::Config(const std::string &conf)
{
    // Set defaults first
    port = 4545;
    host = "127.0.0.1";

    std::ifstream file(conf.c_str());
    if (!file.is_open())
    {
        std::cerr << "[Config] Could not open file: " << conf << ", using defaults" << std::endl;
        return;
    }

    std::string line;
    size_t lineNumber = 0;

    while (std::getline(file, line))
    {
        lineNumber++;
        line = trim(line);

        // Skip empty lines or comments
        if (line.empty() || line[0] == '#')
            continue;

        // Support "key=value" and "key value" forms
        std::string key;
        std::string value;

        size_t pos = line.find('=');
        if (pos != std::string::npos)
        {
            key = trim(line.substr(0, pos));
            value = trim(line.substr(pos + 1));
        }
        else
        {
            std::istringstream iss(line);
            if (!(iss >> key))
                continue;
            if (!(iss >> value))
            {
                std::cerr << "[Config] Warning: Missing value for key '" << key
                          << "' on line " << lineNumber << std::endl;
                continue;
            }
            std::string extra;
            if (iss >> extra)
            {
                std::cerr << "[Config] Warning: Extra tokens ignored on line "
                          << lineNumber << std::endl;
            }
        }

        if (key == "host")
        {
            if (!value.empty())
                host = value;
            else
                std::cerr << "[Config] Warning: host value empty on line " << lineNumber << std::endl;
        }
        else if (key == "port")
        {
            if (!value.empty())
            {
                int p = std::atoi(value.c_str());
                if (p > 0 && p <= 65535)
                    port = p;
                else
                    std::cerr << "[Config] Warning: invalid port '" << value << "' on line " << lineNumber
                              << ", using default " << port << std::endl;
            }
            else
            {
                std::cerr << "[Config] Warning: port value empty on line " << lineNumber << std::endl;
            }
        }
        else
        {
            std::cerr << "[Config] Warning: Unknown key '" << key << "' on line " << lineNumber << std::endl;
        }
    }
}


Config::Config(const Config &other) : port(other.port),
									  host(other.host)
{
}

Config &Config::operator=(const Config &other)
{
	if (this != &other)
	{
		this->port = other.port;
		this->host = other.host;
	}
	return (*this);
}

Config::~Config() {}

/* getter */
int Config::getPort() const
{
	return (this->port);
}
const std::string &Config::getHost() const
{
	return (this->host);
}