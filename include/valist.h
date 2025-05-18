// The valist module provides the valist ADT.

#ifndef VALIST_H
#define VALIST_H

#include "ctypeitem.h"

// A valist stores items in a dynamically resizable array,
// with all items deeply copied into heap memory.
//
// A valist may contain values of any types. Each valist item contains both 
// a value and an associated ctype, achieving similar functionality of 
// a mixed-type list in Python or other dynamically-typed languages.
//
// Memory model:
//   - All inserted items are deeply copied into separately allocated 
//     heap memory.
//   - Stack-allocated or heap-allocated objects are both safe to insert;
//     the valist duplicates them internally.
//   - Stored items are automatically freed when individually removed or
//     when the valist is destroyed.
//   - Clients are responsible for freeing the original heap-allocated 
//     objects after insertion to avoid memory leaks.
//
// Example:
//   int temp = 10;
//   // Deep-copied onto heap
//   valist_append(al, CTYPEITEM(&temp, ctype_int()));
//
//   int *ptr = malloc(sizeof(*ptr));
//   *ptr = 5;
//   // Deep-copied into valist
//   valist_append(al, CTYPEITEM(ptr, ctype_int()));
//   free(ptr);     // Client must free original pointer
//
// See 'examples/demo_valist_*.c' for a full demonstration of valist usage.
typedef struct valist valist;

// valist_pred is a predicate function that checks whether the given item 
//   in al satisfies specific conditions.
// parameters:
//   - al: the valist containing item (may be used for context or ignored)
//   - item: the item to be tested
//   - args: optional external data (may be NULL); typically a pointer to a
//           user-defined struct containing additional predicate parameters
// requires: al and item are not NULL
// note: returns true if item satisfies the conditions; false otherwise
typedef bool (*valist_pred)(const valist *al, 
                            const ctypeitem *item, 
                            const void *args);

// valist_map is a mapping function that optionally modifies the given item 
//   in al. The function may choose to mutate item or simply read it, 
//   depending on args or internal logic.
// parameters:
//   - al: the valist containing item (may be used for context or ignored)
//   - item: a mutable pointer to the item to be optionally modified
//   - args: optional external data (may be NULL); typically a pointer to a 
//           user-defined struct with transformation parameters
// requires: al and item are not NULL
// effects: may modify item
typedef void (*valist_map)(const valist *al, 
                           ctypeitem *item, 
                           const void *args);

// The return value if an item is not in valist (SIZE_MAX)
extern const size_t VALIST_INDEX_NOT_FOUND;

// Method alias
#define valist_length valist_size
#define valist_add valist_append
#define valist_add_all valist_append_all
#define valist_insert_back valist_append
#define valist_find valist_index
#define valist_find_last valist_index_last

// valist_create() creates an empty valist.
// effects: allocates heap memory [caller must free with valist_destroy]
valist *valist_create();

// valist_destroy(al) frees al and its items from the heap memory.
// effects: frees heap memory [al becomes invalid]
void valist_destroy(valist *al);

// valist_clear(al) removes all items from al.
// requires: al is not NULL
// effects: modifies al, frees heap memory
// notes:
//   - valist_clear only frees the items in al
//   - call valist_destroy to completely destroy al
//   - call valist_reclaim to free the emptied storage of al
void valist_clear(valist *al);

// valist_dup(al) creates a deep copy of al.
// requires: al is not NULL
// effects: allocates heap memory [caller must free with valist_destroy]
valist *valist_dup(const valist *al);

// valist_print(al) displays al in the format [X, Y, ...], where 
//   X, Y, ... are items of al.
// requires: al is not NULL
// effects: produces output
void valist_print(const valist *al);

// valist_equals(l1, l2) produces true if l1 and l2 has identical size and
//   and items; false otherwise.
// requires: l1 and l2 are not NULL
bool valist_equals(const valist *l1, const valist *l2);

// valist_size(al) produces the number of items in al.
// requires: al is not NULL
size_t valist_size(const valist *al);

// valist_empty(al) produces true if al is empty and false otherwise.
// requires: al is not NULL
bool valist_empty(const valist *al);

// valist_capacity(al) produces the maximum number of items 
//   currently allocated for al.
// requires: al is not NULL
size_t valist_capacity(const valist *al);

// valist_reserve(al, n) ensures al can hold at least n items.
// requires: al is not NULL
// effects: may allocate heap memory
// note: if al has more than n items, valist_reserve has no effect
void valist_reserve(valist *al, size_t n);

