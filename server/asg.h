/* Revised following meeting on March 17, 2014 to accommodate the following   
 * proposed changes:                                                           
 *                                                                             
 * - switch from "version" to "update id" terminology                          
 * - add probe parameters to asg_read call to support read+probe semantics     
 *   in a single API operation                                                 
 * 
 * Revised 1/7/2015 to accommodate the following proposed changes:
 * 
 * - add asynchronous operations
 * - add batches 
 */

#ifndef SIROCCO_ASG_H
#define SIROCCO_ASG_H

#include <stdint.h>
#include <string.h>

/* current API level is 1.1.0 */

/* update ASG_VERSION_MAJOR on major feature or semantic changes */
#define ASG_VERSION_MAJOR 1
/* update ASG_VERSION_MINOR on minor changes that break API compatibility */
#define ASG_VERSION_MINOR 1
/* update ASG_VERSION_SUB on minor changes that do not break API            
 * compatibility */
#define ASG_VERSION_SUB 0

typedef intptr_t asg_instance_t;

typedef intptr_t asg_location_t;

struct asg_batch_id_t;

typedef uint64_t asg_container_id_t;
typedef uint64_t asg_object_id_t;
typedef uint64_t asg_fork_id_t;
typedef uint64_t asg_record_id_t;

typedef uint64_t asg_offset_t;
typedef uint64_t asg_size_t;

typedef int asg_result_t;

typedef uint64_t asg_update_id_t;

typedef uint64_t asg_time_t;

struct asg_session {
	asg_instance_t inst;
	asg_location_t loc;
	struct asg_batch_id_t *batch;
};

struct asg_id {
	asg_container_id_t c;
	asg_object_id_t o;
	asg_fork_id_t f;
	asg_record_id_t r;
	asg_size_t rcount;
};

/** The highest valid version number */
#define ASG_VERSION_MAX     (2^64-2)

/** Constant indicating multiple versions
 *  (used in reading)                        */
#define ASG_VERSION_MIXED   (2^64-1)


#define ASG_LOCATION_AUTO   0

#define ASG_RECORD_NULL     0
#define ASG_FORK_NULL       0
#define ASG_OBJECT_NULL     0
#define ASG_CONTAINER_NULL  0

/**
 * Return/error codes
 */
typedef enum
{
   ASG_SUCCESS                = 0,
   ASG_ERR_UPDATE_ID          = 1,
   ASG_ERR_LOCATION           = 2,
   ASG_ERR_OTHER              = 666
} asg_ret_t;



/**
 * Flags for use in read,write,punch.
 */
typedef enum
{
   /** For reading: read in order until a record is found which has the
    * same or higher version number than the one specified.
    * For writing/punch: update records in order as specified until the end of
    * the specified range is encounder or until a record is found which has a
    * version greater or equal to the one specified. */
   ASG_COND_UNTIL = 0x0001,

   /** For reading: only read if *all* records in the specified range
    *  have a version number strictly smaller than the one specified.
    *  For writing/punch: only write if *all* records in the specified range
    *  have a version number strictly smaller than the one specified.
    */
   ASG_COND_ALL   = 0x0002,

   /** Always read/write */
   ASG_COND_NONE            = 0x0000,
   ASG_COND_UNCONDITIONAL   = ASG_COND_NONE,

   /** For writing/punch: automatically set the version of the updated records
    * to a version number guaranteed to be higher than any of the previously
    * existing version numbers in the range.  */
   ASG_AUTO_UPDATE_ID = 0x0004,

   ASG_LOCK_ID = 0x0008,

   /** For triggered write, indicates whether or not the server should write 
    * a lock into the record for later reads. */ 
} asg_flags_t;

typedef enum
{
   /** For transaction: operations executed in order and atomically.  Any
    * error in execution will result in the entire transaction rolling back or
    * prevent it from rolling forward */ 
   ASG_TX = 0x0001
} asg_rflags_t;

/* ========================================================================*/

