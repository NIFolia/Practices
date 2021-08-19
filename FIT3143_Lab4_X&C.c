#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

// Define the number of elements to proccess for each thread
#define CHUNKSIZE 2
// Define the number of threads
#define THREADS 4
// Define the size of array
#define N 10

int main()
{
    // Define varable of number of chunks
    int chunk = CHUNKSIZE;
    // Define variables for the ragne of random number
    int min = 1;
    int max = 25;
    // Define variables for counting maches
    int total = 0;
    int l[N];
    int C[N] = {0};
    // Define variables for measure time taken
    clock_t start, endParallel, end;
    double parallelTime, totalTime;
    
    start = clock();
    printf("Parallel computing at:%f\n",0);
    omp_set_num_threads (THREADS);
    // Start parallel computing
    #pragma omp parallel shared(l,chunk)
    {
        srandom(time(NULL));
        
        #pragma omp for schedule(dynamic,chunk)
        for (int i=0;i<N;i++)
        {
            l[i] = (random()%(max))+min;
        }
    }
    // End of parallel computing
    
    // Printing the result
    for (int i=0;i<N;i++)
    {
        printf("l[%d]:%d\n",i,l[i]);
    }
    endParallel = clock();
    parallelTime = ((double)(endParallel-start)/CLOCKS_PER_SEC);
    printf("Match result at:%f\n", parallelTime);
    
    // Start sequential computing
    for (int i=0;i<N;i++)
    {
        for (int j=0;j<N;j++)
        {
            if (l[i] == l[j] && i != j)
            {
                if (C[i] == 0)
                {
                    total+=1;
                    C[i] += 1;
                    C[j] += 1;
                }
                else
                {
                    C[j] += 1;
                    C[i] = C[j];
                }
            }
        }
    }
    // Printing the result
    for (int i=0;i<N;i++)
    {
        printf("l[%d]: %d, maches:%d\n",i,l[i], C[i]);
    }
    printf("Total maches: %d\n", total);
    end = clock();
    totalTime = ((double)(end-start)/CLOCKS_PER_SEC);
    printf("Program halt at:%f\n", totalTime);
}

// To compile: gcc -fopenmp <file name>
// To execute: ./a.out
