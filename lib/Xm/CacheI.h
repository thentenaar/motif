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
/*
 * HISTORY
 */
/* $XConsortium: CacheI.h /main/6 1995/07/14 10:12:39 drk $ */
#ifndef _XmCacheI_h
#define _XmCacheI_h

#include <Xm/XmP.h>
#include <Xm/CacheP.h>

#ifdef __cplusplus
extern "C" {
#endif

/********    Private Function Declarations    ********/

extern void _XmCacheDelete( 
                        XtPointer data) ;
extern void _XmCacheCopy( 
                        XtPointer src,
                        XtPointer dest,
                        size_t size) ;
extern XtPointer _XmCachePart( 
                        XmCacheClassPartPtr cp,
                        XtPointer cpart,
                        size_t size) ;

/********    End Private Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmCacheI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
