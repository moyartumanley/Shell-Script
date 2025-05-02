**Names:** Jonah Zimmer, Moyartu Manley

## Tasks 1-5:
Portions completed:
* **Task 1:** No known bugs. User-input is read without error.
* **Task 2:** No known bugs. Processes are able to be ran and executed.`fork()` occurs without error.
* **Task 3:** No known bugs. Whitespace is able to be parsed within commands.
* **Task 4:** No known bugs. Multi-token commands such as `ls -a -l` are able to be executed without error.
* **Task 5:** No known bugs. Built-in commands such as `exit`, `myinfo`, `cd` and `cd <dir>` have been implemented and execute without error.
## Tasks 6-9:
Portions completed:
* **Task 6:** No known bugs. `CTRL+C` terminates foreground and background processes.
* **Task 7:** 
	* **Known bug:** When a background process is terminated with `CTRL+C`, the message:
	```completed process <PID> with status <STATUS>```
	 only appears after you enter a new command.
	* A background process is able to be created and ran. Zombie children are reaped at the beginning of the while loop.
* **Task 8:** No known bugs. Input and output redirection has been implemented.
* **Task 9:** No known bugs. Piping has been implemented.
## External Sources:
Google
* Jonah googled how to remove the last two characters from a string on line 53 of `./shell.c`

[File descriptors & shell scripting](https://unix.stackexchange.com/questions/13724/file-descriptors-shell-scripting)

* Source was used for piping implementation. Clarified understanding on file descriptors.

[What exactly SIG_DFL do?](https://stackoverflow.com/questions/33922223/what-exactly-sig-dfl-do)

* Source furthered understanding on signals. Allowed Moyartu to realize that the kernel's default response for SIGINT is termination. Thus, in some instances, a new handler would not have to be created/used when the default action already does what we want to achieve.

[strtok() and strtok_r() functions in C with examples](https://www.geeksforgeeks.org/strtok-strtok_r-functions-c-examples/)

* Source provided examples of `strtok()`, which was used in `parseWhiteSpace()`.

[The Open Group Base Specifications Issue 6 IEEE Std 1003.1, 2004 Edition](https://pubs.opengroup.org/onlinepubs/009696899/functions/getenv.html)

* Source allowed Jonah to realize that the built-in `cd` function should use `getenv("HOME")` rather than `getenv("$HOME")`.

[Implementing shell in C and need help handling input/output redirection](https://stackoverflow.com/questions/11515399/implementing-shell-in-c-and-need-help-handling-input-output-redirection)

* Source allowed us to realize that we should use `args[redir]=NULL;` to overwrite arguments and thus get rid of redirection and filename.