#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>


/*Declare globals*/
char* roomNames[10] = {"dark", "light", "under", "sky", "outside", "inside", "small", "large", "scary", "nice"};
char* roomTypes[3] = {"START_ROOM", "END_ROOM", "MID_ROOM"};
char dirName[100];

struct Room {
	char* name;
	char* type;
	int numConnections; //current number on numConnections
	struct Room* connections;
};


char* makeFolder();
void makeFiles(struct Room, char*);
struct Room createRoom(int, int);
struct Room* GetRandomRoom(struct Room*);
void addRandomConnection(struct Room*);
int CanAddConnectionFrom(struct Room*);
int IsGraphFull(struct Room* roomsList);
void connectRoom(struct Room*, struct Room*);
int connectionAlreadyExists(struct Room*, struct Room*);
int isSameRoom(struct Room*, struct Room*);
void PrintRoomOutboundConnections(struct Room);

int main() {
	char* roomsFolder = makeFolder();

	srand(time(NULL));
	int randName;

	struct Room roomsArray[7];
	int i;

	//fill the rooms array
	for(i = 0; i < 7; i++) {
		randName = i;
		roomsArray[i] = createRoom(i, randName);
	}
	
	//make the connections
	while (IsGraphFull(roomsArray) == 0) {
		addRandomConnection(roomsArray);
	}

	//print and make the files
	for(i = 0; i < 7; i++) {
		// PrintRoomOutboundConnections(roomsArray[i]);
		makeFiles(roomsArray[i], roomsFolder);
	}

	//free the allocated memory
	for(i = 0; i < 7; i++) {
		free(roomsArray[i].connections);
	}


	return 0;
}



char* makeFolder() {
	//make the directory name and add the process id to the end of the string
	sprintf(dirName, "bullocgr.rooms.%d", getpid());
	mkdir(dirName, 0755);
	return dirName; 
}


void makeFiles(struct Room room, char* folder){
	//enter the folder we want to be in
  	chdir(folder);
	int i;

	//create a file with the room name and print to the file
	FILE* myFile = fopen(room.name, "a"); 
	fprintf(myFile, "ROOM NAME: %s\n", room.name);


	//add the room connections to the file
	//we use i + 1 to start at 1 not 0
	for(i = 0; i < room.numConnections; i++){ 
		fprintf(myFile, "CONNECTION %d: %s\n", i + 1, room.connections[i].name);
	}

	//add the types to the files
	fprintf(myFile, "ROOM TYPE: %s\n", room.type);
}


struct Room createRoom(int index, int randName) {
	int randConnections;
	struct Room room;
	int k;

	//random name
	room.name = roomNames[randName];

	//setting the type by checking if it's the first or last room in array
	if((index > 0) && (index < 6)) {
		room.type = roomTypes[2];
	}
	else if(index == 0) {
		room.type = roomTypes[0];
	}
	else {
		room.type = roomTypes[1];
	}

	//we need to allocate dynamic memory for the connections because of we don't know the number of connections
	for(k = 0; k < 7; k++) {
		room.connections = malloc(7* sizeof(struct Room));
	}
	return room;
}


void addRandomConnection(struct Room* roomsList) {
	struct Room* A;
	struct Room* B;
	int true = 1;

	while(true) {
		//set A to the random room
	    A = GetRandomRoom(roomsList);
  	    if (CanAddConnectionFrom(A) == 1)
      		break;
	}

	do {
    	B = GetRandomRoom(roomsList);
    } while(CanAddConnectionFrom(B) == 0 || isSameRoom(A, B) == 1 || connectionAlreadyExists(A, B) == 1);

	connectRoom(A, B);  // TODO: Add this connection to the real variables, 
	connectRoom(B, A); //  because this initialRoom and B will be destroyed when this function terminates
}


struct Room* GetRandomRoom(struct Room* roomsList) {
	//get a random index and return that room
	int randRoom;
	randRoom = rand() % 7;
	return &roomsList[randRoom];
}


int CanAddConnectionFrom(struct Room* initialRoom) {
	/*0 is false
	1 is true*/

	int num = initialRoom->numConnections;
	int i;

	//check if the value is less than 6 and if it is then it can add a room
	if(num < 6) {
		return 1;
	}
	
	return 0;
}


int IsGraphFull(struct Room* roomsList) {
	int i;
	for(i = 0; i < 7; i++) {
		//checks if the num of connections is less than 3
		//if it is less than three return false because it needs to be greater than 3
		if(roomsList[i].numConnections < 3)
			return 0;
	}

	return 1;
}


void connectRoom(struct Room* initial, struct Room* connection) {
	//get to the last index in connections by using initial->num connections
	//set that equal to the pointer to the room that we want to connect to
	//increase the number of connections after the connection is set
	initial->connections[initial->numConnections] = *connection;
	initial->numConnections = initial->numConnections+1;
}


int connectionAlreadyExists(struct Room* x, struct Room* y) {
	int i, k;
	for(i = 0; i < x->numConnections; i++) {
		//access the connection we are looking to make
		//if the name of the connection is the same as the connection we want to make then the connection already exists and we return true
		if(x->connections[i].name == y->name) {
			return 1;
		}
	}
	return 0;
}


int isSameRoom(struct Room* x, struct Room* y) {
	//compare the name of the two rooms to see if they are the same
	if(x->name == y->name) {
		return 1;
	}
	return 0;
}


//print function given to us
void PrintRoomOutboundConnections(struct Room input){
  printf("The rooms connected to (%s) are:\n",
         input.name);

  int i;
  for (i = 0; i < input.numConnections; i++)
    printf("  (%s)\n", input.connections[i].name);
  return;
}

















