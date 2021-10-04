
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <unistd.h>

#define SHIFT_ROW 0
#define SHIFT_COL 1
#define SHIFT_HEI 2
#define DISP 1


int IsPrime(int input)
{
    int j;
    int limit = (int)sqrt((double)input);

    for(j = 2; j <= limit; j++)
    {
        if (input % j == 0)
        {
            return 0;
        }
    }
    return 1;
}


int main(int argc, char *argv[]) {

	int ndims=3, size, my_rank, reorder, my_cart_rank, ierr;
	int nx, ny, nz;
	int nbr_i_lo, nbr_i_hi;
	int nbr_j_lo, nbr_j_hi;
    int nbr_k_lo, nbr_k_hi;
	MPI_Comm comm3D;
	int dims[ndims],coord[ndims];
	int wrap_around[ndims];
    int i, num;
    
    // set up matrix
    MPI_Request s_req[4];
    MPI_Request r_req[4];
    MPI_Status s_sta[4];
    MPI_Status r_sta[4];
	
	/* start up initial MPI environment */
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	
	/* process command line arguments*/
	if (argc == 4) {
		nx = atoi (argv[1]);
		ny = atoi (argv[2]);
        nz = atoi (argv[3]);
		dims[0] = nx; /* number of rows */
		dims[1] = ny; /* number of columns */
        dims[2] = nz; /* number of heights */
		if( (nx*ny*nz) != size) {
			if( my_rank ==0) printf("ERROR: nx*ny*nz)=%d * %d * %d= %d != %d\n", nx, ny,nz, nx*ny*nz,size);
			MPI_Finalize(); 
			return 0;
		}
	} else {
		nx=ny=nz=(int)exp(log(size)/3);
		dims[0]=dims[1]=dims[2]=0;
	}
	
	/*************************************************************/
	/* create cartesian topology for processes */
	/*************************************************************/
	MPI_Dims_create(size, ndims, dims);
	if(my_rank==0)
		printf("Root Rank: %d. Comm Size: %d: Grid Dimension = [%d x %d x %d] \n",my_rank,size,dims[0],dims[1],dims[2]);
	
	/* create cartesian mapping */
	wrap_around[0] = wrap_around[1] = wrap_around[2] = 0; /* periodic shift is .false. */
	reorder = 1;
	ierr =0;
	ierr = MPI_Cart_create(MPI_COMM_WORLD, ndims, dims, wrap_around, reorder, &comm3D);
	if(ierr != 0) printf("ERROR[%d] creating CART\n",ierr);
	
	/* find my coordinates in the cartesian communicator group */
	MPI_Cart_coords(comm3D, my_rank, ndims, coord);
	/* use my cartesian coordinates to find my rank in cartesian group*/
	MPI_Cart_rank(comm3D, coord, &my_cart_rank);
	/* get my neighbors; axis is coordinate dimension of shift */
	/* axis=0 ==> shift along the rows: P[my_row-1]: P[me] : P[my_row+1] */
	/* axis=1 ==> shift along the columns P[my_col-1]: P[me] : P[my_col+1] */
	
	MPI_Cart_shift( comm3D, SHIFT_ROW, DISP, &nbr_i_lo, &nbr_i_hi );
	MPI_Cart_shift( comm3D, SHIFT_COL, DISP, &nbr_j_lo, &nbr_j_hi );
    MPI_Cart_shift( comm3D, SHIFT_HEI, DISP, &nbr_k_lo, &nbr_k_hi );

	
	printf("Global rank: %d. Cart rank: %d. Coord: (%d, %d, %d).\n", my_rank, my_cart_rank, coord[0], coord[1], coord[2]);
    
    int action_list[4] = {nbr_j_lo, nbr_j_hi,nbr_i_lo, nbr_i_hi};
    int recv_data[4] = {-1,-1,-1,-1};
    sleep(my_rank);
    
    // get random number
    unsigned int seed = time(NULL);
    num = rand_r(&seed) % 30 + 2;
    // using fixed number iteration
    for (i = 0; i < 4; i++){
        MPI_Isend(&num, 1, MPI_INT, action_list[i], 0, comm3D, &s_req[i]);
        MPI_Irecv(&recv_data[i], 1, MPI_INT, action_list[i], 0, comm3D, &r_req[i]);
    }
    MPI_Waitall(4,s_req, s_sta);
    MPI_Waitall(4,r_req, r_sta);
    // compare neighbour prime numbers
    printf("Global rank: %d. Cart rank: %d. Coord: (%d, %d, %d). Rand: %d. Recv Vals - ", my_rank, my_cart_rank, coord[0], coord[1],coord[2], num);
    for (i = 0; i < 4; i++){
        printf("recv_data[%d] = %d\t", i, recv_data[i]);
    }
    printf("\n");
    
    
	fflush(stdout);
	MPI_Comm_free( &comm3D );
	MPI_Finalize();
	return 0;
}
