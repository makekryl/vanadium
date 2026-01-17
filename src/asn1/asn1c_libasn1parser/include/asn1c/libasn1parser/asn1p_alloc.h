#ifndef ASN1_PARSER_ALLOC
#define ASN1_PARSER_ALLOC

#include <stddef.h>

typedef struct {
    void *ctx;
    void *(*alloc)(void *ctx, size_t);
    void (*free)(void *ctx, void *);
} asn1p_allocator_t;

void asn1p_mem_set_allocator(asn1p_allocator_t);
asn1p_allocator_t asn1p_mem_get_allocator();

void *asn1p_mem_alloc(size_t size) __attribute__((malloc));
void *asn1p_mem_calloc(size_t num, size_t size) __attribute__((malloc));
void *asn1p_mem_realloc(void *ptr, size_t old_size, size_t new_size);
void asn1p_mem_free(void *ptr);

char *asn1p_mem_strdup(const char *s);

#endif /* ASN1_PARSER_ALLOC */
