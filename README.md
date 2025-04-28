## Overview

An alist stores items in a dynamically resizable array,  
with all items deeply copied into heap memory.

Each alist is associated with a specific **datatype**, defining a common type  
for all stored items. Type-specific behaviors (duplication, comparison,  
printing, and deallocation) are handled through the **datatype interface**.

---

## Memory Model

- All inserted items are deeply copied into separately allocated heap memory.
- Stack-allocated or heap-allocated objects are both safe to insert;  
  the alist duplicates them internally.
- Stored items are automatically freed when individually removed or  
  when the alist is destroyed.
- Clients are responsible for freeing the original heap-allocated objects  
  after insertion to avoid memory leaks.

---

## Example Usage (examples/demo_alist.c)

```c
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "alist.h"

// Append stack-allocated, temporary values to al
static void alist_append_stack(alist *al) {
  assert(al);
  int a = 5;
  const int b = 10;
  alist_append(al, &a);
  alist_add(al, &b);
  alist_append(al, WRAP_INT(20));
}

// Append heap-allocated objects to al
static void alist_append_heap(alist *al) {
  assert(al);
  int *ptr = malloc(sizeof(*ptr));
  *ptr = 30;
  alist_append(al, ptr);
  free(ptr);
}

// Predicate to check if an integer is even
static bool is_even(const void *ptr) {
  assert(ptr);
  return (*(const int *)ptr % 2) == 0;
}

// Map function to triple an integer
static void triple(void *ptr) {
  assert(ptr);
  *(int *)ptr *= 3;
}

int main(void) {
  alist *al = alist_create(int_type());

  alist_append_stack(al);
  alist_append_heap(al);

  alist_print(al);  // [5, 10, 20, 30]

  // Inserting values
  alist_insert(al, 2, WRAP_INT(5));
  alist_insert(al, 3, WRAP_INT(25));
  alist_insert_front(al, WRAP_INT(35));

  alist *al_copy = alist_dup(al);
  assert(alist_equals(al, al_copy));

  // Removing elements
  alist_pop(al, 2);
  alist_remove(al, WRAP_INT(25));
  alist_remove_all(al, WRAP_INT(5));
  alist_remove_if(al, is_even);

  alist_clear(al);
  alist_append_all(al, al_copy);

  // Searching
  assert(alist_contains(al, WRAP_INT(10)));
  assert(alist_index(al, WRAP_INT(5)) == 1);
  assert(alist_index_last(al, WRAP_INT(5)) == 3);
  assert(alist_bsearch(al, WRAP_INT(99)) == ALIST_INDEX_NOT_FOUND);

  // Access and modify
  int *item_mutable = alist_get_mutable(al, 0);
  *item_mutable = 40;
  alist_set(al, 5, WRAP_INT(15));

  // Sorting, reversing, mapping
  alist_qsort(al);
  alist_reverse(al);
  alist_map(al, triple);

  // Export to array
  void **arr = alist_to_array(al);
  printf("[");
  for (size_t i = 0; i < alist_size(al); ++i) {
    if (i != 0) printf(", ");
    printf("%d", *(int *)arr[i]);
    free(arr[i]);
  }
  printf("]\n");
  free(arr);

  // Cleanup
  alist_destroy(al);
  alist_destroy(al_copy);
  alist_destroy(NULL); // Safe to destroy NULL
}