// valist_reclaim(al) sets al's capacity to al's current size.
// requires: al is not NULL
// effects: may modify al, may reallocate heap memory
// note: if al's capacity is equal to al's size, valist_reclaim has no effect
void valist_reclaim(valist *al);

// valist_get(al, index) produces a constant ctypeitem to the item 
//   at the given index position in al.
// requires: al is not NULL and not empty
//           0 <= index < valist_size(al)
// notes:
//   - valist_get protects the contents of al by returning a 
//     constant ctypeitem, preventing direct modification
//   - call valist_get_mutable to modify an item in-place
const ctypeitem *valist_get(const valist *al, size_t index);

// valist_get_mutable(al, index) produces a mutable ctypeitem to the item 
//   at the given index position in al.
// requires: al is not NULL and not empty
//           0 <= index < valist_size(al)
// warning: do not free the returned pointer, as doing so will result
//          in a double free when removing the item or destroying al
ctypeitem *valist_get_mutable(const valist *al, size_t index);

// valist_set(al, index, new_item) replaces the old item at the given 
//   index position in al with new_item.
// requires: al is not NULL and not empty
//           0 <= index < valist_size(al)
//           new_item is not NULL
// effects: modifies al [replaces the old item]
void valist_set(valist *al, size_t index, const ctypeitem *new_item);

// valist_swap(al, i, j) swaps the items at index positions i and j in al.
// requires: al is not NULL and not empty
//           0 <= i < valist_size(al)
//           0 <= j < valist_size(al)
// effects: modifies al
void valist_swap(valist *al, size_t i, size_t j);

// valist_append(al, item) adds item to the back of al.
// requires: al and item are not NULL
// effects: modifies al, allocates heap memory
void valist_append(valist *al, const ctypeitem *item);

// valist_append_all(al, src) adds all items in src to the back of al.
// requires: al and src are not NULL
// effects: modifies al, allocates heap memory
void valist_append_all(valist *al, const valist *src);

// valist_insert(al, index, item) inserts item before the given index 
//   position in al (shifting existing items to the right).
// requires: al and item are not NULL
//           0 <= index <= valist_size(al)
// effects: modifies al, allocates heap memory
void valist_insert(valist *al, size_t index, const ctypeitem *item);

// valist_insert_front(al, item) inserts item to the beginning of al.
// requires: al and item are not NULL
// effects: modifies al, allocates heap memory
void valist_insert_front(valist *al, const ctypeitem *item);

// valist_insert_all(al, index, src) inserts all items in src before the 
//   given index position in al.
// requires: al and src are not NULL and have the same type
//           0 <= index <= valist_size(al)
// effects: modifies al, allocates heap memory
void valist_insert_all(valist *al, size_t index, const valist *src);

// valist_pop(al, index) removes the item at the given index position in al.
// requires: al is not NULL and not empty
//           0 <= index < valist_size(al)
// effects: modifies al, frees heap memory
void valist_pop(valist *al, size_t index);

// valist_remove(al, item) removes the first occurrence of item in al.
// requires: al is not NULL and not empty
//           item is not NULL
// effects: modifies al, frees heap memory
// note: returns the index position of item in al 
//       and VALIST_INDEX_NOT_FOUND otherwise
size_t valist_remove(valist *al, const ctypeitem *item);

// valist_remove_last(al, item) removes the last occurrence of item in al.
// requires: al is not NULL and not empty
//           item is not NULL
// effects: modifies al, frees heap memory
// note: returns the index position of item in al 
//       and VALIST_INDEX_NOT_FOUND otherwise
size_t valist_remove_last(valist *al, const ctypeitem *item);

// valist_remove_all(al, item) removes all occurrences of item in al.
// requires: al is not NULL and not empty
//           item is not NULL
// effects: modifies al, frees heap memory
// note: returns the number of items removed
size_t valist_remove_all(valist *al, const ctypeitem *item);

// valist_remove_if(al, pred, args) removes all items in al that satisfies 
//   pred, where args provides additional arguments to the pred function.
// requires: al is not NULL and not empty
//           pred is not NULL
// effects: modifies al, frees heap memory
// note: returns the number of items removed
size_t valist_remove_if(valist *al, valist_pred pred, const void *args);

// valist_remove_range(al, from_index, to_index) removes all items in al
//   from index position from_index to (to_index - 1), inclusive.
// requires: al is not NULL and not empty
//           0 <= from_index < valist_size(al)
//           from_index <= to_index <= valist_size(al)
// effects: modifies al, frees heap memory
void valist_remove_range(valist *al, size_t from_index, size_t to_index);

