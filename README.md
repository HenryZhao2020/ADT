# Overview: `alist` Memory and Insertion Model

An **alist** stores items in a dynamically resizable array.

Each `alist` is associated with a specific `datatype`, defining a common type for all stored items.  
Type-specific behaviors (duplication, comparison, printing, and deallocation) are handled through the **datatype** interface.

---

## Memory Model

- Every item stored in an `alist` resides in separately allocated **heap memory**, either:
  - **Duplicated** from the original (**deep copy**), or
  - **Directly referenced** (**shallow copy** of the pointer).
- Stored items are automatically **freed** when:
  - Individually removed, or
  - When the `alist` is destroyed.

---

## Insertion and Assignment Modes

### 1. Deep Copying (default behavior)
- The item is **duplicated** using the datatype's `dup` function.
- **Stack-allocated (temporary)** variables are safely **deep-copied** into heap memory.
- **Heap-allocated** objects are also **deep-copied**, ensuring:
  - Modifying the original does **not** affect the stored item, and vice versa.
- **Const** objects can be safely deep-copied, preserving immutability.

**Functions**:  
`alist_set`, `alist_append`, `alist_insert`, etc.

---

### 2. Shallow Copying (optional optimization)
- A **shallow copy** of the pointer is stored **without duplication**.
- **Faster** for large objects (no copying overhead).
- **External mutation** of the object affects the stored alist item (and vice versa).
- Shallow-copied items are **automatically freed** by `alist` at removal or destruction.

**Functions**:  
`alist_set_ref`, `alist_append_ref`, `alist_insert_ref`, etc.

---

## Safety Guidelines

- **Prefer deep copying** unless optimization is necessary.
- After deep copying, the **original object must be freed manually** by the client.
- For **shallow copying**:
  - The object must stay **valid** during the entire lifetime of the `alist`.
  - It must be **heap-allocated** and properly compatible with the datatype's `destroy` function.
- **NEVER** shallow-copy:
  - **Stack-allocated** or **temporary** variables.
  - **Const-qualified** pointers (discards constness and leads to undefined behavior).
- Clients must **NOT manually free** shallow-copied items.


---


## Comparison: `alist` vs `glib GArray` vs `Java ArrayList` vs `C++ std::vector`

| Feature | **`alist`** | **GLib `GArray`** | **Java `ArrayList`** | **C++ `std::vector`** |
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
