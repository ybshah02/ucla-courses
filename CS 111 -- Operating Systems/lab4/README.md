# Hey! I'm Filing Here

The code creates a file system using C

## Building

Run `make` to build and compile executable `ext2-create`

## Running

Run `./ext2-create` to run executable to create cs111-base.img
Run `mkdir mnt` to create a directory to mount filesystem onto
Run `sudo mount -o loop cs111-base.img mnt` to mount filesystem onto mnt

Running `ls -ail mnt` after doing the above yields:

total 7
     2 drwxr-xr-x 3 root  root  1024 June   3 18:13 .
942421 drwxr-xr-x 4 cs111 cs111 4096 June   3 18:13 ..
    13 lrwxr-xr-x 1 cs111 cs111   11 Jun    3 18:13 hello -> hello-world
    12 -rwxr-xr-x 1 cs111 cs111   12 Jun    3 18:13 hello-world
    11 drwxr-xr-x 1 root  root  1024 Jun    3 18:13 lost+found

## Cleaning up

Run `sudo unmount mnt` to unmount filesystem when finished
Run `rmdir mnt` to remove mnt directory used for mounting
Run `make clean` to clean the directory of the executable and object files
