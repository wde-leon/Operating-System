                                                                                                                                                                                                                                                                                                        48,1          All
  1 #include <stdio.h>
  2 #include <stdlib.h>
  3 #include <string.h>
  4 #include <unistd.h>      /* header file for the POSIX API */
  5 #include <time.h>        /* to write time */
  6 #include <sys/types.h>   /* standard data types for systems programming */
  7 #include <sys/file.h>    /* file-related system calls */
  8 #include <sys/wait.h>    /* for wait() system call */
  9 #include <signal.h>      /* signal handling */
 10 #include <errno.h>       /* for perror() call */
 11
 12 /*Use this a global file signal handler, to open the log file  */
 13 int log_fd;
 14
 15 /*SIGUSR1 handler for child*/
 16 void handler_sigusr1(int sig ){
 17     char *msg = "Signal\n";
 18     write(log_fd, msg, strlen(msg));
 19 }
 20
 21 /* main program and code begins, Before this you put all the functions going to use*/
 22 int main (int argc, char *argv[]){
 23
 24     pid_t cpid;
 25     int status = 0;
 26     struct sigaction sa;
 27     sigset_t block_mask, susp_mask;
 28
 29     /*1 Block all signals before fork*/
 30     sigfillset(&block_mask);
 31     sigprocmask(SIG_BLOCK, &block_mask, NULL);
 32
 33
 34     /*2.  Setting up SIGUSR1 handler*/
 35     sa.sa_handler = handler_sigusr1;
 36     //block signal while in the handler
 37     sigfillset(&sa.sa_mask);
 38     sa.sa_flags = 0;
 39     sigaction(SIGUSR1, &sa, NULL);
 40
 41     /*3 Start Fork process*/
 42     cpid = fork();
 43     if (cpid < 0){
 44         perror("Fork Error");
 45         exit(1);
 46     }
 47     /* 4.Set up Child process*/
 48     if (cpid == 0){
 49         //open log file for writing
 50     log_fd = open("log", O_WRONLY|O_CREAT|O_TRUNC, 0644);
 51     if (log_fd == -1){
 52        perror("Error opening file: ");
 53         exit(1);
 54     }
 55
 56     //Writing hello to the log file
 57     write(log_fd, "hello\n", 6);// the 6 is the bits setting aside for the hello
 58
 59     // Mask sigsuspend blocking everything except sigusr1
 60     sigfillset(&susp_mask);
 61     sigdelset(&susp_mask, SIGUSR1);
 62
 63     //Going to deep sleep and temp unblocking only SIGUSR1
 64     sigsuspend(&susp_mask);
5
 66     //Return here after SIGUSR1 was caught and handled
 67     write(log_fd, "world\n", 6);
 68     close(log_fd);
 69
 70
 71     exit(0) ;
 72   }
 73
 74     /* 5 Setting up Parent Process to kill the child process*/
 75
 76     else {
 77        // Kill the sigterm and sigusr1 using the kill command
 78         kill(cpid, SIGTERM);
 79         kill(cpid, SIGUSR1);
 80
 81         //Wait for the child process to finish. Harvet the exit code
 82         wait(&status);
 83         if (WIFEXITED(status)){
 84             printf("Child exited with code: %d\n", WEXITSTATUS(status));
 85         }
 86         exit(EXIT_SUCCESS);
 87     }
 88
 89
 90 }