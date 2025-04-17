# Writing a Shell

## Overview

In this assignment, you will create a program, `shell.c`, which implements a
command-line interface for running programs, similar to bash, which you
have been using as the terminal for this course.

Your program will need to loop, reading commands from the user, and use C
methods `fork()` and `exec()` to carry out those instructions. You will need to
handle common errors without exiting the program, like a real shell.

You may work with a partner for this project, though it is not required.

This assignment is meant to stretch your coding ability--it is intentionally
challenging! You can use outside sources for help: google for example code, look
at man pages. But **you must credit your sources in the comments** and **you
must write up the code on your own**. You may **not** copy code from external
sources verbatim. A good general rule is that if you do seek external help, you
should cite your source and write up pseudocode as a comment, then adapt that
pseudocode to real code.

### Rubric

* To receive 85/100:
    - Complete steps 1 through 5 below
    - Create the required code files with good style, comments, function decomposition, etc.
    - Complete the report as described for steps 1 through 5
* To receive 100/100:
    - Complete steps 1 through 9 below
    - Create the required code files with good style, comments, function decomposition, etc.
    - Complete the report as described


### Starter Files/ Deliverables

We are not giving you any starter code files for this assignment. Just this 
`README` to describe what to do. Instead, here we will list your "deliverables,"
what you need to produce for this assignment.

You should deliver:
* `shell.c`, which contains the C source code for your shell
* `Makefile`, which will `make` and `make clean` your source code
* `README.md`, a new README that contains:
    * Your name and your partner's name, if any
    * A list of all the portions of the shell you were asked to complete, and
      in which you've clearly indicated which parts you have completed and which
      parts you have not
    * Any known bugs/errors with your code
    * Include links to and descriptions of any external sources from which you
      got significant help (this is in **addition** to including those links and
      pseudocode in the code files)
    * Start this as a separate Markdown file with a different name, and then move
    and rename them before pushing the final work

## Problem Description

This assignment has multiple parts that you will need to implement for full
credit, each described below. 

If you complete the first five parts successfully, you will receive a B
(85/100) on this assignment. To receive a 100/100, you will need to complete 
everything described below. We want you to prioritize--complete as much as you
can, and be willing to accept the B if you need more time for other
end-of-semester considerations!

Each task includes a reference to the C function and/or Unix system call that
could help you implement that task. You can use these to find the man page. For
example, bring up the man page for `fork(2)` by typing the following in the
command line:

```
man 2 fork
```
Man pages can also be found online; just make sure to access the correct form of each
system call.

If you get stuck on a particular step, you can try faking it and demonstrating
that the rest is working. For example, if you aren't able to tokenize user input
in step 3, you might hard code a pre-tokenized array to demonstrate your
completion of step 4. 

**When handing in the final product, you must clearly indicate any remaining 
"faked" portions or bugs that you have not been able to resolve!**

## Tasks

Before getting into the tasks, here are some guidelines to keep in mind.

### Programming Notes

* Your program needs to handle input lines up to 1024 characters
* You may assume there are no more than 100 tokens (words) on a line
* You may assume there are no more than 10 command segments on the command line
* You may assume/limit the number of background processes

### Error handling

Your program will need to handle the following errors. Note that some of these
errors only become possible after you have implemented the corresponding task;
you only have to handle errors for tasks that you have completed. The first two
errors should be simple to handle, but the last two may result in some processes
running, depending on your implementation of piping.

* Command not found on the path
* Input redirected in from a non-existent file
* Input redirected in to a pipe command that is not the first command
* Output redirected from a pipe command that is not the last command

### Working with environment variables

For the below tasks, you might find it useful to get and set environment variables such as `$PATH` or `$PWD`.

Reference: `getenv(3)`

### Task 1. Read user input

Inside a loop in main(), present a prompt to the user and then read a full
line of input (up to and including the newline character). Continue looping
until `EOF` is reached. You can send the `EOF` character by entering Ctrl-D on
the command line.

Reference: `fgets(3)`

### Task 2. Run a simple process

Run a single word command line (e.g., `ls`) by forking a subprocess and
executing it. You may assume the word is at the start of the line and that there
is no extraneous whitespace. Your main loop should wait until the process
completes before continuing. 

Handle gracefully the case where the user types Enter/Return without typing any 
command: do nothing and go on to the next input prompt.

Reference: `fork(2)`, `execvp(2)`, `waitpid(2)`

### Task 3. Parse whitespace in commands

Parse a command line with whitespace into a vector of strings. 

