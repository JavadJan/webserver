# Webserve

## Description

Webserv is a custom HTTP server written in C++ as part of the 42 curriculum.
The goal of this project is to build a fully functional web server compliant with HTTP/1.1, without using external libraries, and capable of handling multiple clients simultaneously.

This project focuses on:

- Low-level network programming
- Event-driven I/O
- HTTP protocol parsing
- Configuration file parsing
- Robust error handling

## Features
- HTTP/1.1 compliant
- Non-blocking I/O using poll() (or select() / epoll, depending on implementation)
- Multiple client handling
- Configurable server using a .conf file
Supports:
 - GET
 - POST
 - DELETE

- Static file serving
- CGI execution (e.g. PHP, Python)
- Custom error pages
- Directory listing (autoindex)
- File upload support
- Virtual servers (multiple ports / hosts)

## Configuration File
The server is configured using a configuration file inspired by Nginx syntax.
Example:

```conf
server {
    listen 8080;
    server_name localhost;

    root ./www;
    index index.html;

    location /upload {
        methods POST;
        upload_dir ./uploads;
    }

    location /cgi {
        cgi_pass /usr/bin/php-cgi;
    }

    error_page 404 ./errors/404.html;
}
```

## Installation and Usage

- Build
```bash

    make 
```
- Run
```
./webserv config/default.conf
```

## Supported HTTP Methods

| Method | Description             |
| ------ | ----------------------- |
| GET    | Retrieve a resource     |
| POST   | Send data to the server |
| DELETE | Remove a resource       |


## Architecture

+-------------+        +-----------------+
|   Client    | <----> |   Webserv Core  |
+-------------+        +-----------------+
                                 |
        -------------------------------------------------
        |          |            |           |           |
   Socket      Event Loop   Request     Response     Config
   Layer       (poll)       Parser      Generator    Parser
        |                        |
        |                     CGI Handler


- Socket layer

    -Server socket creation
    -Client connection handling

- Event loop

    -Non-blocking polling
    -Read / write multiplexing

- Request parsing

    -Headers
    -Body
    -Chunked encoding

- Response generation

    -Status codes
    -Headers
    -Body

- CGI handler
    -Environment setup
    -Process execution

- Configuration parser
    -Server blocks
    -Location blocks
    -Validation

## Testing
The server was tested using:

- curl
- Web browsers (Firefox / Chrome)
- Stress tests with multiple clients
- Invalid requests and edge cases
Example
 ```bash

 curl -X GET http://localhost:8080
curl -X POST -d "hello=world" http://localhost:8080/upload
```

## Autors
