#include <stdio.h>
#include <mpi.h>
int main(int argc, char* argv[])
{
    int rank, s_value, r_value, size;
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
    do {
        /*
         get massage when in first process
         */
        if (rank == 0) {
            printf("Enter a round number: ");
            fflush(stdout);
            scanf( "%d", &s_value );
            // Add your code here
            /*
             MPI_Send perfoms a standard-mode, blocking send
             this routine will block until the message is sent to the destination
             buf: initial address of send buffer
             count: number of elements send
             datatype: datatype of each send buffer elememt
             dest: rank of destination
             tag: message tag
             comm: communicator
             */
            MPI_Send(&s_value, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD);
            /*
             This basic receive operation, MPI_Recv, is blocking
             Input Parameters:
             count: maximum number of elements to receive
             datatype: datatype of each receive buffer entry
             source: rank of source
             tag: message tag
             comm: communicator
             Output parameters:
             buf: initall address of recieve buffer
             status: status object
             */
            MPI_Recv(&r_value, 1, MPI_INT, size-1, 0, MPI_COMM_WORLD, &status);
            printf( "Process %d got %d from Process %d\n", rank, r_value, size - 1);
            fflush(stdout);
        }
        /*
         pass message recived to next process
         */
        else {
            // Add your code here
            MPI_Recv(&r_value, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, &status);
            if (rank < size-1){
                MPI_Send(&r_value, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD);
            } else {
                MPI_Send(&r_value, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            }
            printf( "Process %d got %d from Process %d\n", rank, r_value, rank - 1);
            fflush(stdout);
        }
    } while (r_value >= 0);
    MPI_Finalize( );
    return 0;
}
