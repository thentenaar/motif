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
static char rcsid[] = "$XConsortium: mvsSetLocCon.c /main/7 1995/07/14 11:22:19 drk $"
#endif
#endif
/***********************************************************************
  	@(#)mvsSetLocCon.c	1.15.1.1	Date:1/22/91
        Author: TAT
        History:
           03/30/90 SJS Add to sccs tree
           05/29/90 SJS add changes for composites
           08/28/90 SJS modify implementation, add Frame constraints
           09/18/90 SJS add in Dialog Shell constraints
           10/03/90 SJS MenuShell children no longer have x,y,borderWidth
           10/11/90 SJS loosen Geometry, put in special constraints
           10/18/90 SJS move xisProcessEvents to here. This function is
		        the "funnel" destination of all mvsSet functions.
        Calls:

        Summary:
                This file contains functions that over-ride the
        MVS concept of certain resource values. The over-rides depend
        on the constraints the parent widget places on its children.
        These constraints prevent the widget from change some resources
        to MVS requested values. The constraints in this file are
        determined as a direct result of statements made in the AES.
************************************************************************/

#include "mvslib.h"
#include <X11/Shell.h>

void mvsSetLocalConstraints(widget_info,resources, args_in, nargs)
MvsWidgetInfoRecord *widget_info;
MvsArgVal resources[];
ArgList args_in;
Cardinal nargs;
{
extern WidgetClass xmMessageBoxWidgetClass;
    register MvsWidgetInfoRecord *parent_info;
    register MvsWidgetClassInfo  *parent_c_info;
    MvsWidgetClassInfo           *w_c_info;
    Widget parent;
    Arg args[MAX_RESOURCES];           /* Args to be Set in local resources */
    Arg loc_args[MAX_RESOURCES];       /* Args to be gotten from widgets */
    Cardinal n=0;
    Cardinal ln=0;
    static int in_SetLocalConstraints = FALSE;
              /* Variables that should eventually be done in WML */
    Dimension width;
    Dimension height;
    Dimension marg_top;
    Dimension marg_bot;
    Dimension marg_left;
    Dimension marg_right;
    Dimension indic_size;
    Dimension dflt_b_s_t;
    Position x,y;
    Pixmap sym_pix;
    Pixmap label_pix;
    Pixmap label_in_pix;
    Pixmap sel_in_pix;
    Pixmap sel_pix;
    short text_columns;

    parent_info = widget_info->parent;
    parent = XtParent(widget_info->widget);
    w_c_info = widget_info->widget_class_info;
    xisProcessEvents(NULL,0);

    if((parent_info == NULL) || (in_SetLocalConstraints) || (parent == NULL)) {
        return;
    }
    parent_c_info = parent_info->widget_class_info;
    in_SetLocalConstraints = TRUE;

    /* This section does constraints that are specific to the type of
       parent the current widget has
    */
    if (parent_c_info == iXmFrameWidgetClass) {
        int frame_m_width;
        int frame_m_height;
        int shadow_t;

        frame_m_width = (int)mvsGetResourceValue(parent,XmNmarginWidth,
                                                 T_Dimension);
        frame_m_height = (int)mvsGetResourceValue(parent,XmNmarginHeight,
                                                 T_Dimension);
        shadow_t = (int)mvsGetResourceValue(parent,XmNshadowThickness,
                                                 T_Dimension);
        if(mvsGetResourceNum(w_c_info,XmNx) != NON_EXISTENT) {
            XtSetArg(args[n], XmNx, shadow_t+frame_m_width); n++;
        }
        if(mvsGetResourceNum(w_c_info,XmNy) != NON_EXISTENT) {
            XtSetArg(args[n], XmNy, shadow_t+frame_m_height); n++;
        }
    }
    else
        if((parent_c_info == iXmApplicationShellWidgetClass) ||
           (parent_c_info == iXmMenuShellWidgetClass)) {

            if(mvsGetResourceNum(w_c_info,XmNx) != NON_EXISTENT) {
                XtSetArg(args[n], XmNx,0); n++;
            }
            if(mvsGetResourceNum(w_c_info,XmNy) != NON_EXISTENT) {
                XtSetArg(args[n], XmNy,0); n++;
            }
        }
        else
            if((parent_c_info == iXmDialogShellWidgetClass) ||
               (parent_c_info == iXmScrolledWindowWidgetClass)) {
                ln = 0;
                XtSetArg(loc_args[ln], XmNx,&x); ln++;
                XtSetArg(loc_args[ln], XmNy,&y); ln++;
                if(parent_c_info == iXmDialogShellWidgetClass)
                    XtGetValues(parent,loc_args,ln);
                else
                    XtGetValues(widget_info->widget,loc_args,ln);

                XtSetArg(args[n], XmNx,x); n++;
                XtSetArg(args[n], XmNy,y); n++;
            }

    /* This section does constraints that are specific to the resources
       being set in the arg list. This is for specific widgets. This will
       ideally become part of WML. The exact Motif/AES handling of these
       resources was not decided until too late (10/19/90) to do this in
       a more language driven manner. Key to look for in WML is the KeyWord
       "Related". Then follow the AES, do not pass Go, do not collect $200
       Implementation-wise, I would like to see this done by associating
       a Constraint function with each widget class just as there is a
       Creation function and a Behavior function. Then, this Constraint
       function couls just be invoked here.
    */
    if((w_c_info == iXmPushButtonWidgetClass) ||
       (w_c_info == iXmPushButtonGadgetClass)) {

        int show;
        int def;
        int arm_p;

        show = (mvsGetResNumInArgs(args_in,(int)nargs,
                                  XmNshowAsDefault) != NON_EXISTENT);
        def = (mvsGetResNumInArgs(args_in, (int)nargs,
                            XmNdefaultButtonShadowThickness) != NON_EXISTENT);
        arm_p = (mvsGetResNumInArgs(args_in, (int)nargs,
                            XmNarmPixmap) != NON_EXISTENT);
        if(show || def) {
            ln = 0;
            XtSetArg(loc_args[ln], XmNmarginTop,&marg_top); ln++;
            XtSetArg(loc_args[ln], XmNmarginBottom,&marg_bot); ln++;
            XtSetArg(loc_args[ln], XmNmarginLeft,&marg_left); ln++;
            XtSetArg(loc_args[ln], XmNmarginRight,&marg_right); ln++;
            if(show) {
                XtSetArg(loc_args[ln], XmNdefaultButtonShadowThickness,
                         &dflt_b_s_t); ln++;
            }
            XtGetValues(widget_info->widget,loc_args,ln);
            XtSetArg(args[n], XmNmarginTop,marg_top); n++;
            XtSetArg(args[n], XmNmarginBottom,marg_bot); n++;
            XtSetArg(args[n], XmNmarginLeft,marg_left); n++;
            XtSetArg(args[n], XmNmarginRight,marg_right); n++;
            if(show) {
                XtSetArg(args[n], XmNdefaultButtonShadowThickness,
                         dflt_b_s_t); n++;
            }
        }
        if(arm_p) {
            ln = 0;
            XtSetArg(loc_args[ln], XmNlabelPixmap,&label_pix); ln++;
            XtGetValues(widget_info->widget,loc_args,ln);
            XtSetArg(args[n], XmNlabelPixmap,label_pix); n++;
        }
    } /* End if(PushButton || PushButtonGadget) */

    if((w_c_info == iXmToggleButtonWidgetClass) ||
       (w_c_info == iXmToggleButtonGadgetClass)) {
        if(mvsGetResNumInArgs(args_in,(int)nargs, XmNselectPixmap)
                                                              != NON_EXISTENT) {
            ln = 0;
            XtSetArg(loc_args[ln], XmNlabelPixmap,&label_pix); ln++;
            XtGetValues(widget_info->widget,loc_args,ln);
            XtSetArg(args[n], XmNlabelPixmap,label_pix); n++;
        }
        if(mvsGetResNumInArgs(args_in,(int)nargs, XmNselectInsensitivePixmap)
                                                              != NON_EXISTENT) {
            ln = 0;
            XtSetArg(loc_args[ln],XmNlabelInsensitivePixmap,&label_in_pix);ln++;
            XtGetValues(widget_info->widget,loc_args,ln);
            XtSetArg(args[n], XmNlabelInsensitivePixmap,label_in_pix); n++;
        }
        if(mvsGetResNumInArgs(args_in,(int)nargs, XmNlabelString)
                                                              != NON_EXISTENT) {
            ln = 0;
            XtSetArg(loc_args[ln], XmNindicatorSize,&indic_size); ln++;
            XtGetValues(widget_info->widget,loc_args,ln);
            XtSetArg(args[n], XmNindicatorSize,indic_size); n++;
        }
        if(mvsGetResNumInArgs(args_in,(int)nargs, XmNindicatorSize)
                                                              != NON_EXISTENT) {
            ln = 0;
            XtSetArg(loc_args[ln], XmNmarginTop,&marg_top); ln++;
            XtSetArg(loc_args[ln], XmNmarginBottom,&marg_bot); ln++;
            XtSetArg(loc_args[ln], XmNmarginLeft,&marg_left); ln++;
            XtSetArg(loc_args[ln], XmNmarginRight,&marg_right); ln++;
            XtGetValues(widget_info->widget,loc_args,ln);
            XtSetArg(args[n], XmNmarginTop,marg_top); n++;
            XtSetArg(args[n], XmNmarginBottom,marg_bot); n++;
            XtSetArg(args[n], XmNmarginLeft,marg_left); n++;
            XtSetArg(args[n], XmNmarginRight,marg_right); n++;
        }
        if(mvsGetResNumInArgs(args_in,(int)nargs, XmNspacing) != NON_EXISTENT) {
            ln = 0;
            XtSetArg(loc_args[ln], XmNmarginLeft,&marg_left); ln++;
            XtGetValues(widget_info->widget,loc_args,ln);
            XtSetArg(args[n], XmNmarginLeft,marg_left); n++;
        }
    } /* End if(ToggleButton || ToggleButtonGadget) */

    if(mvsGetResNumInArgs(args_in, (int)nargs, XmNdialogType) != NON_EXISTENT) {
        if(XtIsSubclass(widget_info->widget, xmMessageBoxWidgetClass)) {
            ln = 0;
            XtSetArg(loc_args[ln], XmNsymbolPixmap,&sym_pix); ln++;
            XtGetValues(widget_info->widget,loc_args, ln);
            XtSetArg(args[n], XmNsymbolPixmap,sym_pix); n++;
        }
    }
    if(mvsGetResourceNum(widget_info->widget_class_info, XmNtextColumns)
                                                              != NON_EXISTENT) {
        if(mvsGetResNumInArgs(args_in, (int)nargs, XmNtextColumns)
                                                              == NON_EXISTENT) {
            ln = 0;
            XtSetArg(loc_args[ln], XmNtextColumns,&text_columns); ln++;
            XtGetValues(widget_info->widget,loc_args, ln);
            XtSetArg(args[n], XmNtextColumns,text_columns); n++;
        }
    }
    if(w_c_info == iXmRowColumnWidgetClass) {
    }
    if(w_c_info == iXmCommandWidgetClass) {
        if(mvsGetResNumInArgs(args_in, (int)nargs,XmNcommand) != NON_EXISTENT) {
            (void)mvsAssignRes(widget_info, XmNtextString, XmNcommand);
        }
        if(mvsGetResNumInArgs(args_in,(int)nargs,XmNtextString)!=NON_EXISTENT) {
            (void)mvsAssignRes(widget_info, XmNcommand, XmNtextString);
        }
        if(mvsGetResNumInArgs(args_in, (int)nargs,XmNhistoryItems)
                                                              != NON_EXISTENT) {
            (void)mvsAssignRes(widget_info, XmNlistItems, XmNhistoryItems);
        }
        if(mvsGetResNumInArgs(args_in, (int)nargs,XmNlistItems)
                                                              != NON_EXISTENT) {
            (void)mvsAssignRes(widget_info, XmNhistoryItems, XmNlistItems);
        }
        if(mvsGetResNumInArgs(args_in, (int)nargs,XmNhistoryItemCount)
                                                              != NON_EXISTENT) {
            (void)mvsAssignRes(widget_info, XmNlistItemCount,
                               XmNhistoryItemCount);
        }
        if(mvsGetResNumInArgs(args_in, (int)nargs,XmNlistItemCount)
                                                              != NON_EXISTENT) {
            (void)mvsAssignRes(widget_info, XmNhistoryItemCount,
                               XmNlistItemCount);
        }
        if(mvsGetResNumInArgs(args_in, (int)nargs,XmNhistoryVisibleItemCount)
                                                              != NON_EXISTENT) {
            (void)mvsAssignRes(widget_info, XmNlistVisibleItemCount,
                               XmNhistoryVisibleItemCount);
        }
        if(mvsGetResNumInArgs(args_in, (int)nargs,XmNlistVisibleItemCount)
                                                              != NON_EXISTENT) {
            (void)mvsAssignRes(widget_info, XmNhistoryVisibleItemCount,
                               XmNlistVisibleItemCount);
        }
        if(mvsGetResNumInArgs(args_in, (int)nargs,XmNpromptString)
                                                              != NON_EXISTENT) {
            (void)mvsAssignRes(widget_info, XmNselectionLabelString,
                               XmNpromptString);
        }
        if(mvsGetResNumInArgs(args_in, (int)nargs,XmNselectionLabelString)
                                                              != NON_EXISTENT) {
            (void)mvsAssignRes(widget_info, XmNpromptString,
                               XmNselectionLabelString);
        }
    }
    if((w_c_info == iXmFileSelectionBoxWidgetClass) ||
       (w_c_info == iXmFileSelectionDialogWidgetClass)) {
        if(mvsGetResNumInArgs(args_in, (int)nargs,XmNfileListLabelString)
                                                              != NON_EXISTENT) {
            (void)mvsAssignRes(widget_info, XmNlistLabelString,
                               XmNfileListLabelString);
        }
        if(mvsGetResNumInArgs(args_in, (int)nargs,XmNlistLabelString)
                                                              != NON_EXISTENT) {
            (void)mvsAssignRes(widget_info, XmNfileListLabelString,
                               XmNlistLabelString);
        }
        if(mvsGetResNumInArgs(args_in, (int)nargs,XmNdirSpec) != NON_EXISTENT) {
            (void)mvsAssignRes(widget_info, XmNtextString, XmNdirSpec);
        }
        if(mvsGetResNumInArgs(args_in,(int)nargs,XmNtextString)!=NON_EXISTENT) {
            (void)mvsAssignRes(widget_info, XmNdirSpec, XmNtextString);
        }
    }
    if(w_c_info == iXmSelectionBoxWidgetClass) {
        int diag;

        diag = mvsGetResNumInArgs(args_in, (int)nargs,XmNdialogType);
        if((diag != NON_EXISTENT) && 
	   (args_in[diag].value == (XtArgVal) XmDIALOG_PROMPT)) {
            (void)mvsAssignVal(widget_info, XmNlistLabelString,(MvsArgVal)NULL);
        }
    }
                         
    /* This section is the "Free Pass" for non-specification of
       geometry management.
    */
    ln = 0;
    XtSetArg(loc_args[ln], XmNwidth,&width); ln++;
    XtSetArg(loc_args[ln], XmNheight,&height); ln++;
    XtGetValues(widget_info->widget,loc_args,ln);
    XtSetArg(args[n], XmNwidth,width); n++;
    XtSetArg(args[n], XmNheight,height); n++;

    mvsSetLocalValues(widget_info,args,&n);
    in_SetLocalConstraints = FALSE;

} /* End mvsSetLocalConstraints() */


void mvsSetLocalStartConstraints(widget_info,resources, args, nargs)
MvsWidgetInfoRecord *widget_info;
MvsArgVal resources[];
ArgList args;
Cardinal nargs;
{
    mvsSetLocalConstraints(widget_info,resources,args,nargs);

} /* End mvsSetLocalStartConstraints() */
