// This module provides the alist ADT.

#ifndef ALIST_H
#define ALIST_H

#include <stdbool.h>
#include "datatype.h"

// alist stores items in a dynamic array, making it efficient at
//   selecting and searching items (if sorted).
// note: All item arguments are deep-copied using the datatype’s dup method.
//       The caller retains ownership of the original item.
typedef struct alist alist;

// The return value if an item is not in alist (SIZE_MAX)
extern const size_t ALIST_INDEX_NOT_FOUND;

// Method alias
#define alist_length alist_size
#define alist_add alist_append
#define alist_add_all alist_append_all
#define alist_insert_back alist_append
#define alist_find alist_index

// alist_create(type) creates a new alist which stores items of 
//   the given type.
// requires: type is not NULL
// effects: allocates heap memory [caller must free with alist_destroy]
// note: Returns NULL if allocation fails
// time: O(1)
alist *alist_create(const datatype *type);

// alist_destroy(al) frees al from the memory.
// effects: frees heap memory [al becomes invalid]
// time: O(n * m)
//   n = number of items in al,
//   m = number of steps to free each item in al
void alist_destroy(alist *al);

// alist_clear(al) removes all items from al.
// requires: al is not NULL
// effects: modifies al, frees heap memory
// note: alist_clear only deallocates the items in al.
//       Call alist_destroy to completely destroy al.
//       Call alist_trim to free the emptied storage of al.
// time: O(n * m)
//   n = number of items in al,
//   m = number of steps to free each item in al
void alist_clear(alist *al);

// alist_dup(al) creates a deep copy of al.
// requires: al is not NULL
// effects: allocates heap memory [caller must free with alist_destroy]
// note: Returns NULL if allocation fails
// time: O(n * m)
//   n = number of items in al,
//   m = number of steps to copy each item in al
alist *alist_dup(const alist *al);

// alist_print(al) displays al in the format [X, Y, ...], where 
//   X, Y, ... are items of al.
// requires: al is not NULL
// effects: produces output
// time: O(n * m)
//   n = number of items in al,
//   m = number of steps to print each item in al
void alist_print(const alist *al);

// alist_type(al) produces the datatype of al.
// requires: al is not NULL
// time: O(1)
const datatype *alist_type(const alist *al);

// alist_equals(a, b) produces true if a and b are equal and false otherwise.
// requires: a and b are not NULL
// time: O(n), n = min(alist_size(a), alist_size(b))
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

// alist_reserve(n, al) reserves space for n items in al.
// requires: al is not NULL
// effects: allocates heap memory
// note: Produces true if enough memory can be reserved for al, 
//       false otherwise
//       If al has more items than n, then alist_reserve produces true 
//       and perform no action.
// time: O(n)
bool alist_reserve(size_t n, alist *al);

// alist_trim(al) trims al's capacity to al's current size.
// requires: al is not NULL
// effects: may modify al, may reallocate heap memory
// note: Produces true if memory reallocation for al is successful,
//       false otherwise
// time: O(n)
bool alist_trim(alist *al);

// alist_get(index, al) produces a pointer to the item at the given
//   index position in al.
// requires: al is not NULL
//           0 <= index < alist_size(al)
// time: O(1)
const void *alist_get(size_t index, const alist *al);

// alist_set(index, item, al) creates a deep copy of item at the given 
//   index position in al.
// requires: item and al are not NULL
//           0 <= index < alist_size(al)
// effects: modifies al (replaces the old item)
// note: Produces true if a deep copy is made successfully, false otherwise
// time: O(m)
//   m = number of steps to deallocate and copy item
bool alist_set(size_t index, const void *item, alist *al);

// alist_swap(i, j, al) swaps the items at index positions i and j in al.
// requires: al is not NULL
//           0 <= i < alist_size(al)
//           0 <= j < alist_size(al)
// effects: modifies al
// time: O(1)
void alist_swap(size_t i, size_t j, alist *al);

// alist_append(item, al) adds a deep copy of item to the back of al.
// requires: item and al are not NULL
// effects: modifies al
// note: Produces true if a deep copy is made successfully, false otherwise
// time: O(1) amortized + O(m)
//   m = number of steps to copy item
bool alist_append(const void *item, alist *al);

