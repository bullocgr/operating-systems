//Grace Bullock
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int main(int argc, char* argv[]) {
	//set the random seed
	srand(time(NULL));

	//variable declaration
	int random;
	int length = 0;
	char letters[27] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', ' '};
	
	//need argument to be greater than 2
	if(argc < 2) {
		fprintf(stderr, "Too few arguments\n" );
		exit(1);
	}

	//set the length to argv[1] because that's how many letters we want
	length = atoi(argv[1]);

	//randomly generate numbers in order to get the letter at the array letter
	int i;
	for(i = 0; i < length; i++) {
		random = rand()%27;
		fprintf(stdout, "%c", letters[random]);
	}

	//print the letters and end with a new line
	fprintf(stdout, "\n");

	return 0;
}