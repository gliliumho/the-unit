#! /usr/bin/python3

import sys
import serial
import socket
import xml.etree.ElementTree as ET
import datetime, time

import threading
import queue
import logging

import configparser

logging.basicConfig(level=logging.DEBUG,
                    format='[%(levelname)s] (%(threadName)-10s) %(message)s',
                    )
class Config:

    def __init__(self):
        self.serialport = '/dev/ttyS4'
        self.trafficservtimeout = 10 #seconds
        self.trafficinterval = 10*60 #minutes
        self.heartbeatinterval = 60*60 #minutes
        self.slaveidlist_file = './idlist.txt'

        conf = configparser.ConfigParser()
        try:
            conf.read('config.ini')

            if 'CUSTOM' in conf:
                customconf = conf['CUSTOM']

                self.serialport = customconf['SerialPort']
                self.trafficservtimeout = int(customconf['TrafficServerTimeout'])
                self.trafficinterval = int(customconf['TrafficDataInterval'])*60
                self.heartbeatinterval = int(customconf['HeartbeatInterval'])*60
                self.slaveidlist_file = customconf['SlaveIDListFile']

            elif 'DEFAULT' in conf:
                customconf = conf['DEFAULT']

                self.serialport = customconf['SerialPort']
                self.trafficservtimeout = int(customconf['TrafficServerTimeout'])
                self.trafficinterval = int(customconf['TrafficDataInterval'])*60
                self.heartbeatinterval = int(customconf['HeartbeatInterval'])*60
                self.slaveidlist_file = customconf['SlaveIDListFile']

        except FileNotFoundError:

            conf['DEFAULT'] = { \
                'SerialPort': self.serialport,
                'TrafficServerTimeout': str(self.trafficservtimeout),
                'TrafficDataInterval': str(self.trafficinterval // 60),
                'HeartbeatInterval': str(self.heartbeatinterval // 60),
                'SlaveIDListFile': self.slaveidlist_file }

            with open(configfilestring,'w') as configfile:
                conf.write(configfile)



    def importconfig(self, configfilestring):
        conf = configparser.ConfigParser()
        try:
            conf.read(configfilestring)

            if 'CUSTOM' in conf:
                customconf = conf['CUSTOM']
                self.serialport = customconf['SerialPort']
                self.trafficservtimeout = int(customconf['TrafficServerTimeout'])
                self.trafficinterval = int(customconf['TrafficDataInterval'])*60
                self.heartbeatinterval = int(customconf['HeartbeatInterval'])*60
                self.slaveidlist_file = customconf['SlaveIDList']
            else:
                raise FileStructureError('Settings need to be in [CUSTOM] section.')

        except FileNotFoundError:
            print("Error: " + configfilestring + "does not exist.")
            raise



def init_serial(port):
    """ Initialize serial port according to platform and
        return the Serial object"""
    try:
        ser_port = serial.Serial(port, 9600)
    except:
        raise SerialPortError("Error opening " + port)

    print("Serial port "+ser_port.name+" opened.")
    return ser_port


def connect_rt_engine(host, port, timeout, queue):
    s = socket.socket()
    s.settimeout(timeout)
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
def get_traffic_data(socket, timeout, queue):

    # queue.put(int(2))
    # return
    try:
        logging.debug("Getting data from " + str(socket.getpeername()))
        socket.settimeout(timeout)
        data = socket.recv(1536)
    except ConnectionResetError:
        # return None
        logging.debug("Connection to RT engine broken")
        queue.put(int(0))
        queue.task_done()
        return
    except ConnectionRefusedError:
        # return None
        logging.debug("Connection refused by server.")
        queue.put(int(0))
        queue.task_done()
        return

    data = data.decode('utf-8')
    try:
        root = ET.fromstring(data)
    except ET.ParseError:
        # print(data)
        # print("Double XML from RT engine")
        queue.put(int(0))
        return


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

    queue.put(int(0))
    queue.task_done()
    return

def send_traffic_data(serialport, pack):
    """ Formats the bytearray(packet) and write to serialport """
    pack[0] = 0x01
    pack[1] = 0x00
    serialport.write(pack)
    logging.debug("Traffic Data - Sent.")
    logging.debug(str(pack))


def getsend_traffic_data(ip_grouplist, serialport, seriallock, timeout, queue):
    traffic_data = [0]*16
    thread_list = []
    # q = queue.Queue()
    for socket, group in ip_grouplist:
        group = int(group)
        t = threading.Thread(
            target=get_traffic_data,
            args=(socket, timeout, queue) )
        t.start()
        thread_list.append(t)
        if group % 2:
            index = ((group + 1) // 2) + 1
            data = queue.get()
            data = (data << 4)
            traffic_data[index] = ((traffic_data[index] & 0x0F) | data)

        else:
            index = (group // 2) + 1
            data = queue.get()
            logging.debug("Group " + str(group) + ': ' + str(data))
            traffic_data[index] = ((traffic_data[index] & 0xF0) | data)

        # logging.debug("Traffic Data - Getting traffic data.")

    for t in thread_list:
        t.join(180)

    logging.debug("Traffic Data - Acquring lock..")
    seriallock.acquire()
    try:
        logging.debug("Traffic Data - Lock Acquired.")
        time.sleep(2)
        send_traffic_data(serialport, traffic_data)
    finally:
        seriallock.release()
        logging.debug("Traffic Data - Lock Released.")
    logging.debug("Traffic Data - Done. Ending thread.")


def traffic_data_thread(ip_grouplist, serialport, seriallock, interval, timeout, queue):
    while True:
        t = threading.Thread(
            target=getsend_traffic_data,
            args=(ip_grouplist, serialport, seriallock, timeout, queue))
        t.start()
        t.join(interval)
        logging.debug("Started TD thread. Sleeping for " + str(interval))
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


def request_heartbeat_loop(serialport, seriallock, idlist_file):
    """ Request heartbeat from slaves in idlist.txt and logs status in
        cli_slave_status.txt """
    #Read list of slave IDs to get heartbeat from
    try:
        idfile = open(idlist_file,'r')
    except FileNotFoundError:
        raise

    idlist = []
    while True:
        idline = idfile.readline()
        #if EOF
        if len(idline) == 0:
            break
        #if line only contains whitespace
        elif idline.isspace():
            continue

        idline_list = idline.split('.')
        #if line starts with '#'
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
        time.sleep(2)
        for i in range(len(idlist)):
            group = idlist[i][0]
            unique = idlist[i][1]
            #get heartbeat from slave
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

    #log slave status
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



def reqhb_thread(serialport, seriallock, interval, idlist_file):
    while True:
        t = threading.Thread(
            target=request_heartbeat_loop,
            args=(serialport, seriallock, idlist_file))
        t.start()
        t.join(interval)
        logging.debug("Started HB thread. Waiting and sleeping for " + str(interval))
        time.sleep(interval)

# -----------------------------------------------------------------------------

if __name__ == "__main__":
    config = Config()

    ser = init_serial(config.serialport)
    serial_lock = threading.Lock()

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
    for ip in rt_iplist:
        t = threading.Thread(
            target=connect_rt_engine,
            args=(ip[0], 9001, config.trafficservtimeout, q) )

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


    t1 = threading.Thread(
        target=traffic_data_thread,
        args=(rt_iplist, ser, serial_lock, config.trafficinterval,
                config.trafficservtimeout, q) )

    t2 = threading.Thread(
        target=reqhb_thread,
        args=(ser, serial_lock, config.heartbeatinterval, config.slaveidlist_file))

    t1.start()
    t2.start()

    t1.join()
    t2.join()

# Configurations
# + timeout for traffic_data
# + interval for traffic_data
# + interval for hearbeat
# + default serial port
# +
