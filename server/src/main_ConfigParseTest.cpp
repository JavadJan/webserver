#include<iostream>
#include "config/ConfigParser.hpp"

int main(int argc, char ** argv){

    if (argc != 2)
    {
        std::cerr << "Usage: ./webserv <config_file>\n";
        return 1;
    }
     try
    {
        ConfigParser parser(argv[1]);
        std::vector<ServerConfig> servers = parser.parse();

        std::cout << "Parsed " << servers.size() << " server(s)\n";

        for (size_t i = 0; i < servers.size(); ++i)
        {
            std::cout << "\n--- Server " << i + 1 << " ---\n";
            std::cout << "Listen port: "
                      << servers[i].getListenPort() << "\n";

            const std::vector<LocationConfig>& locations =
                servers[i].getLocations();

            std::cout << "Locations: " << locations.size() << "\n";

            for (size_t j = 0; j < locations.size(); ++j)
            {
                std::cout << "  Location " << j + 1 << "\n";
                std::cout << "    Path: " << locations[j].getPath() << "\n";
                std::cout << "    Root: " << locations[j].getRoot() << "\n";
               /*  std::cout << "    Index: " << locations[j].getIndex() << "\n";
            } */
            }
        }   
    }
    catch(const std::exception& e)
    {
        std::cerr << "Config error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}