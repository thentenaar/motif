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
static char rcsid[] = "$XConsortium: CreatChildrn.c /main/8 1995/07/14 11:24:23 drk $"
#endif
#endif
/***********************************************************************
  	@(#)CreatChildrn.c	1.9.1.1	Date:1/22/91
  	Author: TAT
	History:
	   06/07/90 TAT Add to sccs tree
	   06/15/90 SJS add copyright
	   06/26/90 SJS add Dialog Shell convenience function checks
	   06/29/90 SJS correct PanedWindow/ScrolledWindow case
	   07/18/90 SJS add test for Null in SelectionBox case
	   07/27/90 SJS take out Paned Window ifdef
	   08/02/90 SJS add FileSelectionBox, add Null checks for ScrolledW
	Calls:

	Summary:
            Creates all the widget infos of the specified widget's children
        and allocs their colors.

        INPUTS
            widget_info  - Pointer to the mvs widget info record

        OUTPUTS
            none

        RETURNS
            nothing
************************************************************************/

#include "mvslib.h"

#include <X11/Shell.h>
#include <X11/Vendor.h>
#include <X11/Xproto.h>
#include <Xm/ArrowB.h>
#include <Xm/ArrowBG.h>
#include <Xm/BulletinB.h>
#include <Xm/CascadeB.h>
#include <Xm/CascadeBG.h>
#include <Xm/ComboBox.h>
#include <Xm/Command.h>
#include <Xm/Container.h>
#include <Xm/ContainerP.h>    /* Possible non-portability */
#include <Xm/CutPaste.h>
#include <Xm/DialogS.h>
#include <Xm/DrawingA.h>
#include <Xm/DrawnB.h>
#include <Xm/FileSB.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/IconG.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/List.h>
#include <Xm/MainW.h>
#include <Xm/MenuShell.h>
#include <Xm/MessageB.h>
#include <Xm/Notebook.h>
#include <Xm/PanedW.h>
#include <Xm/ProtocolsP.h>    /* Possible non-portability */
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/RowColumnP.h>    /* Possible non-portability */
#include <Xm/SashP.h>         /* Possible non-portability */
#include <Xm/Scale.h>
#include <Xm/ScrollBar.h>
#include <Xm/ScrolledW.h>
#include <Xm/SelectioB.h>
#include <Xm/SeparatoG.h>
#include <Xm/Separator.h>
#include <Xm/SpinB.h>
#include <Xm/Text.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>

#ifndef MOTIF1_1
#include <Xm/TearOffBP.h>
#endif /* MOTIF1_1 */

static void UpdatePanes();