For example, given this string:

    "foo |    bar > baz"
        
Your program will produce an array, `args`, containing the following:

```
args[0] = "foo";
args[1] = "|";
args[2] = "bar"
args[3] = ">";
args[4] = "baz";
args[5] = NULL;
```

This part of the assignment can be  quite difficult, so be sure to test your
work thoroughly.

For an **optional** additional challenge, handle parsing a command line where these symbols, > < & |,
may not be separated from the tokens around them with spaces. For example:
```
ls -a|grep foo>foo.txt
```

Reference: `strtok(3)`, `strchr(3)`, `strsep(3)`, `strpbrk(3)`

### Task 4. Run multi-token commands

Take the `args` array created in task 3, above, and run the
program specified. Your shell should wait until the program completes before
displaying the next command prompt. I recommend creating a function that
executes the program and returns the child process ID (PID). 

Note that supporting piping and redirection are additional tasks for this
assignment--for this task, you only have to support commands with more than one
token, so being able to run a program that takes an argument is sufficient for
this. I suggest you try `sort`, `cat`, `ls -a -l`, or similar commands for testing.

Reference: `fork(2)`, `execvp(3)`, `waitpid(2)`

### Task 5. Built-in commands

Implement the following built-in commands. This means that you should not run
them as external processes, but rather capture the command and use your own code
to execute the described functionality.

* `exit` - terminate your shell input loop
* `myinfo` - print out your PID and PPID (parent process ID--the ID of the
  process that started this process) in a readable format
* `cd` - change your working directory to `$HOME`
* `cd <dir>` - change your working directory to `<dir>`

Reference: `chdir(2)`, `getenv(3)`, `getpid(2)`, `getppid(2)`

### Task 6. Trap `SIGINT`

Trap `SIGINT` (triggered when the user enters Ctrl-C at the terminal) and have
it terminate the currently running child process of the shell, if any, and **not
the shell itself**.

**Note:** for this to work correctly, make sure your Makefile is **not** specifying the use of the C99 standard!

Reference: `signal(2)`, `kill(2)`, `getc(3)`

### Task 7. Run background processes

If the last token on the command line is `&`, have that program run in the
background instead of the foreground. This means that the shell program does not wait
for the child to return, instead it just continues on right away.

This task requires you to do the following:
* After creating the `args` array, check to see if the last token is `&`. If so:
    - Set a flag variable or somehow signal that this command is to be run as a background process
    - Remove the `&` token from the `args` array
* In the parent process code after performing a `fork` command:
    - If the current command is a background one, then don't call `waitpid`
* At the start of the loop in `main`, before displaying the user prompt, handle background child processes:
    - Check if any children have completed their execution and 'died'--these are
  known as zombie processes.
    - Ensure that any zombie children have been reaped
    - Display a message to the user that the processes have been reaped

Reference: `waitpid(2)` with a PID of -1 and the `WNOHANG` option

As an **optional** additional challenge, add in support to trap `SIGTSTP` (Ctrl-Z) and have
it change the child program that is currently running into a background process.

### Task 8. Input and output redirection

Support `<` and `>` for input and output redirection to the program. Do this by
opening the requested file for read/write as needed and duping it to
`STDIN_FILENO` or `STDOUT_FILENO`. 

Reference: `open(2)` or `fopen(3)`/`fileno(3)`, `dup2(2)`

### Task 9. Piping

Support command line piping with a single pipe (the `|` character, on the same
key as the backslash, above the return/enter key on most keyboards). For
example, given the following command:

    foo | bar
        
Your shell should set `STDIN_FILENO` of bar to read from the `STDOUT_FILENO` of
foo. You can do this by using `pipe(2)` to create connected pairs of file
descriptors and then use `dup2(2)` to set them up appropriately in the children.
Close the unused ends (e.g., foo should close the descriptor that bar is using
to read). Your shell will need to execute both processes and wait for both of
them to return. Note that you can change the `|` in your argument vector to a
NULL and just pass in different starting locations of the same vector to your
execution function. You may want to have a separate array that keeps track of
the start index of various command segments. 

Reference: `dup2(2)`, `pipe(2)`

## More Optional Challenges

The following are challenge problems. Do not attempt them unless you have
completed all other parts of the assignment.

* Add support for setting and modifying environment variables
* Support a multi-pipe sequence

## Thanks

Thanks to Professor Ben Kuperman for developing the original version of this
assignment and Professor Roberto Hoyle for his additional modifications.
