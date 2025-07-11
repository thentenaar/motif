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
*/ 
/* 
 * HISTORY
*/ 
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: IsParent.c /main/7 1995/07/14 11:17:14 drk $"
#endif
#endif

/***********************************************************************
  	@(#)IsParent.c	1.6.1.1	Date:1/22/91
  	Author: SJS
	History:
            06/05/90 SJS create
            06/25/90 SJS check the parent *not* the current
            06/26/90 SJS add IsParentShell()
            07/03/90 PSN add IsParentRowColumn()
            07/23/90 SJS fix all the functions to do the right error checks
	Calls:

	Summary:
            This file contains functions which determine the parentage of
        a widget. This is based on the class code of the parent. The
	basic assumption is that we have a widget info hierarchy that
	accurately reflects the widgets we have created. This is true,
	EXCEPT when a widget is first being created. The widget exists
	and its parents exist, but their relationship is not yet known
	in the widget info hierarchy. This relationship is established
	upon updating the objects/widget hierarchy.
	    The further implication is that the geometry of RowColumn
	children will be initially incorrect because of RowColumns
	handling of entry border. (IE. We can't do "the right thing"
	with the geometry because we don't know the parent is a RowColumn.
	Thus, mvsIsRowColumnParent() incorrectly returnds FALSE when a
	RowColumn's chilren are first created. Later on, when an update is
	done, the geometry is corrected)

        INPUTS (all functions)
            widget_info - the widget info record in question

        OUTPUTS
            none
 
        RETURNS
            class code
************************************************************************/

#include "mvslib.h"

/************************************************************************/
/*  This function determines if the parent of a widget is a Shell       */
/************************************************************************/
int mvsIsParentShell(widget_info)
MvsWidgetInfoRecord *widget_info;
{
    Widget widget;

    if(widget_info != NULL) {
        widget = XtParent(widget_info->widget);
        if (widget != (Widget)NULL) {
            if(XtIsShell(widget)) {
                return(TRUE);
            }
        }
    }

    return(FALSE);

} /* End mvsIsParentShell() */


/************************************************************************/
/*  This function determines if the parent of a widget is a DialogShell */
/************************************************************************/
int mvsIsParentDialogShell(widget_info)
MvsWidgetInfoRecord *widget_info;
{
    Widget widget;
    int class_code;

    if(widget_info != NULL) {
        widget = XtParent(widget_info->widget);
        if (widget != (Widget)NULL) {
    	    class_code = mvsGetClassCode(widget);
    	    if(class_code == mvsXmDialogShellWidgetClass) {
        	return(TRUE);
    	    }
        }
    }

    return(FALSE);

} /* End mvsIsParentDialogShell() */


/************************************************************************/
/*  This function determines if the parent of a widget is a RowColumn   */
/* RowColumn draws the entryBorder *INSIDE* its child windows. I am     */
/* not especially sure that this is correct from a design standpoint,   */
/* but it seems that no one is going to make this change at such a late */
/* stage in Motif development.                                          */
/************************************************************************/
int mvsIsParentRowColumn(widget_info)
MvsWidgetInfoRecord *widget_info;
{
    Widget widget;
    int class_code;

    if(widget_info != NULL) {
        widget = XtParent(widget_info->widget);
        if (widget != (Widget)NULL) {
    	    class_code = mvsGetClassCode(widget);
    	    if(class_code == mvsXmRowColumnWidgetClass) {
        	return(TRUE);
    	    }
        }
    }

    return(FALSE);

} /* End mvsIsParentRowColumn() */
