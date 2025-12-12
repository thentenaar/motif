/*
 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 * 
 */

#include "xmlist.h"

/************************************************************
 *
 *  Stack code.
 *
 ************************************************************/

#define STACK_INC 25

/************************************************************
 *
 *  Stack Manipulation code.
 *
 ************************************************************/

/*	Function Name: _XmStackInit
 *	Description: Initializes the stack.
 *	Arguments: none
 *	Returns: the stack
 */

XmStack
_XmStackInit()
{
    return((XmStack) XtCalloc(sizeof(XmStackRec), (Cardinal) 1));
}

/*	Function Name: _XmStackFree
 *	Description: Frees all data associated with the stack.
 *	Arguments: stack - the stack
 *	Returns: none
 */

void
_XmStackFree(XmStack stack)
{
    XtFree((XtPointer) stack->elems);
    XtFree((XtPointer) stack);
}

/*	Function Name: _XmStackPush
 *	Description: Pushes an element onto the stack
 *	Arguments: stack - the stack
 *                 elem - element to push onto it.
 *	Returns: none
 */

void
_XmStackPush(XmStack stack, XtPointer elem)
{
    if ((++stack->top) >= stack->alloc) {
	stack->alloc += STACK_INC;
	stack->elems = 
	    (XtPointer *) XtRealloc((XtPointer) stack->elems,
				    sizeof(XtPointer) * stack->alloc);
    }

    stack->elems[stack->top] = elem;

#ifdef STACK_DEBUG
    printf("Pushing %d as elem %d\n", (int) elem, stack->top);    
#endif
}

/*	Function Name: _XmStackPop
 *	Description: Pops an element off of the stack.
 *	Arguments: stack - stack to pop off of.
 *	Returns: elem - the element that has been popped.
 */

XtPointer
_XmStackPop(XmStack stack)
{
    if (stack->top <= 0)
	return(NULL);		/* no elements on the stack. */

#ifdef STACK_DEBUG
    printf("Popping %d from elem %d\n", 
	   (int) stack->elems[stack->top], stack->top);
#endif

    return(stack->elems[(stack->top)--]);
}

/************************************************************
 *
 *  Queue code.
 *
 ************************************************************/

#ifdef QUEUE_DEBUG
#define QUEUE_INC 5
#else
#define QUEUE_INC 25
#endif

/************************************************************
 *
 *  Queue Manipulation code.
 *
 ************************************************************/

/*	Function Name: _XmQueueInit
 *	Description: Initializes the queue.
 *	Arguments: none
 *	Returns: the queue
 */

XmQueue
_XmQueueInit()
{
    return((XmQueue) XtCalloc(sizeof(XmQueueRec), (Cardinal) 1));
}

/*	Function Name: _XmQueueFree
 *	Description: Frees all data associated with the queue.
 *	Arguments: queue - the queue
 *	Returns: none
 */

void
_XmQueueFree(XmQueue queue)
{
    int count;
    _XmQElem *elem = queue->first;
    XmStack stack = _XmStackInit();

    for (count = 0; count < 2; count++) {
	while (elem != NULL) {
	    if (elem->alloced) 
		_XmStackPush(stack, (XtPointer) elem);
	    elem = elem->next;
	}
	
	elem = queue->free_elems;
    }

    while ((elem = (_XmQElem *) _XmStackPop(stack)) != NULL)
	XtFree((XtPointer) elem);

    _XmStackFree(stack);

    XtFree((XtPointer) queue);
}

/*	Function Name: QueuePush
 *	Description: Pushes an element onto the queue
 *	Arguments: queue - the queue
 *                 data_in - the data to push onto the queue.
 *	Returns: none
 */

/*
 * This is being removed because its name was not changed when compiled
 * -DBX.  This means that there is already a QueuePush() defined in the
 * BX 3.1 bx.o, so integrating the EPak into BX causes multiply defined
 * symbols.  Luckily, this function is not currently used in EPak.  We
 * can remove this only when we don't support EPak integration into
 * BX 3.0 or BX 3.1.
 */

#ifdef notdef
void
QueuePush(XmQueue queue, XtPointer data_in)
{
    _XmQElem *elem = _Xm_GetNewElement(queue);

    _Xm_AddQueue(queue, queue->last, elem);

    if (queue->first == NULL) 
	queue->first = elem;

    queue->last = elem;

    elem->data = data_in;

#ifdef QUEUE_DEBUG
    printf("Pushing %d\n", (int) data_in);
#endif
}
#endif

