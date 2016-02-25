#! /usr/bin/python3

import sys
import serial
import socket
import xml.etree.ElementTree as ET
import datetime, time
import threading
import queue
import logging

logging.basicConfig(level=logging.DEBUG,
                    format='[%(levelname)s] (%(threadName)-10s) %(message)s',
                    )


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
    queue.task_done()


#change to get TCP connection from RTEngines
def get_traffic_data(socket, queue):
    try:
        logging.debug("Getting data from " + socket.getpeername())
        data = socket.recv(2048)
    except ConnectionResetError:
        # return None
        logging.debug("Connection from server timed out.......")
        queue.put(int(0))
        queue.task_done()
        return
    except:
        # return None
        logging.debug("Unknown error. Cannot receive data from RT Engine")
        queue.put(int(0))
        queue.task_done()
        return

    data = data.decode('utf-8')
    root = ET.fromstring(data)

    for value in root.iter():
        if value.tag == "IncidentType":
            if value.text == "CongestionStart":
                logging.debug("Congested..")
                queue.put(int(3))
                queue.task_done()
                return
            elif value.text == "CongestionEnd":
                logging.debug("End of congestion..")
                queue.put(int(2))
                queue.task_done()
                return
        elif value.tag == "CongestionLevel":
            # return int(value.text)+1
            logging.debug("Traffic level: " + str(int(value.text)+1))
            queue.put(int(value.text)+1)
            queue.task_done()
            return


def send_traffic_data(serialport, pack):
    """ Formats the bytearray(packet) and write to serialport """
    pack[0] = 0x01
    pack[1] = 0x00
    serialport.write(pack)


def getsend_traffic_data(ip_grouplist, serialport, seriallock, queue):
    traffic_data = [0]*16
    thread_list = []
    # q = queue.Queue()
    for socket, group in ip_grouplist:
        group = int(group)
        t = threading.Thread(
            target=get_traffic_data,
            args=(socket, queue,))
        t.start()
        thread_list.append(t)
        traffic_data[group] = queue.get()
        logging.debug("Group " + str(group) + ': ' + str(traffic_data[group]))

    for t in thread_list:
        t.join(180)

    logging.debug("Traffic Data - Acquring lock..")
    seriallock.acquire()
    try:
        logging.debug("Traffic Data - Lock Acquired.")
        send_traffic_data(serialport, traffic_data)
    finally:
        seriallock.release()
        logging.debug("Traffic Data - Lock Released.")
    logging.debug("Traffic Data - Done. Ending thread.")


def traffic_data_thread(ip_grouplist, serialport, seriallock, interval, queue):
    t = threading.Thread(
        target=getsend_traffic_data,
        args=(ip_grouplist, serialport, seriallock, queue))
    t.start()
    logging.debug("Started TD thread. Waiting and sleeping for " + str(interval))
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
        elif idline.isspace():
            continue

        idline_list = idline.split('.')

        if '#' in idline_list[0]:
            continue

        idline_list = list(map(int, idline_list))
        idlist.append( idline_list )

    idfile.close()
    success_slave = 0

    logging.debug("Heartbeat - Acquiring lock..")
    seriallock.acquire()
    try:
        logging.debug("Heartbeat - Lock Acquired..")
        for i in range(len(idlist)):
            group = idlist[i][0]
            unique = idlist[i][1]
            ret = request_heartbeat(serialport, group, unique)

            ids = str(group) + '.' + str(unique)
            if ret == True:
                success_slave += 1
                idlist[i].append('Alive')
                logging.debug(ids + " alive")
            else:
                idlist[i].append('Dead')
                logging.debug(ids + " dead")
    finally:
        seriallock.release()
        logging.debug("Heartbeat - Released lock..")

    datetoday = str(datetime.date.today().strftime("%y%m%d"))
    datetoday = datetoday + '.log'
    logging.debug("Logging to " + datetoday)

    logfile = open('./log/'+ datetoday, 'a')
    logfile.write('\n'+datetime.datetime.now().isoformat(' ') + '\n')
    for i in range(len(idlist)):
        line = str(idlist[i][0])+'.'+str(idlist[i][1])+' \t'+idlist[i][2]+'\n'
        logfile.write(line)
    logfile.close()
    logging.debug("Heartbeat - Done. Ending thread.")



def reqhb_thread(serialport, seriallock, interval):
    t = threading.Thread(
        target=request_heartbeat_loop,
        args=(serialport, seriallock,))
    t.start()
    logging.debug("Started HB thread. Waiting and sleeping for " + str(interval))
    time.sleep(interval)

# -----------------------------------------------------------------------------

if __name__ == "__main__":
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
        elif line.isspace():
            continue
        line = line.split()
        if '#' in line[0]:
            continue
        rt_iplist.append(line)

    # Connect to RT server sockets
    q = queue.Queue()
    for i, ip in enumerate(rt_iplist):
        t = threading.Thread(target=connect_rt_engine, args=(ip[0], 9001, q,))
        t.start()
        ip[0] = q.get()
    # del queue

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
        logging.debug("In loop now.")
        t1 = threading.Thread(
            target=traffic_data_thread,
            args=(rt_iplist, ser, serial_lock, 300,q))

        t2 = threading.Thread(
            target=reqhb_thread,
            args=(ser, serial_lock, 360,))

        t1.start()
        t2.start()
        t1.join(300)
        t2.join(360)


# Configurations
# + timeout for traffic_data
# + interval for traffic_data
# + interval for hearbeat
