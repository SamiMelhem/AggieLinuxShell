#include <iostream>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <vector>
#include <string>
#include <ctime>
#include <time.h>
#include <cstring>
#include <fstream>
#include <fcntl.h>

#include "Tokenizer.h"

// all the basic colours for a shell prompt
#define RED     "\033[1;31m"
#define GREEN	"\033[1;32m"
#define YELLOW  "\033[1;33m"
#define BLUE	"\033[1;34m"
#define WHITE	"\033[1;37m"
#define NC      "\033[0m"

using namespace std;

int main () {
    // create copies of stdin/stdout; dup()
    int stdin = dup(0);
    int stdout = dup(1);

    vector<pid_t> pids; // to keep track of all the processes

    char currdir[100]; // current directory
    char prevdir[100]; // previous directory

    int status = 0; // status

    for (;;) {
        // Implement iteration over vector of bg pid (vector declared outside of the loop)
        // waitpid() - using flag for non-blocking 

        // implement date/time with time()+ctime()
        // implement username with getenv("USER")
        // implement curdir with getcwd()
        getcwd(currdir, sizeof(currdir));
        time_t rawtime;

        time (&rawtime);
        string time = ctime(&rawtime);
        time.replace(0, 4, "");
        time.replace(20, 1, " ");
        time.replace(16, 5, "");

        // need date/time, username, and absolute path to current dir (Look through the document)
        // cout << YELLOW << "Shell$" << NC << " ";
        cout << RED << time << getenv("USER") << ":" << currdir << "$" << NC << " ";
        
        // get user inputted command
        string input;
        getline(cin, input);

        if (input == "exit") {  // print exit message and break out of infinite loop
            cout << RED << "Now exiting shell..." << endl << "Goodbye" << NC << endl;
            break;
        }

        // get tokenized commands from user input
        Tokenizer tknr(input);
        if (tknr.hasError()) {  // continue to next prompt if input had an error
            continue;
        }

        // implement cd with chdir()
        // if dir (cd <dir>) is "-", then  go to the previous working directory
        // variable storing the previous working directory (it needs to be declared outside loop)
        if (tknr.commands[0]->args[0] == "cd"){
            int channel;
            char* components;
            if (tknr.commands[0]->args[1] == "-"){ // when you need to go prevdir
                getcwd(currdir, 100);
                cout << "curr directory: " << currdir << endl;
                cout << "prev directory: " << prevdir << endl;
                channel = chdir(prevdir);
            }
            else{
                components = new char[tknr.commands[0]->args[1].size() + 1];
                strcpy(components, tknr.commands[0]->args[1].c_str());
                channel = chdir(components);
                delete[] components;
            }
            if (channel < 0)
                cout << "chdir is not successful" << endl;
            else
                cout << "chdir is successful" << endl;
            memcpy(&prevdir, &currdir, sizeof(currdir));
        }
        else{
            // // print out every command token-by-token on individual lines
            // // prints to cerr to avoid influencing autograder
            // Uncomment when you understand how tokenizer
            for (auto cmd : tknr.commands) {
                for (auto str : cmd->args) {
                    cerr << "|" << str << "| ";
                }
                if (cmd->hasInput()) {
                    cerr << "in< " << cmd->in_file << " ";
                }
                if (cmd->hasOutput()) {
                    cerr << "out> " << cmd->out_file << " ";
                }
                cerr << endl;
            }

            // for piping
            // for cmd : commands
            //      call pipe to make pipe
            //      fork() - in child, redirect stdout, in par, redirect stdin (remember to close the file)
            //      ^ already written, just need to use it appropraitely
            // add checks for the first/last command

            int fd[2]; // file descriptors

            for (size_t i = 0; i < tknr.commands.size(); ++i){

                // create the pipe
                pipe(fd);

                // create the child to run the first command
                // fork to create child
                pid_t pid = fork();
                if (pid < 0) {  // error check
                    perror("fork");
                    exit(2);
                }

                // Add check for bg processes - add pid to vector if bg processes is running and don't waitpid() in par
                if (pid == 0) {  // if child, exec to run command
                    // redirect the output to the write end of the pipe
                    if (i < tknr.commands.size()-1)
                        dup2(fd[1], 1);
                    
                    // close the read part of the pipe
                    close(fd[0]);

                    // implement multiple arguments - iterate over args of current command to make
                    //      char* array
                    int arg_size = tknr.commands[i]->args.size();
                    char** args = new char*[arg_size+1];

                    for (int inner = 0; inner < arg_size; ++inner){
                        args[inner] = new char[tknr.commands[i]->args[inner].size() + 1];
                        strcpy(args[inner], tknr.commands[i]->args[inner].c_str());
                    }
                    args[arg_size] = nullptr;

                    // if current command is redirected, then open file and dup2 std(in/out) that's being redirected
                    // implement it safely for both at the same time
                    if (tknr.commands[i]->hasOutput()){
                        int file_descriptor = open((tknr.commands[i]->out_file).c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0600); 
                        dup2(file_descriptor, 1);
                    }
                    if (tknr.commands[i]->hasInput()){
                        int file_descriptor = open((tknr.commands[i]->in_file).c_str(), O_RDONLY, 0600); 
                        dup2(file_descriptor, 0);
                    }

                    // error check
                    if (execvp(args[0], args) < 0) {
                        perror("execvp");
                        exit(2);
                    }
                }
                else {  // if parent, wait for child to finish
                    signal(SIGCHLD, SIG_IGN);
                    // redirect the current input into the read part of the pipe
                    dup2(fd[0], 0);

                    // close the write part of the pipe
                    close(fd[1]);

                    if (tknr.commands[i]->isBackground())
                        pids.push_back(pid); 
                    else
                        waitpid(pid, &status, 0);

                    if (status > 1) {  // exit if child didn't exec properly
                        exit(status);
                    }
                }
                close(fd[0]);
                close(fd[1]);
            }
        }
        // restore stdin/stdout (variable would be outside the loop)
        dup2(stdin, 0);
        dup2(stdout, 1); 
    }
}
