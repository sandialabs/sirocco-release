#include <ssm_md_ref.h>

ssm_md ssm_md_sub(ssm_md target, size_t off, size_t len, ssm_md src)
{
  if(!src)
  {
    target = ssm_md_add(target, off, len);
    return target;
  }
  ssm_Ii i = src->mdns->i(src->mdns);
  ssm_mdn cur = i->next(i);
  while(cur && off > cur->len)
  {
    off -= cur->len;
    cur = i->next(i);
  }
  if(!cur) 
    len = 0;

  size_t foff, flen;
  while(len)
  {
    if(off)
    {
      foff = cur->off + off;
      flen = cur->len - off;
    }  
    else
    {
      foff = cur->off;
      flen = cur->len;
    }
    if(flen > len)
      flen = len;
    target = ssm_md_add(target, foff, flen);
    len -= flen;
    cur = i->next(i);
    if(!cur)
      len = 0;
  }
  i->del(i);
  ssm_dreturn(d, target);
  return target;
}
