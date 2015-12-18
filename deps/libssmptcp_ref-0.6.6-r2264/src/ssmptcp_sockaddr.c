#include <ssmptcp_ref.h>

void ssmptcp_sockaddr(struct sockaddr_in *saddr, ssmptcp_addr addr)
{
  bzero(saddr, sizeof(struct sockaddr_in));
  saddr->sin_family = AF_INET;
  saddr->sin_port = addr->port;
  int r = inet_pton(AF_INET, addr->host, &(saddr->sin_addr));
  if(r != 1)
    ssm_eprint(lit, "couldn't inet_pton", s, strerror(errno));
}
