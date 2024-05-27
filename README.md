# Aggie Linux Shell
## Overview
The Aggie Linux Shell is a custom-built shell designed to function similarly to Bash. It supports command execution, interprocess communication, and file input/output redirection. This project demonstrates my ability to implement complex shell features such as command pipelining, background processes, and input/output redirection.

## Features
- *Command Pipelining*: Allows chaining of commands where the output of one command serves as the input for the next.
- *Input/Output Redirection*: Supports redirecting input from and output to files.
- *Background Processes*: Runs processes in the background, allowing continued use of the shell.
- *Directory Commands*: Implements basic directory navigation commands like cd and pwd.
- *Custom Prompt*: Displays a custom shell prompt with the username, current date-time, and current working directory.
## Technologies
- *C++*: Primary programming language.
- *Linux System Calls*: Utilized for process control and file operations.
## Installation
To set up the Aggie Linux Shell on your local system, follow these steps:

```bash
git clone https://github.com/SamiMelhem/Aggie-Linux-Shell.git
cd Aggie-Linux-Shell
make
```
## Usage
Run the shell using the following command:

```bash
./aggie_shell
```
## File Descriptions
- *main.cpp*: The main script to launch the shell.
- *command.cpp*: Contains the implementation of command parsing and execution.
- *shell.cpp*: Handles the main shell loop and input processing.
