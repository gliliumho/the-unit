#! /usr/bin/python3

# Written to test TCP connection with RT server
import socket
import xml.etree.ElementTree as ET

def get_traffic_data(socket):

    try:
        print("Getting data from " + str(socket.getpeername()))
        data = socket.recv(2048)
    except ConnectionResetError:
        # return None
        print("Connection from server timed out.......")
        # raise
        # return int(0)
    except ConnectionRefusedError:
        # return None
        print("Connection refused by server.")
        return int(0)


    data = data.decode('utf-8')
    try:
        root = ET.XML(data)
    except ET.ParseError:
        print(data)
        print("Double XML from RT engine")
        return -2

    for value in root.iter():
        if value.tag == "IncidentType":
            if value.text == "CongestionStart":
                print("Congested..")
                return int(3)

            elif value.text == "CongestionEnd":
                print("End of congestion..")
                return int(2)

        elif value.tag == "CongestionLevel":
            # return int(value.text)+1
            print("Traffic level: " + str(int(value.text)+1))
            return int(value.text)+1

    return -1


# host = socket.gethostbyname(socket.getfqdn())
# port = 20203
# host = '172.17.11.104'
host = '172.17.11.51'

# host = '8.8.8.8'
port = 9002

s = socket.socket()
# s.connect((host, port))
try:
    s.settimeout(5)
    s.connect((host,port))
    print("Connected to "+host+":"+str(port))
except TimeoutError:
    print("Connection timed out. Cannot connect to "+host)
    s.close()
    raise

print("Connected to "+host+":"+str(port))
# s.settimeout(30)

while True:
    print("Waiting for data from RT server...")
    s.settimeout(330)
    data = get_traffic_data(s)

    print("Congesttion level: " + str(data))
    print("===="*20)

s.close()
