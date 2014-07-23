#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

//#include "matrix.h"
#include "algorithm.h"

void pack_array(int count, MPI_Datatype *type);
void compute_page_rank(rankvec *rsrc , rankvec *rdest, warray *Wg, warray *Wi, warray *Wo, int size, int N, int myrank,int it,int nthreads);
float absoulte_val(float val);

void page_rank(int N,char *filename,int nthreads)
{
	MPI_Init(NULL,NULL);

	// printf("nthreads = %d\n",nthreads );
	int myrank,world_size;

	MPI_Comm_size(MPI_COMM_WORLD,&world_size);
	MPI_Comm_rank(MPI_COMM_WORLD,&myrank);

	int size = (N+ world_size -1)/world_size;			//Ceiling of N/world_size

	// matrix *m = (matrix*)malloc(sizeof(matrix));
	int i,j;

	rankvec rdest;
	rankvec_init(&rdest,N);

	rankvec rsrc;
	rankvec_init(&rsrc,N);
	
	//int *Wo = (int*)malloc(sizeof(int)*N);
	warray Wo;
	Wo.size = N;
	//Wo.elements = (int*)malloc(N*sizeof(int));
	matrix g;
	g.size = size;
	int *g_data = (int*)malloc(size*N*sizeof(int));
	g.elements = (int**)malloc(size*sizeof(int*));
	for(i = 0;i<size;i++)
		g.elements[i] = &(g_data[i*N]);


	//matrix_init(&g,size);
	double  start,end;
	// clock_t start,end,diff;
	warray_init(&Wo,N);

	MPI_Datatype MPI_Int_array;
	MPI_Type_contiguous(N,MPI_INT,&MPI_Int_array);
	MPI_Type_commit(&MPI_Int_array);		

	matrix m;
	m.size = N;
	int *m_data = (int*)malloc(N*N*sizeof(int));
	m.elements = (int**)malloc(N*sizeof(int*));
	for(i = 0;i<N;i++)
		m.elements[i] = &(m_data[i*N]);


	if(myrank==0)
	{
		FILE *fp = fopen(filename,"r");
		
		matrix_read(&m,fp);
		for(i = 0;i<N;i++)
		{
			//for(j = 0;j<N;j++)
			//	printf("%d ",m.elements[i][j]);
			//printf("\n");
		}
		warray_Wo_read(&Wo,&m);	
	}
	MPI_Bcast(Wo.elements,N,MPI_INT,myrank,MPI_COMM_WORLD);
		
	MPI_Scatter(&(m.elements[0][0]),size,MPI_Int_array,&(g.elements[0][0]),size,MPI_Int_array,0,MPI_COMM_WORLD);

	MPI_Barrier(MPI_COMM_WORLD);

	start = MPI_Wtime();

	warray Wg;
	warray_Wg_read(&Wg,&g,size,N);

	warray Wi;
	warray_Wi_read(&Wi,&g,size,N);

	// printf("before\n");
	//printf("hello\n");
	compute_page_rank(&rsrc,&rdest,&Wg,&Wi,&Wo,size,N,myrank,100,nthreads);
	// pagerank(&g,0.85,100,size,N,&rdest,&rsrc,myrank);
	//printf("outside\n");
	// printf("after\n");
	MPI_Barrier(MPI_COMM_WORLD);

	start = MPI_Wtime() - start;
	// diff = end - start;

	// double time_spent = (double)diff/(double)CLOCKS_PER_SEC;
	
//	printf("after\n");
	if(myrank == 0)
	{
		// printf("Process :%d\n",myrank);
		//printf("val = %f\n",g.elements[0][0]);
		printf("*****************************************************************************************************\n");
		printf("For file %s \n",filename);
		printf("Total Number of Pages : %d\n",N);
		printf("Total Number of Nodes in Cluster :%d\n",world_size);
		printf("Total Number of Cores per Node :%d\n",nthreads);
		printf("Running time at %d = %lf\n\n",myrank,start);
		printf("*****************************************************************************************************\n");
		printf("Page Ranks\n");
		printf("-----------------------------------------------------------------------------------------------------\n");
		for(i = 0;i<rdest.size;i++)
			printf("Page %d: %f\n",i+1,rdest.elements[i]);
		//float time_spent = (double)(end -start);
		
		// return;
	}
	
	MPI_Finalize();
}


