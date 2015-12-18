#ifndef SSMD_H
#define SSMD_H

/*
 * This is a program file for the Nebula project, subproject SSM
 *
 * File author:       Shane Farmer, shanefarmer22@gmail.com
 * File maintainer:   Shane Farmer, shanefarmer22@gmail.com
 *
 * Prerelease status: only for distribution to Nebula project members for all
 * project-related purposes
 *
 * Part or all of this file/work may be copyrighted by the Regents of the
 * University of Alabama, and be subject to their standard disclaimer of
 * liability.  Such language will be added to the files before any public
 * release.
 */

// SSMD.H
//  Header file for the SSM Data Structure subsystem

#include <stdint.h>
#include <stddef.h>
#include <ssm.h>

/* File: ssmd.h
 *
 * SSM Data types API.
 *
 * In SSM, there are four primary data structures we need:
 *  - A single-reader single-writer sequential access queue for SSMT
 *  - A single-reader multi-writer queue for SSM commands (put, etc)
 *  - A walkable, many-reads few-writes list for match lists
 *  - An iovec to describe memory
 *
 *  We will call these pipes, funnels, lists, and iovecs, respectively.
 *
 *  All of these are to be optimized for their contract aggressively; detailed
 *  checking of preconditions will not be performed, and violation of intended
 *  use results in undefined behavior.
 */

/* Section: Callbacks
 *
 * Callbacks are a proc and an associated piece of data; the callback, when
 * fired, is provided with the callback data and a pointer to "event data".
 */

/* Function: ssmd_cb_create
 *
 * Creates a ssmd_cb.
 *
 * Parameters:
 *   proc - the ssmd_cb_proc to call on invocation.
 *   cb_data - the data pointer to be provided on each invocation.
 *   flags - Flags describing special behavior
 *
 * Return value:
 *   On success, the new ssmd_cb_id.
 *   On failure, SSM_CB_NONE.
 */
ssm_cb_id ssmd_cb_create(ssm_cb_proc proc, void *cb_data, ssm_cb_flags flags);

/* Function: ssmd_cb_chain
 *
 * Creates a ssmd_cb which consists of two ssmd_cbs in succesion.
 *
 * Parameters:
 *   a - The first ssmd_cb to trigger.
 *   b - The second ssmd_cb to trigger.
 *   flags - Flags describing special behavior
 *   chain_flags - Flags describing special chaining behavior
 *
 * Return value:
 *   On success, the new ssmd_cb_id.
 *   On failure, SSM_CB_NONE.
 */
ssm_cb_id ssmd_cb_chain(ssm_cb_id a, ssm_cb_id b, ssm_cb_flags flags, ssm_cb_flags chain_flags);

/* Function: ssmd_cb_data
 *
 * Retrieves the data pointer associated with an ssmd_cb.
 *
 * Parameters:
 *   cb_id - the cb to retrieve the data of.
 *
 * Return value:
 *   The data pointer associated with the cb.
 */
void *ssmd_cb_data(ssm_cb_id cb_id);

/* Function: ssmd_cb_destroy
 *
 * Destroys an ssmd_cb.
 *
 * Parameters:
 *   cb_id - the ssmd_cb to destroy.
 *
 * Return value:
 *   On success, 0.  On failuire, nonzero.
 */
int ssmd_cb_destroy(ssm_cb_id cb_id);

/* Function: ssmd_cb_invoke
 *
 * Invokes an ssmd_cb with supplied event data.
 * 
 * Parameters:
 *   cb_id - The callback to invoke.
 *   ev_data - The event data to supply.
 *
 * Return value:
 *   None.
 */
void ssmd_cb_invoke(ssm_cb_id cb_id, void *ev_data);

/* Section: Pipes
 *
 * Pipes are non-searchable FIFO queues that are safe for (and optimized for)
 * exactly one getting thread and one putting thread.  An initial size
 * suggestion is provided when created, and (unless otherwise specified) the
 * pipe will grow when necessary.
 */

/* Types: SSMD Pipe types
 * 
 * ssmd_pipe_id    - A handle for a pipe, used to interact with it.
 * ssmd_pipe_flags - Flags associated with a pipe.
 */
typedef void    *ssmd_pipe_id; 
typedef uint16_t ssmd_pipe_flags;

