
#include "gh.h"

# pragma warning (disable : 4113 4133 4047 )

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
#include <crtdbg.h>
#endif

struct student
{
	char firstName[20];
	char name[20];//hash key
	int age;
};

//  ======== name_match ========
qboolean name_match(const void *key, const void *sp)
{
	if ((key != NULL) && (sp != NULL)) {
		if (strcmp(key, ((struct student*)sp)->name) == 0)
			return qtrue;
	}
	return qfalse;
}


// ======== sym_delete ========
void sym_delete(void *value)
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
	struct student *found = (void*)0;
	struct student *found2 = (void*)0;
	struct gh_t_hash_tab *sym_tab;
	
	//Z chain init
	z_chain.next = z_chain.prev = &z_chain;		
	//init 
	
	sym_tab = gh_create(sizeof(struct student),
						    name_hash,
							name_match, 
							sym_delete);	
	//build
	build_ht(sym_tab);
	//find	
	if((found = (struct student *)gh_find(sym_tab, "s5")) == (void*)0)
		Sys_Error("s5 not found!");
	if((found2 = (struct student *)gh_find(sym_tab, "s10")) == (void*)0)
		Sys_Error("s10 not found!");
	//del
	gh_delete(sym_tab);
    
	Z_Stats_f();

	#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
#endif

	if((ch = getchar()) == EOF)
		Sys_Error("cound not stop");
	return 0;
}
