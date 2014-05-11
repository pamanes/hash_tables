#ifndef _Q_COMMON_H
#define _Q_COMMON_H


/*
	grabbed memory management from Quake 2 
	https://github.com/id-Software/Quake-2


*/


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <math.h>

#ifndef __BYTEBOOL__
#define __BYTEBOOL__
typedef enum { qfalse, qtrue } qboolean;
typedef unsigned char byte;
#endif

#define	MAXPRINTMSG	4096

#define	Z_MAGIC		0x1d1d

#define b_flush fflush(stdin)

//could also work with byte * for casting posx...
#define container_of(posx, type, member) \
	((char *)posx - offsetof(type, member))

typedef struct zhead_s
{
	struct zhead_s	*prev, *next;
	short	magic;
	short	tag;			// for group free
	int		size;
} zhead_t;


zhead_t		z_chain;
int		z_count, z_bytes;

void Z_Free (void *ptr);
void *Z_Malloc (int size);			// returns 0 filled memory
void *Z_TagMalloc (int size, int tag);
void Z_FreeTags (int tag);
void Z_Stats_f (void);

void	Sys_Error (char *error, ...);
void Com_Printf (char *fmt, ...);


//hashtable stuff
int ilog2(unsigned long v);
typedef unsigned int u32;
typedef unsigned char u8;
typedef uint64_t u64;

#endif
