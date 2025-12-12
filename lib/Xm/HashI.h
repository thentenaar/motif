/* $TOG: HashI.h /main/6 1997/06/18 17:39:16 samborn $ */
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
/*
 * HISTORY
 */

#ifndef _XmHashI_h
#define _XmHashI_h
 
#include <Xm/XmP.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int	                XmHashValue;
typedef XtPointer               XmHashKey;
typedef struct _XmHashTableRec  *XmHashTable;

typedef Boolean (*XmHashCompareProc)(XmHashKey, XmHashKey);
typedef XmHashValue (*XmHashFunction)(XmHashKey);
typedef Boolean (*XmHashMapProc)(XmHashKey, XtPointer value, XtPointer data);

XmHashTable _XmAllocHashTable(Cardinal, XmHashCompareProc, XmHashFunction);
void _XmResizeHashTable(XmHashTable, Cardinal);
void _XmFreeHashTable(XmHashTable);
XtPointer _XmGetHashEntryIterate(XmHashTable, XmHashKey, XtPointer*);
void _XmAddHashEntry(XmHashTable, XmHashKey, XtPointer);
XtPointer _XmRemoveHashEntry(XmHashTable, XmHashKey);
XtPointer _XmRemoveHashIterator(XmHashTable, XtPointer*);
Cardinal _XmHashTableCount(XmHashTable);
Cardinal _XmHashTableSize(XmHashTable);
void _XmMapHashTable(XmHashTable, XmHashMapProc, XtPointer);
#ifdef DEBUG
void _XmPrintHashTable(XmHashTable);
#endif

#define _XmGetHashEntry(table, key) _XmGetHashEntryIterate(table, key, NULL)

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmHashI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
