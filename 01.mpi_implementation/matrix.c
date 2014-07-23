#include <malloc.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "matrix.h"

void matrix_init(matrix *m,int size)
{
	assert(m!=NULL);
	m->size = size;
	int *data = (int*)malloc(size*size*sizeof(int));
	m->elements = (int**)malloc(size*sizeof(int*));

	assert(m->elements != NULL);
	int i;
	for(i = 0;i<size;i++)
	{	
		m->elements[i] = &(data[i*size]);
		// m->elements[i] = (float*)malloc(sizeof(float)*size) ;
	}
}

void matrix_free(matrix* m)
{
	assert(m != NULL);
	int i;
	for (i = 0; i < m->size; i++)
	{
		free(m->elements[i]);
	}

	free(m->elements);
	m->elements = NULL;
	m->size = 0;
	m = NULL;
}

void matrix_read(matrix *m,FILE *fp)
{
	assert(fp!=NULL);
	int i,j;
	for(i = 0;i < m->size;i++)
	{
		for(j = 0;j<m->size;j++)
		{
			fscanf(fp,"%d",&m->elements[i][j]);
		}
	}

}


