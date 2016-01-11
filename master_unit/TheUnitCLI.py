# import socket
# import serial
from custom_unit_test import test


def isInt(s):
    try:
        int(s)
        return True
    except ValueError:
        return False


def MainMenu():
    while True:
        print("1. Send Traffic Info(manual)")
        print("2. Request Heartbeat")
        print("3. Request Heartbeat from All Slaves (broadcast)")
        print("4. Request Heartbeat from All Slaves (loop)")
        input_char = input("Select: ")

        if not isInt(input_char):
            print("WARNING!!! The input must contain numbers only!!", end="\n\n")
            continue
        else:
            input_char = int(input_char)

        if (1 <= input_char <= 4):
            return input_char
        else:
            print("Invalid choice. Please choose from 1 to 4", end="\n\n")



test(MainMenu())
