[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/76mHqLr5)
# Description

This is a simple shell implementation in C. It supports the following features:

- Running commands with arguments
- Background processes
- Input and output redirection
- Piping
- History
- Signal Handling
- System Commands
- Change Process State to Foreground and Background
- Man Pages
- Neonate Command

## Commands

### warp
Change the current working directory to the specified directory. If no directory is specified, change to the home directory which is the directory from which the shell was started. also supports **"~"** to change to the home directory. it also supports **..** , **.** and **-** to change the directory.

### peek
Prints list of all the files and directories in the current directory in lexicographic order. It also supports the **-a** flag to print all the files and directories including the hidden files and directories. It also supports the **-l** flag to print the files and directories in the long format. It also supports the **-al** and **-la** flag to print all the files and directories in the long format. It also supports **.** , **..** and **~** to print the files and directories in the corresponding directory.

### pastevents
Prints the last 15 commands that were executed in the shell. It also supports **pastevents execute &lt;number&gt;** to execute the pastevents command corresponding to the number. It also supports **pastevents purge** to clear the pastevents.

### system commands

- **foreground** - Run the command in the foreground. if process takes greater than 2 seconds then the time taken by the process will be printed in the prompt.
- **background** - Run the command in the background. if process completly then the command exited normally else the command exited abnormally will be printed. 

### proclore
Prints the process corresponding to the pid. It also supports **proclore &lt;pid&gt;** to print the process corresponding to the pid. if the pid is not specified then it will print the process corresponding to the shell. if the process is foreground then + will be appended to the process state.

### seek

seek command looks for a directory and files in a given path. if No path is given then it will find in current working directory. It also supports **-d** and **-f** flags which finds the directory and files only. it also supports the **-e** flag which is effective when only one file or directory is found. if directory is found it will change the current working directory to that directory. if file is found it will print the content of that file.

### activities

Prints the list of all the activities that are currently running and spawaned by the shell in lexiographic order. This contains Command name, pid and state (either running or stopped)

### ping

Send the signals to processes. It also handles the **CTRL + C** , **CTRL + D** and **CTRL + Z** signals.

### fg and bg

- **fg** - Change the state of the process to foreground. if **pid** is not available then it will print the error message.
- **bg** - Change the state of the stopped process to background. if **pid** is not available then it will print the error message.

### neonate

neonate will print the recently created process pid at every time_arg seconds. if key **'x'** is pressed then it will stop printing and terminate the process.

### iMan 

It will print the manual page of the command. if the command is not available then it will print the error message. The man page is fetch from the website **"man.he.net"**.

### exit

Kill all the background processes and exit the shell.

## Features

### Input and Output Redirection

- **&gt;** - Redirect the output of the command to the file. if the file is not available then it will create the file and redirect the output to the file. if the file is available then it will overwrite the content of the file.
- **&gt;&gt;** - Redirect the output of the command to the file. if the file is not available then it will create the file and redirect the output to the file. if the file is available then it will append the content to the file.
- **&lt;** - Redirect the input of the command to the file. if the file is not available then it will print the error message.

### Piping

- **|** - Pipe the output of the command to the input of the command. if the command is not available then it will print the error message. also if there is nothing to left and right of the pipe then it will print the error message.

### Background Processes

- **&** - Run the command in the background. if the command is not available then it will print the error message.


## How to run

```bash
make
./a.out
```



# Assumptions

- In the pastevents command, if the input contains the **"pastevents"**, then that command will not be stored into pastevents. but if the **"pastevents execute"** is inputted, then the corresponding command will be stored into pastevents if it not same as the last command in pastevents.

- In the Background Process, if it contains some **STDOUTPUT**, then it will be printed out in the terminal. To get the Prompt back, you need to press **Enter**.

- In the Background Process, if it contains some **Error Message**, then it will be printed out in the terminal. After you press **Enter**, it will print command exited normally and the Prompt will be back.

- In the Proclore command to handle the pid which corresponds to the system process the programme should be run as root.

- In the Proclore command, if the input is not a number, then it will print out the error message and the Prompt will be back.

- In the Redirection, There must be a space between the command, the redirection symbol and the file name. For example, **"ls > test.txt"** is correct, but **"ls>test.txt"** is not correct.

- In activities command, to Print lexiographic order, i have considered that Lexiographic means it is sorted in the increasing order of the ASCII value of the characters.

- In the system command implementation, if that command is not handled by the execvp, then that command will not run. For example, if you input **"echo "Lorem Ipsum" | wc | sed 's/ //g' "**, then the **"sed 's/ //g' "** will not run.

- In the command separated by pipes if the **" & "** is present then the command corresponding to that will be run in the background, all the other commands will be run in the foreground.

- **CTRL + D** won't work while running foreground processes.

- in the Neonate command, the signal **CTRL + C** and **CTRL + Z** will not work.

