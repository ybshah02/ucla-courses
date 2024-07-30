# You Spin Me Round Robin

The code creates a round robin process scheduler so that when we run proccesses on our computer, they can be scheduled for the CPU to run without conflicting the process executions. Does this through calculating average wait and response times from arrival and burst times. 

## Building

Run make to compile and create the rr.o executable object file.

## Running

Format of command: `./rr <input file> <quantum length>`
example: `./rr processes.txt 3`
expected: 
    Avg wait time: 7.00
    Avg response time: 2.75

## Cleaning up

Run make clean to clear executable and binary files. 
