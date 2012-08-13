#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define HASHSIZE 524288
#define ENTRY 21

static char a[HASHSIZE][ENTRY];

void print_key(char key[21]) {
	int o; for(o=0;o<21;o++) printf("%d ",key[o]);
}

int ith_index_of(char key[20], int i) {
	int index = ((key[0]*256*256 + 256 * key[1] + (key[2] & 224))/32);
	int jump = ((key[3]+256*key[4]) | 1);
    if (jump<0) jump = -jump;
	int x = (index + i * jump) % HASHSIZE; 
	if (x < 0) x = -x;
	return x;
}

int occupied(char key[21],int index) {
	if (memcmp(a[index],"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",20)==0) return 0;
	if (memcmp(a[index],key,20)==0) return 0;
	return 1;
}

int index_of(char key[20]) {
	int i=0,p=0;
	while(1) {
		p=ith_index_of(key,i++);
		if (!occupied(key, p)) break;
	};	
	return p;
}

int contains(char key[21]) {
	int p = index_of(key);
//	print_key(key);printf("\n");
//	print_key(a[p]);printf("\n");
//	sleep(1);
	if (memcmp(a[p],"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",20)==0) return 0;
	if (memcmp(a[p],key,20)==0) return 1;
	return 0;
}

int contains_processed(char key[21]) {
	int p = index_of(key);
//	printf("pos: %i ",p);
	int result = 0;
	if (memcmp(a[p],"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",20)==0) result = 0;
	if (memcmp(a[p],key,20)==0 && a[p][20] == 1) result = 1;
//	printf("r: %d\n",result);
	return result;
}

void put_local(char key[20]) { // yes, 21 is correct
	int p = index_of(key);
	memcpy (a[p], key, 20);
	a[p][20] = 0;
}

void put(char key[21]) { // yes, 21 is correct
    int p = index_of(key+1);
	memcpy (a[p], key+1, 20);
	a[p][20] = key[0];
}


int count_elements() {
	int c =0 ;
	int i;
	for(i=0;i<HASHSIZE;i++) {
		if (memcmp(a[i],"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",20)!=0) c++;
	}
	printf("S: %i\n",i);
	return c;
}


/*
int main(void) {
	char *text="HJ48dsj^3_3s2}d3is71";
	char *text2="HJ49dsj^3_3s2}d3is71";
	put(text);
	put(text2);
	printf("%i\n",contains(text));	
	printf("%i\n",contains(text2));	
 return 3;
}
*/
