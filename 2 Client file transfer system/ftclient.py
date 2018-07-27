#!/bin/python

# Author: Tanner Quesenberry
# Date: 11/26/17
# References:
#	https://docs.python.org/2/howto/sockets.html
#	https://stackoverflow.com/questions/24196932/how-can-i-get-the-ip-address-of-eth0-in-python
#	https://www.guru99.com/reading-and-writing-files-in-python.html
#       http://stackabuse.com/command-line-arguments-in-python/
#       https://docs.python.org/3/library/socket.html#example
#       https://stackoverflow.com/questions/11266068/python-avoid-new-line-with-print-command
#	https://stackoverflow.com/questions/113224/what-is-the-largest-tcp-ip-network-port-number-allowable-for-ipv4
#	https://mail.python.org/pipermail/python-list/2015-April/689448.html

import socket
import sys

"""
Function: verify_args
Paramters: None
Output: None
Return: None
Description: This function checks that the commmand
line arguments are valid for use. Invalid cause program exit.
"""
def verify_args():
	# Check for correct number or args
	if len(sys.argv) < 5 or len(sys.argv) > 6:
		print("ERROR: Invalid number of arguments. Exiting.")
		print("Usage: ftclient.py <SERVER_HOST> <SERVER_PORT> <COMMAND> <FILENAME> <DATA_PORT>")
		sys.exit(1)
	# Check for acceptable port
	if int(sys.argv[2]) < 1024 or int(sys.argv[2]) > 65535:
		print("ERROR: Invalid port number. Exiting.")
		sys.exit(1)
	# Check for acceptable command
	if sys.argv[3] != "-g" and sys.argv[3] != "-l":
		print("ERROR: Invalid command given. Exiting.")
		sys.exit(1)
	# Check for acceptable data port
	if len(sys.argv) == 5 and (int(sys.argv[4]) < 1024 or int(sys.argv[4]) > 65535):
		print("ERROR: Invalid data port. Exiting.")
		sys.exit(1)
	if len(sys.argv) == 6 and (int(sys.argv[5]) < 1024 or int(sys.argv[5]) > 65535):
		print("ERROR: Invalid data port. Exiting")
		sys.exit(1)
	return


"""
Function: get_ip
Parameters: None
Output: None
Return: IP address
Description: Returns the IP address of the user machine.
Borrowed from reference above
"""
def get_ip():
	s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	s.connect(("8.8.8.8", 80))
	return s.getsockname()[0]


"""
Function: data_socket_setup
Parameters: None
Output: None
Return: Socket connection
Description:
Socket setup from above reference
"""
def data_socket_setup():
	# Port is arg 4 for -l and 5 for -g
	if len(sys.argv) == 5:
		dataport = int(sys.argv[4])
	elif len(sys.argv) == 6:
		dataport = int(sys.argv[5])
	# Create the listening socket
	server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	# bind to public host
	server.bind((socket.gethostname(), dataport))
	# Allow one connection
	server.listen(1)
	data_sock, addr = server.accept()
	return data_sock


"""
Function: receiveFile
Parameters: None
Output: Directory listing or file creation
Return: None
Description: Receives data from the server listing the contents
of a directory or creates a new file with incoming data
"""
def receiveFile():
	# If command is directory
	if sys.argv[3] == "-l":
		print "Receiving directory structure from {}:{}".format(sys.argv[1], sys.argv[2])
		dataSocket = data_socket_setup()
		# Receive data
                name = dataSocket.recv(50)
		# print to screen
		while True:
			print name,
			if not name: break
			name = dataSocket.recv(50)
		dataSocket.close()
	else:
		dataSocket = data_socket_setup()
		# Open file for write
		newfile = open(sys.argv[4], "w+")
		# Receive data and write
		text = dataSocket.recv(500)
		while True:
			newfile.write(text)
			if not text: break
			text = dataSocket.recv(500)
		print "File transfer complete."
		newfile.close()
		dataSocket.close()


"""
Function: initiateContact
Parameters: None
Output: None
Return: socket connected to server
Description: Creates a socket connected to a server
specified via the command line
"""
def initiateContact():
        server = sys.argv[1];
        server_port = int(sys.argv[2]);
	controlsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        controlsock.connect((server, server_port))
	return controlsock

"""
Function: makeRequest
Parameters: socket connection
Output: None
Return: None
Description:
The recv after each send is to make sure all data is sent
as to not overrun the server with data. The exists is only
used when requesting a file and not the directory
"""
def makeRequest(controlSocket):
	exists = ""
	command = sys.argv[3]
	if command == "-g":
		# Send command
		controlSocket.send(command)
		controlSocket.recv(1024)
		# Send filename
		controlSocket.send(sys.argv[4])
		exists = controlSocket.recv(1024)
		# Send port
		controlSocket.send(sys.argv[5])
		controlSocket.recv(1024)
	else:	
		# Send command
		controlSocket.send(command)
		controlSocket.recv(1024)
		# Send port
		controlSocket.send(sys.argv[4])
		controlSocket.recv(1024)
	# Get/send ip address
	controlSocket.send(get_ip())
	controlSocket.recv(1024)
	return exists



if __name__ == "__main__":
	verify_args()
	controlSocket = initiateContact()
	fileFound = makeRequest(controlSocket)
	if(fileFound == "bad"):
		print "{}:{} says FILE NOT FOUND".format(sys.argv[1], sys.argv[2])
		sys.exit(1)
	elif(sys.argv[3] == "-g"):
		print "Receiving \"{}\" from {}:{}".format(sys.argv[4], sys.argv[1], sys.argv[2])
	receiveFile()

