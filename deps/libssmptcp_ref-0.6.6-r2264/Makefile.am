lib_LIBRARIES = libssmptcp.a

libssmptcp_a_SOURCES = src/ssmptcp_wait.c src/addr/ssmptcp_addrdestroy.c src/addr/ssmptcp_addrcpv.c src/addr/ssmptcp_addrlocal.c src/addr/ssmptcp_addrdel.c src/addr/ssmptcp_addrcmp.c src/addr/ssmptcp_addrcp.c src/addr/ssmptcp_addrcreate.c src/ssmptcp_epdel.c src/tp/ssmptcp_tpmap.c src/tp/ssmptcp_tpunmap.c src/tp/ssmptcp_tppull.c src/tp/ssmptcp_tpwake.c src/tp/ssmptcp_tpstop.c src/tp/ssmptcp_tpdel.c src/tp/ssmptcp_tppush.c src/tp/ssmptcp_tp.c src/tp/ssmptcp_tpstart.c src/tp/ssmptcp_tpaddr.c src/ssmptcp_epmod.c src/tx/ssmptcp_tx_ready.c src/tx/ssmptcp_tx_op_stop.c src/tx/ssmptcp_tx_op_ack.c src/tx/ssmptcp_tx_kill.c src/tx/ssmptcp_tx_op_req.c src/tx/ssmptcp_tx_op_done.c src/tx/ssmptcp_tx_finish.c src/tx/ssmptcp_tx_done.c src/tx/ssmptcp_tx_abort.c src/tx/ssmptcp_tx_send.c src/tx/ssmptcp_tx_create.c src/tx/ssmptcp_tx_drop.c src/tx/ssmptcp_tx_op.c src/tx/ssmptcp_tx_op_md.c src/tx/ssmptcp_tx_op_data.c src/tx/ssmptcp_tx_init.c src/ssmptcp_accept.c src/ssmptcp_epadd.c src/ssmptcp_wake.c src/ssmptcp_ctl.c src/ssmptcp_sockaddr.c src/ssmptcp_dropcb.c src/ssmptcp_event.c src/conn/ssmptcp_conn_epollout.c src/conn/ssmptcp_conn_epollin.c src/conn/ssmptcp_conn_readv.c src/conn/ssmptcp_conn_op.c src/conn/ssmptcp_conn_senders.c src/conn/ssmptcp_conn_find.c src/conn/ssmptcp_conn_op_pair.c src/conn/ssmptcp_conn_kill.c src/conn/ssmptcp_conn_writev.c src/conn/ssmptcp_conn_writemr.c src/conn/ssmptcp_conn_op_rport.c src/conn/ssmptcp_conn_init.c src/conn/ssmptcp_conn_finish.c src/conn/ssmptcp_conn_readmr.c src/conn/ssmptcp_conn_send.c src/conn/ssmptcp_conn_readmr_new.c src/conn/ssmptcp_conn_read.c src/conn/ssmptcp_conn_write.c 

libssmptcpincdir = ${includedir}
libssmptcpinc_HEADERS = common/ssmptcp.h 

noinst_HEADERS = inc/ssmptcp_ref.h 

libssmptcp_a_CPPFLAGS = -pthread -I$(top_srcdir)/inc -I$(top_srcdir)/common

EXTRA_DIST = RELEASE.txt INSTALL.txt KNOWN-BUGS.txt