#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "alist.h"

struct alist {
  void **data;
  const datatype *type;
  size_t len;
  size_t capacity;
};

const size_t ALIST_INDEX_NOT_FOUND = SIZE_MAX;
static const size_t DEFAULT_INIT_CAPACITY = 1;

// Helper function declaration
static void qsort_range(alist *al, size_t first, size_t last);

alist *alist_create(const datatype *type) {
  assert(type);
  return alist_create_size(type, DEFAULT_INIT_CAPACITY);
}

alist *alist_create_size(const datatype *type, size_t init_cap) {
  assert(type);
  assert(init_cap);

  alist *al = malloc(sizeof(*al));
  if (!al) {
    return NULL;
  }

  al->data = malloc(sizeof(*al->data) * init_cap);
  if (!al->data) {
    free(al);
    return NULL;
  }
  
  al->type = type;
  al->len = 0;
  al->capacity = init_cap;
  return al;
}

void alist_destroy(alist *al) {
  if (!al) {
    return;
  }

  for (size_t i = 0; i < al->len; ++i) {
    data_destroy(al->data[i], al->type);
  }
  free(al->data);
  free(al);
}

void alist_clear(alist *al) {
  assert(al);
  for (size_t i = 0; i < al->len; ++i) {
    data_destroy(al->data[i], al->type);
  }
  al->len = 0;
}

alist *alist_dup(const alist *al) {
  assert(al);

  alist *al_copy = malloc(sizeof(*al_copy));
  if (!al_copy) {
    return NULL;
  }

  al_copy->data = malloc(sizeof(*al_copy->data) * al->capacity);
  if (!al_copy->data) {
    free(al_copy);
    return NULL;
  }

  for (size_t i = 0; i < al->len; ++i) {
    void *item = data_dup(al->data[i], al->type);
    if (!item) {
      al_copy->len = i;
      alist_destroy(al_copy);
      return NULL;
    }
    al_copy->data[i] = item;
  }

  al_copy->type = al->type;
  al_copy->len = al->len;
  al_copy->capacity = al->capacity;
  return al_copy;
}

void alist_print(const alist *al) {
  assert(al);

  printf("[");
  for (size_t i = 0; i < al->len; ++i) {
    if (i != 0) {
      printf(", ");
    }
    data_print(al->data[i], al->type);
  }
  printf("]\n");
}

const datatype *alist_type(const alist *al) {
  assert(al);
  return al->type;
}

bool alist_equals(const alist *a, const alist *b) {
  assert(a);
  assert(b);

  if (a->len != b->len || !datatype_equals(a->type, b->type)) {
    return false;
  }

  for (size_t i = 0; i < a->len; ++i) {
    if (data_cmp(a->data[i], b->data[i], a->type)) {
      return false;
    }
  }
  return true;
}

size_t alist_size(const alist *al) {
  assert(al);
  return al->len;
}

bool alist_empty(const alist *al) {
  assert(al);
  return (al->len == 0);
}

size_t alist_capacity(const alist *al) {
  assert(al);
  return al->capacity;
}

bool alist_reserve(alist *al, size_t n) {
  assert(al);

  if (n <= al->capacity) {
    return true;
  }

  void **new_data = realloc(al->data, sizeof(void *) * n);
  if (!new_data) {
    return false;
  }

  al->data = new_data;
  al->capacity = n;
  return true;
}

bool alist_reclaim(alist *al) {
  assert(al);
  if (al->len == al->capacity) {
    return true;
  }
  
  void **new_data = realloc(al->data, sizeof(void *) * al->len);
  if (!new_data) {
    return false;
  }
  al->data = new_data;
  al->capacity = al->len;
  return true;
}

const void *alist_get(const alist *al, size_t index) {
  assert(al);
  assert(al->len > 0);
  assert(index < al->len);
  return al->data[index];
}

void *alist_get_mutable(const alist *al, size_t index) {
  assert(al);
  assert(al->len > 0);
  assert(index < al->len);
  return al->data[index];
}

bool alist_set(alist *al, size_t index, const void *item) {
  assert(al);
  assert(al->len > 0);
  assert(item);
  assert(index < al->len);

  void *item_copy = data_dup(item, al->type);
  if (!item_copy) {
    return false;
  }

  data_destroy(al->data[index], al->type);
  al->data[index] = item_copy;
  return true;
}

void alist_swap(alist *al, size_t i, size_t j) {
  assert(al);
  assert(al->len > 0);
  assert(i < al->len);
  assert(j < al->len);

  void *temp = al->data[i];
  al->data[i] = al->data[j];
  al->data[j] = temp;
}

