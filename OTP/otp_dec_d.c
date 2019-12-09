//Grace Bullock

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg) { fprintf(stderr, "%s\n", msg); exit(1); } // Error function used for reporting issues
void decrypt(char buffer[]);

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

			//fork so we can achieve what the assignment wants
			childPid = fork();
			switch(childPid) {
				case -1: {error("HULL BREAK!\n"); break;} //taken from notes

				case 0: //in the child

					//receive the length of the message + key
					recv(establishedConnectionFD, &lengthOfMessage, sizeof(int), 0);

					// Get the message from the client and display it
					int test = 0;
					//need a while loop to make sure we get all the contents of tht message
					memset(buffer, '\0', sizeof(buffer));
					while(test < lengthOfMessage) {
						charsRead = recv(establishedConnectionFD, buffer + test, lengthOfMessage - test, 0); // Read the client's message from the socket
						if (charsRead < 0) error("ERROR reading from socket");
						//this is to say where we left off during the reading
						test += charsRead;
					}

					//decrypt the message and store it to the buffer
					decrypt(buffer);

					//send the information back to the client
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
					//close the connection and wait but don't hang
					close(establishedConnectionFD); // Close the existing socket which is connected to the client
					pid_t actualPid = waitpid(childPid, &childExitStatus, WNOHANG);
					break;
			}
	}

	close(listenSocketFD); // Close the listening socket

	return 0; 
}

void decrypt(char buffer[]) {
	int i, j, temp1;
	char message[70000];
	char key[100000];
	char decryption[70000];

	//search for the special character and everything before it is the message
	char* token = strtok(buffer, "!");
	strcpy(message, token);

	//search for the second ! character and everything before it is the key
	token = strtok(NULL, "!");
	strcpy(key, token);

	//loop through lenght of message
	for(i = 0; i < strlen(message); i++) {	
		//subtract 64 from key to get the size of the alphabet
		temp1 = key[i] - 64;

		//subtract that number from the message at i and store it to decryption
		decryption[i] = message[i] - temp1;

		//if the value is less than the " " then add 26 to get the proper letter
		if(decryption[i] < 64) {
			decryption[i] += 26;
		}

		//if the character is an @ then change it to a space
		if(decryption[i] == 64) {
			decryption[i] = 32;
		}

		//was having a weird case of lowercase letters so i needed to fix it
		if(decryption[i] > 96 && decryption[i] < 123) {
			decryption[i] -= 32;
		}
	}

	//store into buffer
	strcpy(buffer, decryption);
}






















