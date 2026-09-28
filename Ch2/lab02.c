#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>     /* Provides fork(), write(), close(), and pid_t */
#include <fcntl.h>      /* Provides open() flags like O_CREAT, O_WRONLY */
#include <sys/wait.h>   /* Provides wait() and exit status macros */

int main(int argc, char *argv[]) {
    pid_t cpid;         /* Holds the Process ID returned by fork() */
    int status = 0;     /* Variable to catch the child's exit status report */
    int fd;             /* File descriptor integer for our output file */

    /* 1. Duplicate the current process into a Parent and a Child */
    cpid = fork();

    /* 2. Error Check: Verify if the kernel successfully created the clone */
    if (cpid < 0) {
        perror("fork failed"); // Prints system error message if fork fails
        exit(EXIT_FAILURE);    // Terminate program with an error code
    }

    /* 3. CHILD PROCESS BRANCH (Identified because fork() returns 0 to the child) */
    if (cpid == 0) {
        /* Open (or create) a text file named "output.txt" for writing only */
        fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        
        if (fd == -1) {
            perror("file open failed");
            exit(1);
        }

        /* Write a raw string directly to the file descriptor (no null-terminator search) */
        write(fd, "Hello from the child process!\n", 30);

        /* Close the file descriptor to free kernel resources */
        close(fd);

        /* CRITICAL: Always exit the child so it doesn't fall through to parent code */
        exit(0); 
    }

    /* 4. PARENT PROCESS BRANCH (fork() returns the child's actual PID to the parent) */
    else {
        /* Suspend execution and wait for the child process to finish its job */
        wait(&status);

        /* Check if the child exited normally via exit() or return */
        if (WIFEXITED(status)) {
            printf("Child finished successfully with exit code: %d\n", WEXITSTATUS(status));
        }

        exit(EXIT_SUCCESS);
    }
}