#include <stdio.h>
#include <string.h>

char a[524288][20];

int find(int index, int jump, int orgi, int org, char key[20]) {
	//printf("get %s %d\n",key,index);
	if (!(org) && orgi==index)  { exit(-1); }
	else { org = 0; }
	if (strncmp(a[index],"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",20)==0) return 0;
	if (strncmp(a[index],key,20)==0) return 1;
	return find((index+jump) % 524288,jump,orgi,org,key);
}


int contains(char key[20]) {
	int index = (key[0]*256*256 + 256 * key[1] + (key[2] & 224))/32;
	int jump = ((key[3]+256*key[4]) | 1);

	return find(index,jump,index,1,key);
}

int write(int index, int jump, int orgi, int org, char key[20]) {
	//printf("put %s %d\n",key,index);
	if (!(org) && orgi==index)  { exit(-1); }
	else { org = 0; }
	if (strncmp(a[index],"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",20)==0) {
		memcpy (a[index], key, 20);
		return 1;
	}
	else {
	  if (strncmp(a[index],key,20)!=0)  {
		return write((index+jump) % 524288,jump,orgi,org,key);
	  }
	  else {
		return 1;
      } 
	}
}

int put(char key[20]) {
	int index = (key[0]*256*256 + 256 * key[1] + (key[2] & 224))/32;	
	int jump = ((key[3]+256*key[4]) | 1);
	write(index,jump,index,1,key);	
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