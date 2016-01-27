#!/bin/python3

# Written as TCP server for client.py and possibly RT Engine soon
import socket

values = root.items()
for value in values:
    print(value.text)



s = socket.socket()
host = socket.gethostbyname(socket.getfqdn())
port = 20203
s.bind((host, port))
s.listen(5)

data = bytes(traffic_data, 'utf-8')

while True:
    c, addr = s.accept()
    print("Got connection from " + str(addr))
    c.send(data)
    c.close()
