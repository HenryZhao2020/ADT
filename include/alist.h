// This module provides the alist ADT.

#ifndef ALIST_H
#define ALIST_H

#include <stdbool.h>
#include "datatype.h"

// An alist stores items in a dynamically resizable array,
// with all items deeply copied into heap memory.
//
// Each alist is associated with a specific datatype, defining a common type
// for all stored items. Type-specific behaviors (duplication, comparison,
// printing, and deallocation) are handled through the datatype interface.
//
// Memory model:
//   - All inserted items are deeply copied into separately allocated 
//     heap memory.
//   - Stack-allocated or heap-allocated objects are both safe to insert;
//     the alist duplicates them internally.
//   - Stored items are automatically freed when individually removed or
//     when the alist is destroyed.
//   - Clients are responsible for freeing the original heap-allocated 
//     objects after insertion to avoid memory leaks.
//
// Example:
//   int temp = 10;
//   alist_append(al, &temp);  // Deep-copied onto heap
//
//   int *ptr = malloc(sizeof(*ptr));
//   *ptr = 5;
//   alist_append(al, ptr);    // Deep-copied into alist
//   free(ptr);                // Client must free original pointer
//
// See 'examples/demo_alist.c' for a full demonstration of alist usage.
typedef struct alist alist;

// The return value if an item is not in alist (SIZE_MAX)
extern const size_t ALIST_INDEX_NOT_FOUND;

// Method alias
#define alist_length alist_size
#define alist_add alist_append
#define alist_add_all alist_append_all
#define alist_insert_back alist_append
#define alist_find alist_index

// alist_create(type) creates a new, empty alist which stores items of 
//   the given type.
// requires: type is not NULL
// effects: allocates heap memory [caller must free with alist_destroy]
// note: returns NULL if allocation fails
// time: O(1)
alist *alist_create(const datatype *type);

// alist_create_size(type, init_cap) creates a new alist of the given type
//   which has the initial capacity init_cap.
// requires: type is not NULL
//           init_cap > 0
// effects: allocates heap memory [caller must free with alist_destroy]
// note: returns NULL if allocation fails
// time: O(n)
//   n = initial capacity of the alist = init_cap
alist *alist_create_size(const datatype *type, size_t init_cap);

// alist_destroy(al) frees al and its items from the heap memory.
// effects: frees heap memory [al becomes invalid]
// time: O(n * m)
//   n = number of items in al
//   m = number of steps to free each item in al
void alist_destroy(alist *al);

// alist_clear(al) removes all items from al.
// requires: al is not NULL
// effects: modifies al, frees heap memory
// note: 
//   - alist_clear only frees the items in al
//   - call alist_destroy to completely destroy al
//   - call alist_reclaim to free the emptied storage of al
// time: O(n * m)
//   n = number of items in al
//   m = number of steps to free each item in al
void alist_clear(alist *al);

// alist_dup(al) creates a deep copy of al.
// requires: al is not NULL
// effects: allocates heap memory [caller must free with alist_destroy]
// note: returns NULL if allocation fails
// time: O(n * m)
//   n = number of items in al
//   m = number of steps to copy each item in al
alist *alist_dup(const alist *al);

// alist_print(al) displays al in the format [X, Y, ...], where 
//   X, Y, ... are items of al.
// requires: al is not NULL
// effects: produces output
// time: O(n * m)
//   n = number of items in al
//   m = number of steps to print each item in al
void alist_print(const alist *al);

// alist_type(al) produces the datatype of al.
// requires: al is not NULL
// time: O(1)
const datatype *alist_type(const alist *al);

// alist_equals(a, b) produces true if a and b have identical size, type,
//   and items; false otherwise.
// requires: a and b are not NULL
// time: O(n)
//   n = min(alist_size(a), alist_size(b))
bool alist_equals(const alist *a, const alist *b);

// alist_size(al) produces the number of items in al.
// requires: al is not NULL
// time: O(1)
size_t alist_size(const alist *al);

// alist_empty(al) produces true if al is empty and false otherwise.
// requires: al is not NULL
// time: O(1)
bool alist_empty(const alist *al);

// alist_capacity(al) produces the maximum number of items 
//   currently allocated for al.
// requires: al is not NULL
// time: O(1)
size_t alist_capacity(const alist *al);

// alist_reserve(al, n) ensures al can hold at least n items.
// requires: al is not NULL
// effects: allocates heap memory
// note: 
//   - returns true if enough memory can be reserved for al, 
//     false otherwise
//   - if al has more than n items, then alist_reserve returns true 
//     and performs no action
// time: O(n)
bool alist_reserve(alist *al, size_t n);

// alist_reclaim(al) sets al's capacity to al's current size.
// requires: al is not NULL
// effects: may modify al, may reallocate heap memory
// note: 
//   - returns true if memory reallocation for al is successful,
//     false otherwise
//   - if al's capacity is equal to al's size, then alist_reclaim 
//     returns true and performs no action
// time: O(n)
bool alist_reclaim(alist *al);

