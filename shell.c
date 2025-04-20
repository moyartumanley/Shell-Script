#include <stdio.h>

int main(void) {
    char userInput[1024];
    char running = 1;
    char *ret;

    while (running) {
        printf("Enter a command: \n");
        ret = fgets(userInput, 1024, stdin);
        printf("You typed: %s", userInput);
        if (ret == NULL) {
            running = 0;
        }
    }
    return 0;
}