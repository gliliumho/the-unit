#!/bin/python3

def init_serial():
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
    """Function to check  """
    try:
        int(s)
        return True
    except ValueError:
        return False


def main_menu():
    """Prints menu and returns user input (ranging from 0 to 4) """
    while True:
        print("\n1. Send Traffic Info(manual)")
        print("2. Request Heartbeat")
        print("3. Request Heartbeat from All Slaves (broadcast) - EXPERIMENTAL")
        print("4. Request Heartbeat from All Slaves (loop)")
        print("0. Exit")
        input_char = input("Select: ")

        if not is_int(input_char):
            print("ERROR: INPUT MUST CONTAIN NUMBER ONLY", end="\n\n")
            continue
        else:
            input_char = int(input_char)

        if (0 <= input_char <= 4):
            return input_char
        else:
            print("ERROR: INPUT MUST BE FROM 0 TO 4", end="\n\n")


def send_traffic(serialport, pack):
    """Formats the bytearray(packet) and write to serialport"""
    pack[0] = 0x01
    pack[1] = 0x00
    print(pack)
    serialport.write(pack)


def request_heartbeat(serialport, gid, uid):
    """Sends a bytearray for masterRF to request heartbeat"""
    pack = bytearray(4)
    pack[0] = 2
    pack[1] = gid
    pack[2] = uid
    pack[3] = 0x0a

    serialport.write(pack)
    print("written to UART")
    print("waiting response")
    line = serialport.read(5)
    if line[3] == 1:
        return True
    else:
        return False


def request_heartbeat_broadcast():
    return 0


def request_heartbeat_loop():
    """Request heartbeat from slaves in idlist.txt and logs status in
    cli_slave_status.txt """

    #idlist = open("idlist.txt",'r')

# ------------------------------------------------------------------------------
import sys
import serial
#from custom_unit_test import test

print("\nStarting The Unit CLI")
print("====================")

ser = init_serial()

#Infinite loop for the CLI menu
while True:
    # Print main menu
    userinput = main_menu()

    if userinput == 0:      # exit
        print("Exiting The Unit CLI...")
        break
    elif userinput == 1:    # send traffic indo
        print("=====Send traffic info=====")
        pack = bytearray(16)
        for i in range(len(pack)):
            if 2 <= i <= 5:
                pack[i] = int(input("Traffic info for group "+str(i-1)+": "))
            elif i == 15:
                pack[i] = 0x0a
            else:
                pack[i] = 4

        send_traffic(ser, pack)

    elif userinput == 2:    # request heartbeat
        print("=====Request Heartbeat=====")
        gid = int(input("Group ID: "))
        uid = int(input("Unique ID: "))

        print("Requesting heartbeat from "+str(gid)+'.'+str(uid))
        ret = 0
        for i in range(5):
            ret = request_heartbeat(ser, gid, uid)
            if ret == True:
                    print("Slave "+str(gid)+'.'+str(uid)+" is alive")
                    break
            else:
                if i < 4:
                    print("timeout.."+str(i))
                else:
                    print("No reply from slave "+str(gid)+'.'+str(uid))


    elif userinput == 3:    # request all heartbeat (broadcast)
        """
        -serial code to request heartbeat broadcasting & log the slaves alive
        -may compare to idlist.txt to see which slave is alive/dead
        """
        request_heartbeat_broadcast()

    elif userinput == 4:
        """
        -request heartbeat from slaves is idlist.txt
        -the looping part should be from....master
        """
        request_heartbeat_loop()
