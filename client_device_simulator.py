#!/usr/bin/python

import socket
import sys

#create socket (INET and STREAM are defaults)
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

#connect to server
#host = "10.211.55.4"	#server address
host = sys.argv[1]		#server address
#host = socket.gethostname()	#server address
port = 33333			#server port

try:
	s.connect((host, port))
	this_host = socket.gethostname()

	s.send(this_host)
	print "Message sent to %s" % host

	#echo server message
	print s.recv(1024)

except socket.error,e:
	print "Error connecting: %d: %s" % (e.args[0],e.args[1])
	sys.exit(1)

finally:
	if s:
		s.close()				#close connection

