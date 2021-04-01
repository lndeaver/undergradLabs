/*
  Filename   : fish.c
  Author     : Lauren Deaver
  Course     : CSCI 380-01
  Assignment : Assignment 6: Memory Mapping
  Description: write a program using memory mapping to copy
                file data from one file to another.
*/

#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

void
mmapcopy (int fdOG, int fdCOPY, int size);

int
main (int argc, char **argv)
{
    /* Check for required cmd line arg */
    if (argc != 3)
    {
        printf ("usage: %s <originalfile> <copyfile>\n", argv[0]);
        exit(0);
    }
    

    /* Copy input file to new/existing file */
    int fdOG = open (argv[1], O_RDONLY);
    if (fdOG < 0)
    {
        printf ("unable to open file: %s\n", argv[1]);
        return 0;
    }

    int fdCOPY = open (argv[2], O_RDWR | O_CREAT | O_TRUNC, 0600);
    if (fdCOPY < 0)
    {
        printf ("unable to create file: %s\n", argv[2]);
        return 0;
    }
    struct stat stat;
    fstat (fdOG, &stat);
    ftruncate (fdCOPY, stat.st_size);
    mmapcopy (fdOG, fdCOPY, stat.st_size);

    close (fdOG);
    close (fdCOPY);

    exit (0);
}

void
mmapcopy (int fdOG, int fdCOPY, int size)
{
    /* Ptr to memory mapped area */
    void *src, *dst;
    src = mmap (NULL, size, PROT_READ, MAP_SHARED, fdOG, 0);
    if (src == (caddr_t) -1)
    {
        printf ("mmap error for source\n");
    }
    dst = mmap (NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fdCOPY, 0);
    if (dst == (caddr_t) -1)
    {
        printf ("mmap error for destination\n");
    }
    memcpy (dst, src, size);

    munmap (src, size);
    munmap (dst, size);
}