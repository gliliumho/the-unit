
import sys
import serial
from custom_unit_test import test


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
        print("1. Send Traffic Info(manual)")
        print("2. Request Heartbeat")
        print("3. Request Heartbeat from All Slaves (broadcast) - EXPERIMENTAL")
        print("4. Request Heartbeat from All Slaves (loop)")
        print("0. Exit")
        input_char = input("Select: ")

        if not isInt(input_char):
            print("ERROR: INPUT MUST CONTAIN NUMBER ONLY", end="\n\n")
            continue
        else:
            input_char = int(input_char)

        if (0 <= input_char <= 4):
            return input_char
        else:
            print("ERROR: INPUT MUST BE FROM 0 TO 4", end="\n\n")


def send_traffic(serialport, packet):
    """Formats the bytearray(packet) and write to serialport"""
    packet[0] = 1
    packet[1] = 0

    serialport.write(packet)

def request_heartbeat(serialport, gid, uid):
    """Sends a bytearray for masterRF to request heartbeat"""
    pack = bytearray(5)
    pack[0] = 2
    pack[1] = gid
    pack[2] = uid
    pack[3] = '\n'

    serialport.write(packet)

    # add code to wait for heartbeat & status

def request_heartbeat_broadcast():


def request_heartbeat_loop():



# ------------------------------------------------------------------------------
print("\nStarting The Unit CLI")
print("====================")
platform = sys.platform
if platform == "win32":
    ser = serial.Serial('COM3', 9600)
elif platform == "cygwin":
    ser = serial.Serial('/dev/ttyS2', 9600)
elif platform == "linux":
    ser = serial.Serial('/dev/ttymxc2', 9600)
else:
    print("Unknown platform...")
    ser = serial.Serial('/dev/ttyS2', 9600)

print("Serial port "+ser.name+" opened.")





#Infinite loop for the CLI menu
while True:
    userinput = main_menu()
    if userinput == 0:      # exit
        print("Exiting The Unit CLI...")
        break
    elif userinput == 1:    # send traffic indo
        print("=====Send traffic info=====")
        pack = bytearray(16)
        for (i, value) in enumerate(pack):
            if 2 <= i <= 14:
                value = input("Traffic info for group "+(i-1))
            elif i == 15:
                value = '\n'

        send_traffic(ser, pack)

    elif userinput == 2:    # request heartbeat
        print("=====Request Heartbeat=====")
        gid = input("Group ID: ")
        uid = input("Unique ID: ")
        request_heartbeat(ser, gid, uid)

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
