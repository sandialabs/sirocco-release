#ifdef RPC_XDR
%#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

typedef uint64_t entity1;
typedef uint64_t size1;
typedef uint64_t update1;
typedef uint64_t match1;
typedef uint64_t rvers1;
typedef uint64_t rid1;
typedef uint64_t time1;
typedef uint32_t proc1;

enum sosstat1 {
	SOS1_OK		= 0,
	SOS1_FAIL	= 1
};

enum sosop1 {
	SOS1_NONE       = 0,
	SOS1_READ       = 1,
	SOS1_PROBE      = 2,
	SOS1_PUNCH      = 3,
	SOS1_BATCH      = 4,
	SOS1_COMMIT     = 5,
	SOS1_TIME       = 6,
	SOS1_WRITE      = 7
};

struct nsid1 {
	entity1	container;
	entity1	object;
	entity1	fork;
};

enum cond1 {
	SOS1_COND_NONE	= 0,
	SOS1_COND_UNTIL	= 1,
	SOS1_COND_ALL	= 2
};

enum updctl1 {
	SOS1_UPD_NONE	= 0,
	SOS1_UPD_AUTO	= 1
};

/* write */

struct WRITE1args {
	nsid1	nsid;
	rid1	at;
	size1	nrecs;
	size1	rlen;
	cond1	cond;
	updctl1	updctl;
	update1	update;
	match1	match;
	match1  ret;
};

struct WRITE1res {
	sosstat1 status;
	size1 count;
	rvers1 vers;
};

/* read */

struct OPTPROBE1info {
	match1	match;
	size1	bufsiz;
};

union OPTPROBE1args switch (unsigned present) {
case TRUE:
	OPTPROBE1info info;
default:
	void;
};

struct READ1args {
	nsid1	nsid;
	rid1	at;
	size1	nrecs;
	cond1	cond;
	update1	update;
	size1	bufsiz;
	match1	match;
	match1  ret;
	OPTPROBE1args probe;
};

struct rinfo1 {
	rid1	rid;
	size1	runlen;
	size1	rlen;
	rvers1	vers;
};

struct READ1res {
	sosstat1 status;
	size1	count;
	size1	rcount;
	rvers1	vers;
};

/* batch */

struct BATCH1args {
	match1 cmdbuf;
	match1 ret;
	size1 bufsiz;
};

struct BATCH1res {
	sosstat1 status;
};

/* time */

struct TIME1args {
	match1 ret;
};

union TIME1res switch (sosstat1 status) {
 case SOS1_OK:
	 time1 time;
 default:
	 void;
};

union SOSargs switch (sosop1 op) {
 case SOS1_WRITE:
	 WRITE1args w;
 case SOS1_READ:
	 READ1args r;
 case SOS1_BATCH:
	 BATCH1args b;
 case SOS1_TIME:
	 TIME1args t;
 default:
	 void;
};

union SOSres switch (sosop1 op) {
 case SOS1_WRITE:
	 WRITE1res w;
 case SOS1_READ:
	 READ1res r;
 case SOS1_BATCH:
	 BATCH1res b;
 case SOS1_TIME:
	 TIME1res t;
 default:
	 void;
};

struct msg_header {
	proc1 proc;
	match1 reply;
};

program SOS_PROGRAM {
	version SOS1_VERSION {
		BATCH1res
		SOSBATCH1_OP(BATCH1args) = 1;
		time_t
		SOSSVR_TIME() = 4;
	} = 1;
} = 0x20202020;
