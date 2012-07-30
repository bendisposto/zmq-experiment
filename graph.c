#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#define N	3000
#define DENSITY 10

char *vertex[N][20];
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



void print_vertices() {
	int i;
	for (i=0;i<N;i++) {
		printf("%s ",vertex[i]);
	}	
	printf("\n");
}

void init_graph(void) {
	srand((int)time(NULL));
	int i,j;
	for (i=0;i<N;i++) {
		sprintf(vertex[i],"some_work_package %i",i);
	}
	for (i=0;i<N;i++) {
		for (j=0;j<i;j++) {
			int e = rand() % 100;
			int v = (e<=DENSITY);
			matrix[i][j] = v;
			matrix[j][i] = v;
	    }
	}
}

char *produce_work(char *input, int k) {
	int i,n;
	for(i=0;i<N;i++) {
		char *v = vertex[i];
//		printf("%i %i %s\n",i,strlen(input),v);
		if (strncmp(v,input,strlen(input))==0) n=i;
	}	
   if(matrix[n][k]==1) return vertex[k]; 
   return NULL;
}




/*
void main() {
	srand((int)time(NULL));
	init_graph();
	print_vertices();
	print_matrix();
	int i;
	for(i=0;i<N;i++) {
		char *r = produce_work(1,i);
		if (r) printf("%s\n",r);
	}
	printf("%i\n",size());
}
*/
