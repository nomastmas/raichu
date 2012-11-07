#!/usr/bin/python

# server.py
# this denotes the class definition of the raichu server

import platform
import socket
import fcntl
import struct

import sys
from threading import Thread
from threading import Lock
import thread
import signal
import datetime as dt
import time as t

import simplejson as json
#import MySQLdb as db

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

		self.server_sock = socket.socket (socket.AF_INET, socket.SOCK_STREAM)
		self.buf_size 	 = 1024

		try:
			self.server_sock.bind ((self.host, self.port))
		except socket.error, e:
			print_error(e)
			sys.exit(1)

		self.server_sock.listen(5)

		print "----------RAICHU SYSTEM ONLINE----------"
		print "%s %s" % (self.host, self.port)
		print "listening for devices..."

		try:
			while True:
				client_sock, addr = self.server_sock.accept()
				timestamp = get_timestamp()
				print "==========<%s>==========" % timestamp
				print "%s %s connected" %  (addr[0], addr[1])
				print "========================================="
				self.handle_connection(client_sock)
				#thread.start_new_thread(handle_connection, client_sock)

		except socket.error,e:
			print_error(e)
		finally:
			if self.server_sock:
				self.server_sock.close()

	def close(self):
		self.server_sock.close()

	def handle_connection(self, client_sock):
		print client_sock.getpeername()
		in_pkt = client_sock.recv (self.buf_size)
		conn_info = json.loads (in_pkt)

		if conn_info["type"] == "device":
			print "got device"
		elif conn_info["type"] == "client":
			print "got client"
		else:
			print "not sure what I got" 
		client_sock.send ("ok")
		#log connection
			#keep client_sock alive
		#need to specify if device or client
		#device connected, keep alive
			#client specifies what device to connect to
			#all date client sends is passed to device

		client_sock.close()
		pass
#end function def

# main
if __name__ == "__main__":
	#port = sys.argv[1]
	s = raichu_server()

	try:
		s.start()
	except KeyboardInterrupt:
		s.close()
		print ""
		print "== server shutdown =="
		sys.exit(0)

