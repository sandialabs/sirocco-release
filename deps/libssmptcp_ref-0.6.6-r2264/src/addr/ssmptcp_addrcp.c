#include <ssmptcp_ref.h>

ssm_Haddr ssmptcp_addrcp(ssm_Iaddr addr, ssm_Haddr src)
{
  ssmptcp_addr result = SSMPTCP_NEW(addr);
  memmove(result, src, SSMPTCP_SIZEOF(addr));
  return result;
}

size_t ssmptcp_addr_serialize(ssm_Iaddr ia, char *out, size_t len,
			      ssm_Haddr addr)
{
	ssmptcp_addr a = addr;
	size_t rlen = sizeof(a->host) + sizeof(a->port) +
		sizeof(a->origin);
	if (len < rlen)
		return rlen;

	memmove(out, a->host, sizeof(a->host));
	out += sizeof(a->host);
	memmove(out, &a->port, sizeof(a->port));
	out += sizeof(a->port);
	memmove(out, &a->origin, sizeof(a->origin));
	out += sizeof(a->origin);
	return rlen;
}

ssm_Haddr ssmptcp_addr_load(ssm_Iaddr addr, char *in, size_t len)
{
	ssmptcp_addr result = SSMPTCP_NEW(addr);
	if (result == NULL)
		return NULL;
	memmove(result->host, in, sizeof(result->host));
	in += sizeof(result->host);
	memmove(&result->port, in, sizeof(result->port));
	in += sizeof(result->port);
	memmove(&result->origin, in, sizeof(result->origin));
	in += sizeof(result->origin);

	return result;
}

char *ssmptcp_addr_dump(ssm_Haddr Haddr)
{
	char *str = malloc(32);
	ssmptcp_addr addr = Haddr;
	if (str == NULL)
		return NULL;
	uint16_t port = ntohs(addr->port);
	sprintf(str, "%s:%u", addr->host, port);
	return str;
}