/*	Function Name: _XmQueuePop
 *	Description: Pops an element off of the queue.
 *	Arguments: queue - queue to pop off of.
 *	Returns: elem - the element that has been popped.
 */

XtPointer
_XmQueuePop(XmQueue queue)
{
    _XmQElem *first = _Xm_RemQueue(&queue->first);

    if (queue->first == NULL)
	queue->last = NULL;

    if (first == NULL)
	return(NULL);

    _Xm_AddQueue(NULL, queue->free_elems, first);

#ifdef QUEUE_DEBUG
    printf("Popping %d\n", (int) first->data);
#endif

    return(first->data);
}

/*	Function Name: _XmQueueCount
 *	Description: Returns the number of items in the queue
 *	Arguments: queue - queue to check.
 *	Returns: number of items in queue.
 */


int
_XmQueueCount(XmQueue queue)
{
    register int i;
    register _XmQElem *elem = queue->first;

    for (i = 0; elem != NULL; i++)
	elem = elem->next;

    return(i);
}

/*	Function Name: _XmQueuePrint
 *	Description: Prints out the real and free nodes of the queue.
 *	Arguments: queue - queue to print.
 *	Returns: none.
 */


#ifdef QUEUE_DEBUG
void
_XmQueuePrint(XmQueue queue)
{
    _XmQElem *elem;

    printf("Elements:\n");
    for (elem = queue->first; elem != NULL; elem = elem->next)
	printf("Element - %d\n", elem->data);

    printf("Reverse Elements:\n");
    for (elem = queue->last; elem != NULL; elem = elem->prev)
	printf("Element - %d\n", elem->data);

    printf("Free Elements:\n");
    for (elem = queue->free_elems; elem != NULL; elem = elem->next)
	printf("Free Element - %d\n", elem->data);
}
#endif

/************************************************************
 *
 *  Internal Used (Utils Lib) functions.
 *
 ************************************************************/

/*	Function Name: _Xm_AddQueue
 *	Description: Adds an element to the queue, after the element specified.
 *	Arguments: queue - Queue to add to, if NULL then we may be adding
 *                         to the free items list.
 *                 after - the element is added after this one.
 *                 elem - element to add.
 *	Returns: none.
 */

void
_Xm_AddQueue(XmQueue queue, _XmQElem *after, _XmQElem *elem)
{
    if (elem != NULL) {
	elem->prev = after;
	if (after == NULL) 
	    /*
	     * If queue is NULL then this is being added to the 
	     * free elements queue, and we make certain assumptions, like
	     * we are always adding to then end of the list, so that
	     * we never will try to add before the first element.
	     */
	    if (queue != NULL) {
		elem->next = queue->first;
		if (elem->next != NULL)
		    elem->next->prev = elem;
	    }
	    else
		elem->next = NULL;
	else
	    elem->next = after->next;
    }

    if (after != NULL) {
	if (after->next != NULL)
	    after->next->prev = elem;
	after->next = elem;
    }

}

/*	Function Name: _Xm_RemQueue
 *	Description: Removes the specified element form a queue, and sets
 *                   the queue to point to the next element.
 *	Arguments: queue - the queue to remove it from.
 *	Returns: the element removed.
 */

_XmQElem *
_Xm_RemQueue(_XmQElem ** queue)
{
    _XmQElem * elem = *queue;

    if (elem != NULL) {
	*queue = elem->next;

	if (elem->next != NULL)
	    elem->next->prev = elem->prev;

	if (elem->prev != NULL)
	    elem->prev->next = elem->next;
    }

    return(elem);
}

/*	Function Name: GetNewElement
 *	Description: Gets an element off the free_elems list, allocates
 *                   more elems if necessary.
 *	Arguments: queue - the queue to get the element from.
 *	Returns: an element.
 */

_XmQElem *
_Xm_GetNewElement(XmQueue queue)
{
    _XmQElem *elem;

    if ((elem = _Xm_RemQueue(&queue->free_elems)) == NULL) {
	register int i;

	/*
	 * We are out of free elements, alloc some more.
	 */

	queue->free_elems = (_XmQElem *) XtCalloc(sizeof(_XmQElem), QUEUE_INC);
	queue->free_elems->alloced = True;

	queue->free_elems->next = elem = queue->free_elems + 1;
	for (i = 1; i < (QUEUE_INC - 1); i++, elem++) {
	    elem->prev = elem - 1;
	    elem->next = elem + 1;
	}
	elem->prev = elem - 1;

	/*
	 * Pop an element off the free elements queue.
	 */

	elem = _Xm_RemQueue(&queue->free_elems);
    }

    return(elem);
}

