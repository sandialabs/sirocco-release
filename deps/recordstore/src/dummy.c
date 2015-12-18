/*
 * (C) 2014 The University of Chicago
 *
 * See COPYRIGHT in top-level directory.
 */

/* NOTE: this code isn't really used.  This is just a hack; if automake sees
 * at least one .c file in the source then it will use the C linker by
 * default, which is what we want.
 */
int __foo_dummy_fn(void);
int __foo_dummy_fn(void)
{
    return(0);
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ft=c ts=8 sts=4 sw=4 expandtab
 */
