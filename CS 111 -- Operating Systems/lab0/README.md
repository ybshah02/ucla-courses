# A Kernel Seedling

We use kernel commands in the proc directory to determine number of processors being ran in proc.

## Building

run in terminal: 
make

## Running

run terminal:
sudo insmod proc_count.ko
cat /proc/count

this prints a number of the total number of processes running in proc.

## Cleaning Up

run:
sudo rmmod proc_count
make clean

code should automatically remove the file afterward, but this is for additional verification. 

## Testing

Report which kernel release version you tested your module on
(hint: use `uname`, check for options with `man uname`).
It should match release numbers as seen on https://www.kernel.org/.