void pack_array(int count, MPI_Datatype *type)
{
	
	MPI_Type_contiguous(count,MPI_INT,type);
	MPI_Type_commit(type);

}


void compute_page_rank(rankvec *rsrc , rankvec *rdest, warray *Wg, warray *Wi, warray *Wo, int size, int N, int myrank,int it,int nthreads)
{
	float alpha = 0.85;
	int t = 0,i,j,k,p;

	float nolinks;
	float teleportaion = (1 - alpha)/N;
	rankvec dhost;
	rankvec_init(&dhost,size);

	float sum;
	float er = 0;
	omp_set_num_threads(nthreads);
	const float tolerance = 1e-8;
	
	int b = myrank*size, e = (myrank+1)*size-1;
	int POSbegin,POSend;

	// int it = 0;
	do
	{
		//printf("process %d : it %d er = %f\n",myrank, it,er );
		it--;
		rankvec_copy(rsrc,rdest);
		nolinks = 0;
		#pragma omp parallel for
		for(i = 0;i<N;i++)
			if(Wo->elements[i] == 0)
				nolinks += rdest->elements[i];
		nolinks = (nolinks*alpha)/N;

		//printf("nolinks = %f\n",nolinks);
		#pragma omp parallel for
		for(i =0;i<dhost.size;i++)
			dhost.elements[i] = 0;

		#pragma omp parallel for	
		for(j = b;j <= e;j++)
		{
			POSbegin =	Wi->elements[j-b],POSend = Wi->elements[j-b+1]-1;
			for(k = POSbegin; k <= POSend; k++)
			{
				p = Wg->elements[k];
				dhost.elements[j-b] += alpha * (rdest->elements[p]/Wo->elements[p]);
			}
		}

		#pragma omp parallel for
		for(i = 0;i<dhost.size;i++)
			dhost.elements[i] += nolinks + teleportaion;
		/*for(i=0;i< dhost.size;i++)
			printf("P:%d %f\n",myrank,dhost.elements[i]);*/
		#pragma omp parallel for
		for(i = 0;i<dhost.size;i++)
			rdest->elements[myrank * size + i] = dhost.elements[i];

		
		//printf("process %d\n",myrank );
		MPI_Bcast(rdest->elements,size,MPI_FLOAT,myrank,MPI_COMM_WORLD);
		//printf("process end %d\n",myrank);
		/*
		for(i = 0;i<N;i++)
			printf("Rank Dest P%d: %f\n",myrank,rdest->elements[i]);

		for(i = 0;i<N;i++)
			printf("Rank src P%d: %f\n",myrank,rsrc->elements[i]);*/

		//Normalisation
		sum = 0;	
		#pragma omp parallel for reduction(+:sum)
		for(i = 0;i<N;i++)
			sum+=rdest->elements[i];

		for(i = 0;i<N;i++)
			rdest->elements[i] = rdest->elements[i] / sum;


		//Convergence
		er = 0;
		#pragma omp parallel for reduction(+:er)
		for(i = 0;i<N;i++)
		{
			//printf("here\n");
			er +=  absoulte_val(rdest->elements[i] - rsrc->elements[i]);
			//printf("dif = %f\n", rdest->elements[i] - rsrc->elements[i]);
			//printf("error = %f\n",er );
		}

		//printf("error = %f\n",er );
		if(er < tolerance)
			break;
		
	}while(it>0);
}

float absoulte_val(float val)
{
	if(val < 0)
		return 0 - val;
	else 
		return val;
}