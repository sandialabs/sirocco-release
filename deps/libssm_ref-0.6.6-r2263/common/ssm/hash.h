#ifndef SSM_HASH_H
#define SSM_HASH_H

// ssm_hash.h
// ssm data chunk hash

#define SSM_HASH_FUNC_NAME(size) \
  BOOST_PP_CAT(ssm_hash_func_, size)

#define SSM_HASH_CMP_NAME(size) \
  BOOST_PP_CAT(ssm_hash_cmp_, size)

#define SSM_HASH_FUNC_DEFN(size) \
  static inline SSM_HASH_FUNC_NAME(size)(char *data) \
  {                                                  \
    khint_t h = *data;                               \
    if(h)                                            \
      for(int i = 0; i < size; i++)                  \
        h = (h << 5) - h + *key++;                   \
  }

#define SSM_HASH_CMP_DEFN(size) \
  static inline SSM_HASH_CMP_NAME(size)(char *a, char *b) \
  {                                                       \
    return (memcmp(a, b, size) == 0);                     \
  }

#define SSM_HASH_NAME(name, size) \
  BOOST_PP_CAT(ssm_hash, size)

#define SSM_HASH_TYPE(name, size) \
  SSM_HASH_FUNC_DEFN(name, size)  \
  SSM_HASH_CMP_DEFN(name, size)   \
  KHASH_INIT(name, char *, void *, 1, SSM_HASH_FUNC_NAME(name, size), SSM_HASH_CMP_NAME(name, size)) \


#endif
