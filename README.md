## Overview

An alist stores items in a dynamically resizable array, with all items deeply copied into heap memory.

Each alist is associated with a specific **datatype**, defining a common type for all stored items. 
Type-specific behaviors (duplication, comparison, printing, and deallocation) are handled through the **datatype interface**.

---

## Memory Model

- All inserted items are deeply copied into separately allocated heap memory.
- Stack-allocated or heap-allocated objects are both safe to insert; the alist duplicates them internally.
- Stored items are automatically freed when individually removed or when the alist is destroyed.
- Clients are responsible for freeing the original heap-allocated objects after insertion to avoid memory leaks.

---

## Example Usage (examples/demo_alist.c)

```c
// Demonstration of alist usage.

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "alist.h"

// Append stack-allocated, temporary values to al
static void alist_append_stack(alist *al) {
  assert(al);

  int a = 5;
  const int b = 10;
  alist_append(al, &a);   // a is deep-copied into al
  alist_add(al, &b);      // 'alist_add' is an alias for 'alist_append'
  // a and b can now safely exit the stack frame

  // 'WRAP_INT' adds a stack-allocated integer without a variable
  alist_append(al, WRAP_INT(20));
}

// Append heap-allocated objects to al
static void alist_append_heap(alist *al) {
  assert(al);

  int *ptr = malloc(sizeof(*ptr));
  *ptr = 30;
  alist_append(al, ptr);  // ptr is deep-copied into al
  free(ptr);              // Client must free original pointer
}

// Check if num is even
static bool is_even(const void *ptr) {
  assert(ptr);
  const int *int_ptr = ptr;
  return (*int_ptr % 2 == 0);
}

// Multiply the value of num by 3
static void triple(void *ptr) {
  assert(ptr);
  int *int_ptr = ptr;
  *int_ptr *= 3;
}

int main(void) {
  // Create an integer alist ['int_type' is included in datatype.h]
  alist *al = alist_create(int_type());
  
  // Append integers to the back of al
  alist_append_stack(al);
  alist_append_heap(al);
  
  // Print al to the console
  alist_print(al);        // Console: [5, 10, 20, 30]
  // Get the size of al
  assert(alist_size(al) == 4);

  // Inserting integers before index positions
  alist_insert(al, 2, WRAP_INT(5));       // al: [5, 10, 5, 20, 30]
  alist_insert(al, 3, WRAP_INT(25));      // al: [5, 10, 5, 25, 20, 30]
  alist_insert_front(al, WRAP_INT(35));   // al: [35, 5, 10, 5, 25, 20, 30]

  // Duplicate al
  alist *al_copy = alist_dup(al);
  alist_print(al_copy);   // Console: [35, 5, 10, 5, 25, 20, 30]
  // Comparing al and al_copy for equality
  assert(alist_equals(al, al_copy) == true);

  // Remove the item at index position 2
  alist_pop(al, 2);                   // al: [35, 5, 5, 25, 20, 30]
  // Remove the first item with the value 25
  alist_remove(al, WRAP_INT(25));     // al: [35, 5, 5, 20, 30]
  // Remove all items with the value 5
  alist_remove_all(al, WRAP_INT(5));  // al: [35, 20, 30]
  // Remove all even integers from al
  alist_remove_if(al, is_even);       // al: [35]
  alist_print(al);        // Console: [35]
  
  // Remove all elements from al, but al is kept
  alist_clear(al);
  alist_print(al);        // Console: []

  // Add previous items from the copy
  alist_append_all(al, al_copy);  
  alist_print(al);        // Console: [35, 5, 10, 5, 25, 20, 30]

  // Check if integers are in al
  assert(alist_contains(al, WRAP_INT(10)));
  assert(!alist_contains(al, WRAP_INT(99)));
  // First occurrence of 5 is at index position 1
  assert(alist_index(al, WRAP_INT(5)) == 1);
  // Last occurrence of 5 is at index position 3
  assert(alist_index_last(al, WRAP_INT(5)) == 3);
  // 'ALIST_INDEX_NOT_FOUND': 99 is not in al
  assert(alist_index(al, WRAP_INT(99)) == ALIST_INDEX_NOT_FOUND);
  // Count the number of occurrences
  assert(alist_count(al, WRAP_INT(5)) == 2);
  assert(alist_count(al, WRAP_INT(99)) == 0);

  // Access and mutate items at specific index positions
  // Get the item at index position 2
  const int *item = alist_get(al, 2);
  assert(*item == 10);
  // Modify the item at index position 0
  int *item_mutable = alist_get_mutable(al, 0);
  *item_mutable = 40;
  // Set the item at index position 5
  alist_set(al, 5, WRAP_INT(15));
  alist_print(al);        // Console: [40, 5, 10, 5, 25, 15, 30]

  // Quick sort algorithm
  alist_qsort(al);         
  alist_print(al);        // Console: [5, 5, 10, 15, 25, 30, 40]
  // Binary search on the sorted list
  assert(alist_bsearch(al, WRAP_INT(15)) != ALIST_INDEX_NOT_FOUND);
  assert(alist_bsearch(al, WRAP_INT(99)) == ALIST_INDEX_NOT_FOUND);
  // Reverse the list
  alist_reverse(al);
  alist_print(al);        // Console: [40, 30, 25, 15, 10, 5, 5]
  // Multiply all items by 3
  alist_map(al, triple);
  alist_print(al);        // Console: [120, 90, 75, 45, 30, 15, 15]

  // Convert the alist to int array
  void **arr = alist_to_array(al);
  printf("[");
  for (size_t i = 0; i < alist_size(al); ++i) {
    if (i != 0) printf(", ");
    int *ptr = arr[i];
    printf("%d", *ptr);
    free(ptr);
  }
  printf("]\n");          // Console: [120, 90, 75, 45, 30, 15, 15]
  free(arr);

  // IMPORTANT: free al and al_copy to prevent memory leak
  alist_destroy(al);
  alist_destroy(al_copy);
  alist_destroy(NULL);    // Also safe
}
