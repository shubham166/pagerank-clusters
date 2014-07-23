#include <malloc.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "warray.h"

void warray_init(warray *W, int size)
{	
	int i;
	assert(W!=NULL);
	W->size = size;
	W->elements = (int*)malloc(sizeof(int)*size);
	for(i = 0;i<W->size;i++)
		W->elements[i]= 0;
}

/*void warray_init(int *W,int size);
{

}*/

void warray_Wo_read(warray *Wo, matrix *m)
{
	assert(m!=NULL);
	assert(Wo!=NULL);
	
	int i,j;
	for(j=0;j<m->size;j++)
	{
		for(i = 0;i < m->size;i++)
		{		
			if(m->elements[i][j] > 0)
				Wo->elements[j]++;
//			printf(Wo->elements[j]
		}
	}
	
}

void warray_Wg_read(warray *Wg, matrix *m,int r,int c)
{
	assert(m!=NULL);
	assert(Wg!=NULL);

	int size = 0;
	int i,j;
	for(i=0;i<r;i++)
		for(j=0;j<c;j++)
			if(m->elements[i][j]==1)
				size++;

	Wg->size = size;
	Wg->elements = (int*)malloc(size*sizeof(int));

	int index = 0;
	for(i = 0;i<r;i++)
		for(j =0;j<c;j++)
			if(m->elements[i][j]==1)
				Wg->elements[index++] = j;
}

void warray_Wi_read(warray *Wi,matrix *m , int r,int c)
{
	assert(m!=NULL);
	assert(Wi!=NULL);

	Wi->size = r+1;
	Wi->elements = (int*)malloc((r+1)*sizeof(int));

	int count=0,i,j;
	Wi->elements[0] = 0;

	for(i = 1;i<=r;i++)
	{
		for(j = 0;j<c;j++)
		{
			if(m->elements[i-1][j] == 1)
				count++;
		}
		Wi->elements[i] = count;
	}
}

void rankvec_init(rankvec *r, int size)
{
	r->size = size;
	r->elements = (float*)malloc(size*sizeof(float));

	int i;
	for(i = 0;i<size;i++)
		r->elements[i] = 1/(float)size; 
}

void rankvec_copy(rankvec *r1, rankvec *r2)
{
	int size = r1->size;

	int i;
	for(i = 0;i<size;i++)
		r1->elements[i] = r2->elements[i];
}
