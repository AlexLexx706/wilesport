# -*- coding: utf-8 -*-
from serial import Serial
import struct
import threading
import sys

class Port:
    TranssmitPacket = 0
    Echo = 1

    def __init__(self, port="com8", baudrate=57600):
        self.serial = Serial(port=port, baudrate=baudrate)
        self.debug_thread = threading.Thread(target=self.debug_loop)
        self.debug_thread.start()
    
    def debug_loop(self):
        while(1):
            sys.stdout.write(self.serial.read())
    
    def write(self, data):
        header = struct.pack("<BB", len(data) + 1, self.TranssmitPacket)
        self.serial.write(header + data)

if __name__ == '__main__':
    import time
    p = Port(port="com6")

    while(1):
        p.write("1"*100)
        #time.sleep(5)

    