void mvsCreateChildren(widget_info)
MvsWidgetInfoRecord *widget_info;
{
    int class_code = mvsGetClassCode(widget_info->widget);
    Widget widget = widget_info->widget;
    Widget child;
    MvsWidgetInfoRecord *child_info;
    int num_children;
    Widget *children;
    Widget tear_off;
    XmContainerConstraint containerConstraint;
    int i, n, pathMode;
    Arg args[4];

    if (widget_info->parent != NULL && widget_info->parent->widget != NULL)
    	if (mvsGetClassCode(widget_info->parent->widget) 
				== mvsXmPanedWindowWidgetClass)
		UpdatePanes(widget_info->parent);

    switch (class_code) {

        case mvsXmDialogShellWidgetClass:
            num_children = (int)mvsGetResourceValue(widget,XmNnumChildren,
                                                        T_Cardinal);
            children = (Widget *)mvsGetResourceValue(widget,XmNchildren,
                                                         T_WidgetList);
            for (i=0; i<num_children; i++) {

                class_code = mvsGetClassCode(children[i]);
                if (class_code == mvsXmMessageBoxWidgetClass) {
                    child_info = mvsSetupNewWidgetInfo(widget_info, children[i],
                                                      iXmMessageBoxWidgetClass);
                    mvsCreateChildren(child_info);
                }
                else {
                    msg_trace("Unimplemented child of Dialog\n");
                }
            }

            break;

        case mvsXmContainerWidgetClass:

	    child = XtNameToWidget(widget,"Header");

	    if (child != (Widget) 0) {
		child_info = mvsSetupNewWidgetInfo(widget_info, child,
						   iXmIconGadgetClass);
		mvsCreateChildren(child_info);
	    }

            break;

        case mvsXmIconGadgetClass:

	    /*
	     * Need to determine if the parent is a Container
	     */
	    if(strcmp (widget_info->parent->widget_class_info->class_name,
		       "XmContainer") == 0)  {

	    	/* 
	     	* Get the Container constraint part from the IconGadget
	     	* and get the outline button widget from the constraint part
	     	*/
	    	containerConstraint = GetContainerConstraint(widget);
	    	child = containerConstraint->related_cwid;

	    	/*
	     	* Set the Outline Button as an mvs child of 
	     	* the IconGadget, although it is a child of
	     	* the Container widget
	     	*/
            	if (child != NULL) {
                    if (XmIsGadget(child))
                    	child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                       iXmPushButtonGadgetClass);
	    	}
	    }  /* end if parent is a Container */

            break;

        case mvsXmCommandWidgetClass:

            child = XmCommandGetChild(widget,XmDIALOG_COMMAND_TEXT);
            child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                               iXmTextWidgetClass);

            child = XmCommandGetChild(widget,XmDIALOG_PROMPT_LABEL);
            if (child != NULL) {
                if (XmIsGadget(child))
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                       iXmLabelGadgetClass);
                else
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                       iXmLabelWidgetClass);
	    }

            child = XmCommandGetChild(widget,XmDIALOG_HISTORY_LIST);
            child = XtParent(child); /* Get scrolled window parent */

            child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                               iXmScrolledWindowWidgetClass);
            mvsCreateChildren(child_info);
            break;

	case mvsXmComboBoxWidgetClass:

	    child = XtNameToWidget(widget,"*List");
            child = XtParent(child); /* Get scrolled window parent */
            child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                               iXmScrolledWindowWidgetClass);
            mvsCreateChildren(child_info);

	    child = XtNameToWidget(widget,"Text");
            child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                               iXmTextFieldWidgetClass);
            mvsCreateChildren(child_info);
	    break;

	case mvsXmNotebookWidgetClass:

            child = XtNameToWidget(widget,"MajorTabScrollerNext");
            if (child != NULL) {
                if (XmIsGadget(child)) 
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                      iXmArrowButtonGadgetClass);
                else
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                      iXmArrowButtonWidgetClass);
	    }

            child = XtNameToWidget(widget,"MajorTabScrollerPrevious");
            if (child != NULL) {
                if (XmIsGadget(child)) 
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                      iXmArrowButtonGadgetClass);
                else
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                      iXmArrowButtonWidgetClass);
	    }

            child = XtNameToWidget(widget,"MinorTabScrollerNext");
            if (child != NULL) {
                if (XmIsGadget(child)) 
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                      iXmArrowButtonGadgetClass);
                else
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                      iXmArrowButtonWidgetClass);
	    }

            child = XtNameToWidget(widget,"MinorTabScrollerPrevious");
            if (child != NULL) {
                if (XmIsGadget(child)) 
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                      iXmArrowButtonGadgetClass);
                else
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                      iXmArrowButtonWidgetClass);
	    }
	    break;

        case mvsXmFileSelectionBoxWidgetClass:

            /* See if this is a CDE FileSelectionBox */
            n = 0;
            XtSetArg (args[n], XmNpathMode, &pathMode);   n++;
            XtGetValues (widget, args, n);
            if (pathMode == XmPATH_MODE_RELATIVE)  {
		child = XtNameToWidget(widget,"DirText");
		if (child != NULL) {
		    child_info = mvsSetupNewWidgetInfo(widget_info,child,
						       iXmTextFieldWidgetClass);
		}
		child = XtNameToWidget(widget,"DirL");
		if (child != NULL) {
		    child_info = mvsSetupNewWidgetInfo(widget_info,child,
						       iXmLabelGadgetClass);
		}
            }

 
            child = XmFileSelectionBoxGetChild(widget,XmDIALOG_APPLY_BUTTON);
            if (child != NULL) {
                if (XmIsGadget(child)) 
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                      iXmPushButtonGadgetClass);
                else
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                      iXmPushButtonWidgetClass);
	    }

            child =XmFileSelectionBoxGetChild(widget,XmDIALOG_CANCEL_BUTTON);
            if (child != NULL) {
                if (XmIsGadget(child)) 
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                      iXmPushButtonGadgetClass);
                else
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                      iXmPushButtonWidgetClass);
	    }

            child = XmFileSelectionBoxGetChild(widget,XmDIALOG_FILTER_LABEL);
            if (child != NULL) {
                if (XmIsGadget(child)) 
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                       iXmLabelGadgetClass);
                else
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                       iXmLabelWidgetClass);
	    }

            child = XmFileSelectionBoxGetChild(widget,XmDIALOG_FILTER_TEXT);
            if (child != NULL) {
                child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                   iXmTextWidgetClass);
            }
            
            child = XmFileSelectionBoxGetChild(widget,XmDIALOG_HELP_BUTTON);
            if (child != NULL) {
                if (XmIsGadget(child)) 
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                      iXmPushButtonGadgetClass);
                else
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                      iXmPushButtonWidgetClass);
	    }

            child = XmFileSelectionBoxGetChild(widget,XmDIALOG_FILE_LIST);
            child = XtParent(child); /* Get scrolled window parent */
            child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                               iXmScrolledWindowWidgetClass);
            mvsCreateChildren(child_info);

            child = XmFileSelectionBoxGetChild(widget,XmDIALOG_FILE_LIST_LABEL);
            if (child != NULL) {
                if (XmIsGadget(child)) 
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                       iXmLabelGadgetClass);
                else
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                       iXmLabelWidgetClass);
	    }
            
            child = XmFileSelectionBoxGetChild(widget,XmDIALOG_OK_BUTTON);
            if (child != NULL) {
                if (XmIsGadget(child)) 
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                      iXmPushButtonGadgetClass);
                else
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                      iXmPushButtonWidgetClass);
	    }

            child = XmFileSelectionBoxGetChild(widget,XmDIALOG_SEPARATOR);
            if (child != NULL) {
                if (XmIsGadget(child)) 
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                       iXmSeparatorGadgetClass);
                else
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                       iXmSeparatorWidgetClass);
	    }

            child = XmFileSelectionBoxGetChild(widget,XmDIALOG_SELECTION_LABEL);
            if (child != NULL) {
                if (XmIsGadget(child)) 
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                       iXmLabelGadgetClass);
                else
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                       iXmLabelWidgetClass);
	    }

            child = XmFileSelectionBoxGetChild(widget,XmDIALOG_TEXT);
            child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                               iXmTextWidgetClass);

            child = XmFileSelectionBoxGetChild(widget,XmDIALOG_DIR_LIST_LABEL);
            if (child != NULL) {
                if (XmIsGadget(child)) 
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                       iXmLabelGadgetClass);
                else
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                       iXmLabelWidgetClass);
	    }

            child = XmFileSelectionBoxGetChild(widget,XmDIALOG_DIR_LIST);
            child = XtParent(child); /* Get scrolled window parent */
            child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                               iXmScrolledWindowWidgetClass);
            mvsCreateChildren(child_info);
            break;

        case mvsXmMainWindowWidgetClass:
            child = (Widget)mvsGetResourceValue(widget,XmNcommandWindow,
                                                   T_Widget_ref);
            if(child != NULL) {
                child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                   iXmTextWidgetClass);
            }
            child = (Widget)mvsGetResourceValue(widget,XmNmenuBar,
                                                   T_Widget_ref);
            if(child != NULL) {
                child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                   iXmRowColumnWidgetClass);
            }
            child = (Widget)mvsGetResourceValue(widget,XmNmessageWindow,
                                                   T_Widget_ref);
            if(child != NULL) {
                 child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                    iXmTextWidgetClass);
            }
            child = (Widget)mvsGetResourceValue(widget,XmNhorizontalScrollBar,
                                                   T_Widget_ref);
            if(child != NULL) {
                child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                   iXmScrollBarWidgetClass);
            }
            child = (Widget)mvsGetResourceValue(widget,XmNverticalScrollBar,
                                                   T_Widget_ref);
            if(child != NULL) {
                child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                   iXmScrollBarWidgetClass);
            }
            child = XmMainWindowSep1(widget);
            if (child != NULL) {
                if (XmIsGadget(child)) 
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                       iXmSeparatorGadgetClass);
                else
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                       iXmSeparatorWidgetClass);
	    }

            child = XmMainWindowSep2(widget);
            if (child != NULL) {
                if (XmIsGadget(child)) 
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                       iXmSeparatorGadgetClass);
                else
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                       iXmSeparatorWidgetClass);
	    }

            child = XmMainWindowSep3(widget);
            if (child != NULL) {
                if (XmIsGadget(child)) 
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                       iXmSeparatorGadgetClass);
                else
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                       iXmSeparatorWidgetClass);
	    }

            break;

        case mvsXmMessageBoxWidgetClass:

            child = XmMessageBoxGetChild(widget,XmDIALOG_CANCEL_BUTTON);
            if (child != NULL) {
                if (XmIsGadget(child)) 
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                      iXmPushButtonGadgetClass);
                else
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                      iXmPushButtonWidgetClass);
	    }

            child = XmMessageBoxGetChild(widget,XmDIALOG_HELP_BUTTON);
            if (child != NULL) {
                if (XmIsGadget(child)) 
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                      iXmPushButtonGadgetClass);
                else
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                      iXmPushButtonWidgetClass);
	    }

            child = XmMessageBoxGetChild(widget,XmDIALOG_MESSAGE_LABEL);
            if (child != NULL) {
                if (XmIsGadget(child)) 
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                       iXmLabelGadgetClass);
                else
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                       iXmLabelWidgetClass);
	    }

            child = XmMessageBoxGetChild(widget,XmDIALOG_OK_BUTTON);
            if (child != NULL) {
                if (XmIsGadget(child)) 
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                      iXmPushButtonGadgetClass);
                else
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                      iXmPushButtonWidgetClass);
	    }

            child = XmMessageBoxGetChild(widget,XmDIALOG_SEPARATOR);
            if (child != NULL) {
                if (XmIsGadget(child)) 
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                       iXmSeparatorGadgetClass);
                else
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                       iXmSeparatorWidgetClass);
	    }

            child = XmMessageBoxGetChild(widget,XmDIALOG_SYMBOL_LABEL);
            if (child != NULL) {
                if (XmIsGadget(child)) 
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                       iXmLabelGadgetClass);
                else
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                       iXmLabelWidgetClass);
	    }

            break;

        case mvsXmPanedWindowWidgetClass:

            num_children = (int)mvsGetResourceValue(widget,XmNnumChildren,
                                                    T_Cardinal);
            children = (Widget *)mvsGetResourceValue(widget,XmNchildren,
                                                     T_WidgetList);

            for (i=0; i<num_children; i++) {

                class_code = mvsGetClassCode(children[i]);

                if (class_code == mvsXmSashWidgetClass)
                    child_info = mvsSetupNewWidgetInfo(widget_info,children[i],
                                                       iXmSashWidgetClass);
                else if (class_code == mvsXmSeparatorGadgetClass) {
                    if (XmIsGadget(children[i]))
                        child_info = mvsSetupNewWidgetInfo(widget_info,
                                                 children[i],
                                                 iXmSeparatorGadgetClass);
                    else
                        child_info = mvsSetupNewWidgetInfo(widget_info,
                                                 children[i],
                                                 iXmSeparatorWidgetClass);
		}
            }
            break;

        case mvsXmScaleWidgetClass:

            num_children = (int)mvsGetResourceValue(widget,XmNnumChildren,
                                                    T_Cardinal);
            children = (Widget *)mvsGetResourceValue(widget,XmNchildren,
                                                     T_WidgetList);

            for (i=0; i<num_children; i++) {
                class_code = mvsGetClassCode(children[i]);

                if (class_code == mvsXmScrollBarWidgetClass)
                    child_info = mvsSetupNewWidgetInfo(widget_info,
                                                 children[i],
                                                 iXmScrollBarWidgetClass);
                else if (class_code == mvsXmLabelGadgetClass)
                    child_info = mvsSetupNewWidgetInfo(widget_info,
                                                 children[i],
                                                 iXmLabelGadgetClass);
                else if (class_code == mvsXmLabelWidgetClass)
                    child_info = mvsSetupNewWidgetInfo(widget_info,
                                                 children[i],
                                                 iXmLabelWidgetClass);
                else if (class_code == mvsXmSeparatorGadgetClass) {
                    if (XmIsGadget(children[i]))
                        child_info = mvsSetupNewWidgetInfo(widget_info,
                                                 children[i],
                                                 iXmSeparatorGadgetClass);
                    else
                        child_info = mvsSetupNewWidgetInfo(widget_info,
                                                 children[i],
                                                 iXmSeparatorWidgetClass);
	        }
		else
		    printf("Error, in mvsXmScaleWidgetClass\n");
            }
            break;

        case mvsXmScrolledWindowWidgetClass:

            child = (Widget)mvsGetResourceValue(widget,XmNhorizontalScrollBar,
                                                   T_Widget_ref);
            if (child != NULL) {
                child_info = mvsSetupNewWidgetInfo(widget_info, child,
                                                   iXmScrollBarWidgetClass);
            }

            child = (Widget)mvsGetResourceValue(widget,XmNverticalScrollBar,
                                                   T_Widget_ref);
            if (child != NULL) {
                child_info = mvsSetupNewWidgetInfo(widget_info, child,
                                                   iXmScrollBarWidgetClass);
            }

            child = (Widget)mvsGetResourceValue(widget,XmNworkWindow,
                                                T_Widget_ref);

            if (child != NULL) {
                class_code = mvsGetClassCode(child);
                if (class_code == mvsXmListWidgetClass)
                    child_info = mvsSetupNewWidgetInfo(widget_info,
						       child,
						       iXmListWidgetClass);
                else if (class_code == mvsXmTextWidgetClass)
                    child_info = mvsSetupNewWidgetInfo(widget_info,
                                                       child,
                                                       iXmTextWidgetClass);
	    }

            child = (Widget)mvsGetResourceValue(widget,XmNclipWindow,
                                                   T_Widget_ref);
            if (child != NULL) {
                child_info = mvsSetupNewWidgetInfo(widget_info, child,
                                                   iXmDrawingAreaWidgetClass);
            }

            break;

        case mvsXmSelectionBoxWidgetClass:

            child = XmSelectionBoxGetChild(widget,XmDIALOG_APPLY_BUTTON);
            if (child != NULL) {
                if (XmIsGadget(child)) 
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                     iXmPushButtonGadgetClass);
                else
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                     iXmPushButtonWidgetClass);
	    }

            child = XmSelectionBoxGetChild(widget,XmDIALOG_CANCEL_BUTTON);
            if (child != NULL) {
                if (XmIsGadget(child)) 
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                     iXmPushButtonGadgetClass);
                else
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                     iXmPushButtonWidgetClass);
	    }

            child = XmSelectionBoxGetChild(widget,XmDIALOG_HELP_BUTTON);
            if (child != NULL) {
                if (XmIsGadget(child)) 
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                     iXmPushButtonGadgetClass);
                else
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                     iXmPushButtonWidgetClass);
	    }

            child = XmSelectionBoxGetChild(widget,XmDIALOG_LIST);
            if (child != NULL) {
                child = XtParent(child); /* Get Scrolled window parent */
                child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                               iXmScrolledWindowWidgetClass);
	        mvsCreateChildren(child_info);
            }

            child = XmSelectionBoxGetChild(widget,XmDIALOG_LIST_LABEL);
            if (child != NULL) {
                if (XmIsGadget(child)) 
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                       iXmLabelGadgetClass);
                else
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                       iXmLabelWidgetClass);
	    }

            child = XmSelectionBoxGetChild(widget,XmDIALOG_OK_BUTTON);
            if (child != NULL) {
                if (XmIsGadget(child)) 
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                      iXmPushButtonGadgetClass);
                else
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                      iXmPushButtonWidgetClass);
	    }

            child = XmSelectionBoxGetChild(widget,XmDIALOG_SEPARATOR);
            if (child != NULL) {
                if (XmIsGadget(child)) 
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                       iXmSeparatorGadgetClass);
                else
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                       iXmSeparatorWidgetClass);
	    }

            child = XmSelectionBoxGetChild(widget,XmDIALOG_SELECTION_LABEL);
            if (child != NULL) {
                if (XmIsGadget(child)) 
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                       iXmLabelGadgetClass);
                else
                    child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                       iXmLabelWidgetClass);
	    }

            child = XmSelectionBoxGetChild(widget,XmDIALOG_TEXT);
            if (child != NULL) {
                child_info = mvsSetupNewWidgetInfo(widget_info,child,
                                                   iXmTextWidgetClass);
            }

            break;

        case mvsXmRowColumnWidgetClass:

	    /* interim use of widget structure, later use convenience routine */

