#ifndef _LINUX_LIST_H
#define _LINUX_LIST_H

/*
	Adapted it from list.h from the linux kernel
	http://www.cs.fsu.edu/~baker/devices/lxr/http/source/linux/include/linux/list.h
	Here's an explanation of how it works
	http://isis.poly.edu/kulesh/stuff/src/klist/

*/

struct list_head
{
	struct list_head *next,*prev;
};
/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
*/
static __inline void __list_add(struct list_head *newx, struct list_head *prev, struct list_head *next)
{
	next->prev = newx;
	newx->next = next;
	newx->prev = prev;
	prev->next = newx;
}

/**
 * list_add - add a new entry
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
*/
static __inline void list_add(struct list_head *newx, struct list_head *head)
{
	__list_add(newx, head, head->next);
}

/**
 * list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
static __inline void list_add_tail(struct list_head *newx, struct list_head *head)
{
	__list_add(newx, head->prev, head);
}

static __inline void INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}
 /*
  * Delete a list entry by making the prev/next entries
  * point to each other.
  *
  * This is only for internal list manipulation where we know
  * the prev/next entries already!
 */
static __inline void list_del(struct list_head *prev, struct list_head *next)
{
	next->prev = prev;
	prev->next = next;
}

/**
 * list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: list_empty() on entry does not return true after this, the entry is
 * in an undefined state.
 */
static __inline void __list_del_entry(struct list_head *entry)
{
	list_del(entry->prev, entry->next);
}


/**
 * list_replace - replace old entry by new one
 * @old : the element to be replaced
 * @new : the new element to insert
 *
 * If @old was empty, it will be overwritten.
 */
static __inline void list_replace(struct list_head *old,
				struct list_head *newx)
{
	newx->next = old->next;
	newx->next->prev = newx;
	newx->prev = old->prev;
	newx->prev->next = newx;
}

static __inline void list_replace_init(struct list_head *old,
					struct list_head *newx)
{
	list_replace(old, newx);
	INIT_LIST_HEAD(old);
}

/**
 * list_del_init - deletes entry from list and reinitialize it.
 * @entry: the element to delete from the list.
 */
static __inline void list_del_init(struct list_head *entry)
{
	__list_del_entry(entry);
	INIT_LIST_HEAD(entry);
}

/**
* list_move - delete from one list and add as another's head
* @list: the entry to move
* @head: the head that will precede our entry
*/
static __inline void list_move(struct list_head *list, struct list_head *head)
{
        list_del(list->prev, list->next);
        list_add(list, head);
}
/**
 * list_move_tail - delete from one list and add as another's tail
 * @list: the entry to move
 * @head: the head that will follow our entry
 */
static __inline void list_move_tail(struct list_head *list,
                                  struct list_head *head)
{
        list_del(list->prev, list->next);
        list_add_tail(list, head);
}

/**
 * list_is_last - tests whether @list is the last entry in list @head
 * @list: the entry to test
 * @head: the head of the list
 */
static __inline int list_is_last(const struct list_head *list,
                                const struct list_head *head)
{
        return list->next == head;
}

/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
static __inline int list_empty(const struct list_head *head)
{
        return head->next == head;
}

/**
 * list_empty_careful - tests whether a list is empty and not being modified
 * @head: the list to test
 *
 * Description:
 * tests whether a list is empty _and_ checks that no other CPU might be
 * in the process of modifying either member (next or prev)
 *
 * NOTE: using list_empty_careful() without synchronization
 * can only be safe if the only activity that can happen
 * to the list entry is list_del_init(). Eg. it cannot be used
 * if another CPU could re-list_add() it.
 */
static __inline int list_empty_careful(const struct list_head *head)
{
	struct list_head *next = head->next;
	return (next == head) && (next == head->prev);
}

/**
 * list_rotate_left - rotate the list to the left
 * @head: the head of the list
 */
