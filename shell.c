#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

int main(void) {
    char userInput[1024];
    char *ret;
    int pid;
    int status;

    while (1) {
        printf("Enter a command: \n");
        ret = fgets(userInput, 1024, stdin);
        if (strlen(userInput) > 2) {
            userInput[strlen(userInput) - 1] = '\0'; // Googled how to remove last two characters from string, used gemini response
        }
        if (ret == NULL) {
            break;
        }
        else {
            pid = fork();
            if (pid == 0) {
                char *inputs[] = {userInput, NULL};
                execvp(inputs[0], inputs);
                if (strlen(userInput) > 2) {
                    printf("execvp returned with errno: %d\n", errno);
                }
            }
            else {
                wait(&status);
            }
        }
    }
    return 0;
}