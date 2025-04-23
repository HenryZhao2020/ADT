#include <stdio.h>
#include <assert.h>
#include "alist.h"

static bool int_is_odd(const void *item) {
  assert(item);
  const int *int_ptr = item;
  return (*int_ptr % 2 != 0);
}

static void int_add_one(void *item) {
  assert(item);
  int *int_ptr = item;
  (*int_ptr)++;
}

static bool float_is_int(const void *item) {
  assert(item);
  const float *float_ptr = item;
  float float_val = *float_ptr;
  int int_val = *float_ptr;
  return (float_val - int_val == 0);
}

static void float_triple(void *item) {
  assert(item);
  float *float_ptr = item;
  *float_ptr *= 3;
}

static bool double_is_pos(const void *item) {
  assert(item);
  const double *double_ptr = item;
  return (*double_ptr > 0);
}

static void double_truncate(void *item) {
  assert(item);
  double *double_ptr = item;
  int rounded = *double_ptr;
  *double_ptr = rounded;
}

static void test_alist_int(void) {
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
  // Test alist_equals
  assert(alist_equals(al, al_copy));
  assert(alist_equals(al_copy, al));

  // Test alist_get, alist_set
  item = alist_get(0, al_copy);
  assert(*item == -1);
  alist_set(0, WRAP_INT(1), al_copy);
  item = alist_get(0, al_copy);
  assert(*item == 1);
  item = alist_get(0, al);
  assert(*item == -1);
  alist_print(al_copy);   // [1, 3, 6, 3, 1, 10]
  assert(!alist_equals(al, al_copy));

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
  alist_destroy(al_copy);
  al_copy = alist_filter(int_is_odd, al);
  alist_print(al_copy);   // [-1, 1, 3, 3]
  // Test alist_map
  alist_map(int_add_one, al_copy);
  alist_print(al_copy);   // [0, 2, 4, 4]

  // Test alist_reverse
  alist_reverse(al);      // Even number of items
  alist_print(al);        // [10, 6, 3, 3, 1, -1]
  alist_insert(0, WRAP_INT(5), al);
  alist_reverse(al);      // Odd number of items
  alist_print(al);        // [-1, 1, 3, 3, 6, 10, 5]

  // Test alist_destroy
  alist_destroy(al);
  alist_destroy(al_copy);
  alist_destroy(NULL);
}

