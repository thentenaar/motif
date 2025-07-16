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
/*
 * Resources:
 *
 * ---------------------------------------------------------------------------
 * Name            Class             Type              InitialValue
 * ---------------------------------------------------------------------------
 *
 * equalSize       EqualSize         Boolean           False
 * fillOption      FillOption        XmFillOption     XmFillNone
 * marginHeight    Margin            Dimension         0
 * marginWidth     Margin            Dimension         0
 * orientation     Orientation       unsigned char     XmHORIZONTAL
 *
 * ---------------------------------------------------------------------------
 *
 */
#ifndef _XmButtonBox_h
#define _XmButtonBox_h

#if defined(VMS) || defined(__VMS)
#include <X11/apienvset.h>
#endif

#include <Xm/Ext.h>

/************************************************************
 *       INCLUDE FILES
 ************************************************************/

/************************************************************
 *       TYPEDEFS AND DEFINES
 ************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct _XmButtonBoxClassRec    *XmButtonBoxWidgetClass;
typedef struct _XmButtonBoxRec	       *XmButtonBoxWidget;


/************************************************************
 *       MACROS
 ************************************************************/

/************************************************************
 *       GLOBAL DECLARATIONS
 ************************************************************/

/************************************************************
 *       EXTERNAL DECLARATIONS
 ************************************************************/

/*	Function Name: XmCreateButtonBox
 *	Description: Creation Routine for UIL and ADA.
 *	Arguments: parent - the parent widget.
 *                 name - the name of the widget.
 *                 args, num_args - the number and list of args.
 *	Returns: The Widget created.
 */

Widget XmCreateButtonBox(Widget, String, ArgList, Cardinal);

/*
 * Variable argument list functions
 */

extern Widget XmVaCreateButtonBox(
                        Widget parent,
                        char *name,
                        ...);
extern Widget XmVaCreateManagedButtonBox(
                        Widget parent,
                        char *name,
                        ...);

extern WidgetClass xmButtonBoxWidgetClass;

#if defined(__cplusplus)
}
#endif

#if defined(VMS) || defined(__VMS)
#include <X11/apienvrst.h>
#endif

#endif /* _XmButtonBox_h */
