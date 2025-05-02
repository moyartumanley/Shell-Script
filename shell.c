#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>

void parseWhiteSpace(char *userInput, char **args, int *background);
int executeCommand(char *inputs[]);
void printIds();
void changeDirectory(char *args[]);
void foreground_sigint_handler(int sig);
void parent_sigint_handler(int sig);
int checkRedirection(char *args[]);
int checkPipe(char *args[]);
void redirect(char *args[], int redir);

// Global vars:
int foreground_pid = 0;

int main(void)
{
	char userInput[1024];
	char *ret;
	int pid;
	int child_pid;
	int status;

	// Ensures parent signal handler is in place before creation of child processes
	if (signal(SIGINT, parent_sigint_handler) == SIG_ERR)
	{
		printf("Error recieved during interruption: %d", errno);
		exit(1);
	}

	while (1)
	{

		// Reap zombie children
		while ((child_pid = waitpid(-1, &status, WNOHANG)) > 0)
		{
			printf("completed process %d with status %d\n", child_pid, status);
		}

		printf("\nEnter a command: \n");
		fflush(stdout);
		ret = fgets(userInput, 1024, stdin);

		// if (userInput[0] == '\n') {
		// 	exit(0);
		// }

		// EOF Handling:
		if (ret == NULL)
		{
			exit(0);
		}
		char *args[100];

		int background = 0;

		// if the user input is blank, just set the first character of args to an empty string.
		if (userInput[0] == '\n') {
			args[0] = "";
		}
		// if there is a user input, parse it
		else {
			parseWhiteSpace(userInput, args, &background);
		}

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

		// If not any of the built in commands, and not an empty input, try to execute
		else if (userInput[0] != '\n')
		{
			pid = fork();
			if (pid == -1)
			{
				printf("\n fork error: %d\n", errno);
			}
			else if (pid == 0) // Child process:
			{

				/*
				used resources:
				https://unix.stackexchange.com/questions/13724/file-descriptors-shell-scripting
				*/
				int pipeLoc = checkPipe(args);
				int pipefd[2];
				if (pipeLoc != -1)
				{
					args[pipeLoc] = NULL;
					pipe(pipefd);
					int pipePid = fork();
					if (pipePid != 0)
					{ // first process in pipe
						close(pipefd[0]);
						dup2(pipefd[1], STDOUT_FILENO);
						close(pipefd[1]);
						executeCommand(args);
					}
					else
					{ // second process in pipe
						close(pipefd[1]);
						dup2(pipefd[0], STDIN_FILENO);
						close(pipefd[0]);
						executeCommand(args + pipeLoc + 1);
					}
				}

				int redir = checkRedirection(args);
				if (redir >= 0)
				{
					redirect(args, redir);
				}
				executeCommand(args);
				/**
				 * Use default behavior if interrupt signal is recieved, which is termination:
				 * https://stackoverflow.com/questions/33922223/what-exactly-sig-dfl-do
				 */
				if (signal(SIGINT, SIG_DFL) == SIG_ERR)
				{
					printf("\nSignal interruption recieved with error %d\n", errno);
					exit(1); // exit if error is encountered
				}
			}
			else // Parent process:
			{
				if (!background)
				{ // Foreground process:
					foreground_pid = pid;
					if (signal(SIGINT, foreground_sigint_handler) == SIG_ERR)
					{
						printf("\nSIGINT recieved with error %d\n", errno);
						exit(1); // exit if error is encountered
					}

					// Wait for foreground process to complete
					waitpid(foreground_pid, &status, 0);
					foreground_pid = 0;

					// Reset parent signal handler once foreground process is completed
					if (signal(SIGINT, parent_sigint_handler) == SIG_ERR)
					{
						printf("\nParent SIGINT handler recieved with error %d\n", errno);
						exit(1);
					}
				}

				else // Background process:
				{
					printf("\nBackground process started with PID %d\n", pid);
				}
			}
		}
	}
	return 0;
}

