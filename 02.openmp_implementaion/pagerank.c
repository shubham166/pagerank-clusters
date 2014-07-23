#include <stdio.h>
#include <time.h> 
#include <omp.h> 
#include <stdlib.h>
#include <string.h>
void pagerank(int **graph,double alpha,int total_itr,int no_of_vertices,int nthreads);
int main(int argc,char *argv[])
{
	//int v=4;
	// printf("%s\n",argv[1]);
	FILE *fr1;
	fr1=fopen(argv[1],"r");
	int v=atoi(argv[2]);
	int nthreads = atoi(argv[3]);

	FILE *fr;
	fr=fopen(argv[1],"r");
	int i,j,k;
	int **graph=malloc(sizeof(int*)*v);
	for(k=0;k<v;k++)
	{
		graph[k]=(int*)malloc(sizeof(int)*v);
	}
	char line[80];
	i=0;
	while(fgets(line,80,fr)!=NULL)
	{

	//printf("%s",line);
		char* tok=strtok(line," ");
		j=-1;
		while(tok!=NULL){
		j++;
		//printf("(%d,%d)\t%d",i,j,atoi(tok));
		graph[j][i]=atoi(tok);
		tok=strtok(NULL," ");
		}
		i++;
	}
	clock_t start,en;
	start = clock();
	// printf("start = %f\n",(float)start);
	// printf("v = %d\n",v );
	printf("*******************************************************************************************\n");
	printf("For file %s\n", argv[1]);
	printf("Total Number of cores = %d\n",nthreads);
	printf("Total Number of Pages = %d\n",v);
	printf("*******************************************************************************************\n");
	printf("Page rank\n");
	printf("--------------------------------------------------------------------------------------------\n");
	pagerank(graph,0.85,1,v,nthreads);
	en=clock();

	printf("--------------------------------------------------------------------------------------------\n");
	printf("********************************************************************************************\n");
	printf("Running Time = %f\n",(float)(en-start)/(float)CLOCKS_PER_SEC);
	printf("********************************************************************************************\n");
	// start = clock() - start;

	// printf("%f\n",(double)start);
	return 0;
}

void pagerank(int **graph,double alpha,int total_itr,int no_of_vertices,int nthreads)
{
const double tolerance=1e-8;

// printf("num of vertices %d\n",no_of_vertices);

double scale=1.0/no_of_vertices;
double *pagerank=(double*)malloc(sizeof(double)*no_of_vertices);
double *last=(double*)malloc(sizeof(double)*no_of_vertices);
double teleport=(1.0-alpha)/no_of_vertices;
int i;
omp_set_num_threads(nthreads);
#pragma omp parallel for
for(i=0;i<no_of_vertices;i++)
{
pagerank[i]=scale;
last[i]=0.0;
}

int iter=0,id=0;
while(iter<total_itr)
{

	#pragma omp parallel for
	for(i=0;i<no_of_vertices;i++)
		{
		last[i]=pagerank[i];
		pagerank[i]=0.0;
		}
		// printf("hello0\n");
	double zsum=0;
	#pragma omp parallel for reduction(+:zsum)
	for(i=0;i<no_of_vertices;i++){
	if(grphedges(graph,i,no_of_vertices)==0)
		{
		zsum+=last[i];
		}
	// printf("hello1\n");
	double nolinks=alpha*zsum/no_of_vertices;

	#pragma omp parallel for
for(id=0;id<no_of_vertices;id++)
{
	double update=alpha*last[id]/grphedges(graph,id,no_of_vertices);
	int in_var=0;
	while(in_var<no_of_vertices){
	if(graph[id][in_var]==1){
	#pragma omp atomic
	pagerank[in_var]=pagerank[in_var]+update;
	}
	in_var++;
	}


	// printf("hello2\n");
	#pragma omp atomic
	pagerank[id]+=teleport+nolinks;
	//printf("num of vertices %d\n",no_of_vertices);
	// printf("id = %d\n",id );
	}
	// printf("hello2.1 it = %d\n",iter);
	iter++;

	}

	// printf("helloend\n");
	double sum=0.0;
	#pragma omp parallel for reduction(+:sum)
	for(i=0;i<no_of_vertices;i++){
	sum+=pagerank[i];
	}
	sum=1.0/sum;
	// printf("hello3\n");
	#pragma omp parallel for
	for(i=0;i<no_of_vertices;i++){
	pagerank[i]*=sum;
	}
	// printf("hello4\n");
	double err=0.0;
	if(err<tolerance)
	break;
	}

	int pr_var=0;
	while(pr_var<no_of_vertices){
	printf("\nPage %d : %f",(pr_var+1),pagerank[pr_var]);
	pr_var++;
	}
	printf("\n");
}

int grphedges(int** graph,int i,int no_of_vertices)
{
	//printf("%d\n",omp_get_thread_num());
	int j=0;
	int count=0;
	while(j<no_of_vertices)
	{
	if(graph[i][j]==1)
	{
	count++;

	}
	j++;
	}
	return count;
}


