/** Initialize storage system instance */
int asg_initialize (asg_instance_t * instance, const char * options);


/** Close storage system instance */
int asg_finalize (void);

/** batch operations
 * Batches are an optional behavior within the ASG framework that allows 
 * operations to be grouped for execution on the instance targeted.
 * Any batch created by begin() must have a corresponding end(), nesting
 * is allowed but not interleavable (the parent batch cannot end before the
 * child batch). Order of execution is neither implied nor guaranteed, except
 * in the case of a transaction. Batch arguments added to synchronous 
 * operations will be ignored. */

/** create a batch */
int asg_batch_init(struct asg_batch_id_t **batch, 
		   asg_instance_t inst,
		   asg_location_t loc,
		   void (*cb)(void *, asg_result_t),
		   void *cb_data);

/** end a batch */
int asg_batch_submit(struct asg_batch_id_t *batch);
int aasg_batch_submit(struct asg_batch_id_t *batch);

/** batches are, by definition, asynchronous.  When the stat function is 
 * called, the batch (assuming a trigger is present) will either return its 
 * ID (issued by the server), or it will return -EIO or any other error that 
 * may have occurred in transport
 */

int asg_batch_stat(struct asg_batch_id_t *bid);
int asg_batch_return(struct asg_batch_id_t *bid);
int asg_batch_cancel(struct asg_batch_id_t *bid);

/** asynchronous operations
 * operations following entitled asg_a* (such as asg_aread) are asynchronous. 
 * The caller will be notified of an operation's success or failure by 
 * callback.  The supplied callback will have access to a result code and 
 * caller-supplied data (cbdata). 
 */

/**
 * Retrieve data
 *    - The version number of the region is returned in version_info:
 *         either a specific version number, or -- if multiple versions
 *         are present in the range -- ASG_VERSION_MIXED
 *
 * 
 */

typedef struct
{
   asg_record_id_t   record_id;  /** number of first record in the sequence */
   asg_size_t        seq_len;    /** Number of records in sequence */
   asg_size_t        record_len; /** Length of each record in sequence */
   asg_update_id_t     record_update_id; /** update_id of each record in seq */
} asg_record_info_t;

int asg_read (
      struct asg_session ses,
      struct asg_id id,
      asg_flags_t flags,
      asg_update_id_t update_id_condition,
      void * buf,
      size_t bufsize,
      asg_size_t *transferred,
      asg_record_info_t *record_buf,
      asg_size_t rbufsize,
      asg_size_t * record_buf_transferred,
      asg_update_id_t * update_id_info);

int aasg_read (
      struct asg_session ses,
      struct asg_id id,
      asg_flags_t flags,
      asg_update_id_t update_id_condition,
      void *buf,
      asg_size_t bufsize,
      asg_size_t *transferred,
      asg_record_info_t *record_buf,
      asg_size_t rbufsize,
      asg_size_t *record_buf_transferred,
      asg_update_id_t *update_id_info);

int asg_write (
      struct asg_session ses,
      struct asg_id id,
      asg_size_t recordlen,
      asg_flags_t flags,
      asg_update_id_t update_id_condition,
      asg_update_id_t *new_update_id,
      const void * data,
      asg_size_t * transferred);

int aasg_write (
      struct asg_session ses,
      struct asg_id id,
      asg_size_t recordlen,
      asg_flags_t flags,
      asg_update_id_t update_id_condition,
      asg_update_id_t *new_update_id,
      const void *data,
      asg_size_t *transferred);

/**
 * Punched is exactly like write but writes zero length records.
 *
 * Transferred indicates the number of records updated.
 */
int asg_punch (
      struct asg_session ses,
      struct asg_id id,
      asg_size_t recordlen,
      asg_flags_t flags,
      asg_update_id_t update_id_condition,
      asg_update_id_t new_update_id,
      asg_size_t * transferred);

