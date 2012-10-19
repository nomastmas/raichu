#!/usr/bin/python

import os
import socket
import sys
import time

media_path = "./media/"		#where mp3 files are
mp3_list = list()			#mp3 list
out_packet = ""				#packet to get sent out

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

host = sys.argv[1]		#server address
port = 33333			#server port

#for files in os.listdir(media_path):
#	if files.endswith(".mp3"):
#		mp3_list.append(files)
#		print files + " appended"

#print mp3_list

#for file_name in mp3_list:
#	out_packet += file_name + '#'



try:
	s.connect((host, port))
	hostname = "DeviceId:" + socket.gethostname()
	
	s.send("*HELLO*")
	print "sent: *HELLO*"
	time.sleep(1)
	server_response = s.recv(1024)
	print "Response: " + server_response
	
	if(server_response == "ok"):
		s.send(hostname)
		print "sent: " + hostname
	
	time.sleep(1)
	server_response = s.recv(1024)
	print "Response: " + server_response
	
	out_packet = "MP3List:MAROON~1.MP3#01BLUE~1.MP3#"
	if(server_response == "ok"):
		s.send(out_packet)
		print "sent: " + out_packet
		
	time.sleep(1)
	server_response = s.recv(1024)
	print "Response: " + server_response

except socket.error,e:
	print "Error %d: %s" % (e.args[0],e.args[1])
	sys.exit(1)

finally:
	if s:
		s.close()				#close connection