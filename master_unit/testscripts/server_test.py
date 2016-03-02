#! /usr/bin/python3

# Written as TCP server for client.py and possibly RT Engine soon
import socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# host = socket.gethostbyname(socket.getfqdn())
host = '127.0.0.1'
# host = ''
port = 9001
s.bind((host, port))
s.listen(1)

# data = bytes(traffic_data, 'utf-8')
c, addr = s.accept()
print("Got connection from " + str(addr))

while True:
    data = c.send(b'hello')
    sleep(5000)
    # print( data.decode('utf-8') )
    # print('===='*20)

c.close()
