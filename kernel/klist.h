/*
 * klist.h
 *
 * Copyright (c) 2012 Mario Ivančić
 * Based on HellenOS list.c
 *
 * Copyright (c) 2004 Jakub Jermar
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __KLIST_H__
#define __KLIST_H__

// doubly linked circular list node
// it contains only pointers and no real data,
// so it must be included is some more coplex data structure
typedef struct klist_node
{
    struct klist_node* prev;        // a reference to the previous node
    struct klist_node* next;        // a reference to the next node
} klist_node_t;


// doubly linked circular list data structure,
// so the same functions can be used with many doubly linked lists
typedef struct list
{
    klist_node_t    head;           // points to first node and last node in list
                                    // if list is empty points to itself
    int             n;              // number of elements in list
} klist_t;



// declare and initialize statically allocated list (in .data section)
#define KLIST_DECLINIT(name) klist_t name = {	{ &(name).head, &(name).head }, 0 }


// list iterator loop, iterate forward
#define klist_iterate_fw(list_ptr, tmp_node_ptr) \
	for (tmp_node_ptr = (list_ptr)->head.next; \
	    tmp_node_ptr != &(list_ptr)->head; tmp_node_ptr = tmp_node_ptr->next)


// list iterator loop, iterate backward
#define klist_iterate_bw(list_ptr, tmp_node_ptr) \
	for (tmp_node_ptr = (list_ptr)->head.prev; \
	    tmp_node_ptr != &(list_ptr)->head; tmp_node_ptr = tmp_node_ptr->prev)


// test if list node is in some list
#define klist_node_used(node_ptr) \
	( ( (node_ptr)->prev ) && ( (node_ptr)->next ) )



// get pointer to struct 'type' containing klist_node_t with name 'node_member_name'
// basically, just subtract klist_node_t offset in struct 'type' from pointer to klist_node_t
#define klist_get_instance(node_ptr, type, node_member_name) \
	( (type*) ( ( (void*)(node_ptr) ) - ( (void*) &( ( (type*) 0 )->node_member_name ) ) ) )



// init empty list
// this must be the first operation on new list
static inline void klist_init(klist_t* list)
{
    list->head.prev = &list->head;
    list->head.next = &list->head;
    list->n = 0;
}

// init list node
// this must be the first operation on new node
// otherwise, it's not possible to tell is node used or not
static inline void klist_node_init(klist_node_t* node)
{
    node->prev = 0;
    node->next = 0;
}


// Inserting a node after some node in list
void klist_insert_after(klist_t* list, klist_node_t* node, klist_node_t* new_node);



// Removing a node
void klist_remove_node(klist_t* list, klist_node_t* node);



// Inserting a node before some node in list
static inline void klist_insert_before(klist_t* list, klist_node_t* node, klist_node_t* new_node)
{
    klist_insert_after(list, node->prev, new_node);
}



// Inserting a node to list head
static inline void klist_insert_head(klist_t* list, klist_node_t* new_node)
{
    klist_insert_after(list, &list->head, new_node);
}



// Inserting a node to list tail
static inline void klist_insert_tail(klist_t* list, klist_node_t* new_node)
{
    klist_insert_after(list, list->head.prev, new_node);
}



// get pointer to first list node or null pointer if list is empty
static inline klist_node_t *klist_head(const klist_t *list)
{
	return ((list->head.next == &list->head) ? 0 : list->head.next);
}



// get pointer to last list node or null pointer if list is empty
static inline klist_node_t *klist_tail(const klist_t *list)
{
	return ((list->head.prev == &list->head) ? 0 : list->head.prev);
}



// link list nodes node and next_node
static inline void klist_link_nodes(klist_t* list, klist_node_t* node, klist_node_t* next_node)
{
    node->next = next_node;
    next_node->prev = node;
}



// concatenate src list to dest list
// on return src list will be empty
void klist_concatenate(klist_t* dest, klist_t* src);




// get node at index n in a list or null pointer if no such node
// if index is negative count backwards from the end of list
// since there is no -0 you can't index 0th node from the tail of list
klist_node_t *klist_index(const klist_t *list, int n);




// returns number of list nodes (list->n)
static inline int klist_count(const klist_t* list)
{
    return list->n;
}




// count number of list nodes, do not rely on list->n
int klist_recount(const klist_t* list);




// find node and return node index or -1 if node is not in list
int klist_find_node(const klist_t* list, const klist_node_t* node);


void klist_rotate_left(klist_t* list);

void klist_rotate_right(klist_t* list);


#endif //  __KLIST_H__
