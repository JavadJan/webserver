import socket

s = socket.socket()
s.connect(("127.0.0.1", 4243))

big_header = "A" * 17000
request = f"GET / HTTP/1.1\r\nHost: localhost\r\nX-Big: {big_header}\r\n\r\n"
s.send(request.encode())

response = s.recv(4096)
print(response.decode())
s.close()
