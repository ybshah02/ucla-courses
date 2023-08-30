# Hash Hash Hash

The code creates hash tables using two separate implementations that allow for concurrent addition of entries to a hash table without any collisions using the notion of locking calls. 

## Building

Run `make` in the lab3 directory to compile the c and object files to create the hash-table-tester executable. 

## Running

Run `./hash-table-tester -t 8 -s 50000`
Using my implementation, I get

Generation: 72,750 usec
Hash table base: 1,687,601 usec
    - 0 missing
Hash table v1: 1,539,592 usec
    - 0 missing
Hash table v2: 1,007,235 usec
    - 0 missing

## First Implementation

For hash table v1, I declared a mutex lock at initiation of the hash table and called the lock prior to adding any entries, and unlocking once each of the entries has been added in the add_entry function. 

This strategy works because the lock locks the entire function as a critical section, ensuring that only one thread can execute at a time during the function call. That means that the function is thread safe and cannot have collisions, and therefore no data race. 


### Performance

`./hash-table-tester -t 8 -s 50000`
v1: 1,539,592 usec
`./hash-table-tester -t 2 -s 200000`
v1: 1,969,074 usec

We notice that the call with a higher number of threads actually increases the speed of the implementation. That can be attributed to the fact that with more threads, there isn't as much time wasted in waiting for the table_add_entry function to complete, and so the thread is being utilized more efficiently. 

## Second Implementation

For hash table v2, I declared a mutex lock at initiation with each entry that is created inside of the hash table, and call the lock prior to that entry being added, and unlocking after that entry has been added. 

This strategy works because we create a lock at each separate entry, meaning that the only thread that is functioning is the thread that is unlocked by the current entry being added. Therefore the function is thread safe and none of the entries can collide with one another because the lock will properly lock and ensure critical section of adding the entry. 

### Performance

`./hash-table-tester -t 8 -s 50000`
v2: 1,007,235 usec
`./hash-table-tester -t 2 -s 200000`
v2: 994,006 usec

We notice that the call with the higher number of threads is around the same range in performance time. This could be that since the lock is created when each entry is created, it depends on the number of items in the hash table itself rather than the number of threads. 

we notice that v2 is faster than v1 because of concurrency and parallelism since the threads that operate work on other hash buckets with different locks, meaning that nodes can be added in parallel and without any data racing.

## Cleaning up

Run `make clean` to remove the executable and all binary files.
