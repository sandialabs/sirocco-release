#include <ssmptcp_ref.h>

int ssmptcp_tx_drop(ssmptcp_conn conn, ssmptcp_op op, uint64_t a, uint64_t b)
{
  // this requires some semantics re: the op; certain ops have data on the wire
  size_t len = 0;
  switch(op)
  {
    case SSMPTCP_OP_MD: 
      len = a; 
      break;
    case SSMPTCP_OP_DATA: 
      len = a; 
      break;
    case SSMPTCP_OP_REQ: 
    case SSMPTCP_OP_DONE:
    case SSMPTCP_OP_ACK:
    case SSMPTCP_OP_STOP:
      break;
    default:
      ssm_wprint(lit, "Received unsupported op", s, ssmptcp_op_str(op));
      return -1;
  }
  if(len)
  {
    // throw away data
    void *data = SSM_MALLOC(len);
    ssmptcp_conn_read(conn, data, len);
    SSM_DEL(data);
  }
  return 0;
}
