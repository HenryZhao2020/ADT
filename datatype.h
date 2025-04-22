// This module provides the datatype ADT and operations required
//   for implementing generic storage ADTs.

#ifndef DATATYPE_H
#define DATATYPE_H

#include <stddef.h>

// A datatype stores information of its associated data.
typedef struct datatype datatype;

// Wrap stack variables to temporary pointers for generic storage ADTs
#define WRAP_INT(i) (&(int) {i})

// datatype_create(size, dup, dealloc, print, cmp) produces a datatype ADT
//   that stores the data size in bytes and provides the following methods:
//   - dup: dup_XXX(item) creates a deep copy of item on the heap memory
//   - dealloc: dealloc_XXX(item) frees item from the heap memory
//   - print: print_XXX(item) displays item
//   - cmp: cmp_XXX(a, b) produces 0 if a and b are equal, a negative integer 
//          if a < b, and a positive integer if a > b
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

// data_dup_method(type) produces the dup method of type.
// requires: type is not NULL
// time: O(1)
void *(*data_dup_method(const datatype *type))(const void *);

// data_dup(item, type) produces a deep copy of item using
//   the dup method provided by type.
// requires: item and type are not NULL
// effects: allocates heap memory [depends on the dup method of type]
// note: returns NULL if allocation fails
// time: [time of the dup method of type]
void *data_dup(const void *item, const datatype *type);

// data_dealloc_method(type) produces the dealloc method of type.
// requires: type is not NULL
// time: O(1)
void (*data_dealloc_method(const datatype *type))(void *);

// data_dealloc(item, type) frees item from the memory using
//   the dealloc method provided by type.
// requires: item and type are not NULL
// effects: frees memory of the deep copy of item
// time: [time of the dealloc method of type]
void data_dealloc(void *item, const datatype *type);

// data_print_method(type) produces the print method of type.
// requires: type is not NULL
// time: O(1)
void (*data_print_method(const datatype *type))(const void *);

// data_print(item, type) prints item using the print method provided by type.
// requires: item and type are not NULL
// effects: produces output
// time: [time of the print method of type]
void data_print(const void *item, const datatype *type);

// data_cmp_method(type) produces the cmp method of type.
// requires: type is not NULL
// time: O(1)
int (*data_cmp_method(const datatype *type))(const void *, const void *);

// data_cmp(item1, item2, type) produces 0 if item1 and item2 are equal 
//   and a non-zero integer otherwise.
// requires: item1, item2, and type are not NULL
// time: [time of the cmp method of type]
int data_cmp(const void *item1, const void *item2, const datatype *type);

// int_type() returns a shared singleton int datatype ADT.
// note: the returned pointer must not be freed via datatype_destroy
// time: O(1)
const datatype *int_type(void);

#endif