static __inline void list_rotate_left(struct list_head *head)
{
	struct list_head *first;

	if (!list_empty(head)) 
	{
		first = head->next;
		list_move_tail(first, head);
	}
}

/**
 * list_is_singular - tests whether a list has just one entry.
 * @head: the list to test.
 */
static __inline int list_is_singular(const struct list_head *head)
{
        return !list_empty(head) && (head->next == head->prev);
}

static __inline void __list_cut_position(struct list_head *list,
                struct list_head *head, struct list_head *entry)
{
        struct list_head *new_first = entry->next;
        list->next = head->next;
        list->next->prev = list;
        list->prev = entry;
        entry->next = list;
        head->next = new_first;
        new_first->prev = head;
}

/**
 * list_cut_position - cut a list into two
 * @list: a new list to add all removed entries
 * @head: a list with entries
 * @entry: an entry within head, could be the head itself
 *      and if so we won't cut the list
 *
 * This helper moves the initial part of @head, up to and
 * including @entry, from @head to @list. You should
 * pass on @entry an element you know is on @head. @list
 * should be an empty list or a list you do not care about
 * losing its data.
 *
 */
static __inline void list_cut_position(struct list_head *list,
                struct list_head *head, struct list_head *entry)
{
        if (list_empty(head))
                return;
        if (list_is_singular(head) &&
                (head->next != entry && head != entry))
                return;
        if (entry == head)
                INIT_LIST_HEAD(list);
        else
                __list_cut_position(list, head, entry);
}

static __inline void __list_splice(const struct list_head *list,
                                 struct list_head *prev,
                                 struct list_head *next)
{
        struct list_head *first = list->next;
        struct list_head *last = list->prev;

        first->prev = prev;
        prev->next = first;

        last->next = next;
        next->prev = last;
}

/**
 * list_splice - join two lists, this is designed for stacks
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static __inline void list_splice(const struct list_head *list,
                                struct list_head *head)
{
        if (!list_empty(list))
                __list_splice(list, head, head->next);
}

/**
 * list_splice_tail - join two lists, each list being a queue
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static __inline void list_splice_tail(struct list_head *list,
                                struct list_head *head)
{
        if (!list_empty(list))
                __list_splice(list, head->prev, head);
}

/**
 * list_splice_init - join two lists and reinitialise the emptied list.
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * The list at @list is reinitialised
 */
static __inline void list_splice_init(struct list_head *list,
                                    struct list_head *head)
{
        if (!list_empty(list)) {
                __list_splice(list, head, head->next);
                INIT_LIST_HEAD(list);
        }
}

/**
 * list_splice_tail_init - join two lists and reinitialise the emptied list
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * Each of the lists is a queue.
 * The list at @list is reinitialised
 */
static __inline void list_splice_tail_init(struct list_head *list,
                                         struct list_head *head)
{
        if (!list_empty(list)) {
                __list_splice(list, head->prev, head);
                INIT_LIST_HEAD(list);
        }
}

/**
 * list_for_each_safe - iterate over a list safe against removal of list entry
 * @pos:        the &struct list_head to use as a loop cursor.
 * @n:          another &struct list_head to use as temporary storage
 * @head:       the head for your list.
*/
#define list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; \
         pos != (head); \
		 pos = n, n = pos->next)
/**
  * list_for_each_prev_safe - iterate over a list backwards safe against removal of list entry
  * @pos:        the &struct list_head to use as a loop cursor.
  * @n:          another &struct list_head to use as temporary storage
  * @head:       the head for your list.
*/
 #define list_for_each_prev_safe(pos, n, head) \
         for (pos = (head)->prev, n = pos->prev; \
              pos != (head); \
              pos = n, n = pos->prev)
/**
  * __list_for_each      -       iterate over a list
  * @pos:        the &struct list_head to use as a loop cursor.
  * @head:       the head for your list.
  *
  * This variant differs from list_for_each() in that it's the
  * simplest possible list iteration code, no prefetching is done.
  * Use this for code that knows the list to be very short (empty
  * or 1 entry) most of the time.
 */