bool alist_append(alist *al, const void *item) {
  assert(al);
  assert(item);

  void *item_copy = data_dup(item, al->type);
  if (!item_copy) {
    return false;
  }

  if (al->len == al->capacity) {
    size_t new_capacity = al->capacity * 2;
    void **new_data = realloc(al->data,  sizeof(*al->data) * new_capacity);
    if (!new_data) {
      return false;
    }
    al->data = new_data;
    al->capacity = new_capacity;
  }

  al->data[al->len] = item_copy;
  ++al->len;
  return true;
}

bool alist_append_all(alist *al, const alist *src) {
  assert(al);
  assert(src);
  assert(datatype_equals(src->type, al->type));

  for (size_t i = 0; i < src->len; ++i) {
    if (!alist_append(al, src->data[i])) {
      return false;
    }
  }
  return true;
}

bool alist_insert(alist *al, size_t index, const void *item) {
  assert(al);
  assert(item);
  assert(index <= al->len);

  void *item_copy = data_dup(item, al->type);
  if (!item_copy) {
    return false;
  }
  
  if (al->len == al->capacity) {
    size_t new_capacity = al->capacity * 2;
    void **new_data = realloc(al->data,  sizeof(*al->data) * new_capacity);
    if (!new_data) {
      return false;
    }
    al->data = new_data;
    al->capacity = new_capacity;
  }

  for (size_t i = al->len; i > index; --i) {
    al->data[i] = al->data[i - 1];
  }

  al->data[index] = item_copy;
  ++al->len;
  return true;
}

bool alist_insert_front(alist *al, const void *item) {
  assert(al);
  assert(item);
  return alist_insert(al, 0, item);
}

bool alist_insert_all(alist *al, size_t index, const alist *src) {
  assert(al);
  assert(src);
  assert(datatype_equals(src->type, al->type));
  assert(index <= al->len);

  if (!alist_reserve(al, al->len + src->len)) {
    return false;
  }

  // Shift elements backwards to make room
  for (size_t i = al->len; i-- > index;) {
    al->data[i + src->len] = al->data[i];
  }

  for (size_t i = 0; i < src->len; ++i) {
    al->data[index + i] = data_dup(src->data[i], al->type);
    if (!al->data[index + i]) {
      // Rollback inserted items
      for (size_t j = 0; j < i; ++j) {
        data_destroy(al->data[index + j], al->type);
      }
      // Restore shifted items
      for (size_t j = index; j < al->len; ++j) {
        al->data[j] = al->data[j + src->len];
      }
      return false;
    }
  }

  al->len += src->len;
  return true;
}

void alist_pop(alist *al, size_t index) {
  assert(al);
  assert(al->len > 0);
  assert(index < al->len);

  data_destroy(al->data[index], al->type);
  for (size_t i = index; i < al->len - 1; ++i) {
    al->data[i] = al->data[i + 1];
  }
  --al->len;
}

size_t alist_remove(alist *al, const void *item) {
  assert(al);
  assert(al->len > 0);
  assert(item);

  for (size_t i = 0; i < al->len; ++i) {
    if (!data_cmp(item, al->data[i], al->type)) {
      alist_pop(al, i);
      return i;
    }
  }
  return ALIST_INDEX_NOT_FOUND;
}

size_t alist_remove_last(alist *al, const void *item) {
  assert(al);
  assert(al->len > 0);
  assert(item);

  for (size_t i = al->len; i-- > 0;) {
    if (!data_cmp(item, al->data[i], al->type)) {
      alist_pop(al, i);
      return i;
    }
  }
  return ALIST_INDEX_NOT_FOUND;
}

void alist_remove_all(alist *al, const void *item) {
  assert(al);
  assert(al->len > 0);
  assert(item);

  for (size_t i = al->len; i-- > 0;) {
    if (!data_cmp(item, al->data[i], al->type)) {
      alist_pop(al, i);
    }
  }
}

void alist_remove_if(alist *al, bool (*pred)(const void *)) {
  assert(al);
  assert(al->len > 0);
  assert(pred);

  for (size_t i = al->len; i-- > 0;) {
    if (pred(al->data[i])) {
      alist_pop(al, i);
    }
  }
}

void alist_remove_range(alist *al, size_t from_index, size_t to_index) {
  assert(al);
  assert(al->len > 0);
  assert(from_index < al->len);
  assert(from_index <= to_index && to_index <= al->len);

  size_t range = to_index - from_index;
  for (size_t i = from_index; i < to_index; ++i) {
    data_destroy(al->data[i], al->type);
  }
  for (size_t i = to_index; i < al->len; ++i) {
    al->data[i - range] = al->data[i];
  }
  al->len -= range;
}

