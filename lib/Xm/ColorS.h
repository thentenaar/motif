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

#ifndef _XmColorSelector_h
#define _XmColorSelector_h

#if defined(VMS) || defined(__VMS)
#include <X11/apienvset.h>
#endif

#include <Xm/Ext.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct _XmColorSelectorClassRec *XmColorSelectorWidgetClass;
typedef struct _XmColorSelectorRec *XmColorSelectorWidget;


/*	Function Name: XmCreateColorSelector
 *	Description: Creation Routine for UIL and ADA.
 *	Arguments: parent - the parent widget.
 *                 name - the name of the widget.
 *                 args, num_args - the number and list of args.
 *	Returns: The Widget created.
 */
Widget XmCreateColorSelector(Widget, String, ArgList, Cardinal);

/*
 * Variable argument list functions
 */

extern Widget XmVaCreateColorSelector(
                        Widget parent,
                        char *name,
                        ...);
extern Widget XmVaCreateManagedColorSelector(
                        Widget parent,
                        char *name,
                        ...);

extern WidgetClass       xmColorSelectorWidgetClass;

#if defined(__cplusplus)
}
#endif

#if defined(VMS) || defined(__VMS)
#include <X11/apienvrst.h>
#endif

#endif /* _XmColorSelect_h */
