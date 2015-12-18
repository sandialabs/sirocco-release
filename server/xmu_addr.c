/* Various utility functions copied from xmecho.c, from xm
 * distribution. Changed such that the address is passed directly back
 * to user instead of address_buffer_header, which the user can free */
#ifndef _BSD_SOURCE
#define _BSD_SOURCE
#endif
#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE
#endif

#include <arpa/inet.h>
#include <errno.h>
#include <limits.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sxm.h>

static char *
savstr(const char *s)
{
	size_t	n;
	void	*p;

	n = strlen(s) + 1;
	if ((p = malloc(n)) == NULL)
		return NULL;
	return memcpy(p, s, n);
}

static int
parse_u(const char *s, unsigned long lo, unsigned long hi, unsigned long *ulp)
{
	char	*end;

	*ulp = strtoul(s, &end, 0);
	if (*s != '\0' && *end != '\0')
		return -EINVAL;
	if ((*ulp == ULONG_MAX && errno == ERANGE) ||
	    *ulp < lo ||
	    *ulp > hi)
		return -ERANGE;
	return 0;
}

static int
parse_tcp_addr(const char *s, char **ip, uint16_t *port)
{
	char *nam, *cp;
	int	err;
	unsigned long ul;

	if ((nam = savstr(s)) == NULL)
		return -ENOMEM;
	err = 0;
	do {
		cp = strchr(nam, ':');
		if (cp != NULL) {
			*cp++ = '\0';
			if ((err = parse_u(cp, 0, UINT16_MAX, &ul)) != 0)
				break;
		} else
			ul = 0;
		*port = (uint16_t)ul;
		*ip = malloc(strlen(nam) + 1);
		if (*ip == NULL) {
			err = -ENOMEM;
			break;
		}
		strcpy(*ip, nam);
	} while (0);
	free(nam);
	return err;
}

int
xmu_parse_addr(const char *s, struct xm_transport *xmx, ssm_Haddr *addr)
{
	int err;
	char *ip, safe_ip[20]; /* SSM likes to have 20 chars */
	uint16_t port;
	err = parse_tcp_addr(s, &ip, &port);
	if (err == 0) {
		strncpy(safe_ip, ip, 20);
		if (safe_ip[19] != '\0') 
			err = -EINVAL;
		else {
			*addr = xmtcp_addr_create(xmx, safe_ip, port);
			if (*addr == NULL)
				err = -errno;
		}
	}
	free(ip);
	errno = -err;
	return err;
}
