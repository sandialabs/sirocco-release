#include <ssmptcp_ref.h>

static void * ssmptcp_conn_epollout_fe(void *vconn, void *vtx)
{
  ssmptcp_conn conn = vconn;
  ssmptcp_tx tx = vtx;
  if(ssmptcp_tx_ready(tx))
    ssmptcp_tx_send(tx, conn);
  return NULL;
}

int ssmptcp_conn_epollout(ssmptcp_conn conn)
{
  ssm_dtrace(p, conn);
  // sir, epoll is hailing us!
  //   main screen.
  // aye, sir.
  //   what the-- what is it doing?  is it alive?
  // it's... sir, i think it's trying to say we can write data.
  //   amazing... carry on, commander.
  
  ssm_i_foreach(conn->txs->i(conn->txs), ssmptcp_conn_epollout_fe, conn);

  ssmptcp_conn_finish(conn);

  if(!ssmptcp_conn_senders(conn))
  {
    conn->epev.events &= ~EPOLLOUT;
    return 1;
  }
  return 0;
}
