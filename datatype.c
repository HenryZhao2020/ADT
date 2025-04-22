#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "datatype.h"

struct datatype {
  size_t size;
  void *(*dup)(const void *);
  void (*dealloc)(void *);
  void (*print)(const void *);
  int (*cmp)(const void *, const void *);
};

const datatype *datatype_create(size_t size,
                                void *(*dup)(const void *),
                                void (*dealloc)(void *),
                                void (*print)(const void *),
                                int (*cmp)(const void *, const void *)) {
  assert(dup);
  assert(dealloc);
  assert(print);
  assert(cmp);

  datatype *type = malloc(sizeof(*type));
  if (!type) {
    return NULL;
  }
  
  type->size = size;
  type->dup = dup;
  type->dealloc = dealloc;
  type->print = print;
  type->cmp = cmp;
  return type;
}

void datatype_destroy(datatype *type) {
  if (type) {
    free(type);
  }
}

size_t data_size(const datatype *type) {
  assert(type);
  return type->size;
}

void *data_dup(const void *item, const datatype *type) {
  assert(item);
  assert(type);
  return type->dup(item);
}

void data_dealloc(void *item, const datatype *type) {
  assert(item);
  assert(type);
  type->dealloc(item);
}

void data_print(const void *item, const datatype *type) {
  assert(item);
  assert(type);
  type->print(item);
}

int data_cmp(const void *a, const void *b, const datatype *type) {
  assert(a);
  assert(b);
  assert(type);
  return type->cmp(a, b);
}

static void *dup_int(const void *item) {
  assert(item);
  const int *int_ptr = item;
  int *dup = malloc(sizeof(*dup));
  if (!dup) {
    return NULL;
  }
  *dup = *int_ptr;
  return dup;
}

static void print_int(const void *item) {
  assert(item);
  const int *int_ptr = item;
  printf("%d", *int_ptr);
}

static int cmp_int(const void *a, const void *b) {
  assert(a);
  assert(b);
  const int *a_ptr = a;
  const int *b_ptr = b;
  return (*a_ptr > *b_ptr) - (*a_ptr < *b_ptr);
}

const datatype *int_type(void) {
  static const datatype type = {sizeof(int), dup_int, free, 
                                print_int, cmp_int};
  return &type;
}
