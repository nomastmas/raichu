#!/usr/bin/python

# server.py
# this denotes the class definition of the raichu server

import platform
import socket
import fcntl
import struct
#import MySQLdb as db
import sys
from threading import Thread
from threading import Lock
import thread
import datetime as dt
import time as t

def get_ip_address(ifname):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    return socket.inet_ntoa(fcntl.ioctl(
        s.fileno(),
        0x8915,  # SIOCGIFADDR
        struct.pack('256s', ifname[:15])
    )[20:24])

def print_error(error):
	print "Error %d: %s" % (error.args[0], error.args[1])

def get_timestamp():
	return dt.datetime.fromtimestamp(int(t.time())).strftime('%Y-%m-%d %H:%M:%S')

class raichu_server:

	def __init__(self, port=3333):

		self.port = port
		server_platform = platform.system()
		if server_platform == "Linux":
			self.host = get_ip_address("eth0")
		elif server_platform == "Darwin":
			self.host = socket.gethostbyname(socket.gethostname())

	def start(self):

		self.listener = socket.socket (socket.AF_INET, socket.SOCK_STREAM)

		try:
			self.listener.bind ((self.host, self.port))
		except socket.error, e:
			print_error(e)
			sys.exit(1)

		self.listener.listen(5)

		print "----------RAICHU SYSTEM ONLINE----------"
		print "%s:%s" % (self.host, self.port)
		print "listening for devices..."

		try:
			while True:
				conn, addr = self.listener.accept()
				timestamp = get_timestamp()
				print "==========<%s>==========" % timestamp
				print "%s:%s connected" %  (addr[0], addr[1])
				print "========================================="
				#thread.start_new_thread(conn_handler, (conn, addr))

		except socket.error,e:
			print_error(e)
		finally:
			if self.listener:
				self.listener.close()

	def close(self):
		self.listener.close()

	def handle_connection(self):
		pass
#end function def

# main
if __name__ == "__main__":
	#port = sys.argv[1]
	s = raichu_server()
	s.start()
