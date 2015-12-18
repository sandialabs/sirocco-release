#include <stdlib.h>
#include <strings.h>
#include <errno.h>
#include <assert.h>

#include "aasg.h"
#include "aasg_intnl.h"

ssize_t
_aasg_serdes(xdrproc_t xdrproc,
	     void *cooked,
	     void *raw, size_t size,
	     enum xdr_op xop)
{
	XDR	control;

	xdrmem_create(&control, raw, size, xop);
	if (!(*xdrproc)(&control, cooked))
		return -EINVAL;
	return xdr_getpos(&control);
}

ssize_t
_aasg_pserdes(struct asg_batch_id_t *bid,
	      xdrproc_t xdrproc,
	      void *cooked,
	      enum xdr_op xop)
{
	return _aasg_serdes(xdrproc,
			    cooked,
			    bid->raw, sizeof(bid->raw),
			    xop);
}
