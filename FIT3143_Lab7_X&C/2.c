# include <stdio.h>
# include <mpi.h>

/* This example handles a 12 x 12 mesh , on 4 processors only . */

# define MAXN 12

int main (int argc , char * argv []) {
    int rank , value , size , errcnt , toterr , i , j ;
    double xlocal [(12/4) +2][12];
    MPI_Request request[4];
    MPI_Status status[4];

    /* Start the MPI Processing */
    MPI_Init(&argc , &argv) ;
    MPI_Comm_rank(MPI_COMM_WORLD , &rank) ;
    MPI_Comm_size(MPI_COMM_WORLD , &size) ;
    if (size != 4) MPI_Abort(MPI_COMM_WORLD, 1) ;

    /* xlocal [0][] is lower ghostpoints , xlocal [ MAXN +2][] is upper */
    /* Fill the data as specified */
    for ( i =1; i <= MAXN/size ; i++) {
        for ( j =0; j < MAXN ; j ++) {
            xlocal [i][j] = rank ;
        }
    }

    for ( j =0; j < MAXN ; j++) {
        xlocal [0][j] = -1;
        xlocal [MAXN/size+1][j] = -1;
    }

    /* Send up unless I’m at the top , then receive from below */
    /* Note the use of xlocal [i] for & xlocal [i ][0] */
    if (rank < size - 1){ 
        MPI_Isend(xlocal[MAXN/size] ,
        MAXN ,
        MPI_DOUBLE ,
        rank +1 ,
        0 ,
        MPI_COMM_WORLD, 
        &request[0]);
        MPI_Wait(&request[0], &status[0]);
    }
        

    if (rank > 0){
        MPI_Irecv(xlocal[0] ,
        MAXN ,
        MPI_DOUBLE ,
        rank -1 ,
        0 ,
        MPI_COMM_WORLD ,
        &request[1]);
        MPI_Wait(&request[1], &status[1]);
    }

    /* Send down unless I’m at the bottom */
    if ( rank > 0){
        MPI_Isend(xlocal[1],
        MAXN ,
        MPI_DOUBLE ,
        rank -1 ,
        1 ,
        MPI_COMM_WORLD,
        &request[2]);
        MPI_Wait(&request[2], &status[2]);
    }

    if ( rank < size - 1){
        MPI_Irecv(xlocal[MAXN/size +1] ,
        MAXN ,
        MPI_DOUBLE ,
        rank +1 ,
        1 ,
        MPI_COMM_WORLD ,
        &request[3]);
        MPI_Wait(&request[3], &status[3]);
    }

    /* Check that we have the correct results */
    errcnt = 0;
    for ( i=1; i <= MAXN/size ; i++) {
        for ( j =0; j < MAXN ; j++) {
            if (xlocal[i][j] != rank) errcnt++;
        }
    }

    for ( j=0; j < MAXN ; j++) {
        if ( xlocal[0][j] != rank - 1) 
            errcnt++;
        if ( rank < size -1 && xlocal[MAXN/size +1][j] != rank + 1)
            errcnt++;
    }

    MPI_Reduce(&errcnt ,
    &toterr ,
    1 ,
    MPI_INT ,
    MPI_SUM ,
    0 ,
    MPI_COMM_WORLD);

    if (rank == 0) {
        if (toterr) {
            printf ( "! found %d errors \n", toterr ) ;
        } else {
            printf ( "No errors \n" ) ;
        }
    }

    MPI_Finalize() ;

    return 0;
}
