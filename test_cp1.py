# import random
from socket import *
import sys
import random
import os
import time
import threading 


numThreads=6
def thread_main():
	global count, mutex

	
	threadname = threading.currentThread().getName()
 	start_time=time.time()
 	RECV_TOTAL_TIMEOUT = 0.1
	RECV_EACH_TIMEOUT = 0.01


	serverHost=gethostbyname('127.0.0.1')
	serverPort = 1234
	numTrials = 15
	# numWritesReads = 2
	numBytes = 43
	numConnections = 32


	inistring="<ABCDEFGHIJKshadgjhxg832>dy97s78</ABCDEFGHIJKshadgjhxg832>LMNOPQRSTUVWXYZ"*5


	socketset=[]
	for i in xrange(numConnections):
		s = socket(AF_INET, SOCK_STREAM)
		s.connect((serverHost, serverPort))
		socketset.append(s)


	flag=0
	for i in xrange(numTrials):

		teststrings=[]
		for j in xrange(numConnections):
			s=socketset[j]
			a=''.join(random.sample(inistring,numBytes))
			teststrings.append(a)
			s.send(a)
			# print i,"th string",a,"sent"
			# print "string recved: ",b

		for j in xrange(numConnections):
			s=socketset[j]
			# s.settimeout(RECV_EACH_TIMEOUT)
			b=s.recv(len(teststrings[j]))
			if b!=teststrings[j]:
				flag=1
				print "differnet"
				print "send",teststrings[j]
				print "get",b
	if flag==0:
		print threadname,"success"
		print time.time() - start_time

	for j in xrange(numConnections):
		socketset[j].close()
 
def main(num):
	global count, mutex
	threads = []
	count = 1
	for x in xrange(0, num):
		threads.append(threading.Thread(target=thread_main))
	for t in threads:
		t.start()
	for t in threads:
		t.join()  
 
main(numThreads)