/**
 * This function takes in the userInput string and a pointer to a list of strings, args.
 * It then populates the args with the space-separated values from input.
 */
void parseWhiteSpace(char *userInput, char **args, int *background)
{
	char *word;
	short index = 0;

	// Remove new line character
	// Googled how to remove last character from string, used Gemini's answer
	userInput[strlen(userInput) - 1] = '\0';
	
	// I looked at examples of strtok: https://www.geeksforgeeks.org/strtok-strtok_r-functions-c-examples/
	word = strtok(userInput, " ");
	
	while (word != NULL)
	{
		args[index] = word;
		word = strtok(NULL, " ");
		index++;
	}

	// if it is a background call, overwrite the & with NULL and set background to true
	if (strcmp(args[index - 1], "&") == 0)
	{
		*background = 1;
		args[index - 1] = NULL;
	}
	// otherwise, set background to false and add the NULL to the end

	else
	{
		*background = 0;
		args[index] = NULL;
	
	}

}

/**
 * This function takes an array of strings and then executes them.
 * It uses the first string as the file name, and the whole array as arguments.
 */
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

/**
 * Prints process ids
 */
void printIds()
{
	int pid = getpid();
	int ppid = getppid();

	printf("Child process id: %d\n Parent process id: %d\n", pid, ppid);
}

/**
 * Handles change of directory
 */
void changeDirectory(char *args[])
{
	// if the user specified the directory
	if (args[1])
	{
		int result = chdir(args[1]);
		if (result == -1)
		{
			printf("Change of directory returned with errno: %d\n", errno);
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
			printf("Change of directory returned with errno: %d\n", errno);
		}
	}
}

/**
 * Handles signal interruption. If the foreground_pid is a child, then we send the interrupt signal to it.
 * Otherwise, the signal is not interrupted.
 */
void foreground_sigint_handler(int sig)
{
	if (foreground_pid > 0)
	{
		printf("\nInterupting foreground process: %d\n", foreground_pid); // debugging print statement, can remove if u want
		kill(foreground_pid, SIGINT);
	}
	else
	{
		printf("\n");
	}
}

/**
 * Handles signal interruption for a parent process and prompts user for a new command.
 */
void parent_sigint_handler(int sig)
{
	printf("\n");
	fflush(stdout);
	printf("Enter a command: \n");
	fflush(stdout);
}

/**
 * Checks if any of the arguments indicate redirection, and returns the index of either > or <
 * If there is not a redirection character, it returns negative one
 */
int checkRedirection(char *args[])
{
	int i = 0;
	while (args[i] != NULL)
	{
		if (strcmp(args[i], "<") == 0 || strcmp(args[i], ">") == 0)
		{
			printf("Returned index: %d\n", i);
			return i;
		}
		i++;
	}
	return -1;
}

/**
 * Checks whether or not a pipe is present.
 */
int checkPipe(char *args[])
{
	int i = 0;
	while (args[i] != NULL)
	{
		if (strcmp(args[i], "|") == 0)
		{
			return i;
		}
		i++;
	}
	return -1;
}

/**
 * Takes in list of arguments (args) and index of redirection character (redir)
 * It dupes the output or input as indicated to the file name,
 * then overwrites the redirection character to null in order to effectively delete
 * both the redirection character and file name from arguments
 */
void redirect(char *args[], int redir)
{
	if (strcmp(args[redir], ">") == 0)
	{
		FILE *file = fopen(args[redir + 1], "w+");
		int fileNo = fileno(file);
		dup2(fileNo, STDOUT_FILENO);
	}
	else if (strcmp(args[redir], "<") == 0)
	{
		FILE *file = fopen(args[redir + 1], "r");
		int fileNo = fileno(file);
		dup2(fileNo, STDIN_FILENO);
	}

	/**
	https://stackoverflow.com/questions/11515399/implementing-shell-in-c-and-need-help-handling-input-output-redirection
	helped us realize to overwrite arguments to get rid of redirection and filename
	*/
	args[redir] = NULL;
}