// alist_get(al, index) produces a constant pointer to the item 
//   at the given index position in al.
// requires: al is not NULL and not empty
//           0 <= index < alist_size(al)
// note: 
//   - alist_get protects the contents of al by returning a 
//     constant pointer, preventing direct modification
//   - call alist_get_mutable to modify an item in-place
// time: O(1)
const void *alist_get(const alist *al, size_t index);

// alist_get_mutable(al, index) produces a mutable pointer to the item 
//   at the given index position in al.
// requires: al is not NULL and not empty
//           0 <= index < alist_size(al)
// warning: do not free the returned pointer, as doing so will result
//          in a double free when removing the item or destroying al
// time: O(1)
void *alist_get_mutable(const alist *al, size_t index);

// alist_set(al, index, item) replaces the old item at the given 
//   index position in al with a deep copy of item.
// requires: al is not NULL and not empty
//           0 <= index < alist_size(al)
//           item is not NULL
// effects: modifies al [replaces the old item with a deep copy]
// note: returns true if the deep copy of item is successful,
//       false otherwise
// time: O(n + m)
//   n = number of steps to destroy the old item
//   m = number of steps to copy item
bool alist_set(alist *al, size_t index, const void *item);

// alist_swap(al, i, j) swaps the items at index positions i and j in al.
// requires: al is not NULL and not empty
//           0 <= i < alist_size(al)
//           0 <= j < alist_size(al)
// effects: modifies al
// time: O(1)
void alist_swap(alist *al, size_t i, size_t j);

// alist_append(al, item) adds a deep copy of item to the back of al.
// requires: al and item are not NULL
// effects: modifies al, allocates heap memory
// note: returns true if al is resized and item is copied successfully,
//       false otherwise
// time: O(1) amortized + O(m)
//   m = number of steps to copy item
bool alist_append(alist *al, const void *item);

// alist_append_all(al, src) adds deep copies of all items in src 
//   to the back of al.
// requires: al and src are not NULL
//           al and src have the same type
// effects: modifies al, allocates heap memory
// note: returns true if all items in src are appended to al successfully, 
//       false otherwise
// time: O(1) amortized + O(n * m)
//   n = number of items in src
//   m = number of steps to copy each item in src
bool alist_append_all(alist *al, const alist *src);

// alist_insert(al, index, item) inserts a deep copy of item before the 
//   given index position in al (shifting existing items to the right).
// requires: al and item are not NULL
//           0 <= index <= alist_size(al)
// effects: modifies al, allocates heap memory
// note: returns true if al is resized and item is copied successfully,
//       false otherwise
// time: O(1) amortized + O(n + m)
//   n = number of items shifted in al = alist_size(al) - index
//   m = number of steps to copy item
bool alist_insert(alist *al, size_t index, const void *item);

// alist_insert_front(al, item) inserts a deep copy of item to 
//   the beginning of al.
// requires: al and item are not NULL
// effects: modifies al, allocates heap memory
// note: returns true if al is resized and item is copied successfully,
//       false otherwise
// time: O(1) amortized + O(n + m)
//   n = number of items shifted in al = alist_size(al)
//   m = number of steps to copy item
bool alist_insert_front(alist *al, const void *item);

// alist_insert_all(al, index, src) inserts deep copies of all items in src
//   before the given index position in al.
// requires: al and src are not NULL
//           al and src have the same type
//           0 <= index <= alist_size(al)
// effects: modifies al, allocates heap memory
// note: returns true if al is resized and items in src are copied to al 
//       successfully, false otherwise
// time: O(1) amortized + O(n * m + k)
//   n = number of items in src
//   m = number of steps to copy each item in src
//   k = number of items shifted in al = alist_size(al) - index
bool alist_insert_all(alist *al, size_t index, const alist *src);

// alist_pop(al, index) removes the item at the given index position in al.
// requires: al is not NULL and not empty
//           0 <= index < alist_size(al)
// effects: modifies al, frees heap memory
// time: O(n + m)
//   n = number of items shifted in al
//   m = number of steps to free the selected item
void alist_pop(alist *al, size_t index);

// alist_remove(al, item) removes the first occurrence of item in al.
// requires: al is not NULL and not empty
//           item is not NULL
// effects: modifies al, frees heap memory
// note: returns the index position of item in al 
//       and ALIST_INDEX_NOT_FOUND otherwise
// time: O(n + m + k)
//   n = number of items in al
//   m = number of items shifted in al
//   k = number of steps to free the selected item
size_t alist_remove(alist *al, const void *item);

// alist_remove_last(al, item) removes the last occurrence of item in al.
// requires: al is not NULL and not empty
//           item is not NULL
// effects: modifies al, frees heap memory
// note: returns the index position of item in al 
//       and ALIST_INDEX_NOT_FOUND otherwise
// time: O(n + m + k)
//   n = number of items in al
//   m = number of items shifted in al
//   k = number of steps to free the selected item
size_t alist_remove_last(alist *al, const void *item);

