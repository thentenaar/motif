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
/* $XConsortium: XmTabListI.h /main/5 1995/07/13 18:28:19 drk $ */
#ifndef _XmTabListI_h
#define _XmTabListI_h

#include <Xm/XmP.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TAB_OPTIMIZED_BITS	1
#define TAB_MARK_BITS		TAB_OPTIMIZED_BITS
#define TAB_REFCOUNT_BITS	(16 - TAB_OPTIMIZED_BITS)

typedef struct __XmTabRec
{
  unsigned int		mark : TAB_MARK_BITS;
  unsigned int		ref_count : TAB_REFCOUNT_BITS;
  float			value;
  unsigned char		units;
  XmOffsetModel		offsetModel;
  unsigned char		alignment;
  char			*decimal;
  XmTab			next, prev;
} _XmTabRec, *_XmTab;

typedef struct __XmTabListRec
{
  unsigned int	count;
  XmTab		start;
} _XmTabListRec, *_XmTabList;


/*
 * Macros for tab data structure access
 */

#define _XmTabMark(tab)		((_XmTab)(tab))->mark
#define _XmTabValue(tab)	((_XmTab)(tab))->value
#define _XmTabUnits(tab)	((_XmTab)(tab))->units
#define _XmTabPrev(tab)		((_XmTab)(tab))->prev
#define _XmTabNext(tab)		((_XmTab)(tab))->next
#define _XmTabModel(tab)	((_XmTab)(tab))->offsetModel
#define _XmTabAlign(tab)	((_XmTab)(tab))->alignment
#define _XmTabDecimal(tab)	((_XmTab)(tab))->decimal

#define _XmTabLStart(tl)	((_XmTabList)(tl))->start
#define _XmTabLCount(tl)	((_XmTabList)(tl))->count



/********    Private Function Declarations for XmTabList.c    ********/

extern XmTab _XmTabCopy(XmTab tab);
extern Widget _XmCreateTabList(Widget parent,
                               String name,
                               ArgList arglist,
                               Cardinal argcount); 
extern Widget _XmCreateTab(Widget parent,
			   String name,
			   ArgList arglist,
			   Cardinal argcount); 
extern Position _XmTabListGetPosition(
				     Screen * screen,
				     XmTabList tab_list,
                                     unsigned char unit_type,
				     Cardinal tab_position);


/********    End Private Function Declarations    ********/

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmTabListI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
