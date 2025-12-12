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

#ifndef _XmSpinB_h
#define _XmSpinB_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SpinB Widget
 */
externalref WidgetClass xmSpinBoxWidgetClass;

typedef struct _XmSpinBoxClassRec *XmSpinBoxWidgetClass;
typedef struct _XmSpinBoxRec      *XmSpinBoxWidget;


/*
 * Spin externs for application accessible functions
 */

Widget	XmCreateSpinBox(Widget	parent,
				char	*name,
				ArgList	arglist,
				Cardinal argcount);
Widget XmVaCreateSpinBox(
                                Widget parent,
                                char *name,
                                ...);
Widget XmVaCreateManagedSpinBox(
                                Widget parent,
                                char *name,
                                ...);
int	XmSpinBoxValidatePosition(
				Widget	text_field,
				int	*position_value);



#ifdef __cplusplus
}
#endif

#endif /* _SpinB_h */
