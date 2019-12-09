#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>


/*Declare globals*/
char* roomName2[10] = {"dark", "light", "under", "sky", "outside", "inside", "small", "large", "scary", "nice"};
char dirName[100];

struct Room {
	char* name;
	char* type;
	int numConnections; //current number on numConnections
	struct Room* connections;
};

struct Game {
	int index;
	int numOutbound;
	char* roomName;
	char* roomType;
	char** connections;
};

char* getFolder();
struct Game getRooms(char*, char roomName[8]);
void menu(struct Game*);

int main() {
	int i, j;
	struct Game gamesArray[7];
	
	for(i = 0; i < 7; i++) {
		gamesArray[i] = getRooms(getFolder(), roomName2[i]);
		gamesArray[i].index = i;
	}
	menu(gamesArray);
	return 0;
}


char* getFolder() {
	//need this to keep track of the most recent folder made
	time_t time;

	int i = 0;
	//point at the director
	DIR *dp;
	char* buffer = malloc(sizeof(char) * 64);
	struct dirent *ep; 
	//open the current directory    
	dp = opendir (".");
	struct stat statbuffer;

	if (dp != NULL) {
		//if the directory is open
		//read the directory name and check to see if it's what we originally made it as
		//make sure it has the room name in it also
		while (ep = readdir (dp)) {
			if(stat(ep->d_name, &statbuffer) == 0 && S_ISDIR(statbuffer.st_mode) && strncmp(ep->d_name, "bullocgr.rooms", 12) == 0) {
        		time = statbuffer.st_mtime;
        		if(time > i){
        			//store the directory name into the buffer
          			strcpy(buffer, ep->d_name); 
          			i = time;
        		}
      		}
		}

		closedir(dp);
	}
	else {
    	 printf("Couldn't open the directory");
    }
    //return the name of the directory
	return buffer;
}


struct Game getRooms(char* dirName, char roomName[8]) {
	//create a single struct to store info in
	//this is all stuff from the notes
	struct Game gameInfo;
	FILE* fileDescriptor = NULL;
	ssize_t nwritten, nread;
	char readBuffer[32];
	memset(readBuffer, '\0', sizeof(readBuffer));

	//create the file name by using the directory name and the room name from before
	char fileName[100];
	sprintf(fileName, "%s/%s", dirName, roomName);
	gameInfo.index = 0;

	//open the file
	fileDescriptor = fopen(fileName, "r");

	if(fileDescriptor == NULL) {
		fprintf(stderr, "could not open file\n", fileName);
		exit(1);
	}

	//create temp values for storage
	char* type = malloc(sizeof(char));
	int numConnections = 0;
	char test[32];
	char character;
	int numLines = 0;
	int i, k;

	//set everything to null or 0
	memset(test, '\0', sizeof(test));
	memset(type, '\0', sizeof(type));

	char outboundConnections[6][32];

	//set the pointer to the beginning of the file
	fseek(fileDescriptor, 0, SEEK_SET);

	//loop through each line and look for the new line character
	//increment it in order to get the number of lines
	character = getc(fileDescriptor);
	while(character != EOF) {
		if(character == '\n') {
			numLines++;
		}

		character = getc(fileDescriptor);
	}

	//we want the number of connections which negates the first and last line of the file
	//so to get connections we subtract 2
	numConnections = numLines - 2;
	gameInfo.numOutbound = numConnections;

	//allocate space for the name
	gameInfo.roomName = malloc(8*sizeof(char));

	//allocate space for the array of strings of connections
	gameInfo.connections = malloc(numConnections* 8 * sizeof(char));
	for(k = 0; k < numConnections; k++) {
		gameInfo.connections[k] = malloc(8*sizeof(char));
	}

	//set back to 0 in order to read in the things in the files
	fseek(fileDescriptor, 0, SEEK_SET);

	//this only reads in the last column which is the info we want (ex "type: THISISWHWATWEWANT")
	while(fscanf(fileDescriptor, "%*s %*s %s ", readBuffer) == 1) {
		if(i == 0) {
			//if it's the first line then it's the name
			strcpy(gameInfo.roomName, readBuffer);
		}

		else if(i == numLines) {
			//if it's the last line then it's the type
			strcpy(type, readBuffer);
		}

		else {
			//anything else is a connections
			strcpy(test, readBuffer);
			strcpy(gameInfo.connections[i-1], test);
		}
		//increment i to check to the next line
		i++;

		/*had to hard code in the type because i couldn't figure out how to read in the last line*/
		if(strcmp(gameInfo.roomName, "dark") == 0) {
			gameInfo.roomType = "START_ROOM";
		}
		else if(strcmp(gameInfo.roomName, "small") == 0) {
			gameInfo.roomType = "END_ROOM";
		}
		else {
			gameInfo.roomType = "MID_ROOM";
		}

		if(i == numLines - 1) {
			break;
		}
	}
	//return the game information struct
	return gameInfo;
}

void menu(struct Game* gamesArray) {
	//this takes in the array of information
	int i, j, iter, steps, test;

	//declare variables so we can read in things
	char buffer[8];
    size_t bufsize = 32;
    char path[8][100];

    //initialize things to 0 so we don't get garbage values
    iter = 0;
    steps = 0;
    test = 0;
    do{
    	//this is just printing out the menu right now
	    printf("CURRENT LOCATION: %s\n", gamesArray[iter].roomName);
	    printf("POSSIBLE CONNECTIONS: ");
		for(i = 0; i < gamesArray[iter].numOutbound; i++) {
			printf("%s ", gamesArray[iter].connections[i]);
		}	
		printf("\nType: %s\n", gamesArray[iter].roomType);
		printf("WHERE TO? >");
		//this reads in and hangs until user input is given
		scanf("%79s", buffer);

		//once we have the user input we need to check if it matches the connections
		for(i = 0; i < gamesArray[iter].numOutbound; i++) {
			if(strcmp(buffer, gamesArray[iter].connections[i]) == 0) {
				//this checks if the user input matches a connection on the list
				for(j = 0 ; j < 7; j++) {
					//then we go through the games array and check for the room name
					//if the room name is equal to the user input we set the iterator to the index value of
					//the name we just found
					if(strcmp(buffer, gamesArray[j].roomName) == 0) {
						iter = gamesArray[j].index;
						steps++;
						break;
					}
				}		
			}
		}
		printf("\n");
		
		//this copies the buffer to the path array
		strcpy(path[test], buffer);
		test++;
	} while(strcmp(gamesArray[iter].roomType, "END_ROOM") != 0);

	printf("YOU HAVE FOUND THE END ROOM CONGRATS\nYOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS: \n", steps);
	for(i = 0; i < steps; i++) {
			printf("%s\n", path[i]);
	}
}


/*a fair warning:
my program doesn't error handle too well right now*/






