// alist_append_all(src, al) adds deep copies of all items in src 
//   to the back of al.
// requires: src and al are not NULL
//           src and al have the same type
// effects: modifies al
// note: Produces true if deep copies are made successfully, false otherwise
// time: O(1) amortized + O(n * m)
//   n = number of items in src
//   m = number of steps to copy each item in src
bool alist_append_all(const alist *src, alist *al);

// alist_insert(index, item, al) inserts a deep copy of item before the 
//   given index position in al (shifting existing items to the right).
// requires: item and al are not NULL
//           0 <= index <= alist_size(al)
// effects: modifies al, allocates heap memory
// note: Produces true if a deep copy is made successfully, false otherwise
// time: O(n + m)
//   n = number of items shifted,
//   m = number of steps to copy item
bool alist_insert(size_t index, const void *item, alist *al);

// alist_insert_front(item, al) inserts a deep copy of item to 
//   the beginning of al.
// requires: item and al are not NULL
// effects: modifies al
// note: Produces true if a deep copy is made successfully, false otherwise
// time: O(n + m)
//   n = number of items shifted (length of al),
//   m = number of steps to copy the inserted item
bool alist_insert_front(const void *item, alist *al);

// alist_insert_all(index, src, al) inserts src before the given 
//   index position in al.
// requires: src and al are not NULL
//           src and al have the same type
//           0 <= index <= alist_size(al)
// effects: modifies al
// note: Produces true if deep copies are made successfully, false otherwise
// time: O(n * m * k)
//   n = number of items in src
//   m = number of steps to copy an item
//   k = number of items shifted in al
bool alist_insert_all(size_t index, const alist *src, alist *al);

// alist_pop(index, al) removes the item at the given index position in al.
// requires: al is not NULL and not empty
//           0 <= index < alist_size(al)
// effects: modifies al, frees heap memory
// time: O(n + m)
//   n = number of items shifted,
//   m = number of steps to free the selected item
void alist_pop(size_t index, alist *al);

// alist_remove(item, al) removes the first occurrence of item in al.
// requires: item is not NULL
//           al is not NULL and not empty
// effects: modifies al, frees heap memory
// note: Produces the index position of item in al 
//       and ALIST_INDEX_NOT_FOUND otherwise.
// time: O(n + m + k)
//   n = number of items in al,
//   m = number of items shifted,
//   k = number of steps to free the selected item
size_t alist_remove(const void *item, alist *al);

// alist_remove_last(item, al) removes the last occurrence of item in al.
// requires: item is not NULL
//           al is not NULL and not empty
// effects: modifies al, frees heap memory
// note: Produces the index position of item in al 
//       and ALIST_INDEX_NOT_FOUND otherwise.
// time: O(n + m + k)
//   n = number of items in al,
//   m = number of items shifted,
//   k = number of steps to free the selected item
size_t alist_remove_last(const void *item, alist *al);

// alist_remove_all(item, al) removes all occurrences of item in al.
// requires: item is not NULL
//           al is not NULL and not empty
// effects: modifies al, frees heap memory
// time: O((n + m + k) * l)
//   n = number of items in al,
//   m = number of items shifted,
//   k = number of steps to free the selected item
//   l = number of occurrences of item in al
void alist_remove_all(const void *item, alist *al);

// alist_remove_if(pred, al) removes all items in al that satisfies pred.
// requires: pred is not NULL
//           al is not NULL and not empty
// effects: modifies al, frees heap memory
// time: O(n * m * k)
//   n = number of items in al
//   m = number of steps to filter each item by pred
//   k = number of items shifted
void alist_remove_if(bool (*pred)(const void *), alist *al);

// alist_remove_range(from_index, to_index, al) removes all items in al
//   whose index is from from_index to (to_index - 1), inclusive.
// requires: al is not NULL and not empty
//           0 <= from_index < alist_size(al)
//           from_index <= to_index <= alist_size(al)
// effects: modifies al, frees heap memory
// time: O(n * m + k)
//   n = to_index - from_index
//   m = number of steps to free each selected item
//   k = number of items shifted
void alist_remove_range(size_t from_index, size_t to_index, alist *al);

