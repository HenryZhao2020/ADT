#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "calist.h"

#define ASSERT_MSG(cond, msg) do { \
  if (!(cond)) { \
    fprintf(stderr, "[calist assertion failed] %s:%d: %s\n", \
            __FILE__, __LINE__, (msg)); \
    abort(); \
  } \
} while (0)

#define CALIST_MEM_ERR(msg) do { \
  fprintf(stderr, "[calist memory error] %s:%d: %s\n", \
          __FILE__, __LINE__, (msg)); \
  exit(EXIT_FAILURE); \
} while (0)

struct calist {
  void **data;
  const datatype *type;
  size_t size;
  size_t capacity;
};

const size_t CALIST_INDEX_NOT_FOUND = SIZE_MAX;

static const size_t DEFAULT_INIT_CAPACITY = 1;
static const char *CALIST_ASSERT_NULL = "calist cannot be NULL!";
static const char *CALIST_ASSERT_EMPTY = "calist cannot be empty!";
static const char *ITEM_ASSERT_NULL = "item cannot be NULL!";
static const char *INDEX_ASSERT_BOUNDED = "index must be less than the size of calist!";

// Helper function declaration
static void qsort_range(calist *al, size_t first, size_t last);

calist *calist_create(const datatype *type) {
  ASSERT_MSG(type, "datatype cannot be NULL!");
  return calist_create_size(type, DEFAULT_INIT_CAPACITY);
}

calist *calist_create_size(const datatype *type, size_t init_cap) {
  ASSERT_MSG(type, "datatype cannot be NULL!");
  ASSERT_MSG(init_cap, "The initial capacity of calist cannot be zero!");

  calist *al = malloc(sizeof(*al));
  if (!al) {
    CALIST_MEM_ERR("Failed to create calist!");
  }

  al->data = malloc(sizeof(*al->data) * init_cap);
  if (!al->data) {
    CALIST_MEM_ERR("Failed to create calist with the given size!");
  }
  
  al->type = type;
  al->size = 0;
  al->capacity = init_cap;
  return al;
}

void calist_destroy(calist *al) {
  if (!al) return;

  for (size_t i = 0; i < al->size; ++i) {
    data_destroy(al->data[i], al->type);
  }
  free(al->data);
  free(al);
}

void calist_clear(calist *al) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);

  for (size_t i = 0; i < al->size; ++i) {
    data_destroy(al->data[i], al->type);
  }
  al->size = 0;
}

calist *calist_dup(const calist *al) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);

  calist *al_copy = malloc(sizeof(*al_copy));
  if (!al_copy) {
    CALIST_MEM_ERR("Failed to create calist!");
  }

  al_copy->data = malloc(sizeof(*al_copy->data) * al->capacity);
  if (!al_copy->data) {
    CALIST_MEM_ERR("Failed to create calist with the given size!");
  }

  for (size_t i = 0; i < al->size; ++i) {
    void *item = data_dup(al->data[i], al->type);
    if (!item) {
      CALIST_MEM_ERR("Failed to duplicate item!");
    }
    al_copy->data[i] = item;
  }

  al_copy->type = al->type;
  al_copy->size = al->size;
  al_copy->capacity = al->capacity;
  return al_copy;
}

void calist_print(const calist *al) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);

  printf("[");
  for (size_t i = 0; i < al->size; ++i) {
    if (i != 0) {
      printf(", ");
    }
    data_print(al->data[i], al->type);
  }
  printf("]\n");
}

bool calist_equals(const calist *a, const calist *b) {
  ASSERT_MSG(a, CALIST_ASSERT_NULL);
  ASSERT_MSG(b, CALIST_ASSERT_NULL);

  if (a->size != b->size || !datatype_equals(a->type, b->type)) {
    return false;
  }

  for (size_t i = 0; i < a->size; ++i) {
    if (data_cmp(a->data[i], b->data[i], a->type)) {
      return false;
    }
  }
  return true;
}

const datatype *calist_type(const calist *al) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);
  return al->type;
}

size_t calist_size(const calist *al) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);
  return al->size;
}

bool calist_empty(const calist *al) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);
  return (al->size == 0);
}

size_t calist_capacity(const calist *al) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);
  return al->capacity;
}

void calist_reserve(calist *al, size_t n) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);

  if (n <= al->capacity) return;

  void **new_data = realloc(al->data, sizeof(void *) * n);
  if (!new_data) {
    CALIST_MEM_ERR("Failed to reserve the given capacity!");
  }

  al->data = new_data;
  al->capacity = n;
}

void calist_reclaim(calist *al) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);
  
  if (al->size == al->capacity) return;
  
  void **new_data = realloc(al->data, sizeof(void *) * al->size);
  if (!new_data) {
    CALIST_MEM_ERR("Failed to reclaim the unused storage!");
  }

  al->data = new_data;
  al->capacity = al->size;
}

