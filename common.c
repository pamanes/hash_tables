/*
	grabbed memory management from Quake 2
	https://github.com/id-Software/Quake-2
*/

#include "qcommon.h"

/*
========================
Z_Free
========================
*/
void Z_Free (void *ptr)
{
	zhead_t	*z;

	z = ((zhead_t *)ptr) - 1;

	if (z->magic != Z_MAGIC)
		fprintf(stderr, "Z_Free: bad magic");

	z->prev->next = z->next;
	z->next->prev = z->prev;

	z_count--;
	z_bytes -= z->size;
	free (z);
}

/*
========================
Z_Stats_f
========================
*/
void Z_Stats_f (void)
{
	printf("%i bytes in %i blocks\n", z_bytes, z_count);
}

/*
========================
Z_FreeTags
========================
*/
void Z_FreeTags (int tag)
{
	zhead_t	*z, *next;

	for (z=z_chain.next ; z != &z_chain ; z=next)
	{
		next = z->next;
		if (z->tag == tag)
			Z_Free ((void *)(z+1));
	}
}

/*
========================
Z_TagMalloc
========================
*/
void *Z_TagMalloc (int size, int tag)
{
	zhead_t	*z;	
	size = size + sizeof(zhead_t);
	z = malloc(size);

	if(z)
	{
		memset (z, 0, size);
		z_count++;
		z_bytes += size;
		z->magic = Z_MAGIC;
		z->tag = tag;
		z->size = size;

		z->next = z_chain.next;
		z->prev = &z_chain;
		z_chain.next->prev = z;
		z_chain.next = z;
	}
	else
		Sys_Error("Z_Malloc: failed on allocation of %i bytes",size);

	return (void *)(z+1);
}

/*
========================
Z_Malloc
========================
*/
void *Z_Malloc (int size)
{
	return Z_TagMalloc (size, 0);
}


int ilog2(unsigned long v)
{
        int l = 0;
        while ((1UL << l) < v)
                l++;
        return l;
}
