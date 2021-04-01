/*
  Filename   : MySystem.c
  Author     : Lauren Deaver
  Course     : CSCI 380-01
  Assignment : Assignment 7: Redirection
  Description: writing our own version of Linux system call
                where we write the output of the command into
                a file name System.out and only print the
                exit status after completion
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <assert.h>
#include <fcntl.h>

int
mysystem (char* command);

int
main(int argc, char **argv)
{
    /*Check required command line args*/
    if (argc != 2)
    {
        printf("usage <filename> 'command', ");
        if (argc < 2)
        {
            printf("too few args\n");
        }
        else
        {
            printf("too many args\n");
        }
        
        exit(0);
    }
    mysystem(argv[1]);

}

/*Runs like linux system call but prints output of
    command to file*/
int
mysystem (char* command)
{
    /*command array*/
    char* args[] ={"/bin/sh", "-c", command, '\0'};

    /*create child process*/
    pid_t pid = fork ();

    if (pid < 0)
    {
        fprintf (stderr, "fork error (%s) -- exiting\n", 
            strerror (errno));
        exit (-1);
    }
    
    /*child forked off*/
    if (pid == 0)
    {
        int fd = open ("System.out", O_RDWR | O_CREAT | O_TRUNC, 0600);
         if (fd < 0)
        {
            printf ("unable to create file: \"System.out\" \n");
            return 0;
        }
        /*putting output into file instead of stdout*/
        dup2(fd, 1);
        close(fd);
        
        /*run command*/
        int ret = execvp (args[0], args);

        if (ret < 0)
        {
            fprintf (stderr, "exec error (%s) -- exiting\n", 
                strerror (errno));
            exit (1);
        }
    }
    int result;
    pid_t return_child = waitpid (pid, &result, 0);

    /*ensure the child returned is the same as the child created*/
    assert (return_child == pid);

    /*print exit status result of child process*/
    printf ("Exit status: %d\n", WEXITSTATUS (result));
    return (0);
}