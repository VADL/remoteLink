#!/usr/bin/python
import socket
import serial
import time
import sys
import os

minPacketLength = 6
statusLength    = 19

def main( argv ):
    options = Options()
    if options.parse_args(argv):
        return -1

    port = options.port
    baudRate = options.baudRate
    recvDelay = options.recvDelay
    ip = options.ip
    ip_port = options.ip_port

    try:
        ser = serial.Serial(port, baudRate, timeout=recvDelay)
        sock = socket.socket(socket.AF_INET,
                             socket.SOCK_DGRAM)
    except Exception as error:
        print "ERROR: Couldn't open serial port {} / socket".format(port)
        print error
        return -1
    else:
        with ser:
            while True:
                # receive status
                data = ser.read(statusLength)
                if len(data) == 0:
                    print 'No data received.'
                else:
                    print "Received: " + data
                    sock.sendto(data, (ip, ip_port))
            
class Options:
    """
\t--help                   (to show this help and exit)
\t--port                   <serial port name, e.g. COM3 or /dev/ttyUSB0>
\t--baudRate               <baud rate, e.g. 9600>
\t--ip                     <IP address to send to, e.g. 10.1.10.1>
\t--ip_port                <Port number to send to, e.g. 5555>
\t--recvTimeout            <floating point number of seconds to wait for status packets, e.g. 0.01>
    """
    def __init__(self):
        self.port          = "COM4"      #: string port name
        self.baudRate      = 9600        #: int baud rate
        self.ip            = "127.0.0.1" #: string IP address
        self.ip_port       = 5555        #: int port number
        self.recvDelay     = 0.1         #: float number of seconds between packets

    def parse_args(self, args):
        argind = 1
        while argind < len(args):
            if args[argind] == "--port":
                self.port = args[argind+1]
                argind += 1
            elif args[argind] == '--baudRate':
                self.baudRate = int(args[argind+1])
                argind += 1
            elif args[argind] == "--ip":
                self.ip = args[argind+1]
                argind += 1
            elif args[argind] == '--ip_port':
                self.ip_port = int(args[argind+1])
                argind += 1
            elif args[argind] == '--recvTimeout':
                self.recvTimeout = float(args[argind+1])
                argind += 1
            elif args[argind] == "--help":
                self.print_usage(args[0])
                return -1
            argind += 1
        return 0

    def print_usage(self, name):
        print """Usage:\n{}{}""".format(name, self.__doc__)

if __name__ == "__main__":
    main(sys.argv)

