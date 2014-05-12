#ifndef _GH_H
#define _GH_H

#include "qcommon.h"
#include "hash.h"
#include "hashtable.h"

struct gh_node 
{
	struct hlist_node hl;
	u8 data[0];
};


//size of your hash table , always make it a number of power of 2
// 8 : 2^8 = size 256
#define GH_HASH_ORDER 8

struct gh_t_hash_tab {
	u32 val_size;
	DECLARE_HASHTABLE(hash_table, GH_HASH_ORDER);
	u32 (*hash)(const void *key);
	qboolean (*match)(const void *key, const void *value);
	void (*deletex)(void *key);
};

/* symbol table hash functions */
u32 name_hash(const void *key);
qboolean name_match(const void *key, const void *sp);
void sym_delete(void *value);
struct gh_t_hash_tab *gh_create(u32 val_size, u32 (*hash)(const void *), qboolean (*match)(const void *, const void *), void (*deletex)(void *));
void *gh_find(struct gh_t_hash_tab *hash_tab, const void *key);
void *gh_insert(struct gh_t_hash_tab *hash_tab, const void *key, const void *value);
void gh_delete(struct gh_t_hash_tab *hash_tab);


#endif
