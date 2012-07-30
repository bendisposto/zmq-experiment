#include <stdio.h>
#include <string.h>

#define HASHSIZE 524288
char a[HASHSIZE][20];

void print_key(char key[20]) {
	int o; for(o=0;o<20;o++) printf("%d ",key[o]);
}

int find(int index, int jump, int orgi, int org, char key[20]) {
//	printf("get ");
//	print_key(key);
//	printf("\nget %d %d\n",index,jump);
	if (index<0) index = HASHSIZE-index;
    index = index % HASHSIZE;
//	printf("get %d %d\n",index,jump);
	if (!(org) && orgi==index)  { exit(-1); }
	else { org = 0; }
	char *x = a[index];

	if (strncmp(a[index],"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",20)==0) return 0;
	if (strncmp(a[index],key,20)==0) return 1;
	return find((index+jump),jump,orgi,org,key);
}


int contains(char key[20]) {
//	printf("contains\n");
	int index = ((key[0]*256*256 + 256 * key[1] + (key[2] & 224))/32);
	int jump = ((key[3]+256*key[4]) | 1);
    if (jump<0) jump = -jump;
	return find(index,jump,index,1,key);
}

int write_hm(int index, int jump, int orgi, int org, char key[20]) {
//	printf("put ");
//	print_key(key);
//	printf("\nput %d %d\n",index,jump);
	if (index<0) index = HASHSIZE-index;
	index = index % HASHSIZE;
//  printf("put %d %d\n",index,jump);
	//printf("put %s %d\n",key,index);
	if (!(org) && orgi==index)  { exit(-1); }
	else { org = 0; }
	if (strncmp(a[index],"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",20)==0) {
		memcpy (a[index], key, 20);
		return 1;
	}
	else {
	  if (strncmp(a[index],key,20)!=0)  {
		return write_hm((index+jump) % HASHSIZE,jump,orgi,org,key);
	  }
	  else {
		return 1;
      } 
	}
}

int put(char key[20]) {
//	printf("put\n");
	int index = ((key[0]*256*256 + 256 * key[1] + (key[2] & 224))/32) ;
	int jump = ((key[3]+256*key[4]) | 1);
	if (jump<0) jump = -jump;
	write_hm(index,jump,index,1,key);	
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