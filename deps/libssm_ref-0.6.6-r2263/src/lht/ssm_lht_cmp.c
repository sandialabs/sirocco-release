#include <ssm_lht_ref.h>

int ssm_lht_cmp(void *va, void *vb)
{
  ssm_lhtcmp a = va;
  ssm_lhtcmp b = vb;
  return memcmp(a->key, b, a->len);
}
