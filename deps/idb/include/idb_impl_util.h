#ifndef __IDB_IMPL_UTIL_H__
#define __IDB_IMPL_UTIL_H__
static int
is_valid_interval(const struct idb_interval *iv)
{
	/* Satisfy rec + size <= max when... */

	/* rec > 0 */
	if (iv->rec >= 0)
		return iv->size <= (idb_size_t)(IDB_OFF_MAX - iv->rec);

	/* rec + size <= 0 */
	if ((idb_size_t)(iv->rec * -1) >= iv->size) /* End of interval < 0 */
		return 1;

	/* rec + size > 0 */
	return (idb_size_t)iv->rec + iv->size < (idb_size_t)IDB_OFF_MAX;
}

static int
does_front_survive(struct idb_interval *t, struct idb_interval *b)
{
	return t->rec < b->rec && t->rec + (idb_off_t)t->size > b->rec;
}

static void
preserve_front(struct idb_interval *t, struct idb_interval *b)
{
	/* The front of t survives, but the end does not. */
	/* t->size + t->rec = b->rec */
	assert(b->rec > t->rec);
	t->size = (idb_size_t)(b->rec - t->rec);
}

static int
does_end_survive(const struct idb_interval *t, const struct idb_interval *b)
{
	return t->rec < b->rec + (idb_off_t)b->size &&
		t->rec + (idb_off_t)t->size > b->rec + (idb_off_t)b->size;
}

static void
preserve_end(struct idb_interval *t, struct idb_interval *b)
{
	idb_size_t diff;

	diff = b->size + (idb_size_t)(b->rec - t->rec);
	t->log_offset += diff * t->reclen;
	t->rec += (idb_off_t)diff;
	t->size -= diff;

}

static int
is_covered(struct idb_interval *target, struct idb_interval *bound)
{

	if (target->rec >= bound->rec &&
	    target->rec + (idb_off_t)target->size <=
	    bound->rec + (idb_off_t)bound->size)
		return 1;
	return 0;
}

static int
does_abut(struct idb_interval *target, struct idb_interval *bound)
{
	if (target->rec > bound->rec)
		return does_abut(bound, target);
	return (idb_off_t)target->size + target->rec == bound->rec;
}

#endif
