#!/usr/bin/python

# server.py
# this denotes the class definition of the raichu server

import platform
import socket
import fcntl
import struct

import sys
import threading
from threading import Lock
from threading import Condition
import thread
import signal
import datetime as dt
import time as t

import simplejson as json
import pprint as pp
import MySQLdb as db

def get_ip_address(ifname):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    return socket.inet_ntoa(fcntl.ioctl(
        s.fileno(),
        0x8915,  # SIOCGIFADDR
        struct.pack('256s', ifname[:15])
    )[20:24])

def print_error(error):
	print "Error %d: %s" % (error.args[0], error.args[1])

def get_timestamp():
	return dt.datetime.fromtimestamp(int(t.time())).strftime('%Y-%m-%d %H:%M:%S')

#TODO better log function
#TODO device needs to send data too some times (think MP3 player)
#TODO client needs to be able to receive data
#TODO implement locks on execute_sql_batch
#TODO disconnect clients and devices at shutdown

class raichu_server:

	def __init__(self, port=30000):

		self.port = int(port)
		server_platform = platform.system()
		if server_platform == "Linux":
			self.host = get_ip_address("eth0")
		elif server_platform == "Darwin":
			self.host = socket.gethostbyname(socket.gethostname())

		# all devices connected
		self.device_list = {}
		# all clients connected
		self.client_list = {}
		# key value of client to device
		self.connection_list = {}
		# a hash of lists of clients and their assigned devices
		self.assign_list = {}
		# query list to batch execute
		self.query_list = []

		# database handle
		self.db_conn = None

	def start(self):
		self.db_connect()
		listen_worker = threading.Thread(target=self.start_master_listener)
		listen_worker.daemon = True
		listen_worker.start()

		query_worker = threading.Thread(target=self.execute_query_batch)
		query_worker.daemon = True
		query_worker.start()

		# command prompt
		while 1:
			# delimits string by space and returns a list
			cmd = raw_input("> ").split()

			if cmd == []:
				pass
			elif cmd[0] == "list":
				if len(cmd) == 1:
					print "\tdevices"
					print "\tclients"
					print "\tconns"
					print "\tavail"
				elif cmd[1] == "devices":
					for device in self.device_list:
						print device
				elif cmd[1] == "clients":
					for client in self.client_list:
						print client
				elif cmd[1] == "conns":
					for client in self.connection_list:
						print client + " -> " + self.connection_list[client]
				elif cmd[1] == "avail":
					for device in self.device_list:
						if self.device_list[device]['slave'] == 0:
							print device
				elif cmd[1] == "detail":
					if len(cmd) < 3:
						print "usage: list detail clients|devices"
						pass
					elif cmd[2] == "devices":
						for device in self.device_list:
							pp.pprint(self.device_list[device])
					elif cmd[2] == "clients":
						for client in self.client_list:
							pp.pprint(self.client_list[client])
			elif cmd[0] == "send":
				if cmd[1] in self.device_list:
					try:
						out_sock = self.device_list[cmd.pop(1)]["socket"]
						# delete first word index
						cmd.pop(0)
						out_data = ' '.join(cmd)
						out_sock.send(out_data)
					except socket.error, e:
						print_error(e)
				else:
					print "error: device " + cmd[1] + " not found"
			elif cmd[0] == "release":
				if cmd[1] in self.device_list:
					try:
						sock = self.device_list[cmd[1]]["socket"]
						sock.close()
						del self.device_list[cmd[1]]
						print cmd[1] + " disconnected"
					except socket.error, e:
						print_error(e)

			elif cmd[0] == "close" or cmd[0] == "exit":
				self.close()
				break
			else:
				pass

	def close(self):
		#for worker in worker_list:
		#	if worker.isAlive():

		self.server_sock.close()

	def start_master_listener(self):
		try:
			self.server_sock = socket.socket (socket.AF_INET, socket.SOCK_STREAM)
			self.server_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
			self.buf_size 	 = 1024
			self.server_sock.bind ((self.host, self.port))
			self.server_sock.listen(5)
		except socket.error, e:
			print_error(e)
			sys.exit(1)

		print "----------RAICHU SYSTEM ONLINE----------"
		print "%s %s" % (self.host, self.port)
		print "listening for devices..."

		try:
			while True:
				client_sock, addr = self.server_sock.accept()
				timestamp = get_timestamp()
				print "==========<%s>==========" % timestamp
				print "%s %s connected" %  (addr[0], addr[1])
				print "========================================="

				conn_worker = threading.Thread(target=self.handle_connection, args=(client_sock,))
				conn_worker.daemon = True
				conn_worker.start()

		except socket.error,e:
			print_error(e)
		finally:
			if self.server_sock:
				self.server_sock.close()

	def handle_connection(self, client_sock):
		#while True:
		#	try:
		#		buf = client_sock.recv(self.buf_size)
		#		if buf == '':
		#			client_sock.close(0)
		#			sys.exit(1)
		#			break
		#		print ">>>>>>>>>>" + buf
		#		client_sock.send("#####" + buf + " @@@@@")
		#	except socket.error, e:
		#		print_error(e)
		#		client_sock.close(0)
		#		sys.exit(1)
		#		break


		conn_info = {}
		try:
			in_data = client_sock.recv(self.buf_size)
			print ">>>>> " + in_data
			if in_data == "*HELLO*":
				# compensate for wifly devices
				# receiving device data
				in_data = client_sock.recv(self.buf_size)
				client_sock.send("ok")
				print "<<<<< " + "ok"
				print ">>>>> " + in_data
			conn_info = json.loads(in_data)
			#print "conn_info: " + str(conn_info)
			#print "conn_info type: " + str(type(conn_info))
		except json.decoder.JSONDecodeError, e:
			print e
		except socket.error, e:
			print_error(e)

		if conn_info["type"] == "device":
			self.handle_device (client_sock, conn_info)
		elif conn_info["type"] == "client":
			self.handle_client (client_sock, conn_info)
		else:
			print "not sure what I got" 

		
		#log connection
			#keep client_sock alive
		#need to specify if device or client
		#device connected, keep alive
			#client specifies what device to connect to
			#all date client sends is passed to device
		#t.sleep(5)

		# client_sock shouldn't be closed here, depends on if it's
		# a device or client
		#client_sock.close()
		
		pass
	def handle_device(self, device_sock, conn_info):
		print "device connected"
		device_info 						= device_sock.getpeername()
		conn_info["address"]				= device_info
		conn_info["ip"] 					= device_info[0]
		conn_info["port"] 					= device_info[1]
		conn_info["socket"] 				= device_sock
		conn_info["connect_time"]			= get_timestamp()
		conn_info["slave"]					= 0
		# 0 means free device
		# 1 means slave
		# 2 means it's mucked up

		self.device_list[conn_info['name']] = conn_info
		self.db_insert(conn_info)

		while True:
			#consider spinning out recv as separate thread
			try:
				recv_buffer = device_sock.recv(self.buf_size)
				if recv_buffer == '':
					raise RuntimeError(conn_info['name'] + " disconnected")
			except socket.error, e:
				print_error(e)
			except RuntimeError as e:
				print e
				# cleanup of d/c client from server cache
				if self.device_list[conn_info['name']]:
					# a legitimate disconnect will also result in socket d/c
					del self.device_list[conn_info['name']]
				self.db_remove(conn_info)
				sys.exit(1)

			if len(recv_buffer) > 0:
				# debug
				print conn_info['name'] + ">>>raichu_server: " + recv_buffer

		# wait for connection
		# thread sleep?

	def handle_client(self, client_sock, conn_info):
		print "client connected"
		device_info 						= client_sock.getpeername()
		conn_info["address"]				= device_info
		conn_info["ip"] 					= device_info[0]
		conn_info["port"] 				    = device_info[1]
		conn_info["socket"] 				= client_sock
		conn_info["connect_time"]			= get_timestamp()
		# set default for server to relay data
		conn_info["relay"]				= 1
		conn_info["master"]				= 0

		self.client_list[conn_info['name']] = conn_info
		self.db_insert(conn_info)

		# mode designates what server does with client's stream
		# 0 == interpret commands and execute
		# 1 == relay data to device
		mode = 0

		while True:
			#consider spinning out recv as separate thread
			try:
				recv_buffer = client_sock.recv(self.buf_size)
				print "<<<<< " + recv_buffer
				if recv_buffer == '':
					raise RuntimeError(conn_info['name'] + " disconnected")
			except socket.error, e:
				print_error(e)
			except RuntimeError as e:
				print e
				# cleanup of d/c client from server cache
				if self.client_list[conn_info['name']]:
					del self.client_list[conn_info['name']]
				self.device_list[ self.connection_list[ conn_info['name'] ] ]['slave'] = 0
				del self.connection_list[conn_info['name']]
				self.db_remove(conn_info)

				sys.exit(1)

			if len(recv_buffer) > 0:
				# debug
				print conn_info['name'] + ">>>raichu_server: " + recv_buffer
				
				cmd = recv_buffer.split()
				if cmd[0] == "list":
					out_data = json.dumps(self.get_avail_devices())
					if len(out_data) != 2:
						print "sending device_list"
						out_data = "{\"deviceListFromServer\":" + out_data + "}"
						client_sock.send(out_data)
						print ">>>>> " + out_data
					else:
						# send 0 to denote no devices online
						client_sock.send("0")
				elif cmd[0] == "assign":
					# push to assign_list['client']
					# update database so that to reflect
					pass
				elif cmd[0] == "release":
					# remove from assign_list
					# update database
					device_name = cmd[1]
					del self.connection_list[conn_info['name']]
					self.device_list[device_name]["slave"] = 0
					print "released " + device_name + " from " + conn_info['name'] 
					pass
				elif cmd[0] == "connect":
					# need to redo some logic here
					if self.device_list[cmd[1]]['slave'] == 0:
						# assign client to device
						device_name = cmd[1]
						self.connection_list[conn_info['name']] = device_name
						self.device_list[device_name]["slave"] = 1
						client_sock.send("device " + device_name + " assigned")
						print "assigned " + conn_info['name'] + " to " + device_name
					else:
						raise RuntimeError("device does not exist")
				elif cmd[0] == "relay":
					device_name = self.connection_list[conn_info['name']]
					out_sock = self.device_list[device_name]["socket"]
					cmd.pop(0)
					out_data = ' '.join(cmd)
					ret = out_sock.send(out_data)
					if ret != 0:
						print "raichu_server(" + conn_info['name'] + ")>>>" + device_name + ": " + out_data
					else:
						raise RuntimeError("socket connection broken")
				elif cmd[0] == "release":
					pass




		#elif recv_data[0] == "assign":
		#	device_request = recv_data[1]
		#	if device_request in device_list:
		#		# assign device to client
		#		connection_list[conn_info['address']] = device_list[device_request]['socket']
		#else:
		#	device_socket = connection_list[]
		# lock
		# find a random free device
		# assign to client_sim
		# push to connection_list
		# unlock



	def check_device_slave(self):
		pass

	def relay_data(self, data):
		pass

	def get_avail_clients(self):
		return self.client_list

	def get_avail_devices(self):
		#TODO remove avail_devices, just use device_list for getting device updates
		avail_devices = []
		for device in self.device_list:
			# if the device is free and not already existing in list, append
			if self.device_list[device]["slave"] == 0:
				avail_devices.append(device)
		return avail_devices



	def db_connect(self):
		# point host elsewhere if db not on same machine
		host     = 'localhost'
		user     = 'root'
		passwd   = 'admin'
		database = 'raichu'

		try:
			self.db_conn = db.connect(host, user, passwd, database)
			#print "connected to " + database + " database"
		except db.Error, e:
			print_error(e)
			if self.db_conn:
				self.db_conn.close()
			sys.exit(1)

	def db_insert(self, conn_info):
		if conn_info["type"] == "device":
			query = 'insert into device (name, type, ip, port, connect_time, bootup_time) values ("%s","%s","%s","%s","%s","%s")'\
		      		% (conn_info['name'], conn_info['type'],
		      		conn_info['ip'], conn_info['port'], 
		      		conn_info['connect_time'], conn_info['bootup_time'])	
		elif conn_info["type"] == "client":
			query = 'insert into client (name, type, ip, port, connect_time, bootup_time, relay) values ("%s","%s","%s","%s","%s","%s","%s")'\
		      		% (conn_info['name'], conn_info['type'],
		      		conn_info['ip'], conn_info['port'], 
		      		conn_info['connect_time'], conn_info['bootup_time'], 
		      		conn_info['relay'])

		self.query_list.append(query)
		#print "append insert query"
		pass

	def db_remove(self, conn_info):
		if conn_info["type"] == "device":	
		    query = 'update device set online=0, slave=0 where name="%s"' % conn_info['name']
		elif conn_info["type"] == "client":
			query = 'update client set online=0, master=0 where name="%s"' % conn_info['name']
		
		self.query_list.append(query)
		#print "append delete query"

	def db_update(self, conn_info):
		pass

	def db_close(self):
		if self.db_conn:
			self.db_conn.close()
			print "connection to database closed"

	def execute_query_batch(self):
		while True:
			if len(self.query_list) > 0:
				try:
					# in the event the db handle dies, reconnect
					if not self.db_conn:
						self.db_connect()
						print "db reconnection"
					with self.db_conn:
						cur = self.db_conn.cursor()
						query = self.query_list.pop()
						cur.execute(query)
						#print "executed query"
				except db.Error, e:
					print_error(e)
			#TODO fine tune this sleep interval
			t.sleep(1)
		pass

#end function def

# main
if __name__ == "__main__":
	if len(sys.argv) > 1:
		port = sys.argv[1]
		s = raichu_server(port)
	else:
		s = raichu_server()

	try:
		s.start()
	except KeyboardInterrupt:
		s.close()
		print ""
		print "== server shutdown =="
		s.db_close()
		sys.exit(0)