/* Flags: SSMD Pipe flags
 * 
 * SSMD_PIPE_NO_FLAGS - Constant (not bitwise) indicating all flags disabled.
 * SSMD_PIPE_FIXED    - Flag indicating the pipe must not grow when full.
 *                      When set, puts will fail when the pipe is full unless
 *                      SSMD_PIPE_BLOCKING is set.
 * SSMD_PIPE_BLOCKING - Indicates that if a pipe is full and SSMD_PIPE_FIXED
 *                      is set, instead of failing a put should block until
 *                      space is available.
 */
#define SSMD_PIPE_NO_FLAGS 0
#define SSMD_PIPE_FIXED 0x0001
#define SSMD_PIPE_BLOCKING 0x0002

/* Function: ssmd_pipe_create
 *
 * Creates a pipe. 
 * 
 * Thread safety:  N/A 
 *
 * Parameters:
 *   initial_size - Suggests an initial size for the queue.  The
 *                  implementation is not constrained by this value.
 *   flags        - Flags for creation.
 *
 * Returns:
 *   On success, a valid ssmd_pipe_id for the new pipe.
 *   On failure, NULL.
 */
extern ssmd_pipe_id  ssmd_pipe_create(size_t initial_size, ssmd_pipe_flags flags);

/* Function: ssmd_pipe_destroy
 *
 * Destroys a pipe. When the call returns, all associated resources have been
 * freed and the ID is no longer valid.
 *
 * Thread safety:  NOT THREAD SAFE.  Threads besides the caller must not
 * use the associated handle from the point the call begins.
 *
 * Parameters:
 *   pipe_id -  The ID of the pipe to destroy.
 *
 * Returns:
 *   On success, 0.  On failure, nonzero.
 */
extern int           ssmd_pipe_destroy(ssmd_pipe_id pipe_id);

/* Function: ssmd_pipe_peek
 *
 * "Peeks" into a pipe, returning the next entry to be dequeued, or NULL if
 * nothing is available.
 *
 * Thread safety:  ssmd_pipe_peek and ssmd_pipe_get are thread safe for a
 * maximum of one thread and only while other thread safety constraints are
 * met.
 *
 * Parameters:
 *   pipe_id - The ID of the pipe to peek.
 *
 * Returns:
 *   On success, if the pipe has an enqueued pointer, that pointer.
 *   On success, if the pipe is empty, NULL.
 *   On failure, NULL.
 */
extern void * ssmd_pipe_peek(ssmd_pipe_id pipe_id);

/* Function: ssmd_pipe_get
 *
 * Dequeues the next item from the pipe and returns it.  If nothing is
 * available, blocks until something exists to be dequeued.  (Use
 * ssmd_pipe_peek to check for available items before callign ssmd_pipe_get to
 * avoid this behavior.)
 *
 * Thread safety:  ssmd_pipe_peek and ssmd_pipe_get are thread safe for a
 * maximum of one thread and only while other thread safety constraints are
 * met.
 *
 * Parameters:
 *   pipe_id - The ID of the pipe to dequeue from.
 *
 * Returns:
 *   On success, the dequeued pointer.
 *   On failure, NULL.
 */
extern void * ssmd_pipe_get(ssmd_pipe_id pipe_id);

/* Function: ssmd_pipe_put
 *
 * Enqueues a pointer into the pipe.  If the pipe is full or has met
 * implementation-specific size constraints, the pipe will attempt to grow.
 * If the pipe cannot grow or SSMD_PIPE_FIXED is set, the call will fail
 * unless SSMD_PIPE_BLOCKING is set, in which case it will block until space
 * is available.
 *
 * Thread safety:  ssmd_pipe_put is thread safe for a maximum of one thread
 * and only while other thread safety constraints are met.
 *
 * Parameters:
 *   pipe_id - The ID of the pipe to enqueue into.
 *   data    - The pointer to enqueue.
 *
 * Returns:
 *   On success, 0.
 *   On failure, nonzero.
 */
extern int    ssmd_pipe_put(ssmd_pipe_id pipe_id, void * data);


/* Section: Funnels
 *
 * Funnels are similar in concept to Pipes, in that they are non-searchable
 * FIFO queues.  However, Funnels carry an additional implementation
 * constraint that they be thread-safe for multiple writers (the single-reader
 * constraint is maintained).  For this reason, Funnels are likely to be no
 * faster and possibly slower than pipes; they should be used only when
 * multiple threads might write concurrently to a single queue. 
 */

