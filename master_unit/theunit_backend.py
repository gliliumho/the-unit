#! /usr/bin/python3

import sys
import serial
import socket
import xml.etree.ElementTree as ET
import datetime
import threading


def init_serial():
    """ Initialize serial port according to platform and
        return the Serial object"""
    if len(sys.argv) > 1:
        port = "/dev/"+sys.argv[1]
    else:
        platform = sys.platform
        if platform == "win32":
            port = 'COM3'
        elif platform == "cygwin":
            port = '/dev/ttyS2'
        elif platform == "linux":
            port = '/dev/ttymxc2'
        else:
            print("Unknown platform...")
            port = '/dev/ttyS2'

    try:
        ser_port = serial.Serial(port, 9600)
    except:
        print("Error opening ", str(port) )
        return False
    print("Serial port "+ser_port.name+" opened.")
    return ser_port


def connect_rt_engine(host='0', port=9001):
    s = socket.socket()
    if host == '0':
        host = socket.gethostbyname(socket.getfqdn())
    try:
        s.connect((host,port))
        print("Connected to "+host+":"+str(port))
    except OSError:
        s.close()
        s = None
    return s


#change to get TCP connection from RTEngines
def get_traffic_data(socket):

    try:
        data = socket.recv(2048)
    except ConnectionResetError:
        print("Connection from server timed out.......")
        return None
    except:
        print("Unknown error. Cannot receive data from RT Engine")
        return None

    data = data.decode('utf-8')

    root = ET.fromstring(data)

    for value in root.iter():
        if value.tag == "IncidentType":
            if value.text == "CongestionStart":
                return 3
            elif value.text == "CongestionEnd":
                return 2
        elif value.tag == "CongestionLevel":
            return int(value.text)+1


#needs lock
def send_traffic(serialport, pack):
    """ Formats the bytearray(packet) and write to serialport """
    pack[0] = 0x01
    pack[1] = 0x00
    serialport.write(pack)


#needs lock
def request_heartbeat(serialport, gid, uid):
    """ Sends a bytearray for masterRF to request heartbeat """
    pack = bytearray(16)
    pack[0] = 0x02
    pack[1] = gid
    pack[2] = uid
    pack[3] = 0x00

    for i in range(5):
        serialport.write(pack)
        line = serialport.read(16)
        if line[3] == 1:
            return True

    return False


#call this periodically
#need lock
def request_heartbeat_loop(serialport):
    """ Request heartbeat from slaves in idlist.txt and logs status
        in cli_slave_status.txt """
    idfile = open("idlist.txt",'r')
    idlist = []
    while True:
        idline = idfile.readline()
        if len(idline) == 0:
            break
        idline_list = idline.split('.')
        idline_list = list(map(int, idline_list))
        idlist.append( idline_list )
    idfile.close()

    success_slave = 0

    for i in range(len(idlist)):
        ids = str(idlist[i][0]) + '.' + str(idlist[i][1])
        # print("Slave " + ids + '\t: ', end='')
        ret = request_heartbeat(serialport, idlist[i][0], idlist[i][1])
        if ret == True:
            success_slave += 1
            idlist[i].append('Alive')
        else:
            idlist[i].append('Dead')

    logfile = open("cli_slave_status.log", 'a')
    logfile.write('\n'+datetime.datetime.now().isoformat(' '))
    for i in range(len(idlist)):
        line = str(idlist[i][0])+'.'+str(idlist[i][1])+' \t'+idlist[i][2]+'\n'
        logfile.write(line)
    logfile.close()


# -----------------------------------------------------------------------------