#ifndef MOTIF1_1

	    tear_off = ((XmRowColumnWidget)widget)->row_column.tear_off_control;
	    if (tear_off != NULL) {
		child_info = mvsSetupNewWidgetInfo(widget_info,tear_off,
					           iXmTearOffButtonWidgetClass);
	    }

#endif /* MOTIF1_1 */

            break;

        default:
	    /* Do nothing */
	    break;
    } /* End switch() */

} /* End mvsCreateChildren() */


static void UpdatePanes(widget_info)
MvsWidgetInfoRecord *widget_info;
{
   Widget widget = widget_info->widget;
   int class_code = mvsGetClassCode(widget_info->widget);
   Widget child;
   MvsWidgetInfoRecord *child_info;
   MvsWidgetInfoRecord *w_info = NULL;
   int num_children;
   Widget *children;
   int i;

   num_children = (int)mvsGetResourceValue(widget,XmNnumChildren,
                                           T_Cardinal);
   children = (Widget *)mvsGetResourceValue(widget,XmNchildren,
                                            T_WidgetList);

   for (i=0; i<num_children; i++) {

     class_code = mvsGetClassCode(children[i]);
     w_info = mvsWidgetToWidgetInfo(children[i]);

     if (class_code == mvsXmSashWidgetClass && w_info == NULL)
               child_info = mvsSetupNewWidgetInfo(widget_info,children[i],
                                                  iXmSashWidgetClass);
     else if (class_code == mvsXmSeparatorGadgetClass && w_info == NULL) {
               if (XmIsGadget(children[i]))
                    child_info = mvsSetupNewWidgetInfo(widget_info,
                                                       children[i],
                                                       iXmSeparatorGadgetClass);
               else
                   child_info = mvsSetupNewWidgetInfo(widget_info,
                                                      children[i],
                                                      iXmSeparatorWidgetClass);
     }
     w_info = NULL;

   }

}
