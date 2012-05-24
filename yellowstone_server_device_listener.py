#!/usr/bin/python

import socket
import fcntl
import struct
import MySQLdb as db
import sys
from threading import Thread
from threading import Lock
import thread

#ugly global variable
device_name = ""

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
	user = 'root'
	passwd = 'admin'
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


def log_device_name(device_name, addr):
	#write data into database
	print "Recieved %s" % device_name
	query = 'insert into device (hostname, ip_address, port) values ("%s","%s","%i")' % (device_name, addr[0], addr[1])
	if db_insert(query):
		response = "ok"
		print "%s is connected and ACK'd" % device_name
	else:
		response = "dberror"
		print "Error: Could not write to database"
	return response

def create_song_list(data, device):
	#write data into database
	#print "Recieved %s" % client_data
	query = 'insert into song (title, device) values ("%s", "%s")' % (data, device)
	if db_insert(query):
		response = "ok"
		print "inserted song"
	else:
		response = "dberror"
		print "Error: Could not write to database"
	return response

#takes data sent from client, also requires connection handler and address
def process_data_from(client_data, conn, addr):
	#client sends name
	global device_name
	if client_data.find("DeviceId") >= 0:
		print "processing name"
		processed_data = [x for x in client_data.split('=') if x.strip()]
		device_name = processed_data[1]
		return log_device_name(device_name, addr)
		
	elif client_data.find("MP3List") >= 0:
		processed_data = [x for x in client_data.split(':') if x.strip()]
		processed_data = [x for x in processed_data[1].split('#') if x.strip()]
		print "processing mp3"
		for i in range(len(processed_data)):
			response = create_song_list(processed_data[i], device_name)
		return response

		#return mp3 function
		#TODO write mp3 function (takes in data delimited by '#')
	elif client_data.find("HELLO") >= 0:
		return "ok"
	else:
		return client_data;


#def recv_all_from(socket_obj):
#    total_data=[]
#    while True:
#        data = socket_obj.recv(1024)
#        if not data: break
#        total_data.append(data)
#    return ''.join(total_data)

def conn_handler(conn, addr):
	#client_data = recv_all_from(conn)
	while True:
		client_data = conn.recv(1024)
		if not client_data:
			break;
		
		print client_data
		server_response = process_data_from(client_data, conn, addr)
		conn.send(server_response)
	if conn:
		conn.close()

def listen_for_connections():
	listener = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	
	#general usage, fuxes up for linux
	#host = socket.gethostbyname(socket.gethostname())
	
	#specified for linux vm
	host = get_ip_address("eth0")
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
			
			thread.start_new_thread(conn_handler, (conn, addr))

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
