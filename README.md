Introduction
This is a bare bones command shell supporting some UNIX commands.

Built-in commands:

ptime : total amount of time spent executing child processes (the shell is the parent/main process)

history : prints a numbered history of commands entered since launching the shell

`^ `<number>` : executes the corresponding command from history

exit : exits the shell

Assignment Description
(taken from Canvas)

A shell is a user program or environment provides for user interaction. The shell is a command language interpreter that accepts commands from standard input (the keyboard) and executes them. A shell may simply accept user commands and pass them off to the operating system to execute them, it may additionally provide mechanisms, such as redirecting the output of one program to the input of another.
Assignment

Write a C++ program that will act as a command line interpreter for Linux.  Use the [cmd]: string for your prompt. For example, when a user types a line such as

word1 word2 word3


...the shell interprets 'word1' as the name of an executable program and 'word2' and 'word3' as parameters to be passed to that program.  Your shell should accept any number of command line parameters.

By default, when the execvp system call is used, the folders specified in the PATH environment variable are searched to locate the executable file.  If the file is found, the shell should pass all parameters in argv and (attempt to) execute the program.  When the executable file has completed or terminated, the shell program should again resume with the next prompt.  Your shell program must wait to display the next command prompt until the previous command has finished executing.

The shell should track the amount of time spent executing child processes.  Whenever the command 'ptime' is entered into your shell, the number of seconds (display 4 digits past the decimal) spent executing (and waiting for) child processes must be displayed.

The shell should keep a history of the previous commands entered, with no maximum hard-coded history length.  If the user enters the command 'history', the shell should provide a listing of the complete command shell history.  If the user enters the command '^ 10', for example, the 10th command in the history buffer should be executed (a 1 based index).

The command 'exit' is used to terminate your shell program, when entered, your shell program should end.

The following is an example of what the command shell will look like and how programs will be executed.

    [cmd]: ls
          shell
          shell.cpp
    [cmd]: ptime
          Time spent executing child processes: 1.0402 seconds
    [cmd]: nano -S -T 4 shell.cpp
    [cmd]: history
    -- Command History --

    1 : ls
    2 : ptime
    3 : nano -S -T 4 shell.cpp

    [cmd]: 

To repeat, the following are the built in commands you need to provide:

    ptime
    history
    ^ <number>
    exit

Notes

    Create a CMakeLists.txt file that CMake can use to create a makefile that builds your program.
    You must use fork, execvp, wait, and std::chrono to complete this assignment.
    Reference for std::chrono : http://en.cppreference.com/w/cpp/chrono (Links to an external site.)Links to an external site.
    Your shell must accept any number of parameters, if the max number of parameters is hard-coded, at least 10% of your grade will be taken off.
    Your shell must not have any global variables, up to 10% taken off for global variables.
    When/if a bad command is entered, in addition to your shell not crashing, it should indicate this with an error message.
    Make good use of functions.  I see many students try and write the whole program in the main function; that really isn't a good idea for a lot of reasons.  It makes it harder to read, write, debug, and re-use code.

