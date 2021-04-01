/*
  Filename   : pthreads.c
  Author     : Lauren Deaver
  Course     : CSCI 380-01
  Assignment : Assignment 10: Therads
  Description: write pthread program to compute parallel sum of array
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

/******************************************************************/
void
serialSumCalc (int* A);

void*
pthreadSumCalc (void* threadId);

//globals
int* A;
int p;
int N;

/******************************************************************/
int 
main (int argc, char* argv[])
{
    printf("p ==> ");
    scanf("%d", &p);

    printf("N ==> ");
    scanf("%d", &N);

    A = malloc(N * sizeof(int));

    //pupulate random numbers in array
    for (int i = 0; i < N; ++i)
    {
        A[i] = rand() % 5;
    }


    pthread_t* threads = malloc (p * sizeof (pthread_t));

    for (long threadId = 0; threadId < p; ++threadId)  
    {
        // Second arg is attributes object, which we will not use. 
        pthread_create (&threads[threadId], NULL,
		    pthreadSumCalc, (void *) threadId);
    }

    
    long pSum = 0;
    for (long threadId = 0; threadId < p; ++threadId) 
    {
        void* returnVal;
        pthread_join (threads[threadId], &returnVal);
        pSum += (long)returnVal;
    }


    printf("Parallel sum:   %ld\n", pSum);
    serialSumCalc(A);

    free(threads);
    free(A);
}

/******************************************************************/
void
serialSumCalc(int* A)
{

    int serialSum = 0;
    for (int i = 0; i < N; ++i)
    {
        serialSum += A[i];
    }

    printf("Serial sum:     %d\n", serialSum);
}

/******************************************************************/
void*
pthreadSumCalc(void* threadId)
{
    long id = (long)(threadId);
    long chunkSize = N/p;
    long start = chunkSize * id;
    long end = start + chunkSize;

    long threadSum = 0;
    for (int i = start; i < end; ++i)
    {
        threadSum += A[i];
    }

    return (void*) threadSum;
}