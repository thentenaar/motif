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
#ifndef _XmCacheP_h
#define _XmCacheP_h

#include <Xm/GadgetP.h>

#ifdef __cplusplus
extern "C" {
#endif

/* A few convenience macros */

#define ClassCacheHead(cp)	((cp)->cache_head)
#define ClassCacheCopy(cp)	((cp)->cache_copy)
#define ClassCacheCompare(cp)	((cp)->cache_compare)
#define CacheDataPtr(p)		((XtPointer)&((XmGadgetCacheRef*)p)->data)
#define DataToGadgetCache(p)	((char*)p - XtOffsetOf(XmGadgetCacheRef,data))


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmCacheP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
