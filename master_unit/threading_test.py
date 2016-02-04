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
    logging.debug("Waiting for lock")
    lock.acquire()
    try:
        logging.debug("Acquired lock")
        time.sleep(random.random())
        logging.debug("Done")
    finally:
        lock.release()

for i in range(10):
    t = threading.Thread(target=print_something, args=(serial_lock,))
    t.start()

logging.debug('Out of loop')
main_thread = threading.currentThread()
for t in threading.enumerate():
    if t is not main_thread:
        t.join()

logging.debug('Done. Out of script')