/* Types: SSMD Funnel types
 *
 * ssmd_funnel_id - A handle to a Funnel
 * ssmd_funnel_flags - Flags controlling Funnel behavior
 */
typedef void * ssmd_funnel_id;
typedef uint16_t    ssmd_funnel_flags;

/* Flags: SSMD Funnel flags
 *
 * SSMD_FUNNEL_NO_FLAGS - a constant (not bitwise) indicating all flags absent
 * SSMD_FUNNEL_FIXED    - indicates the funnel should not grow when full
 * SSMD_FUNNEL_BLOCKING - indicates that when SSMD_FUNNEL_FIXED is set and the
 *                        funnel is full, instaed of failing, put calls should
 *                        block until room is available.
 */
#define SSMD_FUNNEL_NO_FLAGS 0
#define SSMD_FUNNEL_FIXED 0x0001
#define SSMD_FUNNEL_BLOCKING 0x0002

/* Function: ssmd_funnel_create
 *
 * Creates a funnel.  An initial size is suggested to the implementation,
 * although it is not required to be respected or even meaningful.
 *
 * Thread safety:  N/A
 *
 * Parameters:
 *   initial_size:  Suggestion for initial size of the funnel.  Specific
 *                  meaning (beyond likely upper bound of number of
 *                  simultaneous elements) and relevance is
 *                  implementation-dependent.
 *   flags:         Flags controlling funnel behavior.
 *
 * Returns:
 *   On success, a valid funnel ID for the new handle.
 *   On failure, NULL.
 */
ssmd_funnel_id ssmd_funnel_create(size_t initial_size, ssmd_funnel_flags flags);

/* Function: ssmd_funnel_destroy
 *
 * Destroys a funnel.  When the call returns, all associated resources have
 * been freed and the ID is no longer valid.
 *
 * Thread safety:  NOT THREAD SAFE.  Threads besides the caller must not
 * use the associated handle from the point the call begins.
 *
 * Parameters:
 *   funnel_id -  The ID of the funnel to destroy.
 *
 * Returns:
 *   On success, 0.  On failure, nonzero.
 */
int            ssmd_funnel_destroy(ssmd_funnel_id funnel_id);

/* Function: ssmd_funnel_peek
 *
 * "Peeks" into a funnel, returning the next entry to be dequeued, or NULL if
 * nothing is available.
 *
 * Thread safety:  ssmd_funnel_peek and ssmd_funnel_get are thread safe for a
 * maximum of one thread and only while other thread safety constraints are
 * met.
 *
 * Parameters:
 *   funnel_id - The ID of the funnel to peek.
 *
 * Returns:
 *   On success, if the funnel has an enqueued pointer, that pointer.
 *   On success, if the funnel is empty, NULL.
 *   On failure, NULL.
 */
extern void *  ssmd_funnel_peek(ssmd_funnel_id funnel);

/* Function: ssmd_funnel_get
 *
 * Dequeues the next item from the funnel and returns it.  If nothing is
 * available, blocks until something exists to be dequeued.  (Use
 * ssmd_funnel_peek to check for available items before calling ssmd_funnel_get to
 * avoid this behavior.)
 *
 * Thread safety:  ssmd_funnel_peek and ssmd_funnel_get are thread safe for a
 * maximum of one thread and only while other thread safety constraints are
 * met.
 *
 * Parameters:
 *   funnel_id - The ID of the funnel to dequeue from.
 *
 * Returns:
 *   On success, the dequeued pointer.
 *   On failure, NULL.
 */
extern void *  ssmd_funnel_get(ssmd_funnel_id funnel);

/* Function: ssmd_funnel_put
 *
 * Enqueues a pointer into the funnel.  If the funnel is full or has met
 * implementation-specific size constraints, the funnel will attempt to grow.
 * If the funnel cannot grow or SSMD_PIPE_FIXED is set, the call will fail
 * unless SSMD_PIPE_BLOCKING is set, in which case it will block until space
 * is available.
 *
 * Thread safety:  ssmd_funnel_put is thread safe for any number of threads as
 * long as other thread safety consraints are met.
 *
 * Parameters:
 *   funnel_id - The ID of the funnel to enqueue into.
 *   data    - The pointer to enqueue.
 *
 * Returns:
 *   On success, 0.
 *   On failure, nonzero.
 */
