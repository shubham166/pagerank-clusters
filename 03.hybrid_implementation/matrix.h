#include <malloc.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct matrix
{
	int ** elements;
	int size;
}matrix;


extern void matrix_init(matrix *m, int N);
extern void matrix_free(matrix* m);
extern void matrix_read(matrix *m,FILE *fp);
