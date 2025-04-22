#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include "alist.h"

struct alist {
  void **data;
  const datatype *type;
  size_t len;
  size_t capacity;
};

const size_t ALIST_INDEX_NOT_FOUND = SIZE_MAX;

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
  assert(n > al->capacity);

  void **new_data = realloc(al->data, sizeof(void *) * n);
  if (!new_data) {
    return false;
  }

  al->data = new_data;
  al->capacity = n;
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

bool alist_append(const void *item, alist *al) {
  assert(item);
  assert(al);

  if (al->len == al->capacity) {
    size_t new_capacity = al->capacity * 2;
    void **new_data = realloc(al->data,  sizeof(*al->data) * new_capacity);
    if (!new_data) {
      return false;
    }
    al->data = new_data;
    al->capacity = new_capacity;
  }

  void *item_copy = data_dup(item, al->type);
  if (!item_copy) {
    return false;
  }

  al->data[al->len] = item_copy;
  ++al->len;
  return true;
}

bool alist_insert(size_t index, const void *item, alist *al) {
  assert(item);
  assert(al);
  assert(index <= al->len);

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

  void *item_copy = data_dup(item, al->type);
  if (!item_copy) {
    return false;
  }

  al->data[index] = item_copy;
  ++al->len;
  return true;
}

void alist_pop(size_t index, alist *al) {
  assert(al);
  assert(index < al->len);
  assert(al->len > 0);

  data_dealloc(al->data[index], al->type);
  for (size_t i = index; i < al->len - 1; ++i) {
    al->data[i] = al->data[i + 1];
  }
  --al->len;
}

size_t alist_remove(const void *item, alist *al) {
  assert(item);
  assert(al);

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

  for (size_t i = al->len; i-- > 0;) {
    if (!data_cmp(item, al->data[i], al->type)) {
      alist_pop(i, al);
    }
  }
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

void alist_swap(size_t i, size_t j, alist *al) {
  assert(al);
  assert(i < al->len);
  assert(j < al->len);

  void *temp = al->data[i];
  al->data[i] = al->data[j];
  al->data[j] = temp;
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

static void alist_qsort_range(void **data, size_t first, size_t last,
                              const datatype *type) {
  assert(data);
  assert(type);

  if (last <= first) {
    return;
  }

  void *pivot = data[first];
  size_t pos = last;

  for (size_t i = last; i > first; --i) {
    if (data_cmp(data[i], pivot, type) > 0) {
      void *temp = data[i];
      data[i] = data[pos];
      data[pos] = temp;
      --pos;
    }
  }

  void *temp = data[first];
  data[first] = data[pos];
  data[pos] = temp;

  // Prevent underflow when pos == 0
  if (pos > 0) {
    alist_qsort_range(data, first, pos - 1, type);
  }
  alist_qsort_range(data, pos + 1, last, type);
}

void alist_qsort(alist *al) {
  assert(al);
  if (al->len > 0) {
    alist_qsort_range(al->data, 0, al->len - 1, al->type);
  }
}

alist *alist_filter(bool (*pred)(const void *), const alist *al) {
  assert(pred);
  assert(al);

  alist *filtered = alist_create(al->type);
  for (size_t i = 0; i < al->len; ++i) {
    if (pred(al->data[i])) {
      alist_append(al->data[i], filtered);
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