extern int     ssmd_funnel_put(ssmd_funnel_id funnel, void * data);

/* Section:  Lists
 * 
 * Lists are thread-safe linked lists searchable in a single direction and
 * optimized for common reads and infrequent writes.  Resources are allocated
 * dynamically.
 *
 * Searching lists is accomplished by initiating a 'walk' (a one-way
 * traversal) which must be explicitly stopped when the walk is complete.
 */

/* Types: SSMD List types
 *
 * ssmd_list_id       - ID for an SSMD List
 * ssmd_list_walk_id  - ID for an active SSMD List Walk
 * ssmd_list_flags    - Flags controlling list options
 */
typedef void * ssmd_list_id;
typedef void * ssmd_list_walk_id;
typedef uint16_t ssmd_list_flags;

/* Constants: SSMD List flags
 *
 * SSMD_LIST_NO_FLAGS - A constant (not bitwise) indicating all flags absent
 * SSMD_LIST_ONLY_DESTROY_EMPTY - Indicates attempting to destroy the list
 *                                while it is non-empty is an error condition
 *                                and should fail.
 */
#define SSMD_LIST_NO_FLAGS 0
#define SSMD_LIST_ONLY_DESTROY_EMPTY 0x0001

/* Function: ssmd_list_create
 *
 * Creates a list and returns the associated ID.  The list is initially empty.
 * 
 * Thread safety:  N/A
 *
 * Parameters:
 *   flags - Flags controlling list options.
 *
 * Returns:
 *   On success, the ID of the new list.
 *   On failure, NULL.
 */
extern ssmd_list_id ssmd_list_create(ssmd_list_flags flags);

/* Function: ssmd_list_destroy
 *
 * Destroys a list.  If SSMD_LIST_ONLY_DESTROY_EMPTY is set, fails when the
 * list is not already empty.  Otherwise, each element is removed prior to
 * destroying the list.
 *
 * Thread safety:  NOT THREAD SAFE.  Threads besides the caller must not use
 * the associated handle from the point the call begins.
 *
 * Parameters:
 *   list_id - The ID of the list to destroy.
 *
 * Returns:
 *   On success, 0.
 *   On failure, nonzero.
 */
extern int          ssmd_list_destroy(ssmd_list_id list_id);

/* Function: ssmd_list_walk_start
 *
 * Initiates a list walk and returns the associated ID.  The first call to
 * ssmd_list_walk_next on this ID will return the first element in the list.
 *
 * Thread safety:  fully thread safe for any number of concurrent walks. Note
 * that advancing the walk is not thread safe; if multiple threads need to
 * search the list they must instanstantiate separate walks.  A thread
 * initiating a walk while a walk is active for that thread results in
 * undefined behavior (likely a deadlock).
 *
 * Parameters:
 *   list_id - The ID of the list to initiate a walk upon.
 *
 * Returns:
 *   On success, the ID of the newly-initialized walk bound to list_id.
 *   On failure, NULL.
 */
extern ssmd_list_walk_id ssmd_list_walk_start(ssmd_list_id list_id);

/* Function: ssmd_list_walk_next
 *
 * Advances the walk of a list, returning the next item.
 *
 * Thread safety:  NOT THREAD SAFE; if multiple threads need to search a list,
 * they must each initiate a walk.
 *
 * Parameters:
 *   walk_id - The ID of the walk to advance, previously acquired from
 *             ssmd_list_walk_start.
 *
 * Returns:
 *   If the walk has not reached the end of the list, the next pointer in the
 *   list.
 *   If the walk has reached the end of the list, NULL.
 *   If an error has occurred, NULL.
 */
extern void *            ssmd_list_walk_next(ssmd_list_walk_id walk_id);

/* Function: ssmd_list_walk_this
 *
 * Returns the last item advanced to by a walk (that is, the last item
 * returned by ssmd_list_walk_next).
 *
 * It is an error to call this method on a walk that has been initialized and
 * has not had ssmd_list_walk_next called on it.
 *
 * Thread safety:  NOT THREAD SAFE.  If multiple threads wish to search the
 * list they must instanstantiate one walk per thread.
 *
 * Parameters:
 *   walk_id - The ID of the walk to query, previously acquired from
 *             ssmd_list_walk_start.
 *
 * Returns:
 *   If the walk and associated list are still valid, returns the pointer last
 *   returned by ssmd_list_walk_next.
 *   If an error has occurred, NULL.
 */
