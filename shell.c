#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void parseWhiteSpace(char *input, char **inputList);
int executeCommand(char *inputs[]);

int main(void) {
    char userInput[1024];
    char *ret;
    int pid;

    while (1) {
        printf("Enter a command: \n");
        ret = fgets(userInput, 1024, stdin);

        // Remove new line character
        userInput[strlen(userInput) - 1] = '\0'; // Googled how to remove last two characters from string, gemini reminded me of null terminator

        // if user entered EOF
        if (ret == NULL) {
            break;
        }

        else {
            int status;
            char *args[10];
            parseWhiteSpace(userInput, args);

            pid = fork();
            if (pid == 0) {
                executeCommand(args);
                // Call this break to exit out of the loop when execvp doesn't run to not get stuck in loop
                break;
            }
            else {
                wait(&status);
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

    word = strtok(userInput, " ");
    while (word != NULL) {
        args[index] = word;
        word = strtok(NULL, " ");
        index ++;
    }

    args[index] = NULL;
}

int executeCommand(char *inputs[]) {
    execvp(inputs[0], inputs);
    if (strlen(inputs[0]) > 0) {
        printf("execvp returned with errno: %d\n", errno);
    }

    return -1;
}