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
static char rcsid[] = "$XConsortium: mvsSetUpGlob.c /main/7 1995/07/14 11:23:04 drk $"
#endif
#endif
/***********************************************************************
  	@(#)mvsSetUpGlob.c	1.11.1.2     Date:2/13/91                                 
  	Author: SJS
	History:
            05/08/90 SJS Create
            09/20/90 SJS addin XmNrecomputeSize for Buttons
            10/25/90 SJS remove XmNrecomputeSize for Buttons
	Calls:

	Summary:
            This function conditionally sets up the "global" arguments
        of a widget class. We subjectively choose resource settings
        that we want to have in order to determine the effects of
        other resource changes. (For example, changing the borderPixmap
        will not have a visible effect unless the border_width is
        non-zero.) If the widget class has this resource, then it is
        added to the "global" list of resource arguments for usage later.
            This function needs to be smarter for "special" types of
        widgets.
************************************************************************/
#include "mvslib.h"

void mvsSetUpGlobalArgs(widget_class_info)
MvsWidgetClassInfo *widget_class_info;
{
    int n=0;
    static int first_time = 1;
    static XmString string1 = NULL;
    static char string2[MAX_STRING_LENGTH] = "Text Widget\n\nNew line";
    XmFontList     font_list;

    font_list = XmFontListCreate(mvsFontInfo[0].font_struct,
				     mvsFontInfo[0].char_set_name) ;
    if (first_time) {
        first_time = 0;
        string1 = XmStringCreateLtoR("Some Label",mvsGetCharSet
				     (mvsFontInfo[0].char_set_name));
    }

    if (widget_class_info->num_common_args == 0) {

        if((widget_class_info == iXmTextWidgetClass) ||
           (widget_class_info == iXmScrolledTextWidgetClass)) {

         	if(mvsGetResourceNum(widget_class_info,XmNfontList)
							!= NON_EXISTENT) 
	        {
                	XtSetArg(widget_class_info->common_args[n], 
						XmNfontList, font_list);
                	n++;
	 	}
	}

        if((widget_class_info != iXmTextWidgetClass) &&
           (widget_class_info != iXmScrolledTextWidgetClass) &&
           (widget_class_info != iXmSelectionBoxWidgetClass) &&
           (widget_class_info != iXmSelectionDialogWidgetClass) &&
           (widget_class_info != iXmCommandWidgetClass)) {

            if(mvsGetResourceNum(widget_class_info,XmNwidth) != NON_EXISTENT) {
                XtSetArg(widget_class_info->common_args[n], XmNwidth, 125);
                n++;
            }
            if(mvsGetResourceNum(widget_class_info,XmNheight) != NON_EXISTENT) {
                XtSetArg(widget_class_info->common_args[n], XmNheight, 125);
                n++;
            }
        }
        else {
            if(mvsGetResourceNum(widget_class_info,XmNvalue) != NON_EXISTENT) {
                XtSetArg(widget_class_info->common_args[n], XmNvalue, string2);
                n++;
            }
            if(mvsGetResourceNum(widget_class_info,
                                 XmNeditMode) != NON_EXISTENT) {
                XtSetArg(widget_class_info->common_args[n],XmNeditMode,
                         XmMULTI_LINE_EDIT);
                n++;
            }
        }
        if(mvsGetResourceNum(widget_class_info, XmNx) != NON_EXISTENT) {
            XtSetArg(widget_class_info->common_args[n], XmNx, 50);
            n++;
        }
        if(mvsGetResourceNum(widget_class_info, XmNy) != NON_EXISTENT) {
            XtSetArg(widget_class_info->common_args[n], XmNy, 50);
            n++;
        }
        if(mvsGetResourceNum(widget_class_info,
                             XmNshadowThickness) != NON_EXISTENT) {
            XtSetArg(widget_class_info->common_args[n], XmNshadowThickness, 10);
            n++;
        }
        if(mvsGetResourceNum(widget_class_info,
                             XmNhighlightThickness) != NON_EXISTENT) {
            XtSetArg(widget_class_info->common_args[n], XmNhighlightThickness,
                     10);
            n++;
        }
        if(mvsGetResourceNum(widget_class_info,
                             XmNlabelString) != NON_EXISTENT) {
            XtSetArg(widget_class_info->common_args[n],XmNlabelString,string1);
            n++;
        }

        widget_class_info->num_common_args = n;
    } /* End if() */

} /* End mvsSetUpGlobalArgs() */
