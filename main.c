#include <assert.h>
#include "alist.h"

static bool is_odd(const void *item) {
  assert(item);
  const int *int_ptr = item;
  return (*int_ptr % 2 != 0);
}

static void negate(void *item) {
  assert(item);
  int *int_ptr = item;
  *int_ptr *= -1;
}

int main(void) {
  alist *al = NULL;
  alist *al_copy = NULL;
  const int *item = NULL;

  // Test alist_create
  al = alist_create(int_type());

  // Test alist_size, alist_empty
  assert(alist_size(al) == 0);
  assert(alist_empty(al));
  alist_print(al);        // []

  // Test alist_append
  alist_append(WRAP_INT(3), al);
  alist_append(WRAP_INT(3), al);
  alist_append(WRAP_INT(1), al);
  assert(alist_size(al) == 3);
  assert(!alist_empty(al));
  alist_print(al);        // [3, 3, 1]

  // Test alist_capacity, alist_reserve
  assert(alist_capacity(al) == 4);
  alist_reserve(10, al);
  assert(alist_capacity(al) == 10);
  alist_print(al);        // [3, 3, 1]

  // Test alist_insert
  alist_insert(0, WRAP_INT(-1), al);
  alist_insert(2, WRAP_INT(6), al);
  alist_insert(alist_size(al), WRAP_INT(10), al);
  alist_print(al);        // [-1, 3, 6, 3, 1, 10]

  // Test alist_contains
  assert(alist_contains(WRAP_INT(6), al));
  assert(!alist_contains(WRAP_INT(5), al));
  // Test alist_index
  assert(alist_index(WRAP_INT(3), al) == 1);
  assert(alist_index(WRAP_INT(7), al) == ALIST_INDEX_NOT_FOUND);
  // Test alist_index_last
  assert(alist_index_last(WRAP_INT(3), al) == 3);
  assert(alist_index_last(WRAP_INT(10), al) == 5);
  assert(alist_index_last(WRAP_INT(4), al) == ALIST_INDEX_NOT_FOUND);
  // Test alist_count
  assert(alist_count(WRAP_INT(6), al) == 1);
  assert(alist_count(WRAP_INT(3), al) == 2);
  assert(alist_count(WRAP_INT(9), al) == 0);

  // Test alist_dup
  al_copy = alist_dup(al);
  alist_print(al_copy);   // [-1, 3, 6, 3, 1, 10]

  // Test alist_get, alist_set
  item = alist_get(0, al_copy);
  assert(*item == -1);
  alist_set(0, WRAP_INT(1), al_copy);
  item = alist_get(0, al_copy);
  assert(*item == 1);
  item = alist_get(0, al);
  assert(*item == -1);
  alist_print(al_copy);   // [1, 3, 6, 3, 1, 10]

  // Test alist_pop
  alist_pop(2, al_copy);
  alist_print(al_copy);   // [1, 3, 3, 1, 10]
  // Test alist_remove
  alist_remove(WRAP_INT(10), al_copy);
  alist_print(al_copy);   // [1, 3, 3, 1]
  // Test alist_remove_last
  alist_remove_last(WRAP_INT(1), al_copy);  // [1, 3, 3]
  alist_print(al_copy);
  // Test alist_remove_all
  alist_remove_all(WRAP_INT(3), al_copy);   // [1]
  alist_print(al_copy);

  // Test alist_clear
  alist_clear(al_copy);
  alist_print(al_copy);
  alist_destroy(al_copy);

  // Test alist_swap
  alist_swap(2, 4, al);
  alist_print(al);        // [-1, 3, 1, 3, 6, 10]
  alist_swap(5, 1, al);
  alist_print(al);        // [-1, 10, 1, 3, 6, 3]
  // Test alist_qsort
  alist_qsort(al);
  alist_print(al);        // [-1, 1, 3, 3, 6, 10]
  // Test alist_bsearch
  assert(alist_bsearch(WRAP_INT(1), al) != ALIST_INDEX_NOT_FOUND);
  assert(alist_bsearch(WRAP_INT(10), al) != ALIST_INDEX_NOT_FOUND);
  assert(alist_bsearch(WRAP_INT(5), al) == ALIST_INDEX_NOT_FOUND);

  // Test alist_filter
  al_copy = alist_filter(is_odd, al);
  alist_print(al_copy);   // [-1, 1, 3, 3]
  // Test alist_map
  alist_map(negate, al_copy);
  alist_print(al_copy);   // [1, -1, -3, -3]

  // Test alist_destroy
  alist_destroy(al);
  alist_destroy(al_copy);
  alist_destroy(NULL);
}
