#!/usr/bin/python

import subprocess as sub
import sys
import time as t

if len(sys.argv) < 3:
	print "error: not enough arguments"
	#print "usage: ./multi_device.py -i <iterations> -t <sleep interval> <ip_addr> <port>"
	print "usage: ./multi_device.py <iterations> <sleep interval> <ip_addr> <port>"
	sys.exit(1)

count	= int(sys.argv[1])
interval = int(sys.argv[2])
ip_addr	= sys.argv[3]
port 	= sys.argv[4]

p = []
try:
	print "creating " + str(count) + " devices..."
	for i in range(0, count):
		p.append(
			sub.Popen(["./device.py", ip_addr, port],)
		)
		print "proc #" + str(i+1) + " running"
		t.sleep(interval)

	print "waiting for children to die..."
	for i in range(0, count):
		p[i].wait()
	#x = raw_input("kill all?")

except KeyboardInterrupt:
	for i in range(0, count):
		p[i].kill()
		print "proc #" + str(i+1) + " killed"
		p[i].wait()
	sys.exit(0)


