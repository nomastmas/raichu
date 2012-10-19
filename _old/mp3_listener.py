#!/usr/bin/python

import socket
import fcntl
import struct
import MySQLdb as db
import sys


#function def
def get_ip_address(ifname):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    return socket.inet_ntoa(fcntl.ioctl(
        s.fileno(),
        0x8915,  # SIOCGIFADDR
        struct.pack('256s', ifname[:15])
    )[20:24])

def db_insert(query):
	#connection variables
	con = None
	host = 'localhost'
	user = 'client'
	passwd = ''
	database = 'raichu'

	#print query
	#return False
	try:
		con = db.connect(host, user, passwd, database)

		with con:
			cur = con.cursor()
			cur.execute(query)

	except db.Error,e:
		print "Database error %d: %s" % (e.args[0], e.args[1])
		return False
	finally:
		if con:
			con.close()
	return True

def insert_into_db(data, device):
	#write data into database
	#print "Recieved %s" % client_request
	query = 'insert into song (title, device) values ("%s", "%s")' % (data, device)

	if db_insert(query):
		response = "ok"
		print "inserted song"
	else:
		response = "dberror"
		print "Error: Could not write to database"
	return response
	
def process_for_db(data):
	#stores a list of delimited strings
	processed_data = [x for x in data.split('#') if x.strip()]
	
	for i in range(len(processed_data)):
		response = insert_into_db(processed_data[i])
	
	return response
	

listener = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

#specified for linux vm
host = get_ip_address("eth3")
#school designated ports 33XXX
port = 33334

try:
	listener.bind((host, port))
except socket.error,e:
	print "Error %d: %s" % (e.args[0], e.args[1])
	sys.exit(1)

#TODO multithread this part
listener.listen(5)

print "listening..."

try:
	while True:
		conn, addr = listener.accept()
		client_request = conn.recv(1024)
		
		print "Client data:"
		print client_request
		
		if(len(client_request) > 0):
			response = process_for_db(client_request)
			conn.send(response)
		
except socket.error,e:
	print "Error %d: %s" % (e.args[0], e.args[1])

finally:
	if listener:
		listener.close()