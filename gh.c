#include "gh.h"

/*
 *  ======== name_hash ========
 */
u32 name_hash(const void *key)
{
	u32 hash;
	const char *name = key;

	hash = 0;

	while (*name) 
	{
		hash <<= 1;
		hash ^= *name++;
	}

	return hash;
}

struct gh_t_hash_tab *gh_create(u32 val_size, u32 (*hash)(const void *),
				qboolean (*match)(const void *, const void *),
				void (*deletex)(void *))
{
	struct gh_t_hash_tab *hash_tab;

	//hash_tab = kzalloc(sizeof(struct gh_t_hash_tab), GFP_KERNEL);
	hash_tab = Z_Malloc(sizeof(struct gh_t_hash_tab));
	//check is done by Z_Malloc already
	//if (!hash_tab)
		//return ERR_PTR(-ENOMEM);

	hash_init(hash_tab->hash_table);	

	hash_tab->val_size = val_size;
	hash_tab->hash = hash;
	hash_tab->match = match;
	hash_tab->deletex = deletex;

	return hash_tab;
}

void *gh_find(struct gh_t_hash_tab *hash_tab, const void *key)
{
	struct gh_node *n;
   //struct hlist_node *n;
	u32 key_hash = hash_tab->hash(key);
/*
	#define hash_for_each_possible(name, obj, member, key, ftype, type)			\
	hlist_for_each_entry(obj, &name[hash_min(key, HASH_BITS(name))], member, ftype, type)

	#define hlist_for_each_entry(pos, head, member, ftype, type)				\
	for (pos = wcast_container_of((head)->first,ftype,type,member); \
	     pos;							\
	     pos = wcast_container_of((pos)->member.next, ftype, type, member))
*/
/*
	for
	(
			n = wcast_container_of((&hash_tab->hash_table[hash_min(key, HASH_BITS(hash_tab->hash_table))])->first, struct gh_node *,struct gh_node, hl);
			(n);
			n = wcast_container_of((n)->hl.next, struct gh_node *,struct gh_node, hl)
	)
	{


	}
*/

	hash_for_each_possible(hash_tab->hash_table, n, hl, key_hash, struct gh_node)
	{		
		if (hash_tab->match(key, n->data))
			return n->data;
	}

	return (void*)0;
	//return ERR_PTR(-ENODATA);
}

void *gh_insert(struct gh_t_hash_tab *hash_tab, const void *key,
		const void *value)
{
	struct gh_node *n;	
	n = Z_Malloc(sizeof(struct gh_node) + hash_tab->val_size);
	INIT_HLIST_NODE(&n->hl);
	hash_add(hash_tab->hash_table, &n->hl, hash_tab->hash(key));
	memcpy(n->data, value, hash_tab->val_size);
	return n->data;
}

/*
 *  ======== gh_delete ========
 */
void gh_delete(struct gh_t_hash_tab *hash_tab)
{
	struct gh_node *n;
	struct hlist_node *tmp;
	u32 i;

	if (hash_tab)
	{

		hash_for_each_safe(hash_tab->hash_table, i, tmp, n, struct gh_node, hl)
		{
			hash_del(&n->hl);
			if (hash_tab->deletex)
				hash_tab->deletex(n->data);
			Z_Free(n);
		}

		Z_Free(hash_tab);
	}
}
