import subprocess
import time
import socket
import base64
import os

print "Establish Connection To Reco Traffic Server"

TCP_IP = '10.0.0.212'
TCP_PORT = 7001
BUFFER_SIZE = 512
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((TCP_IP, TCP_PORT))
s.listen(1)
conn, addr = s.accept()
print "Connection address: ", addr

while 1:
	data = conn.recv(BUFFER_SIZE)
	data = data.strip()
	if data == "0":
		print "Received 0...Smooth Traffic"
	elif data == "1":
		print "Received 1...Moving Slowly"
	elif data == "2":
		print "Received 2...Heavy Traffic"
	else:
		print "Received NOT Meaningful DATA...!!!!!"
conn.close()
