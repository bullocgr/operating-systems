#include <stdio.h>     
#include <stdlib.h>    
#include <string.h>    
#include <errno.h>     
#include <fcntl.h>     
#include <limits.h>    
#include <signal.h>    
#include <sys/stat.h> 
#include <sys/types.h>
#include <sys/wait.h>  
#include <unistd.h> 

#define BUF_SIZE 2049



int showStatus(int);
void execArg(char* argv[], pid_t childPid, struct sigaction original, char inputFile[], char outputFile[], int* status, int* backgroundProcess, int* childExitMethod);

int main() {
	int status = 0;
	int childExitMethod = -5;
	pid_t childPid;
	int foregroundProcess;
	int backgroundProcess;
	char input[512];
	char* argv[512];
	size_t bufSize = BUF_SIZE;
	//input = (char *) malloc(BUF_SIZE * sizeof(char));
	struct sigaction original;
	struct sigaction newsig;
	int test;
	int j;
	int k;

	int quit = 0;

	int i;

	for (i = 0; i <= 512; i++){
		argv[i] = "";
	} 

	

	while(!quit) {
		i=0;
		j=0;
		k=0;
		test = 1;
		foregroundProcess = 1;
		backgroundProcess = 0;
		char inputFile[256] = "";
		char outputFile[256] = "";
		childExitMethod = -5;
		status = 0;

		printf(": ");
		fflush(stdout);

		fgets(input, 512, stdin);

		while(test == 1) {
			if(input[j] == '\n') {
				input[j] = '\0';
				test = 0;
			}
			j++;
		}
		

	    char* token = strtok(input, " ");

		while(token != NULL) {
			if(strcmp(token, "<") == 0) {
				token = strtok(NULL, " ");
				strcpy(inputFile, token);
			} else if(strcmp(token, ">") == 0) {
				token = strtok(NULL, " ");
				strcpy(outputFile, token);
			} else if(strcmp(token, "&") == 0) {
				foregroundProcess = 0;
				backgroundProcess = 1;
				break;
			} else {
				argv[i] = strdup(token);
				i++;
			}
			token = strtok(NULL, " ");
		}

		for(k = 0; k < 512; k++) {
			if(strcmp(argv[k], "") == 0) {
				argv[k] = NULL;
				break;
			}
		}
		

		if(input[0] == '#') {
	        ;
	    }

		else if(strncmp(input, "cd", 2) == 0) {
			char token = '\n';
			if(strtok(&input[3], &token) == 0) {
				chdir(getenv("HOME"));
			} else {
				chdir(argv[1]); //might have an issue here i'm not so sure yet
			}

		} else if(strncmp(input, "exit", 4) == 0) {
			exit(0);
		} else if(strncmp(input, "status", 6) == 0) {
			showStatus(childExitMethod);
		} else {
			childPid = fork();
			execArg(argv, childPid, original, inputFile, outputFile, &status, &backgroundProcess, &childExitMethod);
		}

		for(i = 0; i < 512; i++) {
			argv[i]="";
		}
		// strcpy(inputFile, "");
		// strcpy(outputFile, "");

	}






	// for (i = 0; i <= 512; i++){
	// 	free(argv[i]); 
	// } 

	return 0;
}


int showStatus(int status) {
	if(WIFEXITED(status)) {
		printf("terminated normally %i\n", WEXITSTATUS(status));
	} else {
		printf("terminated by signal %i\n", status);
	}
}


void execArg(char* argv[], pid_t childPid, struct sigaction original, char* inputFile, char* outputFile, int* status, int* backgroundProcess, int* childExitMethod) {
	int input, output, result; int i;
	switch(childPid) {
		case -1:
			printf("in case -1\n");
			perror("Hull Breach!\n");
			exit(1);
			break;

		case 0:

			// original.sa_handler = SIG_DFL;
			// sigaction(SIGINT, &original, NULL);

			if(strcmp(inputFile, "")) {
				input = open(inputFile, O_RDONLY, 0644);
				if(input == -1) {
					perror("Couldn't open input file\n");
					exit(1);
				}

				result = dup2(input, 0);
				if(result == -1) {
					perror("Couldn't assign input file\n");
					exit(2);
				}

				fcntl(input, F_SETFD, FD_CLOEXEC);
			}

			if(strcmp(outputFile, "")) {
				output = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
				if(output == -1) {
					perror("Couldn't open output file\n");
					exit(1);
				}

				result = dup2(output, 0);
				if(result == -1) {
					perror("Couldn't assign output file\n");
					exit(2);
				}				

				fcntl(output, F_SETFD, FD_CLOEXEC);
			}

			if(execvp(argv[0], argv) < 0) {
				printf("not a command\n");
				fflush(stdout);
				exit(1);
			}

			break;

		default:
			if(*backgroundProcess == 1) {
				waitpid(childPid, childExitMethod, WNOHANG);
				printf("Child pid: %d\n", childPid);
			} else {
				waitpid(childPid, childExitMethod, 0);

			}
	}
}





















