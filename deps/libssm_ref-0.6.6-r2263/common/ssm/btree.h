#ifndef __SSM_BTREE_H
#define __SSM_BTREE_H

#include <ssm/if.h>

SSM_PROC(int, btreecmp)(void *arg, void *left, void *right);

extern int ssm_btreecmp_Haddr(void *addr, void *left, void *right);

extern ssm_Iaa ssm_btree_aa(ssm_Pbtreecmp cmp, void * cmparg);

#endif
