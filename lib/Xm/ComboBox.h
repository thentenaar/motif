/* $TOG: ComboBox.h /main/7 1998/02/23 10:32:29 cshi $ */
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
/* ComboBox.h */

#ifndef _XmComboBox_h
#define _XmComboBox_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

externalref WidgetClass xmComboBoxWidgetClass;

typedef struct _XmComboBoxClassRec	* XmComboBoxWidgetClass;
typedef struct _XmComboBoxRec 		* XmComboBoxWidget;


/* XmIsComboBox may already be defined for Fast Subclassing  */
#ifndef XmIsComboBox
#define XmIsComboBox(w)		XtIsSubclass(w, xmComboBoxWidgetClass)
#endif  /* XmIsComboBox */

/********    Public Function Declarations    ********/

extern Widget XmCreateComboBox (Widget parent, 
				char *name, 
				ArgList args, 
				Cardinal argCount);
extern Widget XmCreateDropDownComboBox (Widget parent, 
					char *name, 
					ArgList args, 
					Cardinal argCount);
extern Widget XmCreateDropDownList (Widget parent, 
				    char *name, 
				    ArgList args, 
				    Cardinal argCount);
extern Widget XmVaCreateComboBox(
                        Widget parent,
                        char *name,
                        ...);
extern Widget XmVaCreateManagedComboBox(
                        Widget parent,
                        char *name,
                        ...);
extern void XmComboBoxAddItem (Widget   widget,
			       XmString item,
			       int      pos,
			       Boolean  unique);
extern void XmComboBoxDeletePos (Widget widget,
				 int    pos);
extern void XmComboBoxSelectItem (Widget   widget,
				  XmString item);

/* JIM ADDED THIS TO APEASE SYMBOL GOD */
extern XmString  XmCombinationBoxGetValue(Widget widget);

extern void XmComboBoxSetItem (Widget   widget,
			       XmString item);
extern void XmComboBoxUpdate (Widget widget);

/********    End Public Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmComboBox_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */

