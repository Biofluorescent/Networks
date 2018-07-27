/*
 * Author: Tanner Quesenberry
 * Date: 10/25/17
 * References: Previously completed assignments and lectures
 *		https://stackoverflow.com/questions/14707427/taking-string-input-in-char-pointer
 *		http://www.manpages.info/linux/gethostbyname.3.html
 *		https://stackoverflow.com/questions/2218290/concatenate-char-array-in-c
 *		http://www.manpages.info/linux/gethostbyname.3.html
 *		http://www.thegeekstuff.com/2011/12/c-socket-programming/?utm_source=feedburner
 *		http://man7.org/linux/man-pages/man3/fgets.3p.html
 *		https://www.tutorialspoint.com/c_standard_library/string_h.htm
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>



/**************************************************
*  Function: getHandle
*  Return: char *
*  Args: None
*  Purpose: get input from user to fill malloced char array
***************************************************/
char* getHandle(){
    printf("Please enter a max of 10 character usename: ");
    char *name;
    // Size 11 array, name + null terminator
    name = malloc(11);
    // Initialize to null 
    memset(name, '\0', sizeof(name));
    //Read user input
    fgets(name, 11, stdin);
    // Remove the newline
    name[strlen(name) - 1] = '\0';

    return name;
}


/**********************************************************
 * Function: createServerStruct
 * Return: struct sockaddr_in
 * Args: char*, char*
 * Purpose: Create the server address struct to connect to
 **********************************************************/
struct sockaddr_in createServerStruct(char* host, char* port){
    struct sockaddr_in server;
    struct hostent* serverInfo;

    // Clear the struct
    memset((char*)&server, '\0', sizeof(server));
    // Get port number
    int portNum = atoi(port);
    // Create network socket
    server.sin_family = AF_INET;
    // Store port
    server.sin_port = htons(portNum);
    // Create hostent struct with info for provided IPv4 address
    serverInfo = gethostbyname(host);
    // Copy host info address into server struct
    memcpy((char*)&server.sin_addr.s_addr, (char*)serverInfo->h_addr, serverInfo->h_length);

    return server;
}


/****************************************************
 * Function: makeSocket
 * Return: int
 * Args: None
 * Purpose: Create a socket file descriptor
 ***************************************************/
int makeSocket(){
    int FD;
    FD = socket(AF_INET, SOCK_STREAM, 0);
    if(FD < 0){
        // Print error
        printf("Error making socket.\n");
	exit(1);
    }
    return FD;
}


/*************************************************
 * Function: connectServer
 * Return: None
 * Args: int, struct sockaddr_in*
 * Purpose: Connect to the given server
 ***********************************************/
void connectServer(int socketFD, struct sockaddr_in serverAddr){
    if(connect(socketFD, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0){
        //print error
        printf("Erorr connecting to server.\n");
        exit(1);
    }
    return;
}


/*****************************************************
 * Function: communicate
 * Return: None
 * Args: char*
 * Purpose: Back and forth communication with server
 ****************************************************/
void communicate(char * user, int FD){
    // Track chars sent
    int charsWritten;
    int charRead;

    // Dynamically create message array, 501 for null terminator
    char* message = malloc(501);
    char input[490];

    while(1){
        memset(message, '\0', sizeof(message));
        // Copy over handle
        strcpy(message, user);
        // Add on a prompt
        strcat(message, "> ");

        // Get message of user
        printf("%s", message);
        memset(input, '\0', sizeof(input));
        fgets(input, 490, stdin);
        // Remove newline
        input[strlen(input) - 1] = '\0';

        // Check if user wants to quit
        if(strcmp(input, "\\quit") == 0){
            break;
        }

        // Concate message and send
        strcat(message, input);    
        charsWritten = send(FD, message, strlen(message), 0);

        // Get message from server and handle
        memset(input, '\0', sizeof(input));
        memset(message, '\0', sizeof(message));
	charRead = recv(FD, message, 500, 0);
        charRead = recv(FD, input, 11, 0);

        // If server quits, exit
        if(charRead == 0){
            printf("Connection terminated by server.\n");
            break;
        }else{
            printf("%s%s\n", input, message);
        }
    }

    // Close socket when done
    close(FD); 
    // Free memory
    free(message);
    return;
}


int main(int argc, char *argv[]){

    // User handle, length 10 + null term
    char *handle;
    // Get user handle
    handle = getHandle();

    // Create server address information
    struct sockaddr_in server;
    server = createServerStruct(argv[1], argv[2]);

    // Create socket
    int socketDescriptor;
    socketDescriptor = makeSocket();
    
    // initiate contact with server
    connectServer(socketDescriptor, server);

    // Talk to server
    communicate(handle, socketDescriptor);

    return 0;
}
