This repository provides generic storage ADT modules in C.

# `alist` vs `glib GArray` vs `Java ArrayList` vs `C++ std::vector`

| Feature | **Your `alist`** | **GLib `GArray`** | **Java `ArrayList`** | **C++ `std::vector`** |
|:--------|:----------------|:------------------|:---------------------|:---------------------|
| **Language** | C | C | Java | C++ |
| **Data storage** | `void **` (generic pointers) | Raw data (byte array) | Typed objects | Typed objects (template) |
| **Type system** | Manual (with `datatype` abstraction) | Manual (user responsibility) | Strong, with Generics | Strong, with Templates |
| **Dynamic resizing** | ✅ Automatic | ✅ Automatic | ✅ Automatic | ✅ Automatic |
| **Deep copy (per-element)** | ✅ Supported (default) | ❌ Not automatic | ❌ No (always shallow) | ❌ No (always shallow) |
| **Shallow copy (opt-in)** | ✅ Supported (`_ref` functions) | ❌ Manual (by address) | ✅ Always shallow | ✅ Always shallow |
| **Automatic item destruction** | ✅ On remove/clear/destroy | ❌ No (manual free) | ❌ (GC handles) | ✅ (via RAII if smart pointers) |
| **Custom behavior (dup, cmp, print, free)** | ✅ Fully supported | ❌ Only manually outside GArray | ❌ | ❌ |
| **Printing support** | ✅ `data_print` | ❌ No | ✅ `toString()` | ✅ (`operator<<` if user defines) |
| **Sorting** | ✅ Custom quicksort | ✅ `g_array_sort` | ✅ `Collections.sort()` | ✅ `std::sort` |
| **Randomized sorting** | ❌ (First pivot only) | ❌ | ✅ (Timsort) | ✅ (Introsort) |
| **Binary search** | ✅ Built-in | ❌ (manual) | ❌ (manual `Collections.binarySearch()`) | ✅ (`std::binary_search`) |
| **Filtering** | ✅ `alist_filter` | ❌ Manual | ✅ (Streams API) | ✅ (Ranges / Algorithms) |
| **Mapping (in-place)** | ✅ `alist_map` | ❌ Manual | ✅ (Streams API) | ✅ (Algorithms) |
| **Remove if predicate** | ✅ `alist_remove_if` | ❌ Manual | ✅ (removeIf) | ✅ (`std::remove_if`) |
| **Slice / Sublist** | ✅ `alist_slice` | ❌ Manual copy needed | ✅ `subList()` | ✅ (Iterators or `std::span`) |
| **Index search (linear)** | ✅ Built-in | ✅ | ✅ | ✅ |
| **Thread Safety** | ❌ (user responsibility) | ❌ (user responsibility) | ❌ by default | ❌ by default |
| **Memory management model** | Explicit malloc/free | Manual + optional glib memory pool | GC (automatic) | RAII (automatic if smart pointers) |
| **Reserve/Capacity control** | ✅ `alist_reserve`, `alist_reclaim` | ✅ `g_array_set_size()` | ✅ `ensureCapacity()`, `trimToSize()` | ✅ `reserve()`, `shrink_to_fit()` |
| **Empty check** | ✅ `alist_empty` | ✅ | ✅ `isEmpty()` | ✅ `empty()` |
| **Robust error handling** | ✅ Return false/NULL on fail | ❌ Sometimes crashes | ✅ Throws Exception | ✅ Throws Exception |
| **Grow/shrink strategy** | Double capacity | Double capacity | 50% growth (adaptive) | 2x growth (GCC/Clang) |
| **Performance predictability** | ✅ (deterministic behavior) | ✅ | ✅ | ✅ |
| **Container copy (deep)** | ✅ `alist_dup` | ❌ | ✅ `clone()` (shallow copy, deep needs manual) | ✅ Manual copy constructor |
| **Zero-overhead abstraction** | ✅ (except user-supplied functions) | ✅ | ❌ (Java abstraction overhead) | ✅ |
| **Initial capacity setting** | ✅ Supported | ✅ Supported | ✅ Supported | ✅ Supported |
| **Opaqueness of internal data** | ✅ Fully opaque | ❌ (fields public) | ✅ | ✅ |
| **Flexibility to extend** | ✅ Easy (new methods) | ✅ Moderate | ❌ Hard (standard library) | ✅ Easy (extend templates) |
| **Error rollback safety** | ✅ Fully covered | ❌ Partial | ✅ Exception rollback | ✅ Exception rollback |
| **Documentation clarity** | ✅ Excellent | ✅ Good for GArray | ✅ JavaDoc excellent | ✅ STL docs excellent |
