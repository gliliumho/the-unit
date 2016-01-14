
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


def send_traffic():


def request_heartbeat():


def request_heartbeat_broadcast():


def request_heartbeat_loop():



# ------------------------------------------------------------------------------

platform = sys.platform
if platform == "win32":
    # stuff for WINDOWS
else:
    # stuff for Linux/Cygwin



while True:
    userinput = main_menu()
    if userinput == 0:
        print("Exiting The Unit CLI...")
        break
    elif userinput == 1:
        """
        get traffic from traffic server through TCP/IP and send to masterRF
        using serial
        """
        send_traffic()

    elif userinput == 2:
        """
        prompt input for gid & uid then request heartbeat from specified slave
        """
        request_heartbeat()

    elif userinput == 3:
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
