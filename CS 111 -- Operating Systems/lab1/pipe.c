#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	// validate num arguments to pipe 	

    if (argc < 2) 
	{
        // only ./pipe is passed so do nothing
		return 0;
	}

    if (argc == 2)
    {
        // only one argument so execute that argument as normal
        execlp(argv[1], argv[1], NULL);
        return 0;
    }
    
	pid_t curr_pid;
	// loop through arguments
	int i;
	for (i = 1; i < argc-1; i++) 
	{ 
        int arr[2];
        // create pipe
        pipe(arr);
        // fork pipe
		curr_pid = fork();

        // child creation failed
		if (curr_pid < 0) 
		{
			perror("Fork failed.");
			exit(errno);
		}
        // in child process
		else if (curr_pid == 0) {
            // create duplicate and replace stdout with write end
			if (dup2(arr[1], STDOUT_FILENO) == -1)
            {
                perror("Duplicate failed.");
                exit(errno);
            }
            // run process
			if (execlp(argv[i], argv[i], NULL))
            {
                perror("Execution command failed.");
                exit(errno);
            }
		}

		else {
            // wait and check for orphan processes
			int status;
            wait(&status);
            if(WEXITSTATUS(status) != 0)
            {
                perror("Error in waiting for orphan processes.");
                exit(WEXITSTATUS(status));
            }
            // create duplicate and replace stdin with read end
			if(dup2(arr[0], STDIN_FILENO) == -1)
            {
                perror("Duplicate failed.");
                exit(errno);
            }
            // close pipes ends
			close(arr[0]);
            close(arr[1]);
		}
	}

    // execute last argument
    if(execlp(argv[i], argv[i], NULL))
    {
        perror("Execution command failed.");
        exit(errno);
    }
	
	return 0;
}
