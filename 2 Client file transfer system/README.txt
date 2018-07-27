Tanner Quesenberry


File transfer program between diferent servers.

To compile the server type "make". (No quotes)



Run the server by typing: ./server [SERVER_PORT]

The server must be running before starting the client.

Run the server in a different directory than the client.




Note for the client you may need to give ftclient.py executable
 permission with: chmod +x ftclient.py


Run the client by typing: python ftclient.py [SERVER_IP] [SERVER_PORT] [-l|-g] [FILENAME] [DATA_PORT]

Note that you only give a filename when running the -g command, not with the -l command.

To get the server IP, in the terminal running the server, use: hostname -i





Example run:
 make

./server 12345
	
python ftclient.py 128.193.36.41 12345 -l 34567