const void *calist_get(const calist *al, size_t index) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);
  ASSERT_MSG(al->size > 0, CALIST_ASSERT_EMPTY);
  ASSERT_MSG(index < al->size, INDEX_ASSERT_BOUNDED);
  return al->data[index];
}

void *calist_get_mutable(const calist *al, size_t index) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);
  ASSERT_MSG(al->size > 0, CALIST_ASSERT_EMPTY);
  ASSERT_MSG(index < al->size, INDEX_ASSERT_BOUNDED);
  return al->data[index];
}

void calist_set(calist *al, size_t index, const void *new_item) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);
  ASSERT_MSG(al->size > 0, CALIST_ASSERT_EMPTY);
  ASSERT_MSG(index < al->size, INDEX_ASSERT_BOUNDED);
  ASSERT_MSG(new_item, ITEM_ASSERT_NULL); 

  void *item_copy = data_dup(new_item, al->type);
  if (!item_copy) {
    CALIST_MEM_ERR("Failed to duplicate item!");
  }

  data_destroy(al->data[index], al->type);
  al->data[index] = item_copy;
}

void calist_swap(calist *al, size_t i, size_t j) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);
  ASSERT_MSG(al->size > 0, CALIST_ASSERT_EMPTY);
  ASSERT_MSG(i < al->size, INDEX_ASSERT_BOUNDED);
  ASSERT_MSG(j < al->size, INDEX_ASSERT_BOUNDED);

  void *temp = al->data[i];
  al->data[i] = al->data[j];
  al->data[j] = temp;
}

void calist_append(calist *al, const void *item) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);
  ASSERT_MSG(item, ITEM_ASSERT_NULL);

  void *item_copy = data_dup(item, al->type);
  if (!item_copy) {
    CALIST_MEM_ERR("Failed to duplicate item!");
  }

  if (al->size == al->capacity) {
    calist_reserve(al, al->capacity * 2);
  }
  al->data[al->size] = item_copy;
  ++al->size;
}

void calist_append_all(calist *al, const calist *src) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);
  ASSERT_MSG(src, CALIST_ASSERT_NULL);
  ASSERT_MSG(datatype_equals(src->type, al->type),
             "calists cannot have different types!");

  for (size_t i = 0; i < src->size; ++i) {
    calist_append(al, src->data[i]);
  }
}

void calist_insert(calist *al, size_t index, const void *item) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);
  ASSERT_MSG(item, ITEM_ASSERT_NULL);
  ASSERT_MSG(index <= al->size, "index must not exceed the size of calist!");

  void *item_copy = data_dup(item, al->type);
  if (!item_copy) {
    CALIST_MEM_ERR("Failed to duplicate item!");
  }

  if (al->size == al->capacity) {
    calist_reserve(al, al->capacity * 2);
  }
  for (size_t i = al->size; i > index; --i) {
    al->data[i] = al->data[i - 1];
  }
  al->data[index] = item_copy;
  ++al->size;
}

void calist_insert_front(calist *al, const void *item) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);
  ASSERT_MSG(item, ITEM_ASSERT_NULL);
  calist_insert(al, 0, item);
}

void calist_insert_all(calist *al, size_t index, const calist *src) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);
  ASSERT_MSG(src, CALIST_ASSERT_NULL);
  ASSERT_MSG(datatype_equals(src->type, al->type),
             "calists cannot have different types!");
  ASSERT_MSG(index <= al->size, "index must not exceed the size of calist!");

  calist_reserve(al, al->size + src->size);
  // Shift elements backwards to make room
  for (size_t i = al->size; i-- > index;) {
    al->data[i + src->size] = al->data[i];
  }

  for (size_t i = 0; i < src->size; ++i) {
    al->data[index + i] = data_dup(src->data[i], al->type);
    if (!al->data[index + i]) {
      CALIST_MEM_ERR("Failed to duplicate item!");
    }
  }
  al->size += src->size;
}

void calist_pop(calist *al, size_t index) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);
  ASSERT_MSG(al->size > 0, CALIST_ASSERT_EMPTY);
  ASSERT_MSG(index < al->size, INDEX_ASSERT_BOUNDED);

  data_destroy(al->data[index], al->type);
  for (size_t i = index; i < al->size - 1; ++i) {
    al->data[i] = al->data[i + 1];
  }
  --al->size;
}

size_t calist_remove(calist *al, const void *item) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);
  ASSERT_MSG(al->size > 0, CALIST_ASSERT_EMPTY);
  ASSERT_MSG(item, ITEM_ASSERT_NULL);

  for (size_t i = 0; i < al->size; ++i) {
    if (!data_cmp(item, al->data[i], al->type)) {
      calist_pop(al, i);
      return i;
    }
  }
  return CALIST_INDEX_NOT_FOUND;
}

