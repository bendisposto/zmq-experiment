#ifndef HASHMAP_H_INCLUDED
#define HASHMAP_H_INCLUDED

void print_key(char key[21]);
int ith_index_of(char key[20], int i);
int occupied(char key[21],int index);
int index_of(char key[20]);
int contains(char key[21]);
int contains_processed(char key[21]);
void put_local(char key[20]);
void put(char key[21]);
int count_elements();
void init_hashmap();
#endif
