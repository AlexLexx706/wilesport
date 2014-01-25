# -*- coding: utf-8 -*-
from serial import Serial
import struct
import threading
import sys

class Port:
    TranssmitPacket = 0
    Echo = 1

    def __init__(self, port, baudrate=57600):
        self.serial = Serial(port=port, baudrate=baudrate, timeout=10)
        self.debug_thread = threading.Thread(target=self.debug_loop)
        self.serial.read()
    
    def debug_loop(self):
        while(1):
            sys.stdout.write(self.serial.read())
    
    def send_data(self, data):
        packet = struct.pack("<BB", self.TranssmitPacket, len(data)) + data
        self.serial.write(packet)
        data = self.serial.read(3)
        type, size, res = struct.unpack("<BBB", data)
        return res

if __name__ == '__main__':
    import time
    p = Port(port="com6", baudrate=115200)

    while(1):
        print p.send_data("11212")
        #time.sleep(5)

    
