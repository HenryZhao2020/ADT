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

void *(*data_dup_method(const datatype *type))(const void *) {
  assert(type);
  return type->dup;
}

void *data_dup(const void *item, const datatype *type) {
  assert(item);
  assert(type);
  return type->dup(item);
}

void (*data_dealloc_method(const datatype *type))(void *) {
  assert(type);
  return type->dealloc;
}

void data_dealloc(void *item, const datatype *type) {
  assert(item);
  assert(type);
  type->dealloc(item);
}

void (*data_print_method(const datatype *type))(const void *) {
  assert(type);
  return type->print;
}

void data_print(const void *item, const datatype *type) {
  assert(item);
  assert(type);
  type->print(item);
}

int (*data_cmp_method(const datatype *type))(const void *, const void *) {
  assert(type);
  return type->cmp;
}

int data_cmp(const void *item1, const void *item2, const datatype *type) {
  assert(item1);
  assert(item2);
  assert(type);
  return type->cmp(item1, item2);
}

static void *dup_int(const void *item) {
  assert(item);
  const int *int_ptr = item;
  int *dup = malloc(sizeof(*dup));
  if (!dup) {
    return NULL;
  }
  *dup = *int_ptr;
  void *dup_void = dup;
  return dup_void;
}

static void print_int(const void *item) {
  assert(item);
  const int *int_ptr = item;
  printf("%d", *int_ptr);
}

static int cmp_int(const void *item1, const void *item2) {
  assert(item1);
  assert(item2);
  const int *int_ptr1 = item1;
  const int *int_ptr2 = item2;
  return *int_ptr1 - *int_ptr2;
}

const datatype *int_type(void) {
  static const datatype type = {sizeof(int), dup_int, free, 
                                print_int, cmp_int};
  return &type;
}
