#!/usr/bin/python

import socket
import fcntl
import struct

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
host = socket.gethostbyname(socket.gethostname())

port = 33333
s.bind((host, port))

s.listen(5)
print "ip address: " + host
print "listening..."

while True:
	conn, addr = s.accept()
	conn.send('*HELLO*')
	
	print "%s connected" % addr[0]
	message = conn.recv(1024)
	
	if len(message) > 0 :
		print "received: " + message
		conn.send('ok');
	
	message = conn.recv(1024)

conn.close()
