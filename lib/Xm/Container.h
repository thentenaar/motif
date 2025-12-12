/* $XConsortium: Container.h /main/5 1995/07/15 20:50:08 drk $ */
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
#ifndef	_XmContainer_h
#define _XmContainer_h

#include <Xm/Xm.h>
#include <Xm/Manager.h>

#ifdef __cplusplus
extern "C" {
#endif



/* Class record constants */
externalref	WidgetClass	xmContainerWidgetClass;

typedef	struct	_XmContainerClassRec	*XmContainerWidgetClass;
typedef struct  _XmContainerRec	*XmContainerWidget;

#ifndef XmIsContainer
#define XmIsContainer(w) XtIsSubclass(w, xmContainerWidgetClass)
#endif /* XmIsContainer */


/********    Public Function Declarations    ********/
extern	Widget	XmCreateContainer(
			Widget parent,
			String name,
			ArgList arglist,
			Cardinal argcount);
extern Widget XmVaCreateContainer(
                        Widget parent,
                        char *name,
                        ...);
extern Widget XmVaCreateManagedContainer(
                        Widget parent,
                        char *name,
                        ...);
extern	int	XmContainerGetItemChildren(
			Widget		wid,
			Widget		item,
			WidgetList	*item_children);
extern	void	XmContainerRelayout(
			Widget	wid);
extern	void	XmContainerReorder(
			Widget		wid,
			WidgetList	cwid_list,
			int		cwid_count);	
extern  Boolean XmContainerCut(
                        Widget  wid,
                        Time    timestamp);
extern  Boolean XmContainerCopy(
                        Widget  wid,
                        Time    timestamp);
extern  Boolean XmContainerPaste(
                        Widget  wid);
extern  Boolean XmContainerCopyLink(
                        Widget  wid,
                        Time    timestamp);
extern  Boolean XmContainerPasteLink(
                        Widget  wid);
/********    End Public Function Declarations    ********/

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmContainer_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