// valist_contains(al, item) produces true if al contains item
//   and false otherwise.
// requires: al and item are not NULL
bool valist_contains(const valist *al, const ctypeitem *item);

// valist_index(al, item) produces the first index position of item in al
//   and VALIST_INDEX_NOT_FOUND if item is not in al.
// requires: al and item are not NULL
size_t valist_index(const valist *al, const ctypeitem *item);

// valist_index_last(al, item) produces the last index position of item 
//   in al and VALIST_INDEX_NOT_FOUND if item is not in al.
// requires: al and item are not NULL
size_t valist_index_last(const valist *al, const ctypeitem *item);

// valist_index_all(al, item) produces a valist of ctype_size_t() containing 
//   the index positions of all occurrences of item in al.
// requires: al and item are not NULL
// effects: allocates heap memory [caller must free with valist_destroy]
valist *valist_index_all(const valist *al, const ctypeitem *item);

// valist_index_all_if(al, pred, args) produces a valist of ctype_size_t() 
//   containing the index positions of all items in al that satisfy pred, 
//   where args provides additional arguments to the pred function.
// requires: al and pred are not NULL
// effects: allocates heap memory [caller must free with valist_destroy]
valist *valist_index_all_if(const valist *al, valist_pred pred, 
                            const void *args);

// valist_count(al, item) produces the total number of occurrences 
//   of item in al.
// requires: al and item are not NULL
size_t valist_count(const valist *al, const void *item);

// valist_replace(al, old_item, new_item) replaces the first occurrence of 
//   old_item in al with new_item.
// requires: al, old_item, and new_item are not NULL
// effects: may modify al [replaces old_item if found]
// note: returns the index position of old_item, or 
//       VALIST_INDEX_NOT_FOUND if old_item is not in al
size_t valist_replace(valist *al, const ctypeitem *old_item, 
                      const ctypeitem *new_item);

// valist_replace_last(al, old_item, new_item) replaces the last occurrence 
//   of old_item in al with new_item.
// requires: al, old_item, and new_item are not NULL
// effects: may modify al [replaces old_item if found]
// note: returns the index position of old_item, or 
//       VALIST_INDEX_NOT_FOUND if old_item is not in al
size_t valist_replace_last(valist *al, const ctypeitem *old_item, 
                           const ctypeitem *new_item);

// valist_replace_all(al, old_item, new_item) replaces all occurrences of 
//   old_item in al with new_item.
// requires: al, old_item, and new_item are not NULL
// effects: may modify al [replaces old_item if found]
// note: returns the number of items replaced
size_t valist_replace_all(valist *al, const ctypeitem *old_item, 
                          const ctypeitem *new_item);

// valist_replace_if(al, new_item, pred, args) replaces all items in al
//   that satisfy pred with new_item, where args provides additional
//   arguments to the pred function.
// requires: al, old_item, and new_item are not NULL
// effects: may modify al [replaces old_item if found]
// note: returns the number of items replaced
size_t valist_replace_if(valist *al, const ctypeitem *new_item, 
                         valist_pred pred, const void *args);

// valist_reverse(al) reverses the order of items in al.
// requires: al is not NULL
// effects: modifies al
void valist_reverse(valist *al);

// valist_slice(al, from_index, to_index) creates a sublist of al containing
//   items from index position from_index to (to_index - 1), inclusive.
// requires: al is not NULL and not empty
//           0 <= from_index < valist_size(al)
//           from_index <= to_index <= valist_size(al)
// effects: allocates heap memory [caller must free with valist_destroy]
valist *valist_slice(const valist *al, size_t from_index, size_t to_index);

// valist_filter(al, pred, args) produces a valist containing items in al
//   filtered by pred, where args provides additional arguments to the 
//   pred function.
// requires: al and pred are not NULL
// effects: allocates heap memory [caller must free with valist_destroy]
valist *valist_filter(const valist *al, valist_pred pred, const void *args);

// valist_foreach(al, map, args) applies map to each item in al, where args 
//   provides additional arguments to the map function.
// requires: al and map are not NULL
// effects: may modify item [see valist_map documentation]
void valist_foreach(const valist *al, valist_map map, const void *args);

// valist_unique(al) produces a valist containing unique items in al, 
//   preserving the original order.
// requires: al is not NULL
// effects: allocates heap memory [caller must free with valist_destroy]
valist *valist_unique(const valist *al);

// valist_remove_dup(al) removes all duplicate items from al, keeping only 
//   the first occurrence of each value.
// requires: al is not NULL
// effects: modifies al, frees heap memory
// note: returns the number of duplicate items removed
size_t valist_remove_dup(valist *al);

#endif
