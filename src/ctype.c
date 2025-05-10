#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ctype.h"
#include "cerror.h"

struct ctype {
  size_t size;
  void *(*dup)(const void *);
  void (*destroy)(void *);
  void (*print)(const void *);
  int (*cmp)(const void *, const void *);
};

// Helper function declaration
#define DEFINE_DUP(type) \
  static void *dup_##type(const void *item) { \
    ASSERT_NOT_NULL(item, NULL); \
    void *copy = malloc(sizeof(type)); \
    if (!copy) { \
      return NULL; \
    } \
    memcpy(copy, item, sizeof(type)); \
    return copy; \
  }

#define DEFINE_PRINT(type, format) \
  static void print_##type(const void *item) { \
    ASSERT_NOT_NULL(item, NULL); \
    const type *ptr = item; \
    printf(format, *ptr); \
  }

#define DEFINE_CMP(type) \
  static int cmp_##type(const void *item1, const void *item2) { \
    ASSERT_NOT_NULL(item1, NULL); \
    ASSERT_NOT_NULL(item2, NULL); \
    const type *item1_val = item1; \
    const type *item2_val = item2; \
    return (*item1_val > *item2_val) - (*item1_val < *item2_val); \
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
static void print_bool(const void *item);  // Print string "true" or "false"
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
static int cmp_string(const void *item1, const void *item2);
static void print_string(const void *item);

ctype *ctype_create(size_t size,
                    void *(*dup)(const void *),
                    void (*destroy)(void *),
                    void (*print)(const void *),
                    int (*cmp)(const void *, const void *)) {
  ASSERT_NOT_NULL(dup, NULL);
  ASSERT_NOT_NULL(destroy, NULL);
  ASSERT_NOT_NULL(print, NULL);
  ASSERT_NOT_NULL(cmp, NULL);

  ctype *type = malloc(sizeof(*type));
  if (!type) {
    ALLOC_ERROR("ctype");
  }
  
  type->size = size;
  type->dup = dup;
  type->destroy = destroy;
  type->print = print;
  type->cmp = cmp;
  return type;
}

void ctype_destroy(ctype *type) {
  if (type) {
    free(type);
  }
}

bool ctype_equals(const ctype *t1, const ctype *t2) {
  ASSERT_NOT_NULL(t1, "The first ctype");
  ASSERT_NOT_NULL(t2, "The second ctype");
  return (t1 == t2);
}

size_t data_size(const ctype *type) {
  ASSERT_NOT_NULL(type, NULL);
  return type->size;
}

void *data_dup(const void *item, const ctype *type) {
  ASSERT_NOT_NULL(item, NULL);
  ASSERT_NOT_NULL(type, NULL);
  return (item ? type->dup(item) : NULL);
}

void data_destroy(void *item, const ctype *type) {
  ASSERT_NOT_NULL(type, NULL);
  if (item) {
    type->destroy(item);
  }
}

void data_print(const void *item, const ctype *type) {
  ASSERT_NOT_NULL(item, NULL);
  ASSERT_NOT_NULL(type, NULL);
  if (item) {
    type->print(item);
  }
}

int data_cmp(const void *item1, const void *item2, const ctype *type) {
  ASSERT_NOT_NULL(item1, NULL);
  ASSERT_NOT_NULL(item2, NULL);
  ASSERT_NOT_NULL(type, NULL);
  return type->cmp(item1, item2);
}

// === Integral types ===
const ctype *ctype_int(void) {
  static const ctype int_type = {
    .size = sizeof(int),
    .dup = dup_int,
    .destroy = free,
    .print = print_int,
    .cmp = cmp_int,
  };
  return &int_type;
}

const ctype *ctype_long(void) {
  static const ctype long_type = {
    .size = sizeof(long),
    .dup = dup_long,
    .destroy = free,
    .print = print_long,
    .cmp = cmp_long,
  };
  return &long_type;
}

const ctype *ctype_char(void) {
  static const ctype char_type = {
    .size = sizeof(char),
    .dup = dup_char,
    .destroy = free,
    .print = print_char,
    .cmp = cmp_char,
  };
  return &char_type;
}

const ctype *ctype_bool(void) {
  static const ctype bool_type = {
    .size = sizeof(bool),
    .dup = dup_bool,
    .destroy = free,
    .print = print_bool,
    .cmp = cmp_bool,
  };
  return &bool_type;
}

const ctype *ctype_size_t(void) {
  static const ctype size_t_type = {
    .size = sizeof(size_t),
    .dup = dup_size_t,
    .destroy = free,
    .print = print_size_t,
    .cmp = cmp_size_t,
  };
  return &size_t_type;
}

// === Floating-point types ===
const ctype *ctype_float(void) {
  static const ctype float_type = {
    .size = sizeof(float),
    .dup = dup_float,
    .destroy = free,
    .print = print_float,
    .cmp = cmp_float,
  };
  return &float_type;
}

const ctype *ctype_double(void) {
  static const ctype double_type = {
    .size = sizeof(double),
    .dup = dup_double,
    .destroy = free,
    .print = print_double,
    .cmp = cmp_double,
  };
  return &double_type;
}

// === String type ===
const ctype *ctype_string(void) {
  static const ctype string_type = {
    .size = sizeof(char *),
    .dup = dup_string,
    .destroy = free,
    .print = print_string,
    .cmp = cmp_string,
  };
  return &string_type;
}

// Helper function implementation
static void print_bool(const void *item) {
  ASSERT_NOT_NULL(item, NULL);
  const bool *bool_ptr = item;
  printf(*bool_ptr ? "true" : "false");
}

static void *dup_string(const void *item) {
  ASSERT_NOT_NULL(item, NULL);

  const char *str_ptr = item;
  size_t item_len = strlen(str_ptr);

  char *dup_str = malloc(sizeof(char) * (item_len + 1));
  if (!dup_str) {
    return NULL;
  }
  strcpy(dup_str, str_ptr);
  return dup_str;
}

static int cmp_string(const void *s1, const void *s2) {
  ASSERT_NOT_NULL(s1, "The first string");
  ASSERT_NOT_NULL(s2, "The second string");
  const char *s1_ptr = s1;
  const char *s2_ptr = s2;
  return strcmp(s1_ptr, s2_ptr);
}

static void print_string(const void *item) {
  ASSERT_NOT_NULL(item, NULL);
  const char *str = item;
  printf("%s", str);
}
