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
*/ 
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: Cache.c /main/12 1995/07/14 10:12:26 drk $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include "CacheI.h"
#include <Xm/GadgetP.h>


/********    Static Function Declarations    ********/


/********    End Static Function Declarations    ********/


/************************************************************************
 *
 *  _XmCacheDelete
 *	Delete an existing cache record.  NOTE: <data> is a pointer to the
 *      fourth field in the cache record - It is *not* a pointer to the
 *	cache record itself!
 *
 ************************************************************************/
void 
_XmCacheDelete(
        XtPointer data )
{
    XmGadgetCachePtr ptr;

    ptr = (XmGadgetCachePtr) DataToGadgetCache(data);
    if (--ptr->ref_count <= 0) {
      (ptr->prev)->next = ptr->next;
      if (ptr->next)			/* not the last record */
        (ptr->next)->prev = ptr->prev;
      XtFree( (char *) ptr );
    }
}

/************************************************************************
 *
 *  _XmCacheCopy
 *	Copy <size> bytes from <src> to <dest>. 
 *
 ************************************************************************/
void 
_XmCacheCopy(
        XtPointer src,
        XtPointer dest,
        size_t size )
{
    memcpy( dest, src, size);
}

/************************************************************************
 *
 *  _XmCachePart
 *	Pass in a pointer, <cpart>, to <size> bytes of a temporary Cache
 *	record.  
 *	- If the Class cache head is NULL (no entries yet!), allocate a new
 *	  cache record, copy in temporary Cache bytes, append it to the 
 *	  class-cache linked list, and return the address.
 *	- Else, run through the class linked list.
 *	  = If a match is found, increment the ref_count and return the 
 *	    address.
 *	  = Else, allocate a new cache record, copy in temporary Cache bytes,
 *	    append it to the class-cache linked list, and return the address.
 *
 ************************************************************************/
XtPointer 
_XmCachePart(
        XmCacheClassPartPtr cp,
        XtPointer cpart,
        size_t size )
{
    XmGadgetCachePtr ptr, last;
    
    if (ClassCacheHead(cp).next == NULL)       /* First one */
    {
	ClassCacheHead(cp).next = 
	  (struct _XmGadgetCache *)XtMalloc( size + 
					    XtOffsetOf(XmGadgetCacheRef, data));
        ptr = (XmGadgetCachePtr)ClassCacheHead(cp).next;

        ClassCacheCopy(cp)(cpart, CacheDataPtr(ptr), size );  
	ptr-> ref_count = 1;
        ptr-> next = NULL;
	ptr-> prev = (struct _XmGadgetCache *)&ClassCacheHead(cp);
        return (CacheDataPtr(ptr));
    }    
    ptr = (XmGadgetCachePtr)ClassCacheHead(cp).next;
    do
    {

        if ((ClassCacheCompare(cp)( cpart, CacheDataPtr(ptr))))
        {
            ptr->ref_count++;
            return ((XtPointer) CacheDataPtr(ptr));
        }
        else
        {
            last = ptr;
            ptr = (XmGadgetCachePtr)ptr->next;
        }
    } while (ptr);
    
    /* Malloc a new rec off of last, fill it out*/
    ptr = (XmGadgetCachePtr)XtMalloc( size + 
				     XtOffsetOf(XmGadgetCacheRef, data) );
    last->next = (struct _XmGadgetCache *)ptr;
    ClassCacheCopy(cp)(cpart, CacheDataPtr(ptr), size);
    ptr-> ref_count = 1;
    ptr-> next = NULL;
    ptr-> prev = last;
    return (CacheDataPtr(ptr));
}

