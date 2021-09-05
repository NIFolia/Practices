#include <mpi.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <time.h>

int isPrime(int n);

int main(int argc, char **argv)
{
    int rank, s_value, r_value, size, startNum, endNum, i;
    struct timespec start, end;
    double time_taken;
    MPI_Status status;
    MPI_Init( &argc, &argv );
    /*
     This function gives the rank of the process in the particular communicator's group
     */
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    /*
     This function indicates the number of processes involved in a communicator
     */
    MPI_Comm_size( MPI_COMM_WORLD, &size );
    /*
     get massage when in first process
     */
    if (rank == 0) {
        printf("Enter a round number: ");
        fflush(stdout);
        scanf( "%d", &s_value );
    }
    /*
     start counting
     */
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    /*
     Boardcast
     */
    MPI_Bcast(&s_value, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    startNum = rank * s_value/size;
    endNum = startNum + s_value/size;
    /*
     last process take the remainder
     */
    if (rank == size-1)
    {
        endNum += s_value%size+1;
    }
    
    char f[10];
    sprintf(f, "P:%d.txt",rank);
    FILE *fp;
    fp = fopen(f,"w");
    fprintf(fp, "P: %d, S: %d, E: %d\n", rank, startNum, endNum);
    
    for (i = startNum; i<endNum; i++)
    {
        if (i > 1)
        {
            if (isPrime(i))
            {
                fprintf(fp,"%d\n",i);
            }
        }
    }
    fclose(fp);
    /*
     end counting
     */
    clock_gettime(CLOCK_MONOTONIC, &end);
    time_taken = (end.tv_sec - start.tv_sec) * 1e9;
    time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;
    printf("P: %d, speed: %lf\n", rank, time_taken);
            
    MPI_Finalize();

    return 0;
}

/* Function Definition */
int isPrime(int n){
    int i;
    // Exclude the case when n <= 1
    if(n<=1){
        return 0;
    }
    
    // Exclude the case for n is even number and larger than 2
    if(n!=2 && n%2==0){
        return 0;
    }
    
    // If a factor can be found between 3 - sqrt(n), the given number n is a non-prime number
    for(i=3; i<=sqrt(n); i++){
        if(n%i==0){
            return 0;
        }
    }
    
    return 1;
}
