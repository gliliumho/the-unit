#!/bin/python3
import sys
import serial
import configparser



class bcolors:
    PINK = '\033[95m'
    BLUE = '\033[94m'
    GREEN = '\033[92m'
    YELLOW = '\033[93m'
    RED = '\033[91m'
    CYAN = '\033[36m'
    RESET = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

class Config:
    d_serialport = '/dev/ttyS4'
    d_trafficservtimeout = 30 #seconds
    d_trafficinterval = 10*60 #minutes
    d_heartbeatinterval = 60*60 #minutes
    d_slaveidlist = './idlist.txt'

    def ___init___(self):
        self.serialport = '/dev/ttyS4'
        self.trafficservtimeout = 30 #seconds
        self.trafficinterval = 10*60 #minutes
        self.heartbeatinterval = 60*60 #minutes
        self.slaveidlist = './idlist.txt'

        conf = configparser.ConfigParser()
        try:
            conf.read('config.ini')

            if 'CUSTOM' in conf:
                customconf = conf['CUSTOM']
                self.serialport = customconf['SerialPort']
                self.trafficservtimeout = int(customconf['TrafficServerTimeout'])
                self.trafficinterval = int(customconf['TrafficDataInterval'])*60
                self.heartbeatinterval = int(customconf['HeartbeatInterval'])*60
                self.slaveidlist = customconf['SlaveIDList']
            elif 'DEFAULT' in conf:
                customconf = conf['DEFAULT']
                self.serialport = customconf['SerialPort']
                self.trafficservtimeout = int(customconf['TrafficServerTimeout'])
                self.trafficinterval = int(customconf['TrafficDataInterval'])*60
                self.heartbeatinterval = int(customconf['HeartbeatInterval'])*60
                self.slaveidlist = customconf['SlaveIDList']

        except FileNotFoundError:
            self.serialport = self.d_serialport
            self.trafficservtimeout = self.d_trafficservtimeout
            self.trafficinterval = self.d_trafficinterval
            self.heartbeatinterval = self.d_heartbeatinterval
            self.slaveidlist = self.d_slaveidlist

            conf['DEFAULT'] = { 'SerialPort': self.serialport,
                                'TrafficServerTimeout': str(self.trafficservtimeout),
                                'TrafficDataInterval': str(self.trafficinterval // 60),
                                'HeartbeatInterval': str(self.heartbeatinterval // 60),
                                'SlaveIDList': self.slaveidlist }
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
                self.slaveidlist = customconf['SlaveIDList']
            else:
                raise FileStructureError('Settings need to be in [CUSTOM] section.')

        except FileNotFoundError:
            print("Error: " + configfilestring + "does not exist.")
            raise


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
            port = '/dev/ttyS4'
        elif platform == "linux":
            port = '/dev/ttymxc2'
        else:
            print("Unknown platform...")
            port = '/dev/ttyS2'

    ser_port = serial.Serial(port, 9600)
    print("Serial port "+ser_port.name+" opened.")
    return ser_port


def is_int(s):
    """ Function to check if s can be converted to int """
    try:
        int(s)
        return True
    except ValueError:
        return False


# need change
def main_menu():
    """ Prints menu and returns user input (ranging from 0 to 4) """
    while True:
        print(bcolors.PINK + "\n" + "="*80 + bcolors.RESET)
        print(" "*30 + "The Unit Menu")
        print(bcolors.PINK + "="*80 + bcolors.RESET)
        print("1. Send Traffic Info(manual)")
        print("2. Request Heartbeat")
        print("3. Nothing here....")
        print("4. Request Heartbeat from idlist.txt")
        print("0. Exit")
        input_char = input("Select: ")

        if is_int(input_char):
            input_char = int(input_char)
        else:
            print(bcolors.YELLOW + \
                "ERROR: INPUT MUST CONTAIN NUMBER ONLY" + \
                bcolors.RESET, end="\n\n")
            continue

        if (0 <= input_char <= 4):
            return input_char
        else:
            print(bcolors.YELLOW + \
                "ERROR: INPUT MUST BE FROM 0 TO 4" + \
                bcolors.RESET, end="\n\n")

# change to TCP or manual
def get_traffic(pack):
    for i in range(len(pack)):
        if 2 <= i <= 5:
            pack[i] = int(input("Traffic info for group "+str(i-1)+": "))
        elif i == 15:
            pack[i] = 0x0a
        else:
            pack[i] = 4


def send_traffic(serialport, pack):
    """ Formats the bytearray(packet) and write to serialport """
    pack[0] = 0x01
    pack[1] = 0x00
    # print(pack)
    serialport.write(pack)


def menu_get_heartbeat(serialport):
    line = input("Enter GroupID.UniqueID: ")
    ids = line.split('.')
    gid = int(ids[0])
    uid = int(ids[1])

    print("Requesting heartbeat from "+str(gid)+'.'+str(uid))
    ret = request_heartbeat(ser, gid, uid)
    if ret == True:
        print(bcolors.GREEN + \
            "Slave " + str(gid) + '.' + str(uid) + " is alive" + bcolors.RESET)
    else:
        print(bcolors.YELLOW + \
            "No reply from slave " + str(gid) + '.' + str(uid) + bcolors.RESET)
        #return False
    # else:
    #     print("timeout.."+str(i))


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


def request_heartbeat_broadcast():
    print(bcolors.RED + \
        "Feature not implemented yet. Please contact the developer." + \
        bcolors.RESET)
    return 0


def request_heartbeat_loop(serialport):
    """ Request heartbeat from slaves in idlist.txt and logs status
        in cli_slave_status.txt """
    idfile = open("idlist.txt",'r')
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

    total_slave = 0
    success_slave = 0

    for i in range(len(idlist)):
        total_slave += 1
        group = idlist[i][0]
        unique = idlist[i][1]

        print("Slave " + str(group) + '.' + str(unique) + '\t: ', end='')
        ret = request_heartbeat(serialport, group, unique)

        ids = str(group) + '.' + str(unique)
        if ret == True:
            success_slave += 1
            print(bcolors.GREEN + "Alive" + bcolors.RESET)
            idlist[i].append('Alive')
        else:
            print(bcolors.RED + "Dead" + bcolors.RESET)
            idlist[i].append('Dead')

    print(bcolors.BLUE + "Summary: " + \
        str(success_slave) + '/' + str(total_slave) + " slaves alive" + bcolors.RESET)

    logfile = open("cli_slave_status.log", 'w')

    for i in range(len(idlist)):
        line = str(idlist[i][0])+'.'+str(idlist[i][1])+' \t'+idlist[i][2]+'\n'
        logfile.write(line)

    logfile.close()


# ------------------------------------------------------------------------------


# check for arguments
# -h or --help will print help

# config
# --serial-port <port>
# --traffic-server-timeout <timeout>
# --restore-default
# --traffic-data-interval
# --heartbeat-interval
# --slave-idlist
# --print-config

# --print-serial will display default/selected serial port
# --heartbeart filename.output will log the heartbeat status into file


print("Starting..")


ser = init_serial()

#Infinite loop for the CLI menu
while True:
    # Print main menu
    userinput = main_menu()

    if userinput == 0:      # exit
        print("Exiting The Unit CLI...")
        break
    elif userinput == 1:    # send traffic indo
        print(bcolors.BOLD + "=====Send traffic info=====" + bcolors.RESET)
        pack = bytearray(16)
        get_traffic(pack)
        send_traffic(ser, pack)

    elif userinput == 2:    # request heartbeat
        print(bcolors.BOLD + "=====Request Heartbeat=====" + bcolors.RESET)
        menu_get_heartbeat(ser)

    elif userinput == 3:    # request all heartbeat (broadcast)
        """
        -serial code to request heartbeat broadcasting & log the slaves alive
        -may compare to idlist.txt to see which slave is alive/dead
        """
        request_heartbeat_broadcast()

    elif userinput == 4:
        print(bcolors.BOLD + \
            "=====Request Heartbeat (Loop)=====" + bcolors.RESET)
        request_heartbeat_loop(ser)
