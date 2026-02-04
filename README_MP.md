# webserver

[CONFIG] → Server reads host & port
     |
[SOCKET] → Server creates TCP socket
     |
[LISTEN] → Server listens for incoming connections
     |
[POLL LOOP] → Server monitors all sockets
     |
   +------------+
   |            |
[NEW CLIENT]  [EXISTING CLIENT]
   |            |
accept()      recv()
   |            |
add to poll    broadcast to other clients



/* blocks for up to 2000 milliseconds (2 seconds) waiting for any socket in poll_fds to become ready. It returns:
     int ret = poll(poll_fds.data(), poll_fds.size(), 2000); // 2 second timeout

            > 0: number of sockets with events
            0: timeout (no activity in 2 seconds)
            < 0: error
     The 2-second timeout prevents the server from blocking forever—it periodically prints "Listening..." and checks again. */

---------------------------------------------------------------------------------
HTTP is not a stream of messages, it is a protocol with structure:

<request-line>\r\n
<header: value>\r\n
<header: value>\r\n
\r\n
<body...>


So we must:

Read until we find \r\n\r\n → end of headers

Parse request-line

Parse headers

Decide if a body is required (POST, PUT)

Store result in HttpRequest   

------------------------------------------------------------------
Why do we need clear() in Request?

Because one TCP connection can contain multiple HTTP requests.

This is called:

🔥 HTTP Keep-Alive (persistent connections)

Example:

Client --- TCP connection open ---> Server

GET /index.html HTTP/1.1
Host: localhost

GET /about HTTP/1.1
Host: localhost

POST /login HTTP/1.1
Host: localhost
Content-Length: 20

username=bob&pw=123


All 3 requests can come through the same socket without closing the connection.

-------------------------------------------------------------------------------------
-----------Header Line---------------------------------------------------------------
REQUEST LINE:
  - Method
  - Request target (URI)
  - HTTP version

HEADERS (zero or more):
  - Host (required in HTTP/1.1)
  - Connection
  - User-Agent
  - Accept / Accept-Encoding / Accept-Language
  - Content-Type
  - Content-Length
  - Transfer-Encoding
  - Cookie
  - Authorization
  - Range
  - Expect
  - Others…

EMPTY LINE:
  - CRLF (\r\n) to mark end of headers

OPTIONAL BODY:
  - Raw body
  - JSON
  - Form data
  - Multipart data
  - chunked-encoded body
