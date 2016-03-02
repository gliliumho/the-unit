#! /usr/bin/python3

import logging
import random
import threading
import time

logging.basicConfig(level=logging.DEBUG,
                    format='[%(levelname)s] (%(threadName)-10s) %(message)s',
                    )
serial_lock = threading.Lock()

def print_something(lock):
    # threading.Timer(5, print_something).start()

    logging.debug("Waiting for lock")
    lock.acquire()
    try:
        logging.debug("Acquired lock")
        time.sleep(random.randrange(4,7))
        logging.debug("Done")
    finally:
        lock.release()

while True:
    for i in range(2):
        t = threading.Thread(target=print_something, args=(serial_lock,))
        t.start()

    # logging.debug('Out of loop')
    main_thread = threading.currentThread()
    # for t in threading.enumerate():
    #     if t is not main_thread:
    #         t.join(8)

    time.sleep(10)

logging.debug('Done. Out of script')
