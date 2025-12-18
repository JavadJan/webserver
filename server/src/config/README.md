- Purpose: Convert raw config file → structured objects

ConfigParser
 ├── Tokenizes input
 ├── Validates syntax (braces, semicolons)
 ├── Detects blocks (server, location)
 └── Builds ServerConfig & LocationConfig objects

-------------------------------------------------------
ServerConfig Represents:
------------------------------------------

-listen
-server_name
-error_page
-client_max_body_size
-root
-locations
------------------------------------------------------
LocationConfig Represents:
------------------------------------
-path
-root
-index
-allowed methods
-autoindex
-upload path
-CGI settings

-----Minimal Supported Grammar------------------------------
config          ::= server_block+

server_block    ::= "server" "{" server_directive* location_block* "}"

location_block  ::= "location" PATH "{" location_directive* "}"

server_directive ::= listen ";"
                   | server_name ";"
                   | root ";"
                   | error_page ";"
                   | client_max_body_size ";"

location_directive ::= root ";"
                     | index ";"
                     | autoindex ";"
                     | allow_methods ";"
                     | cgi_pass ";"



Important rules:

-Semicolon mandatory
-Braces mandatory
-location only inside server
-Directives are space-separated
-No nested locations


------------------------------------------------------
Class Responsibilities (Very Important)
----------------------------------------------
ConfigParser:
-Reads file
-Tokenizes content
-Builds objects
-Throws exceptions on error

ServerConfig:
-Holds server directives
-Owns multiple LocationConfig

LocationConfig:
-Holds location directives
-Inherits defaults from server if missing
