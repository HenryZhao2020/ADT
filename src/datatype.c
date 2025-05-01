#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "datatype.h"

#define DATATYPE_MEM_ERR(msg) do { \
  fprintf(stderr, "[datatype fatal] %s:%d: %s\n", __FILE__, __LINE__, (msg)); \
  exit(EXIT_FAILURE); \
} while (0)

struct datatype {
  size_t size;
  void *(*dup)(const void *);
  void (*destroy)(void *);
  void (*print)(const void *);
  int (*cmp)(const void *, const void *);
};

// Helper function declaration
#define DEFINE_DUP(type) \
  static void *dup_##type(const void *value) { \
    assert(value); \
    void *copy = malloc(sizeof(type)); \
    if (!copy) { \
      return NULL; \
    } \
    memcpy(copy, value, sizeof(type)); \
    return copy; \
  }

#define DEFINE_PRINT(type, format) \
  static void print_##type(const void *item) { \
    assert(item); \
    const type *ptr = item; \
    printf(format, *ptr); \
  }

#define DEFINE_CMP(type) \
  static int cmp_##type(const void *a, const void *b) { \
    const type *x = a; \
    const type *y = b; \
    return (*x > *y) - (*x < *y); \
  }

// === Integral types ===
DEFINE_DUP(int)
DEFINE_PRINT(int, "%d")
DEFINE_CMP(int)

DEFINE_DUP(long)
DEFINE_PRINT(long, "%ld")
DEFINE_CMP(long)

DEFINE_DUP(char)
DEFINE_PRINT(char, "%c")
DEFINE_CMP(char)

DEFINE_DUP(bool)
static void print_bool(const void *item);   // Print "true" or "false"
DEFINE_CMP(bool)

DEFINE_DUP(size_t)
DEFINE_PRINT(size_t, "%zu")
DEFINE_CMP(size_t)

// === Floating-point types ===
DEFINE_DUP(float)
DEFINE_PRINT(float, "%g")
DEFINE_CMP(float)

DEFINE_DUP(double)
DEFINE_PRINT(double, "%g")
DEFINE_CMP(double)

// === String type ===
static void *dup_string(const void *item);
static int cmp_string(const void *a, const void *b);
static void print_string(const void *item);

datatype *datatype_create(size_t size,
                          void *(*dup)(const void *),
                          void (*destroy)(void *),
                          void (*print)(const void *),
                          int (*cmp)(const void *, const void *)) {
  assert(dup);
  assert(destroy);
  assert(print);
  assert(cmp);

  datatype *type = malloc(sizeof(*type));
  if (!type) {
    DATATYPE_MEM_ERR("Failed to create datatype!");
  }
  
  type->size = size;
  type->dup = dup;
  type->destroy = destroy;
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
  return (a == b);
}

size_t data_size(const datatype *type) {
  assert(type);
  return type->size;
}

void *data_dup(const void *item, const datatype *type) {
  assert(item);
  assert(type);
  return (item ? type->dup(item) : NULL);
}

void data_destroy(void *item, const datatype *type) {
  assert(type);
  if (item) {
    type->destroy(item);
  }
}

void data_print(const void *item, const datatype *type) {
  assert(item);
  assert(type);
  if (item) {
    type->print(item);
  }
}

int data_cmp(const void *a, const void *b, const datatype *type) {
  assert(a);
  assert(b);
  assert(type);
  return type->cmp(a, b);
}

// === Integral types ===
const datatype *int_type(void) {
  static const datatype _int_type = {
    .size = sizeof(int),
    .dup = dup_int,
    .destroy = free,
    .print = print_int,
    .cmp = cmp_int,
  };
  return &_int_type;
}

const datatype *long_type(void) {
  static const datatype _long_type = {
    .size = sizeof(long),
    .dup = dup_long,
    .destroy = free,
    .print = print_long,
    .cmp = cmp_long,
  };
  return &_long_type;
}

const datatype *char_type(void) {
  static const datatype _char_type = {
    .size = sizeof(char),
    .dup = dup_char,
    .destroy = free,
    .print = print_char,
    .cmp = cmp_char,
  };
  return &_char_type;
}

const datatype *bool_type(void) {
  static const datatype _bool_type = {
    .size = sizeof(bool),
    .dup = dup_bool,
    .destroy = free,
    .print = print_bool,
    .cmp = cmp_bool,
  };
  return &_bool_type;
}

const datatype *size_t_type(void) {
  static const datatype _size_t_type = {
    .size = sizeof(size_t),
    .dup = dup_size_t,
    .destroy = free,
    .print = print_size_t,
    .cmp = cmp_size_t,
  };
  return &_size_t_type;
}

// === Floating-point types ===
const datatype *float_type(void) {
  static const datatype _float_type = {
    .size = sizeof(float),
    .dup = dup_float,
    .destroy = free,
    .print = print_float,
    .cmp = cmp_float,
  };
  return &_float_type;
}

const datatype *double_type(void) {
  static const datatype _double_type = {
    .size = sizeof(double),
    .dup = dup_double,
    .destroy = free,
    .print = print_double,
    .cmp = cmp_double,
  };
  return &_double_type;
}

// === String type ===
const datatype *string_type(void) {
  static const datatype _string_type = {
    .size = sizeof(char *),
    .dup = dup_string,
    .destroy = free,
    .print = print_string,
    .cmp = cmp_string,
  };
  return &_string_type;
}

// Helper function implementation
static void print_bool(const void *item) {
  assert(item);
  const bool *bool_ptr = item;
  printf(*bool_ptr ? "true" : "false");
}

static void *dup_string(const void *item) {
  assert(item);

  const char *str_ptr = item;
  size_t item_len = strlen(str_ptr);

  char *dup_str = malloc(sizeof(char) * (item_len + 1));
  if (!dup_str) {
    return NULL;
  }
  strcpy(dup_str, str_ptr);
  dup_str[item_len] = '\0';
  return dup_str;
}

static int cmp_string(const void *a, const void *b) {
  assert(a);
  assert(b);
  const char *a_ptr = a;
  const char *b_ptr = b;
  return strcmp(a_ptr, b_ptr);
}

static void print_string(const void *item) {
  assert(item);
  const char *str = item;
  printf("%s", str);
}
