// This module provides the datatype ADT and operations required
//   for implementing generic storage ADTs.

#ifndef DATATYPE_H
#define DATATYPE_H

#include <stddef.h>
#include <stdbool.h>

// A datatype represents a type descriptor for use in generic ADTs.
// attributes:
//   - id:      a unique identifier assigned to a datatype [not user-defined]
//   - size:    the size in bytes of the associated data
// methods:
//   - dup:     creates a deep copy of the given item (heap-allocated)
//   - dealloc: frees an item previously created by dup
//   - print:   displays a human-readable representation of the item
//   - cmp:     compares two items a and b
//              returns 0 if equal, <0 if a < b, >0 if a > b
// note:
//   - Datatypes are uniquely identified by their IDs.
//   - Clients must ensure only one instance of a given datatype is created.
//     All uses of the same type must refer to the same datatype pointer.
typedef struct datatype datatype;

// Wrap stack variables to temporary pointers for generic storage ADTs
#define WRAP_INT(i) (&(int) {i})
#define WRAP_FLOAT(f) (&(float) {f})
#define WRAP_DOUBLE(d) (&(double) {d})
#define WRAP_BOOL(b) (&(bool) {b})
#define WRAP_CHAR(c) (&(char) {c})

// datatype_create(size, dup, dealloc, print, cmp) creates a datatype 
//   with the given attributes (see datatype documentation above).
// requires: dup, dealloc, print, cmp are not NULL
// effects: allocates heap memory [caller must free with datatype_destroy]
// note: Returns NULL if allocation fails
// time: O(1)
const datatype *datatype_create(size_t size,
                                void *(*dup)(const void *),
                                void (*dealloc)(void *),
                                void (*print)(const void *),
                                int (*cmp)(const void *, const void *));

// datatype_destroy(type) frees type from the memory.
// effects: frees heap memory [type becomes invalid]
// time: O(1)
void datatype_destroy(datatype *type);

// datatype_equals(a, b) produces true if a and b have the same ID
//   and false otherwise.
// requires: a and b are not NULL
// time: O(1)
bool datatype_equals(const datatype *a, const datatype *b);

// data_size(type) produces the data size of type in bytes.
// requires: type is not NULL
// time: O(1)
size_t data_size(const datatype *type);

// data_dup(item, type) creates a deep copy of item in the heap memory
//   using the dup method of type.
// requires: item and type are not NULL
// effects: allocates heap memory
// note: The returned pointer must be freed using data_dealloc, not free
//       Returns NULL if allocation fails
// time: [time of the dup method of type]
void *data_dup(const void *item, const datatype *type);

// data_dealloc(item, type) frees item from the heap memory using
//   the dealloc method of type.
// requires: item and type are not NULL
// effects: frees heap memory
// time: [time of the dealloc method of type]
void data_dealloc(void *item, const datatype *type);

// data_print(item, type) displays item using the print method of type.
// requires: item and type are not NULL
// effects: produces output
// time: [time of the print method of type]
void data_print(const void *item, const datatype *type);

// data_cmp(a, b, type) produces 0 if a and b are equal, a negative integer 
//   if a < b, and a positive integer if a > b.
// requires: a, b, and type are not NULL
// time: [time of the cmp method of type]
int data_cmp(const void *a, const void *b, const datatype *type);

// int_type() produces a shared singleton int datatype ADT.
// note: The returned pointer must not be freed
// time: O(1)
const datatype *int_type(void);

// float_type() produces a shared singleton float datatype ADT.
// note: The returned pointer must not be freed
// time: O(1)
const datatype *float_type(void);

// double_type() produces a shared singleton double datatype ADT.
// note: The returned pointer must not be freed
// time: O(1)
const datatype *double_type(void);

// bool_type() produces a shared singleton bool datatype ADT.
// note: The returned pointer must not be freed
// time: O(1)
const datatype *bool_type(void);

// char_type() produces a shared singleton char datatype ADT.
// note: The returned pointer must not be freed
// time: O(1)
const datatype *char_type(void);

#endif
