/* $XConsortium: alloc.c /main/5 1995/07/15 21:01:19 drk $ */
/*
 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
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
/*
 * HISTORY
 */


/*
 *This file contains a front end to malloc.  The function tcMalloc behaves
 *like malloc, but keeps a record.  Calling tcFreeAll will free all memory
 *allocated by tcMalloc
 */
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>

#define MALLOC_CHUNK 50

static void **entriesMalloced;
static int numEntriesMalloced=0;
static int numEntriesAvail=0;


void *
tcMalloc(size_t size)
{
    if (numEntriesMalloced == numEntriesAvail)
    {
	if (numEntriesAvail == 0)
	{
	    numEntriesAvail = MALLOC_CHUNK;
	    entriesMalloced = (void **) XtMalloc(MALLOC_CHUNK * sizeof(void *));
	}
	else
	{
	    numEntriesAvail += MALLOC_CHUNK;
	    entriesMalloced = (void **) XtRealloc((char *) entriesMalloced,
						  numEntriesAvail * sizeof(void *));
	}
    }
    entriesMalloced[numEntriesMalloced] = (void *)XtMalloc(size);
    return (entriesMalloced[numEntriesMalloced++]);
} 

void
tcFreeAll()
{
    while (numEntriesMalloced > 0)
	XtFree(entriesMalloced[--numEntriesMalloced]);
}
