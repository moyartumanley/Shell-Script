#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

void parseWhiteSpace(char *userInput, char **args, int *background);
int executeCommand(char *inputs[]);
void printIds();
void changeDirectory(char *args[]);
void sigint_handler(int sig);

int main(void)
{
	char userInput[1024];
	char *ret;
	int pid;
	while (1)
	{
		printf("Enter a command: \n");
		ret = fgets(userInput, 1024, stdin);

		// Remove new line character
		// Googled how to remove last two characters from string, gemini reminded me of null terminator
		userInput[strlen(userInput) - 1] = '\0';

		// if user entered EOF
		if (ret == NULL)
		{
			exit(0);
		}

		// ensures handler is in place before creation of child processes
		if (signal(SIGINT, sigint_handler) == SIG_ERR)
		{
			printf("\nsignal interruption recieved with error %d\n", errno);
			exit(1); // exit if error is encountered
		}

		int status;
		char *args[100];

		int background = 0;

		parseWhiteSpace(userInput, args, &background);

		// Handle built-in commands
		if (strcmp(args[0], "exit") == 0)
		{
			exit(0);
		}
		else if (strcmp(args[0], "myinfo") == 0)
		{
			printIds();
		}

		else if (strcmp(args[0], "cd") == 0)
		{
			changeDirectory(args);
		}

		// if not any of the built in commands, try to execute
		else
		{
			pid = fork();
			if (pid == 0)
			{
				// child process
				executeCommand(args);
				// TODO: Working on Task 6
				if (getc(stdin) == SIGINT)
				{
					kill(pid, SIGINT);
				}
			}
			else
			{
				// parent process
				if (!background)
				{
					wait(&status);
				}
			}
		}
	}
	return 0;
}

// This function takes in the userInput string and a pointer to a list of strings, args.
// It then populates the args with the space-separated values from input
void parseWhiteSpace(char *userInput, char **args, int *background)
{

	char *word;
	short index = 0;

	// I looked at examples of strtok: https://www.geeksforgeeks.org/strtok-strtok_r-functions-c-examples/
	word = strtok(userInput, " ");
	while (word != NULL)
	{
		args[index] = word;
		word = strtok(NULL, " ");
		index++;
	}
	if (strcmp(args[index - 1], "&") == 0)
	{
		*background = 1;
		args[index - 1] = NULL;
	}
	else
	{
		*background = 0;
		args[index] = NULL;
	}
}

// This function takes an array of strings and then executes them
// it uses the first string as the file name, and the whole array as arguments
int executeCommand(char *inputs[])
{
	int ret = execvp(inputs[0], inputs);
	if (strlen(inputs[0]) > 0)
	{
		printf("execvp returned with errno: %d\n", errno);
	}
	exit(ret);

	return 0;
}

void printIds()
{
	int pid = getpid();
	int ppid = getppid();

	printf("Child process id: %d\n Parent process id: %d\n", pid, ppid);
}

void changeDirectory(char *args[])
{
	// if the user specified the directory
	if (args[1])
	{
		int result = chdir(args[1]);
		if (result == -1)
		{
			printf("change of directory returned with errno: %d\n", errno);
		}
	}
	// if the user just entered cd
	else
	{
		// I used this resource in order to realize I should say "HOME" instead of "$HOME"
		// https://pubs.opengroup.org/onlinepubs/009696899/functions/getenv.html
		char *varName = getenv("HOME");
		int result = chdir(varName);
		if (result == -1)
		{
			printf("change of directory returned with errno: %d\n", errno);
		}
	}
}

// TODO: Work on for Task 6
void sigint_handler(int sig)
{
	printf("\n SIGINT received, terminating child process.\n");
	exit(0);
}