#!/bin/python3

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'


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


def main_menu():
    """ Prints menu and returns user input (ranging from 0 to 4) """
    while True:
        print(bcolors.HEADER + "\n" + "="*30 + bcolors.ENDC)
        print("1. Send Traffic Info(manual)")
        print("2. Request Heartbeat")
        print("3. Request Heartbeat from All Slaves (broadcast) - EXPERIMENTAL")
        print("4. Request Heartbeat from All Slaves (loop)")
        print("0. Exit")
        input_char = input("Select: ")

        if is_int(input_char):
            input_char = int(input_char)
        else:
            print(bcolors.WARNING + \
                "ERROR: INPUT MUST CONTAIN NUMBER ONLY" + \
                bcolors.ENDC, end="\n\n")
            continue

        if (0 <= input_char <= 4):
            return input_char
        else:
            print(bcolors.WARNING + \
                "ERROR: INPUT MUST BE FROM 0 TO 4" + \
                bcolors.ENDC, end="\n\n")


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
        print(bcolors.OKGREEN + \
            "Slave " + str(gid) + '.' + str(uid) + " is alive" + bcolors.ENDC)
    else:
        print(bcolors.WARNING + \
            "No reply from slave " + str(gid) + '.' + str(uid) + bcolors.ENDC)
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
    print(bcolors.FAIL + \
        "Feature not implemented yet. Please contact the developer." + \
        bcolors.ENDC)
    return 0


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

    total_slave = 0
    success_slave = 0

    for i in range(len(idlist)):
        total_slave += 1
        print("Slave " + str(idlist[i][0]) + '.' + str(idlist[i][1]) + \
            ': ', end='')
        ret = request_heartbeat(serialport, idlist[i][0], idlist[i][1])
        if ret == True:
            success_slave += 1
            print(bcolors.OKGREEN + "Alive" + bcolors.ENDC)
            idlist[i].append('Alive')
        else:
            print(bcolors.FAIL + "Dead" + bcolors.ENDC)
            idlist[i].append('Dead')

    print(bcolors.OKBLUE + "Summary: " + \
        str(success_slave) + '/' + str(total_slave) + " slaves alive" + bcolors.ENDC)

    logfile = open("cli_slave_status.log", 'w')

    for i in range(len(idlist)):
        line = str(idlist[i][0])+'.'+str(idlist[i][1])+' \t'+idlist[i][2]+'\n'
        logfile.write(line)

    logfile.close()


# ------------------------------------------------------------------------------
import sys
import serial
#from custom_unit_test import test

print("\nStarting The Unit CLI")

ser = init_serial()

#Infinite loop for the CLI menu
while True:
    # Print main menu
    userinput = main_menu()

    if userinput == 0:      # exit
        print(bcolors.BOLD + "Exiting The Unit CLI..." + bcolors.ENDC)
        break
    elif userinput == 1:    # send traffic indo
        print(bcolors.BOLD + "=====Send traffic info=====" + bcolors.ENDC)
        pack = bytearray(16)
        get_traffic(pack)
        send_traffic(ser, pack)

    elif userinput == 2:    # request heartbeat
        print(bcolors.BOLD + "=====Request Heartbeat=====" + bcolors.ENDC)
        menu_get_heartbeat(ser)

    elif userinput == 3:    # request all heartbeat (broadcast)
        """
        -serial code to request heartbeat broadcasting & log the slaves alive
        -may compare to idlist.txt to see which slave is alive/dead
        """
        request_heartbeat_broadcast()

    elif userinput == 4:
        print(bcolors.BOLD + \
            "=====Request Heartbeat (Loop)=====" + bcolors.ENDC)
        request_heartbeat_loop(ser)
