///////////////////////////
// Tanner Kvarfordt      //
// A02052217             //
// CS 3100 --Dr. Mathias //
// Assignment 3          //
///////////////////////////

#include "Shell.hpp"

Shell::Shell() :
    M_CMD_DELIMITER(" "), M_EXIT_CMD("exit"),
    M_HISTORY("history"), M_RUN_HISTORY("^"),
    M_PTIME("ptime"), M_CD("cd"),
    M_INCLUDE_RUN_HISTORY(false),
    M_PIPE_DELIM(" | "),
    M_HOME(this->getWD()),
    m_history(), m_child_time_total(0) {}

Shell::Shell(bool const & include_run_hist) :
    M_CMD_DELIMITER(" "), M_EXIT_CMD("exit"),
    M_HISTORY("history"), M_RUN_HISTORY("^"),
    M_PTIME("ptime"), M_CD("cd"),
    M_HOME(this->getWD()),
    M_PIPE_DELIM(" | "),
    M_INCLUDE_RUN_HISTORY(include_run_hist),
    m_history(), m_child_time_total(0) {}

Shell::Shell(bool const & include_run_hist, std::string const & run_hist_cmd) :
    M_CMD_DELIMITER(" "), M_EXIT_CMD("exit"),
    M_HISTORY("history"), M_RUN_HISTORY(run_hist_cmd),
    M_PTIME("ptime"), M_CD("cd"),
    M_HOME(this->getWD()),
    M_PIPE_DELIM(" | "),
    M_INCLUDE_RUN_HISTORY(include_run_hist),
    m_history(), m_child_time_total(0) {}

Shell::Shell(bool const & include_run_hist, std::string const & run_hist_cmd,
             std::string const & hist_cmd, std::string const & exit_cmd,
             std::string const & ptime, std::string const & pipe_delim, std::string const cmd_delim) :
    M_CMD_DELIMITER(cmd_delim), M_EXIT_CMD(exit_cmd),
    M_HISTORY(hist_cmd), M_RUN_HISTORY(run_hist_cmd),
    M_INCLUDE_RUN_HISTORY(include_run_hist), M_CD("cd"),
    M_HOME(this->getWD()),
    M_PIPE_DELIM(pipe_delim),
    M_PTIME(ptime) {}

// Checks if the user entered the @M_RUN_HISTORY command and if so,
// decides whether or not to include that command in history based on @M_INCLUDE_RUN_HISTORY
void Shell::filterHistory(std::vector<std::string> const & input_args) {
    if (!M_INCLUDE_RUN_HISTORY) {
        // If command is M_RUN_HISTORY, remove it from m_history to prevent confusion
        if (input_args.at(0) == M_RUN_HISTORY) m_history.pop_back();
    }
}

// Determines how to run the users unpiped input
void Shell::run_cmd(std::vector<std::string> const & input_args) {
    filterHistory(input_args);

    // Check if command is built-in
    if (!isBuiltIn(input_args)) {
        // Otherwise, send it to OS
        if (fork()) {
            // parent process executes here

            // wait for child process to terminate before continuing
            m_child_time_total += timeChild();
        } else {
            // child process executes here

            exec_cmd(input_args);
            // remove all chance of fork bomb
            exit(EXIT_SUCCESS);
        }
    }
}

// Determines how to run the users piped input
// Will not exit the Shell if the first command is exit
// @input is the entire string of commands input by the user
void Shell::run_piped_cmd(std::string const & input) {
    // vector containing each individual command in the pipe sequence and its arguments
    std::vector<std::string> pipe_chunks;
    parse_string(input, M_PIPE_DELIM, pipe_chunks);

    int savedStdout = dup(STDOUT);
    int savedStdin = dup(STDIN);

    // Each pipe (e.g. cat to grep and grep to cut) has two ends - in and out
    int num_pipe_ends = (pipe_chunks.size() - 1) * 2;
    int pipes[num_pipe_ends];


    for (int i = 0; i < num_pipe_ends / 2; ++i) {
        // store result to avoid compiler warning
        int res = pipe(pipes + (i * 2));
    }
    
    int read_end = 0;
    int write_end = 1;
    for (int i = 0; i < pipe_chunks.size(); ++i) {
        std::vector<std::string> input_args;
        parse_string(pipe_chunks.at(i), M_CMD_DELIMITER, input_args);
        if (fork() == 0) {
            if (i == 0) {
                // first time through the loop
                dup2(pipes[write_end], STDOUT);
                write_end += 2;
                for (int j = 0; j < num_pipe_ends; ++j) close(pipes[j]);
                exec_cmd(input_args);
            }
            else if (i == pipe_chunks.size() - 1) {
                // last time through the loop
                dup2(pipes[num_pipe_ends - 2], STDIN);
                for (int j = 0; j < num_pipe_ends; ++j) close(pipes[j]);
                exec_cmd(input_args);
            }
            else {
                dup2(pipes[read_end], STDIN);
                read_end += 2;
                dup2(pipes[write_end], STDOUT);
                write_end += 2;
                for (int j = 0; j < num_pipe_ends; ++j) close(pipes[j]);
                exec_cmd(input_args);
            }
        }

    }

    for (int j = 0; j < num_pipe_ends; ++j) close(pipes[j]);
    // wait for each child process
    for (int i = 0; i < pipe_chunks.size(); ++i) {
        // TODO: project hangs here while waiting for last child
        std::cout << "Waiting" << std::endl;
        m_child_time_total += timeChild();
    }

    dup2(savedStdout, STDOUT);
    dup2(savedStdin, STDIN);

}

