#! /usr/bin/python3

import sys
import serial
import socket
import xml.etree.ElementTree as ET
import datetime, time
import threading
import queue


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



def connect_rt_engine(host, port, queue):
    s = socket.socket()
    # if host == '0':
    #     host = socket.gethostbyname(socket.getfqdn())
    try:
        s.connect((host,port))
        print("Connected to "+host+":"+str(port))
    except:
        print("Connection error. Cannot connect to "+host)
        s.close()
        s = None
    # return s
    # sock_element = s
    queue.put(s)


#change to get TCP connection from RTEngines
def get_traffic_data(socket, queue):
    try:
        data = socket.recv(2048)
    except ConnectionResetError:
        print("Connection from server timed out.......")
        # return None
        queue.put(None)
    except:
        print("Unknown error. Cannot receive data from RT Engine")
        # return None
        queue.put(None)

    data = data.decode('utf-8')
    root = ET.fromstring(data)

    for value in root.iter():
        if value.tag == "IncidentType":
            if value.text == "CongestionStart":
                # return 3
                queue.put(3)
            elif value.text == "CongestionEnd":
                # return 2
                queue.put(2)
        elif value.tag == "CongestionLevel":
            # return int(value.text)+1
            queue.put(int(value.text)+1)



def send_traffic_data(serialport, pack):
    """ Formats the bytearray(packet) and write to serialport """
    pack[0] = 0x01
    pack[1] = 0x00
    serialport.write(pack)


def getsend_traffic_data(ip_grouplist, serialport, seriallock):
    traffic_data = [0]*16
    ti_queue = queue.Queue()
    thread_list = []
    for socket, group in ip_grouplist:
        t = threading.Thread(
            target=get_traffic_data,
            args=(socket, ti_queue,))

        t.start()
        thread_list.append(t)
        traffic_data[group] = ti_queue.get()

    for t in thread_list:
        t.join(180)

    seriallock.acquire()
    try:
        send_traffic_data(serialport, traffic_data)
    finally:
        seriallock.release()


def traffic_data_thread(ip_grouplist, serialport, seriallock, interval):
    t = threading.Thread(
        target=getsend_traffic_data,
        args=(ip_grouplist, serialport, seriallock,))

    time.sleep(interval)


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


def request_heartbeat_loop(serialport, seriallock):
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

    seriallock.acquire()
    try:
        for i in range(len(idlist)):
            ids = str(idlist[i][0]) + '.' + str(idlist[i][1])
            ret = request_heartbeat(serialport, idlist[i][0], idlist[i][1])
            if ret == True:
                success_slave += 1
                idlist[i].append('Alive')
            else:
                idlist[i].append('Dead')
    finally:
        seriallock.release()

    datetoday = str(datetime.date.today().strftime("%y%m%d"))

    logfile = open(datetoday+'.log', 'a')
    logfile.write('\n'+datetime.datetime.now().isoformat(' '))
    for i in range(len(idlist)):
        line = str(idlist[i][0])+'.'+str(idlist[i][1])+' \t'+idlist[i][2]+'\n'
        logfile.write(line)
    logfile.close()



def reqhb_thread(serialport, seriallock, interval):
    t = threading.Thread(
        target=request_heartbeat_loop,
        args=(serialport, seriallock,))

    time.sleep(interval)

# -----------------------------------------------------------------------------

ser = init_serial()
serial_lock = threading.Lock()

if ser == False:
    exit()

# Get RT server IP addresses and group number from rtengine_iplist.txt
ipfile = open("rtengine_iplist.txt", 'r')
rt_iplist = []
while True:
    line = ipfile.readline()
    if len(line) == 0:
        break
    line = line.split()
    rt_iplist.append(line)

# Connect to RT server sockets
queue = queue.Queue()
for i, ip in enumerate(rt_iplist):
    t = threading.Thread(target=connect_rt_engine, args=(ip[0], 9001, queue,))
    t.start()
    ip[0] = queue.get()

# Set timeout for threads
main_thread = threading.currentThread()
for t in threading.enumerate():
    if t is not main_thread:
        t.join(10)

# Remove elements without socket from list
rt_iplist = [x for x in rt_iplist if x[0] != None]

# Print socket info
for socket, group in rt_iplist:
    host, port = socket.getpeername()
    print(host + "  group = " + str(group))


while True:
    traffic_data_thread(rt_iplist, ser, serial_lock, 600)
    reqhb_thread(ser, serial_lock, 3600)
