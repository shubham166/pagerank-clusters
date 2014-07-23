#include <malloc.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "matrix.h"

typedef struct Warray
{
	int *elements;
	int size;
}warray;


typedef struct Rankarray
{
	float *elements;
	int size;
}rankvec;
// typedef int* warray;

extern void warray_init(warray *W, int size);
extern void warray_Wo_read(warray *Wo, matrix *m);
extern void warray_Wg_read(warray *Wg, matrix *m,int r,int c);
extern void warray_Wi_read(warray *Wi,matrix *m , int r,int c);
extern void rankvec_init(rankvec *r, int size);
extern void rankvec_copy(rankvec *r1, rankvec *r2);
