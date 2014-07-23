#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char**argv)
{
	MPI_Init(NULL,NULL);

	int myrank;
	MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD,&world_size);
	//printf("world size = %d",world_size);

	int s;						//pages per processor
	int N,i;		

	int Wg[8], Wi[5], Wo[4];
	double *rhost = (double*)malloc(N*sizeof(double));
	double *rhostprev = (double*)malloc(N*sizeof(double));
	double *residual = (double*)malloc(N*sizeof(double));
	double sum;					//for calculating residue

	for(i = 0;i<N;i++)			//intializing rhost and rhostprev
	{
		rhost[i] = 1/N;					
		rhostprev[i] = 1/N;
	}

	if(myrank == 0)
	{
		
		printf("Enter number of pages N\n");			//taking input number of pages
		scanf("%d",&N);
		
		s = N+world_size-1/world_size;			// Ceiling of N/world_size

		/*int Wg[] = {0,2};					//Data 
		int Wi[] = {0,2,2};
		int Wo[] = {2,1,3,2};*/

		Wg[0] = 0;Wg[1] =2;
		Wi[0] = 0;Wi[1] = 2;Wi[2] = 2;
		Wo[0] = 2;Wo[1] = 1;Wo[2] = 3;Wo[3] = 2;
 	} 

	else if(myrank == 1)
	{
		/*int Wg[] = {1,2,3,0,2,3};			//Data 
		int Wi[] = {0,3,6};
		int Wo[] = {2,1,3,2};*/


		Wg[0] = 1;Wg[1] = 2;Wg[2] = 3;Wg[3] = 0;Wg[4] = 2;Wg[5] = 3;
		Wi[0] = 0;Wi[1] = 3;Wi[2] = 6;
		Wo[0] = 2;Wo[1] = 1;Wo[2] = 3;Wo[3] = 2;
	}
	// MPI_Scatter()
	//MPI_Scatter(Wg,)				TODO: Scatter data to diff processe

	/*if(MPI_Barrier(MPI_COMM_WORLD) == IERROR)
	{
		printf("ERROR\n");
		return 0;
	}*/

	MPI_Barrier(MPI_COMM_WORLD);

	double alpha = 0.85;			//Weight of actual links
	double epsilon = 0.01;
	double *dhost = (double*)malloc(s*sizeof(double));
	do
	{
		for(i = 0;i<N;i++)
			rhostprev[i] = rhost[i];		//rhostprev = rhost

		for(i = 0;i<s;i++)
			dhost[i] = 0;					//dhost = 0;

		int j,POSbegin,POSend,k,p;
		int b = myrank*s, e = myrank*(s+1) -1;
		for(j = b;j<=e;j++)
		{
			POSbegin = Wi[j-b];
			POSend = Wi[j-b+1]-1;
			for(k = POSbegin;k<=POSend;k++)
			{
				p = Wg[k];
				dhost[j-b]+=alpha*rhost[p]/Wo[p];
			}
		}
		for(i = b;i<=e;i++)
			rhost[i] = dhost[i-b];

		MPI_Bcast(rhost,N,MPI_DOUBLE,myrank,MPI_COMM_WORLD);		//Broadcast rhost

		MPI_Barrier(MPI_COMM_WORLD);								//Waiting for all process to receive rhost

		sum = 0;
		for(i = 0;i<N;i++)											//Calculating residue 
		{															//rhost - rhostprev		
			residual[i] = abs(rhost[i] - rhostprev[i]);
			sum +=residual[i];
		}
		sum/=N;

	}while(sum>epsilon);

	MPI_Barrier(MPI_COMM_WORLD);

	if(myrank == 0)
	{
		printf("Process %d:\n",myrank);
		for(i = 0;i<N;i++)
			printf("page %d: %lf\n",i,rhost[i]);
	}

	MPI_Finalize();

}


