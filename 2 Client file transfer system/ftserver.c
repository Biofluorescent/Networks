/*
 * Author: Tanner Quesenberry
 * Date: 11/26/17
 * References:
 * 	344 Class lecture notes and previous assignment, Piazza posts
 * 	https://stackoverflow.com/questions/1479386/is-there-afunction-in-c-that-will-return-the-index-of-a-char-in-a-char-array
 * 	https://stackoverflow.com/questions/9206091/going-through-a-text-file-line-by-line-in-c
 *      www.thegeekstuff.com/2011/12/c-socket-programming/?utm_source=feedburner
 *      www.linuxhowtos.org/C_C++/socket.htm?userrate=1
 *      https://stackoverflow.com/questions/7652293/how-do-i-dynamically-allocate-an-array-of-strings-in-c
 *      https://stackoverflow.com/questions/2014033/send-and-receive-a-file-in-socket-programming-in-linux-with-c-c-gcc-g
 *      http://man7.org/linux/man-pages/man2/open.2.html
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <netdb.h>
#include <time.h>
#include <fcntl.h>

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues
char buffer[80000];
char filter[80000];

/*
 * Function: get_directory
 * Parameter: array of strings
 * Output: Number of files in dir, file names in array
 * Purpose: Counts the number of regular files in directory and puts file names in array
 * Reference for getting files in dir     https://stackoverflow.com/questions/4204666/how-to-list-files-in-a-directory-in-a-c-program
 * Reference for d_type   https://linux.die.net/man/3/readdir
 */
int get_directory(char ** list){
    int count = 0;
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if(d){
        while((dir = readdir(d)) != NULL){
            if(dir->d_type == DT_REG){
                strcpy(list[count], dir->d_name);
//                printf("%s\n", dir->d_name);
                count += 1;
            }
        }
        closedir(d);
    }else{
        printf("Could not open current directory.");
    }
    return count;
}

int main(int argc, char *argv[])
{

        // Dynamic string array allocation 
        // https://stackoverflow.com/questions/7652293/how-do-i-dynamically-allocate-an-array-of-strings-in-c
        char ** list = malloc(200 * sizeof(char*));
        int strings = 0;
        for(strings; strings < 200; strings++){
            list[strings] = (char*)malloc(100);
            memset(list[strings], '\0', sizeof(list[strings]));
        }
        strings = get_directory(list);

        // Dont forget to free each array element and then the array


	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	socklen_t sizeOfClientInfo;
	struct sockaddr_in serverAddress, clientAddress;


	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) fprintf(stderr, "ERROR opening socket");

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		fprintf(stderr, "ERROR on binding");
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

        printf("Server open on %d\n", portNumber);

        // Create a newSocket to receive info from
        int receiveSocketFD;
        int i;  
        int exists;      
        char ip_addr[50];
        char command[3];
        char port[50];
        char filename[100];
        char * good = "ok";
        char * bad = "bad";
        int requestType = 0;

        while(true){

            memset(ip_addr, '\0', sizeof(ip_addr));
            memset(command, '\0', sizeof(command));
            memset(port, '\0', sizeof(port));
            memset(filename, '\0', sizeof(filename));

	    // Accept a connection, blocking if one is not available until one connects
       	    sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
	    receiveSocketFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
	    if (receiveSocketFD < 0) fprintf(stderr, "ERROR on accept");

            // Get the command
            charsRead = recv(receiveSocketFD, command, 2, 0);
            if(strcmp(command, "-l") == 0){
                requestType = 0;
//                printf("Received list command from client.\n");
                send(receiveSocketFD, good, strlen(good), 0);
                //Get port
                charsRead = recv(receiveSocketFD, port, 49, 0);
                send(receiveSocketFD, good, strlen(good), 0);
//                printf("%s\n", port);
            }else{
                requestType = 1;
//		printf("Received a file request.\n");
                send(receiveSocketFD, good, strlen(good), 0);
                // Get filename
                charsRead = recv(receiveSocketFD, filename, 99, 0);
                // Check dir for file
                exists = 0;
                for(i = 0; i < strings; i++){
                    if(strcmp(list[i], filename) == 0){
                        exists = 1;
                    }
                }

                // Send response on control connection if file exists
                if(exists == 1){
                    send(receiveSocketFD, good, strlen(good), 0);
                }else{
                    send(receiveSocketFD, bad, strlen(bad), 0);
                }

                //Get port
                charsRead = recv(receiveSocketFD, port, 49, 0);
                send(receiveSocketFD, good, strlen(good), 0);

            }
            

            //Get ip
            charsRead = recv(receiveSocketFD, ip_addr, 49, 0);

            printf("Connection from %s\n", ip_addr);
            if(requestType == 0){
                printf("List directory requested on port %s.\n", port);
                printf("Sending directory contents to %s:%s.\n", ip_addr, port);
                // Send directory
            }else{
                printf("File \"%s\" requested on port %s.\n", filename, port);
                // Send file if it exists
                if(exists == 1){
                    printf("Sending \"%s\" to %s:%s.\n", filename, ip_addr, port);
                }else{
                    printf("File not found. Sending error message to %s:%s.\n", ip_addr, argv[1]);
                }
            }
            send(receiveSocketFD, good, strlen(good), 0);

            // Reference: This part was used from my previous 344 coursework.
            // Create data connection
            int socketFD, dataPort, charsWritten;
            struct sockaddr_in serverAddress;
            struct hostent* serverHostInfo;
            
            // Setup server address struct
            memset((char*)&serverAddress, '\0', sizeof(serverAddress));
            dataPort = atoi(port);
            serverAddress.sin_family = AF_INET;
            serverAddress.sin_port = htons(dataPort);
            serverHostInfo = gethostbyname(ip_addr);
            if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); } 
            memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length);
            
            // Setup the socket
            socketFD = socket(AF_INET, SOCK_STREAM, 0);
            // Allow python to set of dataSocket
            sleep(1);
            //Connect to server
            if(connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
                // Error
                printf("Error connecting socket.\n");
            }

            // Send to client on data connection
            if(requestType == 0){
                // Send directory listing     
                for(i = 0; i < strings; i++){
                    charsWritten = send(socketFD, list[i], 50, 0);
                }
            }else{
                // Reference: https://stackoverflow.com/questions/2014033/send-and-receive-a-file-in-socket-programming-in-linux-with-c-c-gcc-g
                char textBuffer[500];
                memset(textBuffer, '\0', sizeof(textBuffer));
                // Open client file
                int FD = open(filename, O_RDONLY);
                while(1){
                    int readBytes = read(FD, textBuffer, sizeof(textBuffer));
                    if(readBytes == 0) break;
                    
                    void *p = textBuffer;
                    while(readBytes > 0) {
                        int writtenBytes = send(socketFD, p, readBytes, 0);
                        readBytes -= writtenBytes;
                        p += writtenBytes;
                    }
                }
                //fclose(FD);
            }
            close(socketFD);
	    //charsRead = send(receiveSocketFD, "I am the server, and I got your message", 39, 0); // Send success back
            
        }

	close(listenSocketFD); // Close the listening socket
	return 0; 
}
