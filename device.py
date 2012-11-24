#!/usr/bin/python

import socket
import sys
import datetime as dt
import time as t
import simplejson as json
import random


def print_error(error):
	print "Error %d: %s" % (error.args[0], error.args[1])

def get_timestamp():
	return dt.datetime.fromtimestamp(int(t.time())).strftime('%Y-%m-%d %H:%M:%S')

def boot_up(s):
	try:
		s.connect ((host, int(port)))
		s.send (json.dumps (device_info))
	except socket.error, e:
		print_error(e)
		s.close()

if __name__ == "__main__":
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

	if len(sys.argv) < 2:
		print "error: not enough arguments"
		print "usage: ./device <ip_addr> <port>"
		sys.exit(1)

	s.setblocking (1)
	host = sys.argv[1]
	port = sys.argv[2]

	# may need to add on more info later on
	device_info = {
		'type'		  : 'device',
		'name'		  : 'device_' + dt.datetime.fromtimestamp(int(t.time())).strftime('%H%M%S') + str(int(random.random()*100)),
		'bootup-time' : get_timestamp(),
		'commands'	  : "be_dumb, hello",
	}

	try:
		boot_up (s)
		t.sleep(1)

		print device_info["name"] + " start up"
		print json.dumps(device_info)
		try:
			while True:
				data = s.recv(1024)
				if data != "":
					# socket is alive
					print data
				elif data == "ping":
					print "recv ping"
					s.send("alive")

				t.sleep(0.1)
				#else:
				#	# socket is no longer alive
				#	break
		except socket.error, e:
			print_error(e)
			
		print device_info["name"] + " shutdown"
	except socket.error, e:
		print_error(e)
	finally:
		if s:
			s.close()