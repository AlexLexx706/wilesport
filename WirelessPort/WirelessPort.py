# -*- coding: utf-8 -*-
from serial import Serial
import struct
import threading
import sys

class Port:
    TranssmitPacket = 0
    Echo = 1

    def __init__(self, port="com8", baudrate=115200):
        self.serial = Serial(port=port, baudrate=baudrate, timeout=10)
        self.serial.read()
    
    def write(self, data):
        header = struct.pack("<BB", self.TranssmitPacket, len(data))
        self.serial.write(header + data)
        type, size, res = struct.unpack("<BBB", self.serial.read(3))
        return res

    def echo(self, data):
        header = struct.pack("<BB", self.Echo, len(data))
        self.serial.write(header + data)
        type, size = struct.unpack("<BB", self.serial.read(2))
        return self.serial.read(size)
        

if __name__ == '__main__':
    import time
    p = Port(port="com6")

    data = "".join([str(i) for i in range(250)])
    data = "x"*64
    size = 0
    start_time = time.time()
    error_count = 0
    
    while(1):
        res = p.write(data)
        if res:
            size = size + len(data)
        else:
            error_count = error_count + 1
  
        t = time.time()

        if t > start_time + 5:
            print "speed:", size/(t - start_time)
            print "errors:", error_count
            start_time = t
            size = 0
            error_count = 0