extern void *            ssmd_list_walk_this(ssmd_list_walk_id walk_id);

/* Function: ssmd_list_walk_stop
 *
 * Stops a walk.  After the call, the walk ID is no longer valid.
 *
 * Thread safety:  NOT THREAD SAFE.  No more than one thread is allowed per
 * walk.
 *
 * Parameters:
 *   walk_id - The ID of the walk to stop. 
 *
 * Returns:
 *   On success, 0.
 *   On failure, nonzero.
 */ 
extern int               ssmd_list_walk_stop(ssmd_list_walk_id walk_id);

/* Function: ssmd_list_entry_exists
 *
 * Searches a list for an item and returns nonzero if found.  This function
 * is an optimization opportunity for the implementation and is not meant to
 * provide functionality that could not be duplicated by executing a walk
 * (indeed, it may be implemented thus).
 *
 * Thread safety:  fully thread safe.
 *
 * Parameters:
 *   list_id - the ID of the list to check.
 *   data - the item to search for.
 *
 * Returns:
 *   If the item is found, nonzero.
 *   If the item is not found, 0.
 *   On error, 0 (to avoid side effects).
 */
extern int  ssmd_list_entry_exists(ssmd_list_id list_id, void * data);

/* Function:  ssmd_list_insert_at_head
 *
 * Inserts an item into the list.  The item is placed at the beginning of the
 * list.  Inserting a duplicate item results in undefined behavior.
 *
 * Thread safety:  fully thread safe.
 *
 * Parameters:
 *   list_id  - The ID of the list to insert into.
 *   data     - the item to insert.
 *
 * Returns:
 *   On success, 0.
 *   On failure, nonzero.
 */
extern int  ssmd_list_insert_at_head(ssmd_list_id list_id, void * data);

/* Function:  ssmd_list_insert_at_tail
 *
 * Inserts an item into the list.  The item is placed at the end of the
 * list.  Inserting duplicate items results in undefined behavior.
 *
 * Thread safety:  fully thread safe.
 *
 * Parameters:
 *   list_id  - The ID of the list to insert into.
 *   data     - the item to insert.
 *
 * Returns:
 *   On success, 0.
 *   On failure, nonzero.
 */
extern int  ssmd_list_insert_at_tail(ssmd_list_id list_id, void * data);

/* Function:  ssmd_list_insert_before
 *
 * Inserts an item into the list.  The item is placed before the specified
 * anchor element.  If the anchor cannot be found, the item is placed instead
 * at the end of the list.  Inserting duplicate items results in undefined
 * behavior.
 *
 * Thread safety:  fully thread safe.
 *
 * Parameters:
 *   list_id  - The ID of the list to insert into.
 *   anchor   - the item that the new item is placed before.
 *   data     - the item to insert.
 *
 * Returns:
 *   On success, 0.
 *   On failure, nonzero.
 */
extern int  ssmd_list_insert_before(ssmd_list_id list_id, void * anchor, void * data);

/* Function:  ssmd_list_insert_after
 *
 * Inserts an item into the list.  The item is placed after the specified
 * anchor element.  If the anchor cannot be found, the item is instead placed
 * at the end of the list.  Inserting duplicate items results in undefined
 * behavior.
 *
 * Thread safety:  fully thread safe.
 *
 * Parameters:
 *   list_id  - The ID of the list to insert into.
 *   anchor   - The item to place the new item after.
 *   data     - the item to insert.
 *
 * Returns:
 *   On success, 0.
 *   On failure, nonzero.
 */
extern int  ssmd_list_insert_after(ssmd_list_id list_id, void * anchor, void *data);

/* Function:  ssmd_list_remove
 *
 * Removes an item from the list.
 *
 * Thread safety:  fully thread safe.
 *
 * Parameters:
 *   list_id - The ID of the list to remove the item from
 *   data    - The item to remove
 *
 * Returns:
 *   On success, 0.
 *   If the item does not exist in the list, > 0.
 *   On failure, < 0.
 */
extern int  ssmd_list_remove(ssmd_list_id list_id, void * data);

/* Section:  Map
 *
 * A keymap.  Still in heavy development.
 * Apologies for sparse documentation.
 */

typedef void * ssmd_map_id;
#define SSMD_MAP_NONE ((ssmd_map_id)NULL)

