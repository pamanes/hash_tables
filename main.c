#include "qcommon.h"
#include "list.h"
#include "hashtable.h"

#	pragma warning (disable : 4113 4133 4047 )

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
#include <crtdbg.h>
#endif
/*
struct debug_bucket 
{
	struct hlist_head	list;
	char *key;
	char *value;
};

static struct debug_bucket	obj_hash[100];
*/
struct ht //hash table
{
       struct hlist_node   node;
       //void            *object;   
	   byte *object;
};

struct student
{
	char firstName[20];
	char name[20];
	int age;
};

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
static u32 name_hash(const void *key);
static qboolean name_match(const void *key, const void *sp);
static void sym_delete(void *value);

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
	u32 key_hash = hash_tab->hash(key);
	
	hash_for_each_possible(hash_tab->hash_table, n, hl, key_hash)
	{		
		if (hash_tab->match(key, n->data))
			return n->data;
	}
	
	return (void*)0;
	//return ERR_PTR(-ENODATA);
}

/*
 *  ======== gh_insert ========
 */

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

/*
 *  ======== name_hash ========
 */
static u32 name_hash(const void *key)
{
	u32 hash;
	const char *name = key;

	hash = 0;

	while (*name) {
		hash <<= 1;
		hash ^= *name++;
	}

	return hash;
}

/*
 *  ======== name_match ========
 */
static qboolean name_match(const void *key, const void *sp)
{
	if ((key != NULL) && (sp != NULL)) {
		if (strcmp(key, ((struct student*)sp)->name) == 0)
			return qtrue;
	}
	return qfalse;
}

/*
 *  ======== sym_delete ========
 */
static void sym_delete(void *value)
{
	//in case we need to delete stuff within the struct	
	//struct student *s = (struct student*)value;
	//Z_Free(s->whatever);
}

void build_ht(struct gh_t_hash_tab *sym_tab)
{
	int i = 0;
	struct student s;
	
	for(;i<255;i++)
	{
		sprintf(s.name, "s%d", i);
		sprintf(s.firstName, "alex %d", i);
		s.age = i;
		gh_insert(sym_tab, (void*)&s.name, (void*)&s);	
	}
}

int main()
{	
	//struct hlist_head name[1 << 8];
	int ch = 0;
	struct student s;
	struct student *found = (void*)0;
	struct student *found2 = (void*)0;
	struct gh_t_hash_tab *sym_tab;
	
	//Z chain init
	z_chain.next = z_chain.prev = &z_chain;		
	//init 
	sym_tab = gh_create(sizeof(struct student),
						    name_hash,
							name_match, sym_delete);	
	//build
	build_ht(sym_tab);
	//find	
	found = (struct student *)gh_find(sym_tab, "s5");
	found2 = (struct student *)gh_find(sym_tab, "s10");
	
	//add a new one, replacing s5
	s.age = 31;
	strcpy(s.firstName, "Alex");
	strcpy(s.name, "s5");
	gh_insert(sym_tab, (void*)&s.name, (void*)&s);

	found = (struct student *)gh_find(sym_tab, "s5");
	//delete
	gh_delete(sym_tab);
	Z_Stats_f();

	#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
#endif

	ch = getchar();

	/*
	struct hlist_head obj_pool;
	struct ht *newx, *tmp;
	struct hlist_node *pos, *q;
	int i = 0;
	int ch = 0;
	char *text;	

	//INIT_HLIST_NODE(&(obj_pool.first));
	INIT_HLIST_HEAD(&(obj_pool));
	
	//Z chain init
	z_chain.next = z_chain.prev = &z_chain;	

	for(;i<1;i++)
	{
		newx = Z_Malloc(sizeof(struct ht));	
		text = Z_Malloc(6);
		strcat(text, "hello");
		newx->object = text;
		hlist_add_head(&newx->node, &obj_pool);
	}	

	hlist_for_each(pos, &obj_pool)
	{
		tmp = container_of(pos, struct ht, node);
		printf("text is %s", (char*)tmp->object);
	}
	
	if(!hlist_empty((&obj_pool)))
	{
		hlist_for_each_safe(pos, q, &obj_pool)
		{
			tmp = container_of(pos, struct ht, node);
			printf("del text is %d", tmp->object);
			hlist_del(&tmp->node);
			//Z_Free(tmp->object);
			Z_Free(tmp->object);
			Z_Free(tmp);
		}
	}
	
	#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
#endif
	Com_Printf("done.");
	ch = getchar();
	
	*/
	return 0;
}
