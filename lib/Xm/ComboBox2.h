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
#ifndef _XmCominationBox2_h_
#define _XmCominationBox2_h_

#include <Xm/DropDown.h>
#include <Xm/XmP.h>

#if defined(__cplusplus)
extern "C" {
#endif

enum {
    XiCOMBINATIONBOX2_LABEL = XmDROPDOWN_LABEL,
    XiCOMBINATIONBOX2_TEXT  = XmDROPDOWN_TEXT,
    XiCOMBINATIONBOX2_ARROW_BUTTON = XmDROPDOWN_ARROW_BUTTON,
    XiCOMBINATIONBOX2_LIST  = XmDROPDOWN_LIST
};

XM_ALTERNATIVE(Use XmDropDown instead)
extern WidgetClass xmCombinationBox2WidgetClass;

typedef XmDropDownWidgetClass *XmCombinationBox2WidgetClass;
typedef XmDropDownWidget      *XmCombinationBox2Widget;

/*	Function Name: XmCombinationBox2GetValue
 *	Description:   Retreives the value from the combo box.
 *	Arguments:     w - the combination box.
 *	Returns:       The value in the text widget.
 */
extern XM_DEPRECATED String XmCombinationBox2GetValue(Widget w);

/*	Function Name: XmCreateCombinationBox2
 *	Description: Creation Routine for UIL and ADA.
 *	Arguments: parent - the parent widget.
 *                 name - the name of the widget.
 *                 args, num_args - the number and list of args.
 *	Returns: The Widget created.
 */
XM_ALTERNATIVE(Use XmDropDown instead)
extern Widget XmCreateCombinationBox2(
	Widget w,
	char *name,
	ArgList args,
	Cardinal argCount
);

/*
 * Variable argument list functions
 */
XM_ALTERNATIVE(Use XmDropDown instead)
extern Widget XmVaCreateCombinationBox2(Widget parent, char *name, ...);

XM_ALTERNATIVE(Use XmDropDown instead)
extern Widget XmVaCreateManagedCombinationBox2(Widget parent, char *name, ...);

/*      Function Name:  XmCombinationBox2GetLabel
 *      Description:    Returns the "label" child of the XmCombinationBox2
 *      Arguments:      w - The XmCombinationBox2 Widget
 *      Returns:        The specified child of the XmCombinationBox2
 */
Widget XM_DEPRECATED XmCombinationBox2GetLabel(Widget w);

/*      Function Name:  XmCombinationBox2GetArrow
 *      Description:    Returns the "arrow" child of the XmCombinationBox2
 *      Arguments:      w - The XmCombinationBox2 Widget
 *      Returns:        The specified child of the XmCombinationBox2
 */
Widget XM_DEPRECATED XmCombinationBox2GetArrow(Widget w);

/*      Function Name:  XmCombinationBox2GetText
 *      Description:    Returns the "text" child of the XmCombinationBox2
 *      Arguments:      w - The XmCombinationBox2 Widget
 *      Returns:        The specified child of the XmCombinationBox2
 */
Widget XM_DEPRECATED XmCombinationBox2GetText(Widget w);

/*      Function Name:  XmCombinationBox2GetList
 *      Description:    Returns the "list" child of the XmCombinationBox2
 *      Arguments:      w - The XmCombinationBox2 Widget
 *      Returns:        The specified child of the XmCombinationBox2
 */
Widget XM_DEPRECATED XmCombinationBox2GetList(Widget w);

/*      Function Name:  XmCombinationBox2GetChild
 *      Description:    Returns the child widget id of the XmCombinationBox2
 *      Arguments:      w - The XmCombinationBox2 Widget
 *      Returns:        The specified child of the XmCombinationBox2
 */
Widget XM_DEPRECATED XmCombinationBox2GetChild(Widget w, int child);

#if defined(__cplusplus)
}
#endif
#endif /* _XmCombinationBox2_h_ */