// alist_contains(item, al) produces true if al contains item
//   and false otherwise.
// requires: item and al are not NULL
// time: O(n * m)
//   n = number of items in al,
//   m = number of steps to compare item with each element in al
bool alist_contains(const void *item, const alist *al);

// alist_index(item, al) produces the first index position of item in al
//   and ALIST_INDEX_NOT_FOUND if item is not in al.
// requires: item and al are not NULL
// time: O(n * m)
//   n = number of items in al,
//   m = number of steps to compare item with each element in al
size_t alist_index(const void *item, const alist *al);

// alist_index_last(item, al) produces the last index position of item 
//   in al and ALIST_INDEX_NOT_FOUND if item is not in al.
// requires: item and al are not NULL
// time: O(n * m)
//   n = number of items in al,
//   m = number of steps to compare item with each element in al
size_t alist_index_last(const void *item, const alist *al);

// alist_count(item, al) produces the total number of occurrences 
//   of item in al.
// requires: item and al are not NULL
// time: O(n * m)
//   n = number of items in al,
//   m = number of steps to compare item with each element in al
size_t alist_count(const void *item, const alist *al);

// alist_qsort(al) sorts al using the cmp method provided by al's type
//   and the quicksort algorithm.
// requires: al is not NULL
// effects: modifies al
// time: O(nlog(n) * m) average case, O(n^2 * m) worst case
//   n = number of items in al
//   m = number of steps to compare two elements in al
void alist_qsort(alist *al);

// alist_bsearch(item, al) produces any index position of item in al
//   using binary search and ALIST_INDEX_NOT_FOUND if item is not in al.
// requires: item and al are not NULL
//           al is non-empty
//           al is sorted in ascending order [not asserted]
// time: O(log(n) * m)
//   n = number of items in al,
//   m = number of steps to compare item with each element in al
size_t alist_bsearch(const void *item, const alist *al);

// alist_reverse(al) reverses the order of items in al.
// requires: al is not NULL
// effects: modifies al
// time: O(n)
void alist_reverse(alist *al);

// alist_slice(from_index, to_index, al) creates a sublist of al containing
//   items whose index is from from_index to (to_index - 1), inclusive.
// requires: al is not NULL
//           0 <= from_index < alist_size(al)
//           from_index <= to_index <= alist_size(al)
// effects: allocates heap memory [caller must free with alist_destroy]
// time: O(n * m)
//   n = to_index - from_index
//   m = number of steps to copy each selected item
alist *alist_slice(size_t from_index, size_t to_index, const alist *al);

// alist_filter(pred, al) produces an alist containing items in al
//   filtered by pred.
// requires: pred and al are not NULL
// effects: allocates heap memory [caller must free with alist_destroy]
// time: O(n * m * k)
//   n = number of items in al
//   m = number of steps to filter each item by pred
//   k = number of steps to copy each selected item
alist *alist_filter(bool (*pred)(const void *), const alist *al);

// alist_map(map, al) applies map to all items in al.
// requires: map and al are not NULL
// effects: modifies al
// time: O(n * m)
//   n = number of items in al
//   m = number of steps to modify each item by map
void alist_map(void (*map)(void *), alist *al);

// alist_from_array(arr, len, type) creates an alist of the given type
//   containing deep copies of len items in arr.
// requires: arr and type are not NULL
//           len represents the length of arr [not asserted]
// effects: allocates heap memory [caller must free with alist_destroy]
// time: O(n * m)
//   n = len = number of items in arr
//   m = number of steps to copy each selected item
alist *alist_from_array(const void **arr, size_t len, const datatype *type);

// alist_to_array(al) produces an array containing deep copies of 
//   all items in al.
// requires: al is not NULL
// effects: allocates heap memory
// time: O(n * m)
//   n = len = number of items in arr
//   m = number of steps to copy each selected item
void **alist_to_array(const alist *al);

#endif
