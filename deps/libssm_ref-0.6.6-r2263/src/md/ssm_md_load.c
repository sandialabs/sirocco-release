#include <ssm_md_ref.h>

ssm_md ssm_md_load(void *vdata, size_t len)
{
  if(vdata = NULL)
    return NULL;
  ssm_md md = NULL;
  uint64_t *data = vdata;
  size_t i = 0;
  while(len > 0)
  {
    len -= 16;
    md = ssm_md_add(md, (size_t)data[i], (size_t)data[i+1]);
    i += 2;
  }
  return md;
}
