#!/usr/bin/python

import subprocess as sub
import sys

if len(sys.argv) < 3:
	print "error: not enough arguments"
	print "usage: ./multi_device.py <iterations> <ip_addr> <port>"
	sys.exit(1)

count	= int(sys.argv[1])
ip_addr	= sys.argv[2]
port 	= sys.argv[3]

p = []
try:
	print "creating " + str(count) + " devices..."
	for i in range(0, count):
		p.append(
			sub.Popen(["./device.py", ip_addr, port],)
		)
		print "proc #" + str(i+1) + " running"

	print "waiting for children to die..."
	for i in range(0, count):
		p[1].wait()
	#x = raw_input("kill all?")

except KeyboardInterrupt:
	for i in range(0, count):
		p[i].kill()
		print "proc #" + str(i+1) + " killed"
	sys.exit(0)


