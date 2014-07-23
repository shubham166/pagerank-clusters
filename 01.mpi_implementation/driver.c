#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "algorithm.h"

int main(int argc, char**argv)
{
	// MPI_Init(NULL,NULL);

	assert(argc > 1);

	int N = atoi(argv[1]);
	
	page_rank(N,argv[2]);
	

	// MPI_Finalize();
	return 0;
}