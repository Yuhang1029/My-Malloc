#include "my_malloc.h"
/*
  There are three global variables in this program.
  head points to the first data_block in the free list, if there is no free region now, it points to NULL.
  useful_space records the size of the regions which are currently be used.
  total_space records the entire size of the heap allocation.
*/

data_block* head = NULL;
unsigned long useful_space = 0;
unsigned long total_space = 0;

/*
  This function is used for debugging.
*/
void num(){
  int ans = 0;
  data_block* curr = head;
  while(curr != NULL){
    ans++;
    if(ans == 1)
      printf("%zu  %zu  %zu\n", (size_t)curr, (size_t)curr->next, curr->size + sizeof(data_block));
    else
      printf("%zu  %zu  %zu\n", (size_t)curr, (size_t)curr->prev, curr->size + sizeof(data_block));
    curr = curr->next;
  }
  printf("%d\n", ans);
}

/*
  This function is the algorithm of the first fit allocation.
  It will allocate an address from the first free region with enough space to fit the requested allocation size.
*/

data_block* find_first_fit(size_t required_size) {
  data_block* ans = NULL;
  data_block* curr = head;
  while (curr) {
    if (curr->size >= required_size) {
      ans = curr;
      break;
    }
    curr = curr->next;
  }
  return ans;
}

/*
  This function is the algorithm of the best fit allocation.
  It will examine all of the free space information, and allocate an address from the free region 
  which has the smallest number of bytes greater than or equal to the requested allocation size.
*/

data_block* find_best_fit(size_t required_size) {
  data_block* ans = NULL;
  data_block* curr = head;
  size_t minimum = INT_MAX;
  while (curr) {
    if (curr->size >= required_size) {
      if (curr->size == required_size){
	ans = curr;
	break;
      }
      if (curr->size < minimum) {
	minimum = curr->size;
	ans = curr;
      }
    }
    curr = curr->next;
  }
  return ans;
}


/*
  This function is used to to merge the newly freed region with any currently free adjacent regions.
*/

void combine(data_block* p) {
  if (p->next != NULL) {
    if ((char*)p + p->size + sizeof(data_block) == (char*)p->next) {
      p->size = p->size + sizeof(data_block) + p->next->size;
      Remove(p->next);
    }
  }
  
  if (p->prev != NULL) {
    if ((char*)p - p->prev->size - sizeof(data_block) == (char*)p->prev) {
      p->prev->size = p->prev->size + sizeof(data_block) + p->size;
      Remove(p);
    }
  }
}

/*
  When there is no suitable space for the allocation,
  the function will call sbrk() to assign new memory from heap.
*/

data_block* extend_space(size_t required_size) {
  void* temp = sbrk(required_size + sizeof(data_block));
  if (temp == (void*)-1) {
    return NULL;
  }

  data_block* now = (data_block*) temp;
  total_space += required_size + sizeof(data_block);
  useful_space += required_size;

  now->size = required_size;
  now->next = NULL;
  now->prev = NULL;

  return now;
}


/*
  This function is used to free the memory of the current data_block.
*/
void free_opretaion(void* p) {
  if (p == NULL) {
    return;
  }
  
  data_block* ptr = (data_block*)((char*)p - sizeof(data_block));
  
  if (head == ptr || ptr->next != NULL || ptr->prev != NULL) {
    return;
  }
  else {
    useful_space -= ptr->size;
    Add(ptr);
    combine(ptr);
  }
}


//Remove the node from the free list
void Remove(data_block* curr) {
  //Remove a node from a linkedlist can be divided into two parts.
  //The node which will be removed is the head or not.
  if (curr == head) {
    head = curr->next;
    curr->next = NULL;
    if (head!= NULL) {
      head->prev = NULL;
    }
  }
  else{
    curr->prev->next = curr->next;
    if (curr->next != NULL) {
      curr->next->prev = curr->prev;
    }
    curr->prev = NULL;
    curr->next = NULL;
  }
}

//Add the node in the free list inorder
void Add(data_block* curr) {
  //Need to add in the head of the free list
  if (head == NULL || curr < head) {
    curr->next = head;
    curr->prev = NULL;
    if (head != NULL) {
      head->prev = curr;
    }
    head = curr;
  }
  
  else {
    data_block* temp = head;
    while (temp->next != NULL && temp->next < curr) {
      temp = temp->next;
    }
    //temp is in the tail of the free list
    if (temp->next == NULL) {
      temp->next = curr;
      curr->prev = temp;
      curr->next = NULL;
    }
    //temp is in the middle of the free list
    else {
      curr->next = temp->next;
      curr->prev = temp;
      curr->next->prev = curr;
      temp->next = curr;
    }
  }
}

void split(data_block* curr, size_t required_size) {
  //Cannot split
  if (curr->size <= required_size + sizeof(data_block)) {
    useful_space += curr->size;
    Remove(curr);
  }
  //Can split
  else {
    useful_space += required_size;
    
    //deal with new_ptr
    data_block* new_ptr = (data_block*)((char*)curr + required_size + sizeof(data_block));
    new_ptr->size = curr->size - required_size - sizeof(data_block);
    curr->size = required_size;
    
    new_ptr->prev = NULL;
    new_ptr->next = NULL;
    Remove(curr);
    Add(new_ptr);
  }	
}

//Two function for first fit allocation
void* ff_malloc(size_t size) {
  if (size <= 0) {
    return NULL;
  }

  data_block* insert = find_first_fit(size);
  if (insert != NULL) {
    split(insert, size);
  }
  else {
    insert = extend_space(size);
  }
  return insert + 1;
}

void ff_free(void* ptr) {
  free_opretaion(ptr);
}


//Two function for best fit allocation
void* bf_malloc(size_t size) {
  if (size <= 0) {
    return NULL;
  }
  
  data_block* insert = find_best_fit(size);
  if (insert != NULL) {
    split(insert, size);
  }
  else {
    insert = extend_space(size);
  }
  return insert + 1;
}

void bf_free(void* ptr) {
  free_opretaion(ptr);
}




unsigned long get_data_segment_size() {
  return total_space;
}

unsigned long get_data_segment_free_space_size(){
  return total_space - useful_space;
}
 
