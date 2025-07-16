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
 *
 */

#ifndef _XmIconBox_h
#define _XmIconBox_h

#if defined(VMS) || defined(__VMS)
#include <X11/apienvset.h>
#endif

#include <Xm/Ext.h>

/************************************************************
*	INCLUDE FILES
*************************************************************/

/************************************************************
*	TYPEDEFS AND DEFINES
*************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

#define XmIconBoxAnyCell (-5)

typedef struct _XmIconBoxClassRec	*XmIconBoxWidgetClass;
typedef struct _XmIconBoxRec		*XmIconBoxWidget;

typedef struct _XmIconBoxDropData {
    Position cell_x, cell_y;	/* drop location in cell coordinates. */
} XmIconBoxDropData;

/************************************************************
*	MACROS
*************************************************************/

/************************************************************
*	GLOBAL DECLARATIONS
*************************************************************/

/*	Function Name: XmIconBoxIsCellEmpty
 *	Description:   Returns true if this cell is unused.
 *	Arguments:     w - the icon box.
 *                     x, y - cell to check.
 *                     ignore - ignore this widget when checking.
 *	Returns:       Returns true if this cell is unused.
 */

Boolean XmIconBoxIsCellEmpty(Widget, Position, Position, Widget);

/*	Function Name: XmCreateIconBox
 *	Description: Creation Routine for UIL and ADA.
 *	Arguments: parent - the parent widget.
 *                 name - the name of the widget.
 *                 args, num_args - the number and list of args.
 *	Returns: The Widget created.
 */

Widget XmCreateIconBox(Widget, String, ArgList, Cardinal);

/************************************************************
*	EXTERNAL DECLARATIONS
*************************************************************/

extern WidgetClass xmIconBoxWidgetClass;

#if defined(__cplusplus)
}
#endif

#if defined(VMS) || defined(__VMS)
#include <X11/apienvrst.h>
#endif

#endif /* _XmIconBox_h */
