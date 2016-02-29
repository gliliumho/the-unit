#!/bin/python3
import sys
import serial
import curses


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


# def init_tcp_conn():
#


def init_menuscreen(stdscr, menuscr, clear_stdscr=0, clear_menu=0):
    if clear_stdscr == 1:
        stdscr.clear()
        stdscr.move(0,0)
        stdscr.addstr("="*160)
        stdscr.addstr(2,30, "The Unit CLI Menu\n")
        stdscr.addstr("="*160)
        stdscr.refresh()

    if clear_menu == 1:
        menuscr.clear()
        menuscr.move(0,0)
        menuscr.addstr("1. Send Traffic Info(manual)\n")
        menuscr.addstr("2. Request Heartbeat\n")
        menuscr.addstr("3. Request All Heartbeat (broadcast) - EXPERIMENTAL\n")
        menuscr.addstr("4. Request All Heartbeat (loop)\n")
        menuscr.addstr("q. Exit\n\n")
        menuscr.addstr("Select: ")
        menuscr.refresh()


def main_menu(stdscr, menuscr):
    """ Prints menu and returns user input (ranging from 0 to 4) """
    init_menuscreen(stdscr, menuscr, 0, 1)
    while True:
        menuscr.move(6,8)
        c = menuscr.getch()
        c = chr(c)
        if ('1' <= c <= '4') or (c == 'q'):
            init_menuscreen(stdscr, menuscr, 1, 1)
            return c
        else:
            menuscr.addstr(5,0, "ERROR: INPUT MUST CONTAIN NUMBER ONLY")
            menuscr.refresh()


def get_traffic(infowin, pack):
    for i in range(len(pack)):
        if 2 <= i <= 5:
            infowin.addstr(i-2,0, "Traffic info for group "+str(i-1)+": ")
            c = chr(infowin.getch())
            pack[i] = int(c)
        elif i == 15:
            pack[i] = 0x0a
        else:
            pack[i] = 4


def send_traffic(serialport, pack):
    """ Formats the bytearray(packet) and write to serialport """
    pack[0] = 0x01
    pack[1] = 0x00
    serialport.write(pack)


def menu_get_heartbeat(infowin, serialport):
    infowin.addstr(0,0, "Enter GroupID.UniqueID (eg. 3.4): ")
    infowin.refresh()
    # s = infowin.getstr()
    gid = int(infowin.getkey())
    infowin.addch('.')
    uid = int(infowin.getkey())
    s = str(gid) + '.' + str(uid)

    infowin.addstr(1,0,"Requesting heartbeat from "+s+'\n')
    infowin.refresh()
    ret = request_heartbeat(serialport, gid, uid)
    if ret == True:
        infowin.addstr("Slave "+s+" is alive\n")
    else:
        infowin.addstr("No reply from slave "+s+"\n")
    infowin.refresh()


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


def request_heartbeat_broadcast(infowin):
    infowin.addstr(0,0, "Feature not implemented yet. Please contact dev.")
    infowin.refresh()
    return False


def request_heartbeat_loop(infowin, serialport):
    """ Request heartbeat from slaves in idlist.txt and logs status
        in cli_slave_status.txt """
    idfile = open("idlist.txt",'r')
    idlist = []
    while True:
        idline = idfile.readline()
        if len(idline) == 0:
            break
        #if line only contains whitespace
        elif idline.isspace():
            continue

        idline_list = idline.split('.')
        #if line starts with '#'
        if '#' in idline_list[0]:
            continue

        idline_list = idline.split('.')
        idline_list = list(map(int, idline_list))
        idlist.append( idline_list )
    idfile.close()

    win_yx = infowin.getmaxyx()
    success_slave = 0
    infowin.idlok(1)
    infowin.scrollok(True)

    for i in range(len(idlist)):
        ids = str(idlist[i][0]) + '.' + str(idlist[i][1])
        if i < win_yx[0]:
            row = i
        else:
            row = win_yx[0]-1
            # infowin.scroll()
        infowin.addstr(row, 0, "Slave " + ids + '\t: ')
        infowin.refresh()
        # print("Slave " + ids + '\t: ', end='')
        ret = request_heartbeat(serialport, idlist[i][0], idlist[i][1])
        if ret == True:
            success_slave += 1
            infowin.addstr("Alive\n")
            infowin.refresh()
            idlist[i].append('Alive')
        else:
            infowin.addstr("Dead\n")
            infowin.refresh()
            idlist[i].append('Dead')

    # infowin.move(win_yx[0]-1,0)
    # infowin.scroll(2)
    infowin.addstr("Summary: " + \
        str(success_slave) + '/' + str(len(idlist)) + " slaves alive")
    infowin.refresh()
    logfile = open("./log/cli_slave_status.log", 'w')
    for i in range(len(idlist)):
        line = str(idlist[i][0])+'.'+str(idlist[i][1])+' \t'+idlist[i][2]+'\n'
        logfile.write(line)
    logfile.close()


# ------------------------------------------------------------------------------

def main():
    #from custom_unit_test import test

    print("\nStarting The Unit CLI..")

    ser = init_serial()
    if ser == False:
        exit()
    stdscr = curses.initscr()
    curses.cbreak()
    menu = curses.newwin(7, 60, 6, 10)
    infowin = curses.newwin(15,60,17,10)

    init_menuscreen(stdscr, menu,1,1)

    #Infinite loop for the CLI menu
    while True:
        # Print main menu
        userinput = main_menu(stdscr, menu)
        infowin.clear()
        if userinput == 'q':      # exit
            break

        elif userinput == '1':    # send traffic info
            stdscr.move(14,0)
            stdscr.addstr("-"*80)
            stdscr.addstr(15,31,"Send traffic info \n")
            stdscr.addstr("-"*80)
            stdscr.refresh()
            pack = bytearray(16)
            get_traffic(infowin, pack)
            send_traffic(ser, pack)

        elif userinput == '2':    # request heartbeat
            stdscr.move(14,0)
            stdscr.addstr("-"*80)
            stdscr.addstr(15,31,"Request Heartbeat \n")
            stdscr.addstr("-"*80)
            stdscr.refresh()
            menu_get_heartbeat(infowin, ser)

        elif userinput == '3':    # request all heartbeat (broadcast)
            stdscr.move(14,0)
            stdscr.addstr("-"*80)
            stdscr.addstr(15,11,"Request All Heartbeats (broadcast) - EXPERIMENTAL\n")
            stdscr.addstr("-"*80)
            stdscr.refresh()
            request_heartbeat_broadcast(infowin)

        elif userinput == '4':
            stdscr.move(14,0)
            stdscr.addstr("-"*80)
            stdscr.addstr(15,26,"Request All Heartbeats (loop)\n")
            stdscr.addstr("-"*80)
            stdscr.refresh()
            request_heartbeat_loop(infowin, ser)

    curses.nocbreak()
    curses.endwin()
    print("Exiting The Unit CLI...")

if __name__ == "__main__":
    main()