size_t calist_remove_last(calist *al, const void *item) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);
  ASSERT_MSG(al->size > 0, CALIST_ASSERT_EMPTY);
  ASSERT_MSG(item, ITEM_ASSERT_NULL);

  for (size_t i = al->size; i-- > 0;) {
    if (!data_cmp(item, al->data[i], al->type)) {
      calist_pop(al, i);
      return i;
    }
  }
  return CALIST_INDEX_NOT_FOUND;
}

size_t calist_remove_all(calist *al, const void *item) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);
  ASSERT_MSG(al->size > 0, CALIST_ASSERT_EMPTY);
  ASSERT_MSG(item, ITEM_ASSERT_NULL);

  size_t total = 0;
  for (size_t i = al->size; i-- > 0;) {
    if (!data_cmp(item, al->data[i], al->type)) {
      calist_pop(al, i);
      ++total;
    }
  }
  return total;
}

size_t calist_remove_if(calist *al, calist_pred pred, const void *args) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);
  ASSERT_MSG(al->size > 0, CALIST_ASSERT_EMPTY);
  ASSERT_MSG(pred, "calist_pred cannot be NULL!");

  size_t total = 0;
  for (size_t i = al->size; i-- > 0;) {
    if (pred(al, al->data[i], args)) {
      calist_pop(al, i);
      ++total;
    }
  }
  return total;
}

void calist_remove_range(calist *al, size_t from_index, size_t to_index) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);
  ASSERT_MSG(al->size > 0, CALIST_ASSERT_EMPTY);
  ASSERT_MSG(from_index < al->size, INDEX_ASSERT_BOUNDED);
  ASSERT_MSG(to_index >= from_index, 
             "The ending index cannot be less than the starting index!");
  ASSERT_MSG(to_index <= al->size, "index must not exceed the size of calist!");

  size_t range = to_index - from_index;
  for (size_t i = from_index; i < to_index; ++i) {
    data_destroy(al->data[i], al->type);
  }
  for (size_t i = to_index; i < al->size; ++i) {
    al->data[i - range] = al->data[i];
  }
  al->size -= range;
}

bool calist_contains(const calist *al, const void *item) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);
  ASSERT_MSG(item, ITEM_ASSERT_NULL);
  return (calist_index(al, item) != CALIST_INDEX_NOT_FOUND);
}

size_t calist_index(const calist *al, const void *item) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);
  ASSERT_MSG(item, ITEM_ASSERT_NULL);

  for (size_t i = 0; i < al->size; ++i) {
    if (!data_cmp(item, al->data[i], al->type)) {
      return i;
    }
  }
  return CALIST_INDEX_NOT_FOUND;
}

size_t calist_index_last(const calist *al, const void *item) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);
  ASSERT_MSG(item, ITEM_ASSERT_NULL);

  if (al->size == 0) {
    return CALIST_INDEX_NOT_FOUND;
  }

  for (size_t i = al->size; i-- > 0;) {
    if (!data_cmp(item, al->data[i], al->type)) {
      return i;
    }
  }
  return CALIST_INDEX_NOT_FOUND;
}

calist *calist_index_all(const calist *al, const void *item) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);
  ASSERT_MSG(item, ITEM_ASSERT_NULL);

  calist *indices = calist_create(size_t_type());
  for (size_t i = 0; i < al->size; ++i) {
    if (!data_cmp(item, al->data[i], al->type)) {
      calist_append(indices, &i);
    }
  }
  return indices;
}

calist *calist_index_all_if(const calist *al, calist_pred pred, 
                            const void *args) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);
  ASSERT_MSG(pred, "calist_pred cannot be NULL!");

  calist *indices = calist_create(size_t_type());
  for (size_t i = 0; i < al->size; ++i) {
    if (pred(al, al->data[i], args)) {
      calist_append(indices, &i);
    }
  }
  return indices;
}

size_t calist_count(const calist *al, const void *item) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);
  ASSERT_MSG(item, ITEM_ASSERT_NULL);

  size_t total = 0;
  for (size_t i = 0; i < al->size; ++i) {
    if (!data_cmp(item, al->data[i], al->type)) {
      ++total;
    }
  }
  return total;
}

size_t calist_replace(calist *al, const void *old_item, 
                      const void *new_item) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);
  ASSERT_MSG(old_item, ITEM_ASSERT_NULL);
  ASSERT_MSG(new_item, ITEM_ASSERT_NULL);

  size_t index = calist_index(al, old_item);
  if (index != CALIST_INDEX_NOT_FOUND) {
    calist_set(al, index, new_item);
  }
  return index;
}

