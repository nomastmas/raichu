#!/usr/bin/python

import socket
import sys
import datetime as dt
import time as t

import simplejson as json

def print_error(error):
	print "Error %d: %s" % (error.args[0], error.args[1])

def get_timestamp():
	return dt.datetime.fromtimestamp(int(t.time())).strftime('%Y-%m-%d %H:%M:%S')

def raichu_send(out_sock, out_data):
	total_sent = 0
	while total_sent < len(out_data):
		try:
			sent = out_sock.send(out_sock[total_sent:])
			print "sent: " + out_sock[total_sent:]
			if sent == 0:
				raise RuntimeError("socket connection broken")
			total_sent += sent
		except socket.error, e:
			print_error(e)

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
		out_data = json.dumps(device_info)
		print out_data
		ret = s.send (out_data)
		if ret == 0:
			raise RuntimeError("socket connection broken")
		
		#raichu_send(s, out_data)
		print device_info["name"] + " start up"

		t.sleep(1)
		ret = s.send("list")
		if ret == 0:
			raise RuntimeError("socket connection broken")
		buf = s.recv(1024)
		print "buffer: " + buf


		#server_response = s.recv(1024)
		#print server_response

		t.sleep(1)

	except socket.error, e:
		print_error(e)
	finally:
		if s:
			s.close()
		print "shutting down..."
		sys.exit(0)