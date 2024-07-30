# Pipe Up

This program simulates the system pipe functionality that's called by using the pipe operator '|' in between system calls to redirect standard output of one process to the standard input to the next process and onwards. 

## Building

cd into the lab1 folder
run ‘make’ to build the the executable pipe
run ‘./pipe’ preceded by arguments that are processes that can be executed

## Running

Arguments will be passed similarly to the native pipe functionality, however without the use of the pipe operator.

Running the following should yield us the same output: 		7	7	63
‘ls | cat | wc’
‘./pipe ls cat wc’

If there are errors like ./pipe is passed with no arguments or errors in the arguments that are passed, expected errors and their error messages will be returned after exiting.

## Cleaning up

run ‘make clean’ to delete all binary files
