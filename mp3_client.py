#!/usr/bin/python

import os
import socket

media_path = "./media/"		#where mp3 files are
mp3_list = list()			#mp3 list
out_packet = ""				#packet to get sent out

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

host = sys.argv[1]		#server address
port = 33334			#server port

for files in os.listdir(media_path):
	if files.endswith(".mp3"):
		mp3_list.append(files)
		print files + " appended"

#print mp3_list

for file_name in mp3_list:
	out_packet += file_name + '#'

print out_packet
print "packet size: " + str(len(out_packet))

try:
	s.connect((host, port))
	this_host = socket.gethostname()

	s.send(out_packet)
	print "Message sent to %s" % host

	#echo server message
	#server_response = recv_all(s)
	server_response = s.recv(1024)
	print server_response

except socket.error,e:
	print "Error connecting: %d: %s" % (e.args[0],e.args[1])
	sys.exit(1)

finally:
	if s:
		s.close()				#close connection