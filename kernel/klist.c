/*
 * klist.c
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

#include "klist.h"


/*
Traversing the list:
Forwards

klist_node_t* node = list.head.next;
while(node != &list.head)
{
    <do something with node>
    node = node->next;
}

Backwards

klist_node_t* node = list.head.prev;
while(node != &list.head)
{
    <do something with node>
    node = node->prev;
}
*/


// Inserting a node after some node in list
void klist_insert_after(klist_t* list, klist_node_t* node, klist_node_t* new_node)
{
    new_node->prev = node;
    new_node->next = node->next;
    node->next->prev = new_node;
    node->next = new_node;
    list->n++;
}


// Removing a node
void klist_remove_node(klist_t* list, klist_node_t* node)
{
    if(node->next && node->prev)
    {
        node->prev->next = node->next;
        node->next->prev = node->prev;
        list->n--;
    }
}


// concatenate src list to dest list
// on return src list will be empty
void klist_concatenate(klist_t* dest, klist_t* src)
{
    if(src->n)
    {
        dest->head.prev->next = src->head.next;
        src->head.next->prev = dest->head.prev;
        src->head.prev->next = &dest->head;
        dest->head.prev = src->head.prev;

        dest->n += src->n;
        klist_init(src);
    }
}

// get node at index n in a list or null pointer if no such node
// if index is negative count backwards from the end of list
// since there is no -0 you can't index 0th node from the tail of list
klist_node_t *klist_index(const klist_t *list, int n)
{
	klist_node_t* node;

	if(n < 0)
	{
	    klist_iterate_bw(list, node)
	    {
	        if(n == 0) return node;
	        n++;
	    }
	}
	else
	{
	    klist_iterate_fw(list, node)
	    {
	        if(n == 0) return node;
            n--;
	    }
	}

	return 0;
}


// count number of list nodes, do not rely on list->n
int klist_recount(const klist_t* list)
{
    int count = 0;
    klist_node_t* node;

    klist_iterate_fw(list, node)
    {
        count++;
    }

    return count;
}


// find node and return node index or -1 if node is not in list
int klist_find_node(const klist_t* list, const klist_node_t* node)
{
    int i = 0;
    klist_node_t* tmp;
    klist_iterate_fw(list, tmp)
    {
        if(tmp == node) return i;
    }

    return -1;
}


void klist_rotate_left(klist_t* list)
{
    klist_node_t* t = klist_head(list);
    klist_remove_node(list, t);
    klist_insert_tail(list, t);
}


void klist_rotate_right(klist_t* list)
{
    klist_node_t* t = klist_tail(list);
    klist_remove_node(list, t);
    klist_insert_head(list, t);
}

