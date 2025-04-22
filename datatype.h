// This module provides the datatype ADT and operations required
//   for implementing generic storage ADTs.

#ifndef DATATYPE_H
#define DATATYPE_H

#include <stddef.h>

// A datatype stores the size (in bytes) of its associated data and
//   provides the following methods for use in generic ADTs:
//   - dup:     creates a deep copy of an item (heap-allocated)
//   - dealloc: frees a deep copy of an item (heap-allocated)
//   - print:   displays an item
//   - cmp:     compares two items (returns 0 if equal, a positive integer 
//              if the first item is greater than the second, and a negative 
//              integer if the first item is less than the second)
typedef struct datatype datatype;

// Wrap stack variables to temporary pointers for generic storage ADTs
#define WRAP_INT(i) (&(int) {i})

// datatype_create(size, dup, dealloc, print, cmp) produces a datatype ADT
//   with the given attributes (see datatype documentation above).
// requires: dup, dealloc, print, cmp are not NULL
// effects: allocates heap memory [caller must free with datatype_destroy]
// note: returns NULL if allocation fails
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

// data_size(type) produces the data size of type in bytes.
// requires: type is not NULL
// time: O(1)
size_t data_size(const datatype *type);

// data_dup(item, type) produces a deep copy of item in the heap memory
//   using the dup method of type.
// note: the returned pointer must be freed using data_dealloc, not free
// requires: item and type are not NULL
// effects: allocates heap memory
// note: returns NULL if allocation fails
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
// note: the returned pointer must not be freed
// time: O(1)
const datatype *int_type(void);

#endif
