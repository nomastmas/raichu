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
			print "sent: " + out_data[total_sent:]
			sent = out_sock.send(out_data[total_sent:])
			if sent == 0:
				raise RuntimeError("socket connection broken")
			total_sent += sent
		except socket.error, e:
			print_error(e)

def raichu_recv(in_sock, buf_size):
	in_data = in_sock.recv(int(buf_size))
	if in_data == '':
		raise RuntimeError("socket connection broken")
	else:
		return in_data

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
		out_data = json.dumps(device_info)
		
		raichu_send(s, out_data)
		print device_info["name"] + " start up"

		t.sleep(1)
		raichu_send(s, "list")
		buf = raichu_recv(s, 1024)
		print "buffer: " + buf

		if len(buf) == 1:
			if int(buf) == 0:
				print "no devices online"
				pass

		# hard code assign first device to client
		# design randomized algorithm later
		devices = json.loads(buf)
		#raichu_send(s, "assign 1")
		raichu_send(s, "assign " + devices[0])
		buf = raichu_recv(s, 1024)
		print "buffer: " + buf

		t.sleep(1)
		for i in range(0, 10):
			raichu_send(s, "relay blah blah blah")
			t.sleep(0.5)

		#server_response = s.recv(1024)
		#print server_response
		#while True:
		#	buf = s.recv(1024)
		#	t.sleep(0.1)

	except socket.error, e:
		print_error(e)
	finally:
		if s:
			s.close()
		print "shutting down..."
		sys.exit(0)