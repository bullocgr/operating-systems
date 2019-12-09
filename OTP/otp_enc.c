//Grace Bullock
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(const char *msg) { fprintf(stderr, "%s\n", msg); exit(1); } // Error function used for reporting issues
int checkBadChar(char* message, char* key);
int checkLength(char* message, char* key);
void openMessageFile(char* messageFile, char buffer[180000]);
void openKeyFile(char* keyFile, char buffer[180000]);

int main(int argc, char *argv[])
{
	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[180000];
	char message[70000];
	char key[100000];
	char temp[180000];
    
	if (argc < 4) { fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); exit(0); } // Check usage & args

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) error("CLIENT: ERROR opening socket");
	
	//open the message file
	openMessageFile(argv[1], buffer);
	buffer[strcspn(buffer, "\n")] = '\0'; // Remove the trailing \n that fgets adds
	//copy the buffer into message 
	strcpy(message, buffer);
	//copy the message into temp
	strcpy(temp, buffer);
	//add a special character at the end of the message
	strcat(temp, "!");

	//do the same thing just with the key then put key and message into the temp file
	openKeyFile(argv[2], buffer);
	buffer[strcspn(buffer, "\n")] = '\0'; // Remove the trailing \n that fgets adds
	strcpy(key, buffer);
	strcat(temp, buffer);
	strcat(temp, "!");

	//check for bad chars in key and message
	if(checkBadChar(message, key)) {
		error("Bad char");
	}

	//check for proper message lengths
	if(checkLength(message, key)) {
		error("message is longer than key");
	}



	//Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		error("CLIENT: ERROR connecting");

	//get the length of the key and message
	int lengthSend = strlen(temp);
	send(socketFD, &lengthSend, sizeof(int), 0);

	int test = 0;
	// Send message to server
	while(test < lengthSend) {
		charsWritten = send(socketFD, temp + test, strlen(temp) - test, 0);	 // Write to the server
		if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
		if (charsWritten < strlen(temp)) printf("CLIENT: WARNING: Not all data written to socket!\n");
		//need to make sure that we remember where we left off
		test += charsWritten;
	}

	// Get return message from server
	test = 0;
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	while(test < strlen(message)) {
		charsRead = recv(socketFD, buffer + test, strlen(message) - test, 0); // Read data from the socket, leaving \0 at end
		if (charsRead < 0) error("CLIENT: ERROR reading from socket");
		test += charsRead;
	}
	printf("%s\n", buffer);

	close(socketFD); // Close the socket
	return 0;
}

int checkBadChar(char* message, char* key) {
	int i;
	for(i = 0; i < strlen(message); i++) {
		//check to make sure no bad chars in ascii
		if((message[i] < 65 && message[i] != 32) || (message[i] > 90)) {
			return 1;
			//return that there is a bad character
		}
	}

	//do same thing just for the key
	for(i = 0; i < strlen(key); i++) {
		if((key[i] < 65 && key[i] != 32) || (key[i] > 90)) {
			return 1;
		}
	}

	return 0;
}


int checkLength(char* message, char* key) {
	//check to see if the message is longer than the key and if it is return 1
	if(strlen(message) >= strlen(key)) {
		return 1;
	}

	return 0;
}

void openMessageFile(char* messageFile, char buffer[180000]) {
	int length;

	FILE* fileDescriptor = NULL;
	fileDescriptor = fopen(messageFile, "r");

	//error handle
	if(fileDescriptor == NULL) {
		fprintf(stderr, "could not open file\n", messageFile);
		exit(1);
	}
	//open file and store contents into buffer
	else {
		fseek (fileDescriptor, 0, SEEK_END);
		length = ftell (fileDescriptor);
		fseek (fileDescriptor, 0, SEEK_SET);
		if (buffer) {
			fread (buffer, 1, length, fileDescriptor);
		}
		fclose (fileDescriptor);
	}
}

void openKeyFile(char* keyFile, char buffer[180000]) {
	int length;

	FILE* fileDescriptor = NULL;
	fileDescriptor = fopen(keyFile, "r");

	//error handle
	if(fileDescriptor == NULL) {
		fprintf(stderr, "could not open file\n", keyFile);
		exit(1);
	}
	//open file and store contents into buffer
	else {
		fseek (fileDescriptor, 0, SEEK_END);
		length = ftell (fileDescriptor);
		fseek (fileDescriptor, 0, SEEK_SET);
		if (buffer) {
			fread (buffer, 1, length, fileDescriptor);
		}
		fclose (fileDescriptor);
	}
}
















