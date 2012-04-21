#!/usr/bin/python

import socket
import fcntl
import struct
import MySQLdb as db
import sys
from threading import Thread
from threading import Lock

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
	
def insert_into_db(client_request, addr):
	#write data into database
	print "Recieved %s" % client_request
	query = 'insert into device (hostname, ip_address, port) values ("%s","%s","%i")' % (client_request,addr[0], addr[1])

	if db_insert(query):
		response = "ok"
		print "Client existence ACK'd"
	else:
		response = "dberror"
		print "Error: Could not write to database"
	return response

def recv_all_from(socket_obj):
    total_data=[]
    while True:
        data = socket_obj.recv(1024)
        if not data: break
        total_data.append(data)
    return ''.join(total_data)
	
def listen_for_connections():
	listener = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	#host = socket.gethostname()
	#host = socket.gethostbyname(socket.gethostname())

	#specified for linux vm
	host = get_ip_address("eth3")
	#school designated ports 33XXX
	port = 33333

	try:
		listener.bind((host, port))
	except socket.error,e:
		print "Error %d: %s" % (e.args[0], e.args[1])
		sys.exit(1)

	#TODO multithread this part
	listener.listen(5)

	print "listening for devices..."

	#TODO use catch exceptions
	try:
		while True:

			conn, addr = listener.accept()
			print "%s:%s is connected" %  (addr[0], addr[1])
			
			#client_request = recv_all_from(conn)
			client_request = conn.recv(1024)

			if len(client_request) > 0:
				response = insert_into_db(client_request, addr)
				conn.send(response)
			conn.close()

	except socket.error,e:
		print "Error %d: %s" % (e.args[0], e.args[1])

	finally:
		if listener:
			listener.close()
#end function def

#main
if __name__ == "__main__": 
	listen_for_connections()

#threading is complicating things, deferred
#t = Thread(target=listen_for_connections, args=())
#print ""
#t.start()

#poll active connections