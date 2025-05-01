// This module provides the datatype ADT, which is essential for 
//   implementing generic storage ADTs.

#ifndef DATATYPE_H
#define DATATYPE_H

#include <stddef.h>
#include <stdbool.h>

// A datatype describes a type for use in generic storage ADTs.
// attributes:
//   - size:    the size in bytes of the associated data
// methods:
//   - dup:     creates a deep copy of the given value (heap-allocated),
//              returns NULL if allocation fails
//   - destroy: frees an value previously created by dup
//   - print:   displays a human-readable representation of the value
//   - cmp:     compares two items a and b
//              returns 0 if equal, <0 if a < b, >0 if a > b
// note: clients must create only one instance of a given datatype;
//       all uses of the same type must refer to the same datatype pointer
typedef struct datatype datatype;

// Wrap stack variables to temporary pointers for generic storage ADTs
// === Integral types ===
#define WRAP_INT(i) (&(int) {i})
#define WRAP_LONG(l) (&(long) {l})
#define WRAP_CHAR(c) (&(char) {c})
#define WRAP_BOOL(b) (&(bool) {b})
#define WRAP_SIZE_T(st) (&(size_t) {st})
// === Floating-point types ===
#define WRAP_FLOAT(f) (&(float) {f})
#define WRAP_DOUBLE(d) (&(double) {d})

// datatype_create(size, dup, destroy, print, cmp) creates a datatype 
//   with the given attributes (see datatype documentation above).
// requires: dup, destroy, print, cmp are not NULL
// effects: allocates heap memory [caller must free with datatype_destroy]
datatype *datatype_create(size_t size,
                          void *(*dup)(const void *),
                          void (*destroy)(void *),
                          void (*print)(const void *),
                          int (*cmp)(const void *, const void *));

// datatype_destroy(type) frees type from the heap memory.
// effects: frees heap memory [type becomes invalid]
void datatype_destroy(datatype *type);

// datatype_equals(a, b) produces true if a and b represent the same type
//   and false otherwise.
// requires: a and b are not NULL
bool datatype_equals(const datatype *a, const datatype *b);

// data_size(type) produces the data size of type in bytes.
// requires: type is not NULL
size_t data_size(const datatype *type);

// data_dup(value, type) creates a deep copy of value in the heap memory
//   using the dup method of type.
// requires: type is not NULL
// effects: allocates heap memory [caller must free with data_destroy]
// note: returns NULL if value is NULL
void *data_dup(const void *value, const datatype *type);

// data_destroy(value, type) frees value from the heap memory using
//   the destroy method of type.
// requires: type is not NULL
// effects: frees heap memory [value becomes invalid]
void data_destroy(void *value, const datatype *type);

// data_print(value, type) displays value using the print method of type.
// requires: type is not NULL
// effects: produces output [if value is not NULL]
void data_print(const void *value, const datatype *type);

// data_cmp(a, b, type) produces 0 if a and b are equal, a negative integer 
//   if a < b, and a positive integer if a > b.
// requires: a, b, and type are not NULL
int data_cmp(const void *a, const void *b, const datatype *type);

// The following methods each produce a shared singleton datatype ADT
//   for common built-in C types.
// note: the returned pointer must not be freed
//
// === Integral types ===
const datatype *int_type(void);
const datatype *long_type(void);
const datatype *char_type(void);
const datatype *bool_type(void);
const datatype *size_t_type(void);
// === Floating-point types ===
const datatype *float_type(void);
const datatype *double_type(void);
// === String type ===
const datatype *string_type(void);

#endif
