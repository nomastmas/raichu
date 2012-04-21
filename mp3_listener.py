#!/usr/bin/python

import socket
import fcntl
import struct
import MySQLdb as db
import sys

#function def
def get_ip_address(ifname):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    return socket.inet_ntoa(fcntl.ioctl(
        s.fileno(),
        0x8915,  # SIOCGIFADDR
        struct.pack('256s', ifname[:15])
    )[20:24])

listener = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

#specified for linux vm
host = get_ip_address("eth3")
#school designated ports 33XXX
port = 33334

try:
	listener.bind((host, port))
except socket.error,e:
	print "Error %d: %s" % (e.args[0], e.args[1])
	sys.exit(1)

#TODO multithread this part
listener.listen(5)

try:
	while True:
		conn, addr = listener.accept()
		client_request = conn.recv(1024)
		
		if(len(client_request) > 0):
			conn.send("OK")
		
except socket.error,e:
	print "Error %d: %s" % (e.args[0], e.args[1])

finally:
	if listener:
		listener.close()