#define list_for_each(pos, head) \
        for (pos = (head)->next; pos != (head); pos = pos->next)
/**
  * list_for_each_prev   -       iterate over a list backwards
  * @pos:        the &struct list_head to use as a loop cursor.
  * @head:       the head for your list.
*/
#define list_for_each_prev(pos, head) \
        for (pos = (head)->prev; pos != (head); pos = pos->prev)
/**
  * list_entry - get the struct for this entry
  * @ptr:        the &struct list_head pointer.
  * @type:       the type of the struct this is embedded in.
  * @member:     the name of the list_struct within the struct.
*/
#define list_entry(ptr, type, member) \
	container_of(ptr, type, member);
	


//helps debugging
#define LIST_POISON1  ((void *) 0x00100100)
#define LIST_POISON2  ((void *) 0x00200200)
/*
 * Double linked lists with a single pointer list head.
 * Mostly useful for hash tables where the two pointer list head is
 * too wasteful.
 * You lose the ability to access the tail in O(1).
 */

struct hlist_head {
        struct hlist_node *first;
};

struct hlist_node {
        struct hlist_node *next, **pprev;
};

#define HLIST_HEAD_INIT { .first = NULL }
#define HLIST_HEAD(name) struct hlist_head name = {  (void*)0 }
#define INIT_HLIST_HEAD(ptr) ((ptr)->first = NULL)
static __inline void INIT_HLIST_NODE(struct hlist_node *h)
{
        h->next = (void *)0;
        h->pprev = (void *)0;
}

static __inline int hlist_unhashed(const struct hlist_node *h)
{
        return !h->pprev;
}

static __inline int hlist_empty(const struct hlist_head *h)
{
        return !h->first;
}

static __inline void __hlist_del(struct hlist_node *n)
{
        struct hlist_node *next = n->next;
        struct hlist_node **pprev = n->pprev;
        *pprev = next;
        if (next)
                next->pprev = pprev;
}

static __inline void hlist_del(struct hlist_node *n)
{
        __hlist_del(n);
        n->next = LIST_POISON1;
        n->pprev = LIST_POISON2;
}

static __inline void hlist_del_init(struct hlist_node *n)
{
        if (!hlist_unhashed(n)) {
                __hlist_del(n);
                INIT_HLIST_NODE(n);
        }
}

static __inline void hlist_add_head(struct hlist_node *n, struct hlist_head *h)
{
        struct hlist_node *first = h->first;
        n->next = first;
        if (first)
                first->pprev = &n->next;
        h->first = n;
        n->pprev = &h->first;
}

/* next must be != NULL */
static __inline void hlist_add_before(struct hlist_node *n,
                                        struct hlist_node *next)
{
        n->pprev = next->pprev;
        n->next = next;
        next->pprev = &n->next;
        *(n->pprev) = n;
}

static __inline void hlist_add_after(struct hlist_node *n,
                                        struct hlist_node *next)
{
        next->next = n->next;
        n->next = next;
        next->pprev = &n->next;

        if(next->next)
                next->next->pprev  = &next->next;
}

/*
 * Move a list from one list head to another. Fixup the pprev
 * reference of the first entry if it exists.
 */
static __inline void hlist_move_list(struct hlist_head *old,
                                   struct hlist_head *newx)
{
        newx->first = old->first;
        if (newx->first)
                newx->first->pprev = &newx->first;
        old->first = (void *)0;
}

#define hlist_entry(ptr, type, member) container_of(ptr,type,member)

#define hlist_for_each(pos, head) \
         for (pos = (head)->first; (pos); \
              pos = pos->next)

#define hlist_for_each_safe(pos, n, head) \
	for(pos = (head)->first; pos != (void*)0 && ((n = pos->next) || 1); pos = n)