bool alist_contains(const alist *al, const void *item) {
  assert(al);
  assert(item);

  for (size_t i = 0; i < al->len; ++i) {
    if (!data_cmp(item, al->data[i], al->type)) {
      return true;
    }
  }
  return false;
}

size_t alist_index(const alist *al, const void *item) {
  assert(al);
  assert(item);

  for (size_t i = 0; i < al->len; ++i) {
    if (!data_cmp(item, al->data[i], al->type)) {
      return i;
    }
  }
  return ALIST_INDEX_NOT_FOUND;
}

size_t alist_index_last(const alist *al, const void *item) {
  assert(al);
  assert(item);

  if (al->len == 0) {
    return ALIST_INDEX_NOT_FOUND;
  }

  for (size_t i = al->len; i-- > 0;) {
    if (!data_cmp(item, al->data[i], al->type)) {
      return i;
    }
  }
  return ALIST_INDEX_NOT_FOUND;
}

size_t alist_count(const alist *al, const void *item) {
  assert(al);
  assert(item);

  size_t count = 0;
  for (size_t i = 0; i < al->len; ++i) {
    if (!data_cmp(item, al->data[i], al->type)) {
      ++count;
    }
  }
  return count;
}

void alist_qsort(alist *al) {
  assert(al);
  if (al->len > 0) {
    qsort_range(al, 0, al->len - 1);
  }
}

size_t alist_bsearch(const alist *al, const void *item) {
  assert(al);
  assert(item);

  if (al->len == 0) {
    return ALIST_INDEX_NOT_FOUND;
  }

  size_t low = 0;
  size_t high = al->len - 1;

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
  return ALIST_INDEX_NOT_FOUND;
}

void alist_reverse(alist *al) {
  assert(al);
  for (size_t i = 0; i < al->len / 2; ++i) {
    alist_swap(al, i, al->len - i - 1);
  }
}

alist *alist_slice(const alist *al, size_t from_index, size_t to_index) {
  assert(al);
  assert(al->len > 0);
  assert(from_index < al->len);
  assert(from_index <= to_index && to_index <= al->len);

  alist *sub = alist_create(al->type);
  if (!sub) {
    return NULL;
  }

  for (size_t i = from_index; i < to_index; ++i) {
    if (!alist_append(sub, al->data[i])) {
      alist_destroy(sub);
      return NULL;
    }
  }
  return sub;
}

alist *alist_filter(const alist *al, bool (*pred)(const void *)) {
  assert(al);
  assert(pred);

  alist *filtered = alist_create(al->type);
  if (!filtered) {
    return NULL;
  }
  
  for (size_t i = 0; i < al->len; ++i) {
    if (pred(al->data[i]) && !alist_append(filtered, al->data[i])) {
      alist_destroy(filtered);
      return NULL;
    }
  }
  return filtered;
}

void alist_map(alist *al, void (*map)(void *)) {
  assert(al);
  assert(map);

  for (size_t i = 0; i < al->len; ++i) {
    map(al->data[i]);
  }
}

alist *alist_from_array(const void **arr, size_t len, const datatype *type) {
  assert(arr);
  assert(type);

  alist *al = alist_create(type);
  if (!al) {
    return NULL;
  }

  if (!alist_reserve(al, len)) {
    alist_destroy(al);
    return NULL;
  }

  for (size_t i = 0; i < len; ++i) {
    if (!alist_append(al, arr[i])) {
      alist_destroy(al);
      return NULL;
    }
  }
  return al;
}

void **alist_to_array(const alist *al) {
  assert(al);

  void **arr = malloc(sizeof(void *) * al->len);
  if (!arr) {
    return NULL;
  }

  for (size_t i = 0; i < al->len; ++i) {
    arr[i] = data_dup(al->data[i], al->type);
    if (!arr[i]) {
      for (size_t j = 0; j < i; ++j) {
        data_destroy(arr[j], al->type);
      }
      free(arr);
      return NULL;
    }
  }
  return arr;
}

// Helper function implementation
static void qsort_range(alist *al, size_t first, size_t last) {
  assert(al);

  if (last <= first) {
    return;
  }

  void *pivot = al->data[first];
  size_t pos = last;

  for (size_t i = last; i > first; --i) {
    if (data_cmp(al->data[i], pivot, al->type) > 0) {
      alist_swap(al, i, pos);
      --pos;
    }
  }
  alist_swap(al, first, pos);

  // Prevent underflow when pos == 0
  if (pos > 0) {
    qsort_range(al, first, pos - 1);
  }
  qsort_range(al, pos + 1, last);
}