/************************************************************
 *
 *  List code.
 *
 ************************************************************/

/*
 * This code is very dependant on the queue manipulation code, they
 * are sharing much functionality, several of these functions are just
 * wrappers around the queue maipulation code.
 */

/*	Function Name: _XmListInit
 *	Description: Initializes the list.
 *	Arguments: none
 *	Returns: the queue
 */

XmList
_XmListInit()
{
    return((XmList) _XmQueueInit());
}

/*	Function Name: _XmListFree
 *	Description: Frees all data associated with the list.
 *	Arguments: list - the list
 *	Returns: none
 */

void
_XmListFree(XmList list)
{
    _XmQueueFree((XmQueue) list);
}

/*	Function Name: _XmListAddAfter
 *	Description: Adds an element after the specified element.
 *	Arguments: list - the list
 *                 elem - The element to add this one after.
 *                        If NULL then add this element to the beginning of 
 *                        the list
 *                 data_in - the data to push onto the queue.
 *	Returns: The new added element.
 */

XmListElem *
_XmListAddAfter(XmList list, XmListElem *elem_in, XtPointer data_in)
{
    XmListElem *elem = (XmListElem *) _Xm_GetNewElement((XmQueue) list);

    _Xm_AddQueue((XmQueue) list, elem_in, elem);

    if (elem_in == NULL) 
	list->first = elem;

    if (elem_in == list->last)
	list->last = elem;

    elem->data = data_in;

    return(elem);
}

/*	Function Name: _XmListAddBefore
 *	Description: Adds an element before the specified element.
 *	Arguments: list - the list
 *                 elem - The element to add this one before.
 *                        If NULL then add this element to the end of 
 *                        the list.
 *                 data_in - the data to push onto the queue.
 *	Returns: The new added element.
 */

XmListElem *
_XmListAddBefore(XmList list, XmListElem *elem_in, XtPointer data_in)
{
    XmListElem *after;

    if (elem_in == NULL) 
	after = list->last;
    else
	after = elem_in->prev;

    return(_XmListAddAfter(list, after, data_in));
}

/*	Function Name: _XmListRemove
 *	Description: Removes an element from the list.
 *	Arguments: list - the list to remove the element from.
 *	Returns: none.
 */

void
_XmListRemove(XmList list, XmListElem *elem)
{
    XmListElem *dead = (XmListElem *) _Xm_RemQueue((_XmQElem **) &elem);    

    /*
     * if NULL is passed, then no action is taken.
     */

    if (dead == NULL) 
	return;

    if (list->first == dead)
	if ((list->first = dead->next) == NULL)
	    list->last = NULL;

    if (list->last == dead)
	if ((list->last = dead->prev) == NULL)
	    list->first = NULL;

    _Xm_AddQueue(NULL, list->free_elems, (_XmQElem *) dead);
}

/*	Function Name: _XmListCount
 *	Description: Returns the number of items in the list.
 *	Arguments: list - list to check.
 *	Returns: number of items in list.
 */


int
_XmListCount(XmList list)
{
    return(_XmQueueCount((XmQueue) list));
}

/*	Function Name: _XmListExec
 *	Description: Executes a function on all nodes in the list.
 *	Arguments: list - the list to use.
 *                 start - the element to start with.
 *                 end - the element to end with.
 *                 exec_func - the function to execute.
 *                 data - data to pass to the function.
 *	Returns: NULL is returned if all items from start to end 
 *               have been operated on.  Otherwise the node in which
 *               the operation stopped is returned.
 *
 * NOTES: If start is NULL the start is the first element in the list.
 *        If end is NULL then all elements after start will be searched.
 *        The list is always executed if forward order from start -> 
 *        	start->next, start->next->next...
 *        
 */

XmListElem *
_XmListExec(XmList list, XmListElem *start, XmListElem *end, 
	 XmListFunc func, XtPointer data)
{
    if (start == NULL)
	start = list->first;

    for ( ; (start != end) && (start != NULL); start = start->next) {
	if (!(*func)(start, data))
	    return(start);
    }
    
    return(NULL);
}
 
/*	Function Name: _XmListPrint
 *	Description: Prints out the real and free nodes of the list.
 *	Arguments: list - list to print.
 *	Returns: none.
 */


#ifdef QUEUE_DEBUG
void
_XmListPrint(XmList list)
{
    _XmQueuePrint((XmQueue) list);
}
#endif
   
