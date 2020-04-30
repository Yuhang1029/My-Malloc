#ifndef MY_MALLOC_H
#define MY_MALLOC_H

#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<limits.h>

/*
  The struct named data_block is the metadata.
  It can be seen as the node in the free list.
  size is the size of the data it allocated for.
  next points to the next free region.
  prev points to the previos free region.
*/

typedef struct _data_block data_block;
struct _data_block {
	size_t size;
	data_block* next;
	data_block* prev;
};

void* ff_malloc(size_t size);
void ff_free(void* ptr);

void* bf_malloc(size_t size);
void bf_free(void* ptr);

unsigned long get_data_segment_size();
unsigned long get_data_segment_free_space_size();


//The function we need to apply the operation
data_block* find_first_fit(size_t required_size);
data_block* find_best_fit(size_t required_size);
void combine(data_block* p);
data_block* extend_space(size_t required_size);
void free_opretaion(void* p);
void Remove(data_block* curr);
void Add(data_block* curr);
void split(data_block* curr, size_t required_size);
void num();
#endif
