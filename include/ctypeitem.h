// The ctypeitem module provides the ctypeitem ADT.

#ifndef CTYPEITEM_H
#define CTYPEITEM_H

#include <stdbool.h>
#include "ctype.h"

#define CTYPEITEM(value, type) (ctypeitem_create(value, type))

// A ctypeitem stores a value and its associated ctype.
typedef struct ctypeitem ctypeitem;

// ctypeitem_create(value, type) creates a ctypeitem with the given
//   value and type.
// requires: value and type are not NULL
// effects: allocates heap memory [caller must free with ctypeitem_destroy]
ctypeitem *ctypeitem_create(const void *value, const ctype *type);

// ctypeitem_destroy(item) frees the value of item from the heap memory.
// requires: item is not NULL
// effects: frees heap memory
// note: the ctype associated with item will not be freed
ctypeitem *ctypeitem_destroy(ctypeitem *item);

// ctypeitem_dup(item) creates a deep copy of item.
// requires: item is not NULL
// effects: allocates heap memory [caller must free with ctypeitem_destroy]
ctypeitem *ctypeitem_dup(const ctypeitem *item);

// ctypeitem_equals(i1, i2) produces true if i1 and i2 have the same type
//   and the same value.
// requires: i1 and i2 are not NULL
bool ctypeitem_equals(const ctypeitem *i1, const ctypeitem *i2);

// ctypeitem_value(item) produces a constant pointer to the value of item.
// requires: item is not NULL
const void *ctypeitem_value(const ctypeitem *item);

// ctypeitem_type(item) produces the ctype associated with item.
// requires: item is not NULL
const ctype *ctypeitem_type(const ctypeitem *item);

#endif
