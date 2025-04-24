#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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

// Helper function declaration
static void qsort_range(size_t first, size_t last, alist *al);

alist *alist_create(const datatype *type) {
  alist *al = malloc(sizeof(*al));
  if (!al) {
    return NULL;
  }

  al->data = malloc(sizeof(*al->data));
  if (!al->data) {
    free(al);
    return NULL;
  }
  
  al->type = type;
  al->len = 0;
  al->capacity = 1;
  return al;
}

void alist_destroy(alist *al) {
  if (!al) {
    return;
  }

  for (size_t i = 0; i < al->len; ++i) {
    data_dealloc(al->data[i], al->type);
  }
  free(al->data);
  free(al);
}

void alist_clear(alist *al) {
  assert(al);
  for (size_t i = 0; i < al->len; ++i) {
    data_dealloc(al->data[i], al->type);
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

bool alist_reserve(size_t n, alist *al) {
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

bool alist_trim(alist *al) {
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

const void *alist_get(size_t index, const alist *al) {
  assert(al);
  assert(index < al->len);
  return al->data[index];
}

bool alist_set(size_t index, const void *item, alist *al) {
  assert(item);
  assert(al);
  assert(index < al->len);

  void *item_copy = data_dup(item, al->type);
  if (!item_copy) {
    return false;
  }

  data_dealloc(al->data[index], al->type);
  al->data[index] = item_copy;
  return true;
}

void alist_swap(size_t i, size_t j, alist *al) {
  assert(al);
  assert(i < al->len);
  assert(j < al->len);

  void *temp = al->data[i];
  al->data[i] = al->data[j];
  al->data[j] = temp;
}

bool alist_append(const void *item, alist *al) {
  assert(item);
  assert(al);

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

bool alist_append_all(const alist *src, alist *al) {
  assert(src);
  assert(al);
  assert(datatype_equals(src->type, al->type));

  for (size_t i = 0; i < src->len; ++i) {
    if (!alist_append(src->data[i], al)) {
      return false;
    }
  }
  return true;
}

bool alist_insert(size_t index, const void *item, alist *al) {
  assert(item);
  assert(al);
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

bool alist_insert_front(const void *item, alist *al) {
  assert(item);
  assert(al);
  return alist_insert(0, item, al);
}

bool alist_insert_all(size_t index, const alist *src, alist *al) {
  assert(src);
  assert(al);
  assert(datatype_equals(src->type, al->type));
  assert(index <= al->len);

  if (!alist_reserve(al->len + src->len, al)) {
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
        data_dealloc(al->data[index + j], al->type);
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

void alist_pop(size_t index, alist *al) {
  assert(al);
  assert(al->len > 0);
  assert(index < al->len);

  data_dealloc(al->data[index], al->type);
  for (size_t i = index; i < al->len - 1; ++i) {
    al->data[i] = al->data[i + 1];
  }
  --al->len;
}

size_t alist_remove(const void *item, alist *al) {
  assert(item);
  assert(al);
  assert(al->len > 0);

  for (size_t i = 0; i < al->len; ++i) {
    if (!data_cmp(item, al->data[i], al->type)) {
      alist_pop(i, al);
      return i;
    }
  }
  return ALIST_INDEX_NOT_FOUND;
}

size_t alist_remove_last(const void *item, alist *al) {
  assert(item);
  assert(al);
  assert(al->len > 0);

  for (size_t i = al->len; i-- > 0;) {
    if (!data_cmp(item, al->data[i], al->type)) {
      alist_pop(i, al);
      return i;
    }
  }
  return ALIST_INDEX_NOT_FOUND;
}

void alist_remove_all(const void *item, alist *al) {
  assert(item);
  assert(al);
  assert(al->len > 0);

  for (size_t i = al->len; i-- > 0;) {
    if (!data_cmp(item, al->data[i], al->type)) {
      alist_pop(i, al);
    }
  }
}

void alist_remove_if(bool (*pred)(const void *), alist *al) {
  assert(pred);
  assert(al);
  assert(al->len > 0);

  for (size_t i = al->len; i-- > 0;) {
    if (pred(al->data[i])) {
      alist_pop(i, al);
    }
  }
}

void alist_remove_range(size_t from_index, size_t to_index, alist *al) {
  assert(al);
  assert(al->len > 0);
  assert(from_index < al->len);
  assert(from_index <= to_index && to_index <= al->len);

  size_t range = to_index - from_index;
  for (size_t i = from_index; i < to_index; ++i) {
    data_dealloc(al->data[i], al->type);
  }
  for (size_t i = to_index; i < al->len; ++i) {
    al->data[i - range] = al->data[i];
  }
  al->len -= range;
}

bool alist_contains(const void *item, const alist *al) {
  assert(item);
  assert(al);

  for (size_t i = 0; i < al->len; ++i) {
    if (!data_cmp(item, al->data[i], al->type)) {
      return true;
    }
  }
  return false;
}

size_t alist_index(const void *item, const alist *al) {
  assert(item);
  assert(al);

  for (size_t i = 0; i < al->len; ++i) {
    if (!data_cmp(item, al->data[i], al->type)) {
      return i;
    }
  }
  return ALIST_INDEX_NOT_FOUND;
}

size_t alist_index_last(const void *item, const alist *al) {
  assert(item);
  assert(al);

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

size_t alist_count(const void *item, const alist *al) {
  assert(item);
  assert(al);

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
    qsort_range(0, al->len - 1, al);
  }
}

size_t alist_bsearch(const void *item, const alist *al) {
  assert(item);
  assert(al);
  assert(al->len > 0);

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
    alist_swap(i, al->len - i - 1, al);
  }
}

alist *alist_slice(size_t from_index, size_t to_index, const alist *al) {
  assert(al);
  assert(from_index < al->len);
  assert(from_index <= to_index && to_index <= al->len);

  alist *al_sub = alist_create(al->type);
  if (!al_sub) {
    return NULL;
  }

  for (size_t i = from_index; i < to_index; ++i) {
    if (!alist_append(al->data[i], al_sub)) {
      alist_destroy(al_sub);
      return NULL;
    }
  }
  return al_sub;
}

alist *alist_filter(bool (*pred)(const void *), const alist *al) {
  assert(pred);
  assert(al);

  alist *filtered = alist_create(al->type);
  if (!filtered) {
    return NULL;
  }
  
  for (size_t i = 0; i < al->len; ++i) {
    if (pred(al->data[i]) && !alist_append(al->data[i], filtered)) {
      alist_destroy(filtered);
      return NULL;
    }
  }
  return filtered;
}

void alist_map(void (*map)(void *), alist *al) {
  assert(map);
  assert(al);

  for (size_t i = 0; i < al->len; ++i) {
    map(al->data[i]);
  }
}

alist *alist_from_array(const void **arr, size_t len, const datatype *type) {
  assert(arr);
  assert(type);

  alist *al = alist_create(type);
  if (!al || !alist_reserve(len, al)) {
    return NULL;
  }

  for (size_t i = 0; i < len; ++i) {
    if (!alist_append(arr[i], al)) {
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
        data_dealloc(arr[j], al->type);
      }
      free(arr);
      return NULL;
    }
  }
  return arr;
}

// Helper function implementation
static void qsort_range(size_t first, size_t last, alist *al) {
  assert(al);

  if (last <= first) {
    return;
  }

  void *pivot = al->data[first];
  size_t pos = last;

  for (size_t i = last; i > first; --i) {
    if (data_cmp(al->data[i], pivot, al->type) > 0) {
      alist_swap(i, pos, al);
      --pos;
    }
  }
  alist_swap(first, pos, al);

  // Prevent underflow when pos == 0
  if (pos > 0) {
    qsort_range(first, pos - 1, al);
  }
  qsort_range(pos + 1, last, al);
}
