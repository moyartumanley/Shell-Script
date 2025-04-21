#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

void parseWhiteSpace(char *input, char **inputList);
int executeCommand(char *inputs[]);
void printIds();
void sigint_handler(int sig);

int main(void) {
    char userInput[1024];
    char *ret;
    int pid;

    while (1) {
        printf("Enter a command: \n");
        ret = fgets(userInput, 1024, stdin);

        // Remove new line character
        // Googled how to remove last two characters from string, gemini reminded me of null terminator
        userInput[strlen(userInput) - 1] = '\0'; 

        // if user entered EOF
        if (ret == NULL) {
            exit(0);
        }

        else {
            int status;
            char *args[100];
            parseWhiteSpace(userInput, args);

            // Handle built-in commands
            if (strcmp(args[0], "exit") == 0) {
                exit(0);
            }
            else if (strcmp(args[0], "myinfo") == 0) {
                printIds();
            }

			//TODO: Working on Task 6
			// else if (strcmp(args[0], "^C" == 0)){
			// 	signal(SIGINT, sigint_handler);
			// }
            else if (strcmp(args[0], "cd") == 0) {
                // if there is a second argument
                if (args[1]) {
                    chdir(args[1]);
                }
                // if the user just entered cd
                else {
                     // I used this resource in order to realize I should say "HOME" instead of "$HOME"
                     // https://pubs.opengroup.org/onlinepubs/009696899/functions/getenv.html
                    char *varName = getenv("HOME");
                    printf("%s\n", varName);
                    chdir(varName);
                }
            }

            // if not any of the built in commands, try to execute
            else {
                pid = fork();
                if (pid == 0) {
                    executeCommand(args);                
                }
                else {
                    wait(&status);
                }
            }
    
        }
    }
    return 0;
}

// This function takes in the userInput string and a pointer to a list of strings, args.
// It then populates the args with the space-separated values from input
void parseWhiteSpace(char *userInput, char **args) {

    char *word;
    short index = 0;

    // I looked at examples of strtok: https://www.geeksforgeeks.org/strtok-strtok_r-functions-c-examples/
    word = strtok(userInput, " ");
    while (word != NULL) { 
        args[index] = word;
        word = strtok(NULL, " ");
        index ++;
    }

    args[index] = NULL;
}

// This function takes an array of strings and then executes them
// it uses the first string as the file name, and the whole array as arguments
int executeCommand(char *inputs[]) {

    printf("%s\n", inputs[0]);
    int ret = execvp(inputs[0], inputs);
    if (strlen(inputs[0]) > 0) {
        printf("execvp returned with errno: %d\n", errno);
    }
    exit(ret);

    return 0;
}

void printIds() {
    int pid = getpid();
    int ppid = getppid();

    printf("Child process id: %d\n Parent process id: %d\n", pid, ppid);
}

//TODO: Work on for Task 6
void sigint_handler(int sig){
	exit(0);
}