size_t calist_replace_last(calist *al, const void *old_item, 
                           const void *new_item) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);
  ASSERT_MSG(old_item, ITEM_ASSERT_NULL);
  ASSERT_MSG(new_item, ITEM_ASSERT_NULL);

  size_t index = calist_index_last(al, old_item);
  if (index != CALIST_INDEX_NOT_FOUND) {
    calist_set(al, index, new_item);
  }
  return index;
}

size_t calist_replace_all(calist *al, const void *old_item, 
                          const void *new_item) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);
  ASSERT_MSG(old_item, ITEM_ASSERT_NULL);
  ASSERT_MSG(new_item, ITEM_ASSERT_NULL);

  size_t total = 0;
  for (size_t i = 0; i < al->size; ++i) {
    if (!data_cmp(old_item, al->data[i], al->type)) {
      calist_set(al, i, new_item);
      ++total;
    }
  }
  return total;
}

size_t calist_replace_if(calist *al, const void *new_item, 
                         calist_pred pred, const void *args) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);
  ASSERT_MSG(new_item, ITEM_ASSERT_NULL);
  ASSERT_MSG(pred, "calist_pred cannot be NULL!");

  size_t total = 0;
  for (size_t i = 0; i < al->size; ++i) {
    if (pred(al, al->data[i], args)) {
      calist_set(al, i, new_item);
      ++total;
    }
  }
  return total;
}

void calist_qsort(calist *al) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);

  if (al->size > 0) {
    qsort_range(al, 0, al->size - 1);
  }
}

size_t calist_bsearch(const calist *al, const void *item) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);
  ASSERT_MSG(item, ITEM_ASSERT_NULL);

  if (al->size == 0) {
    return CALIST_INDEX_NOT_FOUND;
  }

  size_t low = 0;
  size_t high = al->size - 1;

  while (low <= high) {
    size_t mid = (low + high) / 2;
    int cmp = data_cmp(al->data[mid], item, al->type);
    if (!cmp) {
      return mid;
    } else if (cmp < 0) {
      low = mid + 1;
    } else {
      high = mid - 1;
    }
  }
  return CALIST_INDEX_NOT_FOUND;
}

void calist_reverse(calist *al) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);
  for (size_t i = 0; i < al->size / 2; ++i) {
    calist_swap(al, i, al->size - i - 1);
  }
}

calist *calist_slice(const calist *al, size_t from_index, size_t to_index) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);
  ASSERT_MSG(al->size > 0, CALIST_ASSERT_EMPTY);
  ASSERT_MSG(from_index < al->size, INDEX_ASSERT_BOUNDED);
  ASSERT_MSG(to_index >= from_index, 
             "The ending index cannot be less than the starting index!");
  ASSERT_MSG(to_index <= al->size, "index must not exceed the size of calist!");

  calist *sub = calist_create(al->type);
  calist_reserve(sub, to_index - from_index);
  for (size_t i = from_index; i < to_index; ++i) {
    calist_append(sub, al->data[i]);
  }
  return sub;
}

calist *calist_filter(const calist *al, calist_pred pred, const void *args) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);
  ASSERT_MSG(pred, "calist_pred cannot be NULL!");

  calist *filtered = calist_create(al->type);
  for (size_t i = 0; i < al->size; ++i) {
    if (pred(al, al->data[i], args)) {
      calist_append(filtered, al->data[i]);
    }
  }
  return filtered;
}

void calist_foreach(const calist *al, calist_map map, const void *args) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);
  ASSERT_MSG(map, "calist_map cannot be NULL!");

  for (size_t i = 0; i < al->size; ++i) {
    map(al, al->data[i], args);
  }
}

calist *calist_unique(const calist *al) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);

  calist *unique = calist_create(al->type);
  for (size_t i = 0; i < al->size; ++i) {
    if (!calist_contains(unique, al->data[i])) {
      calist_append(unique, al->data[i]);
    }
  }
  return unique;
}

size_t calist_remove_dup(calist *al) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);

  size_t total = 0;
  for (size_t i = 0; i < al->size; ++i) {
    size_t count = calist_count(al, al->data[i]);
    while (count > 1) {
      calist_remove_last(al, al->data[i]);
      --count;
      ++total;
    }
  }
  return total;
}

// Helper function implementation
static void qsort_range(calist *al, size_t first, size_t last) {
  ASSERT_MSG(al, CALIST_ASSERT_NULL);

  if (last <= first) {
    return;
  }

  void *pivot = al->data[first];
  size_t pos = last;

  for (size_t i = last; i > first; --i) {
    if (data_cmp(al->data[i], pivot, al->type) > 0) {
      calist_swap(al, i, pos);
      --pos;
    }
  }
  calist_swap(al, first, pos);

  // Prevent underflow when pos == 0
  if (pos > 0) {
    qsort_range(al, first, pos - 1);
  }
  qsort_range(al, pos + 1, last);
}