static void test_alist_float(void) {
  alist *al = NULL;
  alist *al_copy = NULL;
  const float *item = NULL;

  // Test alist_create
  al = alist_create(float_type());

  // Test alist_size, alist_empty
  assert(alist_size(al) == 0);
  assert(alist_empty(al));
  alist_print(al);        // []

  // Test alist_append
  alist_append(WRAP_FLOAT(3.0f), al);
  alist_append(WRAP_FLOAT(3.0f), al);
  alist_append(WRAP_FLOAT(1.5f), al);
  assert(alist_size(al) == 3);
  assert(!alist_empty(al));
  alist_print(al);        // [3.0, 3.0, 1.5]

  // Test alist_capacity, alist_reserve
  assert(alist_capacity(al) == 4);
  alist_reserve(10, al);
  assert(alist_capacity(al) == 10);
  alist_print(al);        // [3.0, 3.0, 1.5]

  // Test alist_insert
  alist_insert(0, WRAP_FLOAT(-1.0f), al);
  alist_insert(2, WRAP_FLOAT(6.25f), al);
  alist_insert(alist_size(al), WRAP_FLOAT(10.0f), al);
  alist_print(al);        // [-1.0, 3.0, 6.25, 3.0, 1.5, 10.0]

  // Test alist_contains
  assert(alist_contains(WRAP_FLOAT(6.25f), al));
  assert(!alist_contains(WRAP_FLOAT(5.0f), al));
  // Test alist_index
  assert(alist_index(WRAP_FLOAT(3.0f), al) == 1);
  assert(alist_index(WRAP_FLOAT(7.0f), al) == ALIST_INDEX_NOT_FOUND);
  // Test alist_index_last
  assert(alist_index_last(WRAP_FLOAT(3.0f), al) == 3);
  assert(alist_index_last(WRAP_FLOAT(10.0f), al) == 5);
  assert(alist_index_last(WRAP_FLOAT(4.0f), al) == ALIST_INDEX_NOT_FOUND);
  // Test alist_count
  assert(alist_count(WRAP_FLOAT(6.25f), al) == 1);
  assert(alist_count(WRAP_FLOAT(3.0f), al) == 2);
  assert(alist_count(WRAP_FLOAT(9.0f), al) == 0);

  // Test alist_dup
  al_copy = alist_dup(al);
  alist_print(al_copy);   // [-1.0, 3.0, 6.25, 3.0, 1.5, 10.0]
  // Test alist_equals
  assert(alist_equals(al, al_copy));
  assert(alist_equals(al_copy, al));

  // Test alist_get, alist_set
  item = alist_get(0, al_copy);
  assert(*item == -1.0f);
  alist_set(0, WRAP_FLOAT(1.0f), al_copy);
  item = alist_get(0, al_copy);
  assert(*item == 1.0f);
  item = alist_get(0, al);
  assert(*item == -1.0f);
  alist_print(al_copy);   // [1.0, 3.0, 6.25, 3.0, 1.5, 10.0]
  assert(!alist_equals(al, al_copy));

  // Test alist_pop
  alist_pop(2, al_copy);
  alist_print(al_copy);   // [1.0, 3.0, 3.0, 1.5, 10.0]
  // Test alist_remove
  alist_remove(WRAP_FLOAT(10.0f), al_copy);
  alist_print(al_copy);   // [1.0, 3.0, 3.0, 1.5]
  // Test alist_remove_last
  alist_remove_last(WRAP_FLOAT(1.5f), al_copy);
  alist_print(al_copy);   // [1.0, 3.0, 3.0]
  // Test alist_remove_all
  alist_remove_all(WRAP_FLOAT(3.0f), al_copy);
  alist_print(al_copy);   // [1.0]

  // Test alist_clear
  alist_clear(al_copy);
  alist_print(al_copy);

  // Test alist_swap
  alist_swap(2, 4, al);
  alist_print(al);        // [-1.0, 3.0, 1.5, 3.0, 6.25, 10.0]
  alist_swap(5, 1, al);
  alist_print(al);        // [-1.0, 10.0, 1.5, 3.0, 6.25, 3.0]

  // Test alist_qsort
  alist_qsort(al);
  alist_print(al);        // [-1.0, 1.5, 3.0, 3.0, 6.25, 10.0]
  // Test alist_bsearch
  assert(alist_bsearch(WRAP_FLOAT(1.5f), al) != ALIST_INDEX_NOT_FOUND);
  assert(alist_bsearch(WRAP_FLOAT(10.0f), al) != ALIST_INDEX_NOT_FOUND);
  assert(alist_bsearch(WRAP_FLOAT(5.0f), al) == ALIST_INDEX_NOT_FOUND);

  // Test alist_filter
  alist_destroy(al_copy);
  al_copy = alist_filter(float_is_int, al);
  alist_print(al_copy);   // [-1, 3, 3, 10]
  // Test alist_map
  alist_map(float_triple, al_copy);
  alist_print(al_copy);   // [-3, 9, 9, 30]

  // Test alist_reverse
  alist_reverse(al);      // Even number of items
  alist_print(al);        // [10.0, 6.25, 3.0, 3.0, 1.5, -1.0]
  alist_insert(0, WRAP_FLOAT(5.0f), al);
  alist_reverse(al);      // Odd number of items
  alist_print(al);        // [-1.0, 1.5, 3.0, 3.0, 6.25, 10.0, 5.0]

  // Test alist_destroy
  alist_destroy(al);
  alist_destroy(al_copy);
  alist_destroy(NULL);
}


