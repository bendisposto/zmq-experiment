#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#define N	10000
#define DENSITY 10

int **matrix[N][N];

void print_matrix() {
	int i,j;
	for (i=0;i<N;i++) {
		for (j=0;j<N;j++) {
			printf("%i ",matrix[i][j]);
	    }
		printf("\n");
	}
}

void init_graph(void) {
	srand((int)time(NULL));
	int i,j;

	for (i=0;i<N;i++) {
		for (j=0;j<i;j++) {
			int e = rand() % 100;
			int v = (e<=DENSITY);
			matrix[i][j] = v;
			matrix[j][i] = v;
	    }
	}
}

char *produce_work(int i, int j) {
//	printf("looking at %i %i %i\n",i,j,matrix[i][j]);
      if(matrix[i][j]==1) {
		char *result=malloc(10);
  	    sprintf(result,"%i",j);
        return result; }
		return NULL;
}



