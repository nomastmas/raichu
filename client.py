#!/usr/bin/python

import socket
import sys
import time

def print_error(error):
	print "Error %d: %s" % (error.args[0], error.args[1])

if __name__ == "__main__":
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	host = sys.argv[1]
	port = sys.argv[2]

	device_info = {
		'type'		  : 'client_sim',
		'name'		  : 'client_' + dt.datetime.fromtimestamp(int(t.time())).strftime('%H%M%S'),
		'bootup-time' : get_timestamp(),
	}

	try:
		s.connect ((host, int(port)))
		print s.getpeername()
		s.send ("hello")
		#server_response = s.recv(1024)
		#print server_response

		time.sleep(1)

	except socket.error, e:
		print_error(e)
	finally:
		if s:
			s.close()