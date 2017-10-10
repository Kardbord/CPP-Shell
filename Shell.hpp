///////////////////////////
// Tanner Kvarfordt      //
// A02052217             //
// CS 3100 --Dr. Mathias //
// Assignment 3          //
///////////////////////////

#ifndef SHELL_HPP
#define SHELL_HPP

#include "Utils.hpp" // parse_string()

#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <iomanip>
#include <exception>
#include <chrono>     // timing capabilities
#include <iomanip>    // setw()
#include <cstring>    // strcpy()
#include <unistd.h>   // execvp(), fork()
#include <sys/wait.h> // wait()

using Duration = std::chrono::duration<double>;
using High_Res_Clock = std::chrono::high_resolution_clock;
using Time_Point = std::chrono::time_point<High_Res_Clock>;

class Shell {
public:

    // ----------------------------- CONSTRUCTORS ---------------------------------------- //

    Shell();

    Shell(bool const & include_run_hist);

    Shell(bool const & include_run_hist, std::string const & run_hist_cmd);

    Shell(bool const & include_run_hist, std::string const & run_hist_cmd,
          std::string const & hist_cmd, std::string const & exit_cmd,
          std::string const & ptime, std::string const cmd_delim = " ");


    // ------------------------ PUBLIC MEMBER FUNCTIONS ---------------------------------- //

    // Runs the shell
    void run();


private:

    // ------------------------ VARIABLES AND CONSTANTS ---------------------------------- //

    // The delimiter to separate commands and parameters in the Shell
    const std::string M_CMD_DELIMITER;

    // The user command to exit the Shell
    const std::string M_EXIT_CMD;

    // The user command to display the current Shell session's command history
    const std::string M_HISTORY;

    // The user command to display the total time spent running child processes
    const std::string M_PTIME;

    // The user command to run a history item
    const std::string M_RUN_HISTORY;

    // If true, @M_RUN_HISTORY commands will also be included in history
    const bool M_INCLUDE_RUN_HISTORY;

    // An unparsed history of user commands for the session, regardless of command validity
    std::vector<std::string> m_history;

    // Total time spent executing child processes
    Duration m_child_time_total;


    // ------------------------ PRIVATE MEMBER FUNCTIONS ---------------------------------- //

    // Prompts a user for input and returns the result
    std::string prompt();

    // Prints a message displaying the amount of time (stored in @m_child_time_total) spent executing child processes
    void printPtime();

    // @input_args contains the user's command and all of its arguments.
    // This function translates the command its arguments into cstrings,
    // then passes them off to the OS via execvp to be executed
    void exec_cmd(std::vector<std::string> const &input_args);

    // Prints the contents of m_history
    void printHistory();

    // Runs the command in m_history[@entry]
    Duration runHistoryEntry(std::string const &entry);

    // Checks to see if the command in @input_args is built-in
    // If so, it calls the appropriate function
    // If not, it returns false
    bool isBuiltIn(std::vector<std::string> const &input_args);

    // Returns the amount of time spent waiting on a child process to finish executing
    Duration timeChild();

};

#endif
