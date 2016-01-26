import socket

# host = socket.gethostbyname(socket.getfqdn())
# port = 20203
host = '172.17.13.105'
port = 9002


s = socket.socket()
s.connect((host, port))
print("Connected to "+host+":"+str(port))
# s.settimeout(30)
i = 0
buffered = 0

while True:
    print("Waiting for data from RT server...")
    data = s.recv(4096)

    buffered = len(data)
    print( str(i) + " -- " + str(buffered))
    print( data.decode('utf-8') )
    print("===="*20)
    i += 1

s.close()