extern ssmd_map_id ssmd_map_create(size_t key_bytes);
extern int ssmd_map_destroy(ssmd_map_id id);
extern int ssmd_map_put(ssmd_map_id id, void *key, void *ptr);
extern void *ssmd_map_get(ssmd_map_id id, void *key);
extern void *ssmd_map_drop(ssmd_map_id id, void *key);

/* Section:  Cache
 * 
 * A keyed cache.
 * Still in heavy development.  Apologies for sparse documentation.
 *
 */
typedef void * ssmd_cache_id;
#define SSMD_CACHE_NONE ((ssmd_cache_id)NULL)


/* Function:  ssmd_cache_create
 *
 * Creates a cache.
 *
 * Thread safety:  fully thread safe.
 *
 * Parameters:
 *   depth  - Items held before discarding begins
 *   key_bytes - Bytes in the key for each value
 *   
 * Returns:
 *   On success, the id of the new cache.  On failure, SSMD_CACHE_NONE.
 */
extern ssmd_cache_id ssmd_cache_create(size_t depth, size_t key_bytes);

/* Function:  ssmd_cache_destroy
 *
 * Destroys a cache.  Calls the destroy cb on each element.
 *
 * Thread safety:  NOT THREAD SAFE.
 *
 * Parameters:
 *   id - ID of the cache to destroy.
 *
 * Returns:
 *   On success, 0.  On failure, nonzero.
 */
extern int ssmd_cache_destroy(ssmd_cache_id id);

/* Function: ssmd_cache_put
 *
 * Puts something into the cache if key is new.  Oldest item discarded if needed.
 *
 * Thread safety:  Thread safe (locking).
 *
 * Parameters:
 *   id - Cache ID
 *   key - ptr to key data
 *   ptr - Ptr to store (and retrieve)
 *   destroy_cb - CB for when something drops out of the cache; called with key and ptr
 *
 * Return value:
 *   On success, 0.  On failure, < 0.
 */
extern int ssmd_cache_put(ssmd_cache_id id, void *key, void *ptr, ssm_cb_id destroy_cb);

/* Function: ssmd_cache_get
 *
 * Returns a value from the cache or NULL if not there.
 *
 * Thread safety:  Thread safe (locking).
 *
 * Parameters:
 *   id - ID of the cache
 *   key - Key to look up
 *
 * Return value:
 *   If found, the ptr that was stored.
 *   If not found, NULL.
 */
extern void *ssmd_cache_get(ssmd_cache_id id, void *key);




/* Macro: SSMD_DEEP_DESTROY_PIPE
 *
 * Calls SSM_DELETE on each pointer in a pipe, then calls ssmd_pipe_destroy.
 * With this macro, the return value is lost; however, unless a serious error
 * occurs, it should usually succeed.
 *
 * Parameters:
 *  ssmdddpipe - The pipe ID to deep destroy.  WARNING: Not type checked.
 *
 * Return value:
 *   None (is a macro).
 */
#define SSMD_PIPE_DEEP_DESTROY(ssmdddpipe) \
  do{void *ssmddd_ptr;\
     while(ssmd_pipe_peek(ssmdddpipe) != NULL) { \
       ssmddd_ptr = ssmd_pipe_get(ssmdddpipe);  \
       SSM_DELETE(ssmddd_ptr); }                 \
     ssmd_pipe_destroy(ssmdddpipe); } while(0)

/* Macro: SSMD_DEEP_DESTROY_FUNNEL
 *
 * Calls SSM_DELETE on each pointer in a funnel, then calls ssmd_funnel_destroy.
 * With this macro, the return value is lost; however, unless a serious error
 * occurs, it should usually succeed.
 *
 * Parameters:
 *  ssmdddfunnel - The funnel ID to deep destroy.  WARNING: Not type checked.
 *
 * Return value:
 *   None (is a macro).
 */
#define SSMD_FUNNEL_DEEP_DESTROY(ssmdddfunnel) \
  do{void *ssmddd_ptr;\
     while(ssmd_pipe_peek(ssmdddfunnel) != NULL) { \
       ssmddd_ptr = ssmd_funnel_get(ssmdddfunnel);  \
       SSM_DELETE(ssmddd_ptr); }                 \
     ssmd_funnel_destroy(ssmdddfunnel); } while(0)


#endif