// alist_remove_all(al, item) removes all occurrences of item in al.
// requires: al is not NULL and not empty
//           item is not NULL
// effects: modifies al, frees heap memory
// time: O((n + m + k) * l)
//   n = number of items in al
//   m = number of items shifted in al
//   k = number of steps to free the selected item
//   l = number of occurrences of item in al
void alist_remove_all(alist *al, const void *item);

// alist_remove_if(al, pred) removes all items in al that satisfies pred.
// requires: al is not NULL and not empty
//           pred is not NULL
// effects: modifies al, frees heap memory
// time: O(n * m * k)
//   n = number of items in al
//   m = number of steps to filter each item in al by pred
//   k = number of items shifted in al
void alist_remove_if(alist *al, bool (*pred)(const void *));

// alist_remove_range(al, from_index, to_index) removes all items in al
//   from index position from_index to (to_index - 1), inclusive.
// requires: al is not NULL and not empty
//           0 <= from_index < alist_size(al)
//           from_index <= to_index <= alist_size(al)
// effects: modifies al, frees heap memory
// time: O(n * m + k)
//   n = to_index - from_index
//   m = number of steps to free each selected item in al
//   k = number of items shifted in al
void alist_remove_range(alist *al, size_t from_index, size_t to_index);

// alist_contains(al, item) produces true if al contains item
//   and false otherwise.
// requires: al and item are not NULL
// time: O(n * m)
//   n = number of items in al
//   m = number of steps to compare item with each item in al
bool alist_contains(const alist *al, const void *item);

// alist_index(al, item) produces the first index position of item in al
//   and ALIST_INDEX_NOT_FOUND if item is not in al.
// requires: al and item are not NULL
// time: O(n * m)
//   n = number of items in al
//   m = number of steps to compare item with each item in al
size_t alist_index(const alist *al, const void *item);

// alist_index_last(al, item) produces the last index position of item 
//   in al and ALIST_INDEX_NOT_FOUND if item is not in al.
// requires: al and item are not NULL
// time: O(n * m)
//   n = number of items in al
//   m = number of steps to compare item with each item in al
size_t alist_index_last(const alist *al, const void *item);

// alist_count(al, item) produces the total number of occurrences 
//   of item in al.
// requires: al and item are not NULL
// time: O(n * m)
//   n = number of items in al
//   m = number of steps to compare item with each item in al
size_t alist_count(const alist *al, const void *item);

// alist_qsort(al) sorts al using the quicksort algorithm.
// requires: al is not NULL
// effects: modifies al
// time: O(nlog(n) * m) average case, O(n^2 * m) worst case
//   n = number of items in al
//   m = number of steps to compare two elements in al
void alist_qsort(alist *al);

// alist_bsearch(al, item) produces any index position of item in al
//   using binary search and ALIST_INDEX_NOT_FOUND if item is not in al.
// requires: al and item are not NULL
//           al must be sorted in ascending order [not asserted]
// time: O(log(n) * m)
//   n = number of items in al
//   m = number of steps to compare item with each item in al
size_t alist_bsearch(const alist *al, const void *item);

// alist_reverse(al) reverses the order of items in al.
// requires: al is not NULL
// effects: modifies al
// time: O(n)
void alist_reverse(alist *al);

// alist_slice(al, from_index, to_index) creates a sublist of al containing
//   items from index position from_index to (to_index - 1), inclusive.
// requires: al is not NULL and not empty
//           0 <= from_index < alist_size(al)
//           from_index <= to_index <= alist_size(al)
// effects: allocates heap memory [caller must free with alist_destroy]
// time: O(n * m)
//   n = to_index - from_index
//   m = number of steps to copy each selected item
alist *alist_slice(const alist *al, size_t from_index, size_t to_index);

// alist_filter(al, pred) produces an alist containing items in al
//   filtered by pred.
// requires: al and pred are not NULL
// effects: allocates heap memory [caller must free with alist_destroy]
// time: O(n * m * k)
//   n = number of items in al
//   m = number of steps to filter each item in al by pred
//   k = number of steps to copy each filtered item
alist *alist_filter(const alist *al, bool (*pred)(const void *));

// alist_map(al, map) applies map to all items in al.
// requires: al and map are not NULL
// effects: modifies al
// note: alist_map modifies items in place, unlike alist_filter or 
//       alist_slice which return new lists.
// time: O(n * m)
//   n = number of items in al
//   m = number of steps to modify each item by map
void alist_map(alist *al, void (*map)(void *));

// alist_from_array(arr, len, type) creates an alist of the given type
//   containing deep copies of len items in arr.
// requires: arr and type are not NULL
//           len represents the length of arr [not asserted]
//           type matches the types of items in arr [not asserted]
// effects: allocates heap memory [caller must free with alist_destroy]
// time: O(n * m)
//   n = len = number of items in arr
//   m = number of steps to copy each item in arr
alist *alist_from_array(const void **arr, size_t len, const datatype *type);

// alist_to_array(al) produces an array containing deep copies of 
//   all items in al.
// requires: al is not NULL
// effects: allocates heap memory
// time: O(n * m)
//   n = len = number of items in arr
//   m = number of steps to copy each item in al
void **alist_to_array(const alist *al);

#endif