void Shell::run() {

    // register signal SIGINT with signalHandler
    signal(SIGINT, Shell::signalHandler);

    std::vector<std::string> input_args;
    std::string input = "";
    while (true) {
        input_args.clear();
        input = prompt();
        if (input != "") {
            m_history.push_back(input);

            // check for pipes
            if (input.find(M_PIPE_DELIM) != std::string::npos) {
                run_piped_cmd(input);
            } else {
                parse_string(input, M_CMD_DELIMITER, input_args);
                if (input_args.size() > 0) {
                    if (input_args.at(0) == M_EXIT_CMD) return;
                    run_cmd(input_args);
                }
            }
        }
    }
}

// Prompts a user for input and returns the result
std::string Shell::prompt() const {
    std::cout << "[" << getWD() << "]: ";
    std::string input = "";
    std::getline(std::cin, input);
    return input;
}

// @input_args contains the user's command and all of its arguments.
// This function translates the command its arguments into cstrings,
// then passes them off to the OS via execvp to be executed
void Shell::exec_cmd(std::vector<std::string> const &input_args) const {
    char** argv = new char*[input_args.size() + 1];
    for (unsigned int i = 0; i < input_args.size(); ++i) {
        // cstrings require a null terminator, hence the + 1
        argv[i] = new char[input_args.at(i).length() + 1];
        strncpy(argv[i], input_args[i].c_str(), input_args[i].size() + 1);
    }
    // NULL to let execvp know there are no more args
    argv[input_args.size()] = NULL;
    execvp(argv[0], argv);
    std::cout << input_args.at(0) << ": command not found" << std::endl;
    // protect against fork bomb
    exit(EXIT_FAILURE);
}

// Prints the contents of m_history
void Shell::printHistory() const {
    std::cout << std::endl << "-- Command History --" << std::endl << std::endl;
    for (unsigned int i = 0; i < m_history.size(); ++i) {
        std::cout << std::setw(std::to_string(m_history.size()).length()) << i << " : " << m_history.at(i) << std::endl;
    }
    std::cout << std::endl;
}

// Runs the command in m_history[@entry]
// Returns the amount of time spent running a child process, if any
Duration Shell::runHistoryEntry(std::string const &entry) {
    // check for non-int type
    if (entry.find('.') != std::string::npos) return Duration(0);

    int entry_int;
    try {
        entry_int = std::stoi(entry);
        // check that history index is in range
        if (entry_int < 0 || entry_int > m_history.size() - 1) return Duration(0);
    } catch (std::exception e) {
        // invalid history index
        return Duration(0);
    }

    // parse the input_args from the history index
    std::vector<std::string> input_args;
    parse_string(m_history.at(entry_int), M_CMD_DELIMITER, input_args);

    // Check for built in
    if (isBuiltIn(input_args)) {
        return Duration(0);
    } else {
        // Otherwise, send it to the OS
        if (fork()) {
            // parent process executes here

            return timeChild();
        } else {
            // child process executes here

            exec_cmd(input_args);

            // protect against fork bomb
            exit(EXIT_FAILURE);
        }
    }
}

// Checks to see if the command in @input_args is built-in
// If so, it calls the appropriate function
// If not, it returns false
bool Shell::isBuiltIn(std::vector<std::string> const &input_args) {

    if (input_args.at(0) == M_CD) {
        changeDirectory(input_args);
        return true;
    }

    if (input_args.at(0) == M_HISTORY) {
        printHistory();
        return true;
    }
    if (input_args.at(0) == M_RUN_HISTORY && input_args.size() > 1) {
        m_child_time_total += runHistoryEntry(input_args.at(1));
        return true;
    }
    if (input_args.at(0) == M_PTIME) {
        printPtime();
        return true;
    }
    return false;
}

void Shell::printPtime() const {
    std::cout << std::endl << "Time spent executing child processes: "
              << std::fixed << std::setprecision(4)
              << m_child_time_total.count() << " seconds" << std::endl << std::endl;
}

// Returns the amount of time spent waiting on a child process to finish executing
Duration Shell::timeChild() const {
    Time_Point start = High_Res_Clock::now();
    wait(NULL);
    Time_Point end = High_Res_Clock::now();
    return end - start;
}

// Returns a string containing the current working directory
// or "cmd" in the event of a failure
// See http://pubs.opengroup.org/onlinepubs/009695399/functions/getcwd.html
std::string Shell::getWD() const {
    long size = pathconf(".", _PC_PATH_MAX);
    char * buf;
    std::string wd = "cmd";
    if ((buf = (char *)malloc((size_t)size)) != NULL) {
        wd = getcwd(buf, (size_t) size);
    }

    free(buf);

    // TODO: Wash hands cuz you touched 'malloc'
    // TODO: Pray to the goddess Mnemosyne that this isn't leaking memory

    return wd;
}

// Attempts to change the working directory to the directory provided in @input_args[1]
// If no argument is provided, attempts to change to @M_HOME directory
// See http://pubs.opengroup.org/onlinepubs/009695399/functions/chdir.html
void Shell::changeDirectory(std::vector<std::string> const & input_args) const {
    if (input_args.size() < 2) {
        // store result in an int to avoid compiler warning
        int ret = chdir(M_HOME.c_str());
        return;
    }
    // store result in an int to avoid compiler warning
    int ret = chdir(input_args.at(1).c_str());
}
