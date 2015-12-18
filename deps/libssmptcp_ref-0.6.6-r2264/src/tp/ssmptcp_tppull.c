#include <ssmptcp_ref.h>

int ssmptcp_tppull(ssm_Itp vtp, ssm_Il txs, ssm_Haddr vaddr, ssm_Il heads, ssm_Il mmrs, ssm_Il mds)
{
  ssm_dtrace(p, vtp, p, txs, p, vaddr, p, heads, p, mmrs, p, mds);
  ssmptcp_tp tp = (ssmptcp_tp)vtp;
  ssmptcp_addr addr = vaddr;

  ssmptcp_conn conn = ssmptcp_conn_find(tp, addr);

  ssm_Ii txi = txs->i(txs);
  ssm_Ii headi = heads->i(heads);
  ssm_Ii mmri = mmrs->i(mmrs);
  ssm_Ii mdi = mds->i(mds);

  while(txi->next(txi))
  {
    ssm_mmr mmr = mmri->next(mmri);
    ssm_mmr head = headi->next(headi);

    ssmptcp_tx tx = SSMPTCP_NEW(tx);
    ssmptcp_tx_init(tx, (conn->next_id)++, 0, mmr->mr, mdi->next(mdi), SSMPTCP_CALL_PULL, 0, txi->cur(txi));
    ssmptcp_conn_send(conn, 0, SSMPTCP_OP_PAIR, head->mr, NULL, ssm_mr_len(head->mr), tx->id);
    conn->txs->addbot(conn->txs, tx);
  }
  txi->del(txi);
  headi->del(headi);
  mmri->del(mmri);
  mdi->del(mdi);
  return 0;
}

