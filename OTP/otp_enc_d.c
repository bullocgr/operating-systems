//Grace Bullock
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg) { fprintf(stderr, "%s\n", msg); exit(1); } // Error function used for reporting issues
void encrypt(char buffer[]);

int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead, charsWritten;
	socklen_t sizeOfClientInfo;
	char buffer[180000];
	struct sockaddr_in serverAddress, clientAddress;
	int lengthOfMessage;


	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("ERROR on binding");
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections


	while(1) {

			// Accept a connection, blocking if one is not available until one connects
			sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
			establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
			if (establishedConnectionFD < 0) error("ERROR on accept");

			pid_t childPid = -5;
			int childExitStatus = -5;

			//fork for the assignment
			childPid = fork();
			switch(childPid) {
				case -1: {error("HULL BREAK!\n"); break;}

				case 0: //in child
					recv(establishedConnectionFD, &lengthOfMessage, sizeof(int), 0);

					// Get the message from the client and display it
					int test = 0;
					//need to have this in order to send all the contents of the message
					memset(buffer, '\0', sizeof(buffer));
					while(test < lengthOfMessage) {
						charsRead = recv(establishedConnectionFD, buffer + test, lengthOfMessage - test, 0); // Read the client's message from the socket
						if (charsRead < 0) error("ERROR reading from socket");
						test += charsRead;
						//remember where we left of when receiving the message
					}

					//encrypt the message
					encrypt(buffer);

					//send the encrypted message
					test = 0;
					while(test < strlen(buffer)) {
						charsWritten = send(establishedConnectionFD, buffer + test, strlen(buffer) - test, 0);	 // Write to the server
						if (charsWritten < 0) error("SERVER: ERROR writing to socket");
						if (charsWritten < strlen(buffer)) printf("SERVER: WARNING: Not all data written to socket!\n");
						test += charsWritten;
					}
					exit(0);
					break;
				

				default: //in the parent
					//close the connection
					close(establishedConnectionFD); // Close the existing socket which is connected to the client
					pid_t actualPid = waitpid(childPid, &childExitStatus, WNOHANG);
					break;
			}
	}

	close(listenSocketFD); // Close the listening socket

	return 0; 
}

void encrypt(char buffer[]) {
	int i, j;
	char message[70000];
	char key[100000];
	char encryption[70000];

	//look for the special character and everything before it is the message	
	char* token = strtok(buffer, "!");
	strcpy(message, token);

	//look for the second special character and everything before it is the key
	token = strtok(NULL, "!");
	strcpy(key, token);

	//loop through the length of the message
	for(i = 0; i < strlen(message); i++) {
		//if the message is a space then change it to the @ symbol
		if(message[i] == 32) {
			//do the same for the key
			message[i] = 64;
		}

		//check for the space and change it
		if(key[i] == 32) {
			key[i] = 64;
		}

		//subtract 64 to get the alphabet and add it to the message
		encryption[i] = message[i] + (key[i] - 64);

		//if the encryption is above 90 subtract 26 to get the proper letter
		if(encryption[i] > 90) {
			encryption[i] -= 26;
		}

		//if the ecryption is the @ sign turn it into a space
		if(encryption[i] == 64) {
			encryption[i] = 32;
		}

	}
	//store in buffer
	strcpy(buffer, encryption);
}






