int aasg_punch (
      struct asg_session ses,
      struct asg_id id,
      asg_size_t recordlen,
      asg_flags_t flags,
      asg_update_id_t update_id_condition,
      asg_update_id_t new_update_id,
      asg_size_t *transferred);

/**
 * Returns the number of entities that have been reset in reset_count
 *
 * Passing ASG_RECORD_NULL resets all records in the fork.
 * Passing ASG_FORK_NULL resets all forks in the object.
 * Passing ASG_OBJECT_NULL resets all objects in the container.
 * Passing ASG_CONTAINER_NULL resets all containers in the storage system.
 */
int asg_reset (
      struct asg_session ses,
      struct asg_id id,
      asg_size_t recordlen,
      asg_size_t * reset_count);

int aasg_reset (
      struct asg_session ses,
      struct asg_id id,
      asg_size_t recordlen,
      asg_size_t *reset_count);

/**
 * Probe retrieves information about containers, objects, forks or records.
 *
 * In all cases, start_item indicates the id of the first item (container,
 * object, fork or record) for which data should be retrieved.
 *
 * Buf points to a set of asg_container_info_t, asg_object_info_t,
 * asg_fork_info_t or asg_record_info_t structures.
 *
 * maxitems indicates the maximum number of info structures that should be
 * retrieved.
 *
 * transferred will contain the number of
 * structures stored in buf.
 *
 * returns
 *   ASG_SUCCESS            if all information was retrieved.
 *
 *   ASG_ERR_xxx
 *
 * If more items are available, *next is set to the id of the next item for
 * which information can be retrieved (and this value can be passed to 'start'
 * in subsequent calls).
 *
 * If no more items are availalbe, *next is set to ASG_xxx_NULL
 *
 * Probe does not create a snapshot.
 */

typedef struct
{
   asg_container_id_t container_id;
   asg_size_t         seq_len;
   asg_size_t         object_count;  /* objects per container in the seq. */
} asg_container_info_t;

int asg_probe_system (
      struct asg_session ses,
      struct asg_id id,
      asg_container_info_t * buf,
      asg_size_t maxitems,
      asg_size_t * transferred,  /* number of container structures returned */
      asg_container_id_t * next);


int aasg_probe_system (
      struct asg_session ses,
      struct asg_id id,
      asg_container_info_t *buf,
      asg_size_t maxitems,
      asg_size_t *transferred,
      asg_container_id_t *next);

typedef struct
{
   asg_container_id_t object_id;
   asg_size_t         seq_len;
   asg_size_t         fork_count;
} asg_object_info_t;

int asg_probe_container (
      struct asg_session ses,
      struct asg_id id,
      asg_object_info_t * buf,
      asg_size_t maxitems,
      asg_size_t * transferred,
      asg_object_id_t * next);


int aasg_probe_container (
      struct asg_session ses,
      struct asg_id id,
      asg_object_info_t *buf,
      asg_size_t maxitems,
      asg_size_t *transferred,
      asg_object_id_t *next);

typedef struct
{
   asg_fork_id_t      fork_id;
   asg_size_t         seq_len;
   asg_size_t         record_count;
} asg_fork_info_t;

int asg_probe_object (
      struct asg_session ses,
      struct asg_id id,
      asg_fork_info_t * buf,
      asg_size_t maxitems,
      asg_size_t * transferred,
      asg_fork_id_t * next);

int aasg_probe_object (
      struct asg_session ses,
      struct asg_id id,
      asg_fork_info_t *buf,
      asg_size_t maxitems,
      asg_size_t *transferred,
      asg_fork_id_t *next);

int asg_probe_fork (
      struct asg_session ses,
      struct asg_id id,
      asg_record_info_t * buf,
      asg_size_t maxitems,
      asg_size_t * transferred,
      asg_record_id_t * next);

int aasg_probe_fork (
      struct asg_session ses,
      struct asg_id id,
      asg_record_info_t *buf,
      asg_size_t maxitems,
      asg_size_t *transferred,
      asg_record_id_t *next);

#endif
