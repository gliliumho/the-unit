#! /usr/bin/python3

# Written to test TCP connection with RT server
import socket

# host = socket.gethostbyname(socket.getfqdn())
# port = 20203
host = '172.17.0.222'
port = 9001

s = socket.socket()
# s.connect((host, port))
try:
    s.connect((host,port))
    print("Connected to "+host+":"+str(port))
except TimeoutError:
    print("Connection timed out. Cannot connect to "+host)
    s.close()
    exit()
print("Connected to "+host+":"+str(port))
# s.settimeout(30)
i = 0
buffered = 0

while True:
    print("Waiting for data from RT server...")
    data = s.recv(2048)

    buffered = len(data)
    print( str(i) + " -- " + str(buffered))
    print( data.decode('utf-8') )
    print("===="*20)
    i += 1

s.close()
