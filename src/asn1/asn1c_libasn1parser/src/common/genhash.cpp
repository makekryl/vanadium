// genhash from asn1c turned out to be not thread-safe due to a global iterator state per element
// This is a wrapper over std::unordered_map<void*, void*> with genhash-compatible interface

#include "genhash.h"

#include <cerrno>
#include <cstring>
#include <functional>
#include <memory>
#include <unordered_map>

extern "C" {
#include "asn1p_alloc.h"
}

struct genhash_s {
  struct hash {
    size_t (*impl)(const void *key);
    std::size_t operator()(void *key) const {
      return impl(key);
    }
  };
  struct eq {
    int (*impl)(const void *key1, const void *key2);

    bool operator()(void *key1, void *key2) const {
      return static_cast<bool>(impl(key1, key2));
    }
  };

  std::unordered_map<void *, void *, hash, eq> storage;
  void (*keydestroyf)(void *key);
  void (*valuedestroyf)(void *value);

  using storage_iterator_t = decltype(genhash_s::storage)::iterator;

  struct iter_s {
    storage_iterator_t pos;
    storage_iterator_t end;
  };
};

genhash_t *
genhash_new(int (*keycmpf)(const void *key1, const void *key2),
      size_t (*keyhashf)(const void *key), void (*keydestroyf)(void *key),
      void (*valuedestroyf)(void *value)) {
  genhash_t *h;

  h = (genhash_t *)asn1p_mem_alloc(sizeof(genhash_t));
  if(!h) {
    return nullptr;
  }

  std::construct_at(
    h, genhash_s{
         .storage = decltype(h->storage)(
           0, genhash_t::hash{keyhashf}, genhash_t::eq{keycmpf}  //
           ),
         .keydestroyf = keydestroyf,
         .valuedestroyf = valuedestroyf,
       });

  return h;
}

int
genhash_count(genhash_t *h) {
  return static_cast<int>(h->storage.size());
}

int
genhash_add(genhash_t *h, void *key, void *value) {
  if(key == nullptr) {
    errno = EINVAL;
    return -1;
  }

  h->storage[key] = value;
  return 0;
}

int
genhash_addunique(genhash_t *h, void *key, void *value) {
  if(h->storage.contains(key)) {
    errno = EEXIST;
    return -1;
  }
  return genhash_add(h, key, value);
}

void *
genhash_get(genhash_t *h, const void *key) {
  if(auto it = h->storage.find((void *)key); it != h->storage.end()) {
    return it->second;
  }

  errno = ESRCH;
  return nullptr;
}

int
genhash_del(genhash_t *h, void *key) {
  if(auto it = h->storage.find((void *)key); it != h->storage.end()) {
    h->storage.erase(it);

    if(h->keydestroyf) {
      h->keydestroyf(it->first);
    }
    if(h->valuedestroyf) {
      h->valuedestroyf(it->second);
    }

    return 0;
  }

  errno = ESRCH;
  return -1;
}

/*
 * Initialize a hash iterator.
 */
int
genhash_iter_init(genhash_iter_t *iter, genhash_t *h, int reverse_order) {
  std::construct_at((genhash_s::iter_s *)iter, genhash_s::iter_s{
                           .pos = h->storage.begin(),
                           .end = h->storage.end(),
                         });
  return genhash_count(h);
}

int
genhash_iter(genhash_iter_t *iter, void **key_p, void **val_p) {
  auto &iter_impl = *(genhash_s::iter_s *)iter;
  if(iter_impl.pos != iter_impl.end) {
    *key_p = iter_impl.pos->first;
    *val_p = iter_impl.pos->second;
    ++iter_impl.pos;
    return iter_impl.pos != iter_impl.end;
  }
  return 0;
}

void
genhash_iter_done(genhash_iter_t *iter) {
  std::destroy_at((genhash_s::iter_s *)iter);
}

void
genhash_destroy(genhash_t *h) {
  if(h) {
    genhash_empty(h);
    std::destroy_at(h);
    asn1p_mem_free(h);
  }
}

void
genhash_empty(genhash_t *h) {
  if(h->keydestroyf || h->valuedestroyf) {
    for(auto &[pk, pv] : h->storage) {
      if(h->keydestroyf) {
        h->keydestroyf(pk);
      }
      if(h->valuedestroyf) {
        h->valuedestroyf(pv);
      }
    }
  }

  h->storage.clear();
}


/*----- Simple hash and compare functions for common data types ------*/

size_t
hashf_int(const void *key) {
  return std::hash<int>{}(*(const int *)key);
}

int
cmpf_int(const void *key1, const void *key2) {
  return (*(const int *)key1 != *(const int *)key2);
}

size_t
hashf_void(const void *key) {
  return std::hash<const void *>{}(key);
}

int
cmpf_void(const void *key1, const void *key2) {
  return (key1 != key2);
}

size_t
hashf_string(const void *keyarg) {
  return std::hash<std::string_view>{}((const char *)keyarg);
}

int
cmpf_string(const void *key1, const void *key2) {
  return std::strcmp((const char *)key1, (const char *)key2) == 0;
}
