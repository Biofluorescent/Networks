#!/bin/python

# Author: Tanner Quesenberry
# Date: 10/25/17
# References:
#		https://docs/python.org/2/library/socket.html
#		https://docs.python.org/3.4/library/string.html
#		https://learnxinyminutes.com/docs/python/
#		https://stackoverflow.com/questions/22899410/limiting-raw-input-in-python

import socket
import sys

def communicate(connection):
	"""
	Function: communicate
	Args: socket connection
	Return: None	
	Purpose: Chat with connected client. Client must send first message
	"""
	# Prepend to all sent messages
	Name = "Server>"

	while 1:
		# Receive message from connected user
		message = ""
		message = conn.recv(501)
		# Display message
		print message
		# Cease communication if no message
		if not message: break
		# Get message from server
		message = ""
		message = raw_input("{}".format(Name))[:500]
		# Check if time to quit
		if message == "\quit":
			connection.close()
			break
		# Send message to client, and handle
		connection.send(message)
		connection.send(Name)


if __name__ == "__main__":
	# Check that PORT is specified on command line
	if len(sys.argv) != 2:
		print "Usage: python chatserve.py [PORT_NUMBER]"
		exit(1)
	HOST = ''
	# Get user specified port number
	PORT = sys.argv[1]
	# Create the socket
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	# Bind to port
	sock.bind((HOST, int(PORT)))
	# Listen on socket
	sock.listen(1)

	while 1:
		# Accept a connection
		conn, addr = sock.accept();
		# Chat with client
		communicate(conn)

	
