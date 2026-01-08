#include "asn1p_alloc.h"

#include <threads.h>
#include <string.h>

static thread_local asn1p_allocator_t asn1p_current_allocator = {};

void asn1p_mem_use_allocator(asn1p_allocator_t allocator) {
  asn1p_current_allocator = allocator;
}

asn1p_allocator_t asn1p_mem_get_allocator() {
  return asn1p_current_allocator;
}


void* asn1p_mem_alloc(size_t size) {
  return asn1p_current_allocator.alloc(asn1p_current_allocator.ctx, size);
}

void *asn1p_mem_calloc(size_t num, size_t size) {
  size_t bytes = num * size;

  void* ptr = asn1p_mem_alloc(bytes);
  if (ptr == NULL) {
    return NULL;
  }

  memset(ptr, 0, bytes);

  return ptr;
}

void *asn1p_mem_realloc(void *ptr, size_t old_size, size_t new_size) {
  if (__builtin_expect(new_size <= old_size, 0)) {
    return ptr;
  }

  void *newptr = asn1p_mem_alloc(new_size);
  memcpy(newptr, ptr, old_size);
  asn1p_mem_free(ptr);

  return newptr;
}

void asn1p_mem_free(void* ptr) {
  asn1p_current_allocator.free(asn1p_current_allocator.ctx, ptr);
}

char* asn1p_mem_strdup(const char *s) {
  size_t len = strlen(s) + 1;
  void *newptr = asn1p_mem_alloc(len);

  if (newptr == NULL) {
    return NULL;
  }

  return (char *) memcpy(newptr, s, len);
}