#define hlist_entry(ptr, type, member) container_of(ptr,type,member)
/*
#define hlist_for_each(pos, head) \
	for (pos = (head)->first; pos ; pos = pos->next)

#define hlist_for_each_safe(pos, n, head) \
	for (pos = (head)->first; pos && ({ n = pos->next; 1; }); \
	     pos = n)
*/

#define hlist_entry_safe(ptr, type, member) \
	( ptr ? hlist_entry(ptr, type, member) : NULL)								  
/*								   
#define hlist_entry_safe(ptr, type, member) \
	({ typeof(ptr) ____ptr = (ptr); \
	   ____ptr ? hlist_entry(____ptr, type, member) : NULL; \
	})
	*/
/**
 * hlist_for_each_entry	- iterate over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the hlist_node within the struct.
 */
#define hlist_for_each_entry(pos, head, member)				\
	for (pos = (head)->first;\
	     pos;							\
	     pos = (pos)->member.next)

 /**
 * hlist_for_each_entry - iterate over list of given type
 * @tpos:       the type * to use as a loop cursor.
 * @pos:        the &struct hlist_node to use as a loop cursor.
 * @head:       the head for your list.
 * @member:     the name of the hlist_node within the struct.
 */
#define hlist_for_each_entryx(tpos, pos, type, head, member)                    \
        for (pos = (head)->first;                                        \
             pos &&                      \
                ( tpos = hlist_entry(pos, type, member) || 1); \
             pos = pos->next)

/**
 * hlist_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @tpos:       the type * to use as a loop cursor.
 * @pos:        the &struct hlist_node to use as a loop cursor.
 * @n:          another &struct hlist_node to use as temporary storage
 * @head:       the head for your list.
 * @member:     the name of the hlist_node within the struct.
 */
/*
#define hlist_for_each_entry_safe(tpos, pos, n, type, head, member)            \
        for (pos = (head)->first;                                        \
             pos && ( (n = pos->next) || 1) &&                           \
                ( (tpos = hlist_entry(pos, type, member)) || 1 ); \
             pos = n)

*/
#define hlist_for_each_entry_safe(pos, n, head, type, member) 		\
	for (pos = hlist_entry_safe((head)->first, type, member);\
	     pos && ((n = pos->member.next) || 1);			\
	     pos = hlist_entry_safe(n, type, member))
/**
462  * list_for_each_entry_from - iterate over list of given type from the current point
463  * @pos:        the type * to use as a loop cursor.
464  * @head:       the head for your list.
465  * @member:     the name of the list_struct within the struct.
466  *
467  * Iterate over list of given type, continuing from current position.
468  
 #define list_for_each_entry_from(fcast,pos, type, head, member)                     \
         for (; &pos->member != (head);      \
              pos = (fcast)list_entry(pos->member.next, type, member))
*/

//you can loop this way
/*
	__list_for_each(((struct list_head*)pos), &mylist.list)
	{
		//pointer, type , member	
		temp = (struct nlist *) (pos - offsetof(struct nlist,list));
		printf("number is: %d\n", temp->result);
	}
	//or this other way (more code)
	for (pos = (&mylist.list)->next; pos != (&mylist.list); pos = ((struct list_head *)pos)->next)
	{
		//pointer, type , member	
		temp = (struct nlist *) (pos - offsetof(struct nlist,list));
		printf("number is: %d\n", temp->result);
	}
	
	list_for_each_prev(((struct list_head*)pos), &mylist.list)
	{
		temp = (struct nlist *) (pos - offsetof(struct nlist,list));
		printf("number is: %d\n", temp->result);
	}
	*/
	//use for each safe to delete the list
	/*
	printf("deleting the list using list_for_each_safe()\n");
	list_for_each_safe(((struct list_head*)pos), q, &mylist.list){
		 temp = (struct nlist *) (pos - offsetof(struct nlist,list));
		 printf("freeing item: %d\n", temp->result);
		 list_del(((struct list_head*)pos)->prev, ((struct list_head*)pos)->next);
		 Z_Free(temp);
	}
	*/

#endif
