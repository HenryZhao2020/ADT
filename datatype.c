#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "datatype.h"

struct datatype {
  size_t id;
  size_t size;
  void *(*dup)(const void *);
  void (*dealloc)(void *);
  void (*print)(const void *);
  int (*cmp)(const void *, const void *);
};

// next_id uniquely identifies datatypes.
// Values 0-99 are reserved for built-in types:
//   0: int
//   1: float
//   2: double
//   3: bool
//   4: char
//   ...
// next_id is incremented by 1 when a datatype is created by datatype_create
static size_t next_id = 100;

// Helper function declaration
static void *dup_memcpy(const void *src, size_t size);
static void *dup_int(const void *item);
static void print_int(const void *item);
static int cmp_int(const void *a, const void *b);
static void *dup_float(const void *item);
static void print_float(const void *item);
static int cmp_float(const void *a, const void *b);
static void *dup_double(const void *item);
static void print_double(const void *item);
static int cmp_double(const void *a, const void *b);

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
  
  type->id = (next_id++);
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

bool datatype_equals(const datatype *a, const datatype *b) {
  assert(a);
  assert(b);
  return (a->id == b->id);
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

const datatype *int_type(void) {
  static const datatype type = {
    .id = 0,
    .size = sizeof(int),
    .dup = dup_int,
    .dealloc = free,
    .print = print_int,
    .cmp = cmp_int,
  };
  return &type;
}

const datatype *float_type(void) {
  static const datatype type = {
    .id = 1,
    .size = sizeof(float),
    .dup = dup_float,
    .dealloc = free,
    .print = print_float,
    .cmp = cmp_float,
  };
  return &type;
}

const datatype *double_type(void) {
  static const datatype type = {
    .id = 2,
    .size = sizeof(double),
    .dup = dup_double,
    .dealloc = free,
    .print = print_double,
    .cmp = cmp_double,
  };
  return &type;
}

// Helper function implementation
static void *dup_memcpy(const void *src, size_t size) {
  assert(src);
  void *copy = malloc(size);
  if (!copy) {
    return NULL;
  }
  memcpy(copy, src, size);
  return copy;
}

static void *dup_int(const void *item) {
  assert(item);
  return dup_memcpy(item, sizeof(int));
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

static void *dup_float(const void *item) {
  assert(item);
  return dup_memcpy(item, sizeof(float));
}

static void print_float(const void *item) {
  assert(item);
  const float *float_ptr = item;
  printf("%.6g", *float_ptr);
}

static int cmp_float(const void *a, const void *b) {
  assert(a);
  assert(b);
  const float *a_ptr = a;
  const float *b_ptr = b;
  return (*a_ptr > *b_ptr) - (*a_ptr < *b_ptr);
}

static void *dup_double(const void *item) {
  assert(item);
  return dup_memcpy(item, sizeof(double));
}

static void print_double(const void *item) {
  assert(item);
  const double *double_ptr = item;
  printf("%.6g", *double_ptr);
}

static int cmp_double(const void *a, const void *b) {
  assert(a);
  assert(b);
  const double *a_ptr = a;
  const double *b_ptr = b;
  return (*a_ptr > *b_ptr) - (*a_ptr < *b_ptr);
}
