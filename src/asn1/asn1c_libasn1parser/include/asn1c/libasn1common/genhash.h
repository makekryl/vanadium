/*
 * Copyright (c) 2002-2005  Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Copyright (c) 2001-2004  Netli, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $Id: genhash.h 447 2005-06-07 06:51:10Z vlm $
 */
#ifndef __GENHASH_H__
#define __GENHASH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

/*
 * General purpose hashing framework.
 * Refer to the corresponding .c source file for the detailed description.
 *
 * WARNING: Generally, functions don't allow NULL's to be passed
 * as the genhash_t pointers, if not explicitly stated otherwise.
 */

typedef struct genhash_s genhash_t;

/*
 * Create a new hash table 
 * keycmpf	: function which returns 0 if keys are equal, else !0
 * keyhashf	: function which computes the hash value of a key
 * keydestroyf	: function for destroying keys, can be NULL for no destructor
 * valuedestroyf: function for destroying values, can be NULL for no destructor
 */
genhash_t *genhash_new(
	int (*keycmpf) (const void *key1, const void *key2),
	size_t (*keyhashf) (const void *key),
	void (*keydestroyf) (void *key),
	void (*valuedestroyf) (void *value));

/*
 * destroys a hash, freeing each key and/or value.
 * Keys are always destroyed before values using the destructors
 * specified upon hash creation.
 * This function is immune to NULL argument.
 */
void genhash_destroy(genhash_t *h);

/*
 * Delete all elements from the hash, retaining the hash structure itself.
 */
void genhash_empty(genhash_t *h);

/*
 * Add, returns 0 on success, -1 on failure (ENOMEM). Note, you CAN add
 * records with duplicate keys. No guarantees about order preservations.
 *
 * EXAMPLE:
 * 	char *key_str = strdup("key");
 * 	char *val_str = strdup("arbitrary value");
 * 	if(genhash_add(h, key_str, val_str) != 0) {
 * 		free(key_str);
 * 		free(val_str);
 * 		perror("genhash_add failed");
 * 		exit(EX_SOFTWARE);
 * 	}
 */
int genhash_add(genhash_t *h, void *key, void *value);

/*
 * Add, but only if a mapping is not there already.
 * RETURN VALUES:
 * 0:		Element added successfully.
 * -1/EINVAL:	Invalid arguments (key == NULL).
 * -1/EEXIST:	Duplicate entry is found.
 * -1/ENOMEM:	Memory allocation failed
 */
int genhash_addunique(genhash_t *h, void *key, void *value);

/*
 * Fetch - returns pointer to a value, NULL/ESRCH if not found
 */
void *genhash_get(genhash_t *h, const void *key);

/*
 * Delete - returns 0 on success, -1/ESRCH if not found.
 * Keys are always destroyed before values using the destructors
 * specified upon hash creation.
 */
int genhash_del(genhash_t *h, void *key);

/*
 * Return the number of elements in a hash.
 * This function is immune to NULL argument.
 */
int genhash_count(genhash_t *h);

/*
 * External iterator structure for using with iterator-based walking functions.
 * This declaration is NOT INTENDED TO BE USED BY AN APPLICATION DIRECTLY
 * The pointer to the already allocated structure must be passed to
 * genhash_iter*() functions.
 */
typedef struct genhash_iter_s {
	char pimpl_mem[16];
} genhash_iter_t;

/*
 * Initialize the iterator for walking through the hash.
 * The memory block to be used as iterator is provided by the (*iter) pointer.
 * This memory must be allocated (possibly, on the stack) by the caller.
 * OWNERSHIP:
 * 	The initialized iterator must be disposed of by calling
 * 	genhash_iter_done().
 * ORDER:
 * 	By default, the elements are iterated in the "most recent first" order,
 * 	use reverse_order to change that. For very small number of entries
 * 	(currently, 4) the order may be IGNORED.
 * RETURN VALUES:
 * 	number of entries the hash had at the moment.
 */
int genhash_iter_init(genhash_iter_t *iter,
	genhash_t *hash_to_use, int reverse_order);

/*
 * Returns the key and value of each element in optional (key) and (value),
 * which must be passed as the pointers to pointers (hence these ***'s).
 * OWNERSHIP:
 * 	The key and value are pointers to the internally manageed locations.
 * RETURN VALUES:
 * 	0 if no more elements will be returned, otherwise 1.
 * EXAMPLE:
 *	key_type_t *key;			// Pointer to key
 * 	value_type_t *val;			// Pointer to value
 * 	genhash_iter_t iter;			// Iterator structure
 * 	genhash_iter_init(&iter, hash_ptr, 0);	// Prepare iterator
 * 	while(genhash_iter(&iter, &key, &val))	// Iterate over hash elements
 *		print_keyval(key, val);		// Use key and value
 * 	genhash_iter_done(&iter);		// Done iterations.
 */
int genhash_iter(genhash_iter_t *iter, void **key, void **val);

/*
 * Dispose of the iterator.
 * After this operations, the iterator contents unusable
 * and shall not be accesed. (genhash_iter_init() is OK).
 */
void genhash_iter_done(genhash_iter_t *iter);

/****************************************************************************/

/*
 * The following hashing and comparison functions are provided for
 * you, or you may supply your own.
 */
size_t hashf_int (const void *key); /* Key is an int * */
int cmpf_int (const void *key1, const void *key2);

size_t hashf_void (const void *key);
int cmpf_void (const void *key1, const void *key2);

size_t hashf_string (const void *key);
int cmpf_string (const void *key1, const void *key2);

#ifdef __cplusplus
}
#endif

#endif	/* __GENHASH_H__ */