static void test_alist_double(void) {
  alist *al = NULL;
  alist *al_copy = NULL;
  const double *item = NULL;

  // Test alist_create
  al = alist_create(double_type());

  // Test alist_size, alist_empty
  assert(alist_size(al) == 0);
  assert(alist_empty(al));
  alist_print(al);        // []

  // Test alist_append
  alist_append(WRAP_DOUBLE(3.0), al);
  alist_append(WRAP_DOUBLE(3.0), al);
  alist_append(WRAP_DOUBLE(1.5), al);
  assert(alist_size(al) == 3);
  assert(!alist_empty(al));
  alist_print(al);        // [3, 3, 1.5]

  // Test alist_capacity, alist_reserve
  assert(alist_capacity(al) == 4);
  alist_reserve(10, al);
  assert(alist_capacity(al) == 10);
  alist_print(al);        // [3, 3, 1.5]

  // Test alist_insert
  alist_insert(0, WRAP_DOUBLE(-1.0), al);
  alist_insert(2, WRAP_DOUBLE(6.25), al);
  alist_insert(alist_size(al), WRAP_DOUBLE(10.0), al);
  alist_print(al);        // [-1, 3, 6.25, 3, 1.5, 10]

  // Test alist_contains
  assert(alist_contains(WRAP_DOUBLE(6.25), al));
  assert(!alist_contains(WRAP_DOUBLE(5.0), al));
  // Test alist_index
  assert(alist_index(WRAP_DOUBLE(3.0), al) == 1);
  assert(alist_index(WRAP_DOUBLE(7.0), al) == ALIST_INDEX_NOT_FOUND);
  // Test alist_index_last
  assert(alist_index_last(WRAP_DOUBLE(3.0), al) == 3);
  assert(alist_index_last(WRAP_DOUBLE(10.0), al) == 5);
  assert(alist_index_last(WRAP_DOUBLE(4.0), al) == ALIST_INDEX_NOT_FOUND);
  // Test alist_count
  assert(alist_count(WRAP_DOUBLE(6.25), al) == 1);
  assert(alist_count(WRAP_DOUBLE(3.0), al) == 2);
  assert(alist_count(WRAP_DOUBLE(9.0), al) == 0);

  // Test alist_dup
  al_copy = alist_dup(al);
  alist_print(al_copy);   // [-1, 3, 6.25, 3, 1.5, 10]
  // Test alist_equals
  assert(alist_equals(al, al_copy));
  assert(alist_equals(al_copy, al));

  // Test alist_get, alist_set
  item = alist_get(0, al_copy);
  assert(*item == -1.0);
  alist_set(0, WRAP_DOUBLE(1.0), al_copy);
  item = alist_get(0, al_copy);
  assert(*item == 1.0);
  item = alist_get(0, al);
  assert(*item == -1.0);
  alist_print(al_copy);   // [1, 3, 6.25, 3, 1.5, 10]
  assert(!alist_equals(al, al_copy));

  // Test alist_pop
  alist_pop(2, al_copy);
  alist_print(al_copy);   // [1, 3, 3, 1.5, 10]
  // Test alist_remove
  alist_remove(WRAP_DOUBLE(10.0), al_copy);
  alist_print(al_copy);   // [1, 3, 3, 1.5]
  // Test alist_remove_last
  alist_remove_last(WRAP_DOUBLE(1.5), al_copy);
  alist_print(al_copy);   // [1, 3, 3]
  // Test alist_remove_all
  alist_remove_all(WRAP_DOUBLE(3.0), al_copy);
  alist_print(al_copy);   // [1]

  // Test alist_clear
  alist_clear(al_copy);
  alist_print(al_copy);

  // Test alist_swap
  alist_swap(2, 4, al);
  alist_print(al);        // [-1, 3, 1.5, 3, 6.25, 10]
  alist_swap(5, 1, al);
  alist_print(al);        // [-1, 10, 1.5, 3, 6.25, 3]

  // Test alist_qsort
  alist_qsort(al);
  alist_print(al);        // [-1, 1.5, 3, 3, 6.25, 10]
  // Test alist_bsearch
  assert(alist_bsearch(WRAP_DOUBLE(1.5), al) != ALIST_INDEX_NOT_FOUND);
  assert(alist_bsearch(WRAP_DOUBLE(10.0), al) != ALIST_INDEX_NOT_FOUND);
  assert(alist_bsearch(WRAP_DOUBLE(5.0), al) == ALIST_INDEX_NOT_FOUND);

  // Test alist_filter
  alist_destroy(al_copy);
  al_copy = alist_filter(double_is_pos, al);
  alist_print(al_copy);   // [1.5, 3, 3, 6.25, 10]
  // Test alist_map
  alist_map(double_truncate, al_copy);
  alist_print(al_copy);   // [1, 3, 3, 6, 10]

  // Test alist_reverse
  alist_reverse(al);
  alist_print(al);        // [10, 6.25, 3, 3, 1.5, -1]
  alist_insert(0, WRAP_DOUBLE(5.0), al);
  alist_reverse(al);
  alist_print(al);        // [-1, 1.5, 3, 3, 6.25, 10, 5]

  // Test alist_destroy
  alist_destroy(al);
  alist_destroy(al_copy);
  alist_destroy(NULL);
}

int main(void) {
  printf("test_alist_int:\n");
  test_alist_int();
  printf("\n");

  printf("test_alist_float:\n");
  test_alist_float();
  printf("\n");

  printf("test_alist_double:\n");
  test_alist_double();
  printf("\n");
}
