#include "config/ConfigParser.hpp"
#include "config/Token.hpp"
#include "config/ServerConfig.hpp"
#include "config/LocationConfig.hpp"

#include <string>
#include <algorithm> // std::find
#include <vector>
#include <stdexcept>

// --------------------------------------------------
// Constructor
// --------------------------------------------------

ConfigParser::ConfigParser(const std::string &filename) : _pos(0)
{
    std::ifstream file(filename.c_str());
    if (!file)
        throw std::runtime_error("Failed to open config file");

    std::stringstream buffer;
    buffer << file.rdbuf();

    tokenize(buffer.str());
}

// --------------------------------------------------
// Token helpers
// --------------------------------------------------

Token &ConfigParser::peek()
{
    return _tokens[_pos];
}
Token &ConfigParser::next()
{
    return _tokens[_pos++];
}
bool ConfigParser::accept(TokenType type)
{
    if (peek().type == type)
    {
        next();
        return true;
    }
    return false;
}
void ConfigParser::expect(TokenType type)
{
    if (!accept(type))
    {
        throw std::runtime_error("Unexpected token: " + peek().value);
    }
}

// --------------------------------------------------
//  Inheritance Logic
// --------------------------------------------------

static void applyInheritance(ServerConfig &server)
{
    std::vector<LocationConfig> &locations = server.getLocations();

   /*  for (size_t i = 0; i < locations.size(); ++i)
    {
        if (locations[i].getRoot().empty())
            locations[i].setRoot(server.getRoot());

        if (locations[i].addIndex().empty())
            locations[i].setIndex(server.getIndex());
    } */
}

// --------------------------------------------------
// Parsing entry point
// --------------------------------------------------

std::vector<ServerConfig> ConfigParser::parse()
{
    std::vector<ServerConfig> servers;
    std::vector<int> ports;

    while (peek().type != TOKEN_EOF)
    {
        ServerConfig server = parseServer();
        applyInheritance(server);
        servers.push_back(server);
    }
    // check Duplicate port
    for (size_t i = 0; i < servers.size(); ++i)
    {
        int port = servers[i].getListenPort();
        if (std::find(ports.begin(), ports.end(), port) != ports.end())
            throw std::runtime_error("Duplicate listen port");
        ports.push_back(port);
    }

    return servers;
}

// --------------------------------------------------
// Server parsing (Phase 1–2)
// --------------------------------------------------

ServerConfig ConfigParser::parseServer()
{
    ServerConfig server;

    if (peek().type != TOKEN_WORD || peek().value != "server")
        throw std::runtime_error("Expected 'server'");
    next();
    expect(TOKEN_LBRACE); // '{'

    while (peek().type != TOKEN_RBRACE && peek().type != TOKEN_EOF)
    {
        if (peek().type != TOKEN_WORD)
        {
            throw std::runtime_error("Expected directive in server block");
        }
        std::string directive = next().value;
        if (directive == "listen")
        {
            if (peek().type != TOKEN_WORD)
                throw std::runtime_error("Expected port after listen");

            int port = std::atoi(next().value.c_str());
            if (port <= 0 || port > 65535)
                throw std::runtime_error("Invalid listen port");
            server.setListenPort(port);

            expect(TOKEN_SEMICOLON);
        }
        else if (directive == "root")
        {
            if (peek().type != TOKEN_WORD)
                throw std::runtime_error("Expected value after root");
            server.setRoot(next().value);
            expect(TOKEN_SEMICOLON);
        }
        else if (directive == "index")
        {
            if (peek().type != TOKEN_WORD)
                throw std::runtime_error("Expected value after index");
            server.setIndex(next().value);
            expect(TOKEN_SEMICOLON);
        }
        else if (directive == "location")
        {
            LocationConfig location = parseLocation();
            server.addLocation(location);
        }
        else
        {
            throw std::runtime_error("Unknown directive:" + directive);
        }
    }
    expect(TOKEN_RBRACE);
    if (server.getListenPort() == -1)
        throw std::runtime_error("Server block missing listen directive");

    return server;
}

// --------------------------------------------------
// Location parsing (stub – not active yet)
// --------------------------------------------------

LocationConfig ConfigParser::parseLocation()
{
    LocationConfig location;

    // expect path
    if (peek().type != TOKEN_WORD)
        throw std::runtime_error("Expected path after 'location'");
    location.setPath(next().value);

    expect(TOKEN_LBRACE);

    while (peek().type != TOKEN_RBRACE)
    {
        std::string directive = next().value;
        if (peek().type != TOKEN_WORD)
            throw std::runtime_error("Expected directive in location block");

        if (directive == "root")
        {
            if (peek().type != TOKEN_WORD)
                throw std::runtime_error("Expected value after root");
            location.setRoot(next().value);
            expect(TOKEN_SEMICOLON);
        }
        else if (directive == "index")
        {
            if (peek().type != TOKEN_WORD)
                throw std::runtime_error("Expected value after index");
            while(peek().type == TOKEN_WORD)
                location.addIndex(next().value);
            expect(TOKEN_SEMICOLON);
        }
        else if (directive == "allowed_methods")
        {
            if (peek().type != TOKEN_WORD)
                throw std::runtime_error("Expected value after index");
            while(peek().type == TOKEN_WORD)
                location.addAllowedMethod(next().value);
            expect(TOKEN_SEMICOLON);
        }
        else if(directive == "autoindex")
        {
            if (peek().type != TOKEN_WORD)
                throw std::runtime_error("Expected value after index");
            if (next().value == "one")
                location.setAutoindex(true);
            else if (next().value == "off")
                location.setAutoindex(false);
            else
                throw std::runtime_error("Invalid autoindex value");
            expect(TOKEN_SEMICOLON);
        }
        else
        {
            throw std::runtime_error("Unknown location directive: " + directive);
        }
    }

    expect(TOKEN_RBRACE);
    return location;
}

/* location_block ::= "location" PATH "{" location_directive* "}"

location_directive ::= root ";"
                     | index ";" */

// --------------------------------------------------
// Tokenizer
// --------------------------------------------------

void ConfigParser::tokenize(const std::string &content)
{
    size_t i = 0;
    while (i < content.size())
    {
        if (isspace(content[i]))
        {
            ++i;
            continue;
        }
        if (content[i] == '{')
        {
            _tokens.push_back(Token(TOKEN_LBRACE));
            ++i;
        }
        else if (content[i] == '}')
        {
            _tokens.push_back(Token(TOKEN_RBRACE));
            ++i;
        }
        else if (content[i] == ';')
        {
            _tokens.push_back(Token(TOKEN_SEMICOLON));
            ++i;
        }
        else
        {
            size_t start = i;
            while (i < content.size() && !isspace(content[i]) && content[i] != '{' && content[i] != '}' && content[i] != ';')
            {
                ++i;
            }

            std::string word = content.substr(start, i - start);
            _tokens.push_back(Token(TOKEN_WORD, word));
        }
    }
    _tokens.push_back(Token(TOKEN_EOF));
}
