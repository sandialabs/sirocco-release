#include <xm.h>

struct tp {
	struct xm_transport *xmx;
	unsigned me_pool_started:1;
	unsigned unex_started:1;
};

int
_asg_start_tp(struct tp *tp, int tp_flags)
{
	int rc;

	tp->me_pool_started = tp->unex_started = 0;
	rc = Xm_tcp_create(&tp->xp);
	if (rc != 0) {
		ERR("Could not start XM TCP transport: %s\n", strerror(-rc));
		return rc;
	}
	Xm_tcp_set_verbose(tp->xp, 1);
	do {
		/*if (tp_flags & _ASG_TP_UNEX) {*/
		if (1) {
			rc = _asg_start_unex(asg);
			if (rc)
				break;
			asg->unex_started = 1;
		}

		/* if (tp_flags & _ASG_TP_MES) { */
		if (0) {
			rc = _asg_start_me_pool(asg);
			if (rc != 0)
				break;
			asg->me_pool_started = 1;
		}
		return 0;
	} while(0);
	if (asg->unex_started)
		(void)_asg_stop_unex(asg);
	/* Xm_fini(asg->xp); */ /* Can't call yet */
	return rc;
}

int
_asg_stop_tp(struct asg_instance *asg)
{
	int rc;

	if (asg->unex_started) {
		rc = _asg_stop_unex(asg);
		if (rc) {
			WARN("Could not stop unexpected message queue\n");
			return rc;
		}
		asg->unex_started = 0;
	}

	if (asg->me_pool_started) {
		rc = _asg_stop_me_pool(asg);
		if (rc) {
			WARN("Could not stop match entry pool\n");
			return rc;
		}
		asg->me_pool_started = 0;
	}

	/* Xm_fini, when ready */
	return 0;
}

int
_asg_init(struct asg_instance **_asg, const char *addr, int tp_flags)
{
	int rc;
	struct asg_instance *asg;
	int tp_started = 0;

	*_asg = asg = malloc(sizeof(*asg));
	if (asg == NULL)
		return -errno;

	do {
		rc = xmu_parse_addr(addr, &asg->listenaddr);
		if (rc != 0) {
			asg->listenaddr = NULL;
			ERR("Could not parse addr %s: %s\n", addr,
			    strerror(-rc));
			break;
		}

		rc = _asg_start_tp(asg, tp_flags);
		if (rc != 0)
			break;
		tp_started = 1;

		rc = Xm_listen(asg->xp, asg->listenaddr);
		if (rc != 0) {
			ERR("Xm_listen: %s\n", strerror(-rc));
			break;
		}

		return 0;
	} while(0);

	if (tp_started)
		_asg_stop_tp(asg);
	free(asg->listenaddr);
	free(asg);
	return -rc;
}
