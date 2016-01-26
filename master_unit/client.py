import socket

s = socket.socket()
host = socket.gethostbyname(socket.getfqdn())
port = 20203

s.connect((host, port))
print(s.recv(2048).decode('utf-8'))
s.close()
