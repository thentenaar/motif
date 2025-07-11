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
static char rcsid[] = "$XConsortium: GetSubObjcts.c /main/11 1995/07/14 10:58:57 drk $"
#endif
#endif
/***********************************************************************
	Calls:

	Summary:
            This function returns the Object hierarchy associated
        with a given widget.
************************************************************************/

#include <AutoMessages.h>
#include "mvslib.h"
#include <stdlib.h>	      /* for MB_CUR_MAX */

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
#include <Xm/NotebookP.h>     /* Possible non-portability */
#include <Xm/PanedW.h>
#include <Xm/ProtocolsP.h>    /* Possible non-portability */
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/SashP.h>         /* Possible non-portability */
#include <Xm/Scale.h>
#include <Xm/ScrollBar.h>
#include <Xm/ScrolledW.h>
#include <Xm/SelectioB.h>
#include <Xm/SeparatoG.h>
#include <Xm/Separator.h>
#include <Xm/SpinB.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>

#define	MIN_SLIDER_SIZE	4
/*
 * Need this huge number because Notebook is a pig
 */
#define	MAX_NUMBER_OF_CHILDREN	10000


/*
 *	Restructured the routine into distinct data section and code section.
 *	Also, changed the Text widget part of the code to try and get the font
 *	from the text widget's own fontlist; if no valid font can be obtained
 *	from the text widget, then use the final fallback font from the
 *	font_info array.
 */
XisObjectRecord *mvsGetSubObjects(parent_obj,widget,object_type,instance,
	PreDefComposite)

XisObjectRecord *parent_obj;
Widget widget;
int object_type;
int instance;
int PreDefComposite;
{
    XisObjectRecord *object, *child_obj = NULL;
    Widget child[MAX_NUMBER_OF_CHILDREN];
    int border_width;
    int num_children,num_predefined;
    int user_defined_num,sash_num,separator_num;
    Widget *children = NULL;
    MvsWidgetInfoRecord *winfo;
    int class_code;
    int i=0;
    int j, widget_class_code;
    static short next_avail_id_code=1;
    unsigned char scroll_p;
    MvsArgVal *mvs_resources;
    MvsWidgetInfoRecord *w_info;

    /* mvsXmComboBoxWidgetClass */
    int cb_x, cb_y, cb_width;
    int stringDirection;
    int cb_marginWidth;
    int cb_marginHeight;
    int cb_highlightThickness;
    int cb_shadowThickness;
    int arrowSize;
    unsigned char combobox_type;
    XisObjectRecord *CBDropDwnBtn;

    /* mvsXmSpinBoxWidgetClass */
    int sb_x, sb_y, sb_width, sb_height;
    int layoutDirection;
    int sb_marginWidth;
    int sb_marginHeight;
    unsigned char arrowLayout;
    XisObjectRecord *SBIncrementBtn, *SBDecrementBtn;

    /* mvsXmContainerWidgetClass */
    int cont_class_code;
    int num_icon_gadgets;
    int num_pushb_tabs;

    /* mvsXmIconGadgetClass */
    XmContainerConstraint containerConstraint;
    Widget OutlineBtnWidget, ParentIconGadget;
    int ContainerWidgetX, ContainerWidgetY;
    unsigned char OutlineBtnPolicy;
    XisObjectRecord *OutlineBtnObject, *OutlineBtnRegion;
    XisObjectRecord *ParentIGObject, *IG_current_object;
    Boolean found_IGObject;

    /* mvsXmFileSelectionBoxWidgetClass */
    unsigned char pathMode;

    /* mvsXmRowColumnWidgetClass */
    unsigned char  rc_type;
    Widget tear_off;

    /* mvsXmScrollBarWidgetClass: */
    int sb_w, sb_h;
    Widget w;
    XisObjectRecord *slider, *arrow[2],*trough[2];
    int showArrows;
    int orientation;
    int sliderSize;
    int processingDirection;
    int hl, sh;
    int user_min, user_max, user_value;
    int arrow_size,arrow_length,crunched_arrow_size;
    int trough_length,trough_x,trough_y,trough_breadth;
    float trueSize, userSize, factor, slideStart, slideSize; 
    int slider_x,slider_y,slider_width,slider_height,slider_offset;

    /* mvsXmListWidgetClass */
    int l_x, l_y;
    int l_w, l_h;
    XmFontList fontList;
    int visibleItemsCount;
    int listMarginHeight;
    int listMarginWidth;
    int borderWidth;
    int listSpacing;
    int topItemPosition;
    int shadowThickness;
    int highlightThickness;
    XmStringTable items;
    int itemCount;
    int num_visible_items;
    int curr_y,curr_x;
    Dimension s_h;

    /* mvsNotebookWidgetClass */
    MvsWidgetInfoRecord *child_info, *widget_info;
    static int created_page_scroller = False;

    /* mvsXmTextWidgetClass */
    int t_x, t_y;
    int t_w, t_h;
    char *string;
    int stringLength;
    int marginHeight;
    int marginWidth;
    int topCharacter;
    int lineHeight;
    int start_x;
    XFontStruct *fs = NULL;
    XmFontContext font_context;
    XmStringCharSet charset;

    int 	  	n;
    Arg		  	args[25];

    if (parent_obj == NULL_Object && widget == NULL) {
        next_avail_id_code = 1;
        return(0);
    }

    if(widget == NULL)
        return(NULL_Object);

    if( (!XtIsWidget(widget)) && (!XmIsGadget(widget)))
        return(NULL_Object);

    if(!XtIsRealized(widget))
        return(NULL_Object);

    if((object = xisFindObjectFromWidget(widget)) != NULL_Object)
        return(object);

    if(XtIsWidget(widget)) {

        /* Select real window parent as parent_obj if different */
        if ( XtIsShell(widget) ) {
            parent_obj = (&xisObjects[0]);
        }
        else if ( XtWindow(XtParent(widget)) != parent_obj->id.window ) {
            parent_obj = xisFindParentObjectFromWindow(XtWindow(widget));
            if (parent_obj == NULL_Object) {
                winfo = mvsWidgetToWidgetInfo(XtParent(widget));
                if (winfo == NULL) {
                    parent_obj = (&xisObjects[0]);
                }
                else {
                    if (winfo->parent->window != 0)
                        object = xisFindObjectFromWindow(winfo->parent->window);
                    else
                        object = (&xisObjects[0]);

                    parent_obj = mvsGetSubObjects(object,winfo->widget,0,0,
                                                  False);
                    object = xisFindObjectFromWidget(widget);
                    if (object != NULL_Object)
                        return(object);
                }
            }
        } /* End if( XtWindow(XtParent(widget)) != parent_obj->id.window) */
    } /* End  if(XtIsWidget(widget)) */

    object = xisAllocObject(parent_obj,widget,object_type,instance);

    if (XtIsWidget(widget) || XmIsGadget(widget)) {
        mvsGetWidgetGeometry(widget,&object->x,&object->y,&object->width,
                             &object->height,&border_width);
        object->id_code = next_avail_id_code++;
        /*  NOOOO!!!!!!!!!!!!
        object->width += 2*border_width;
        object->height += 2*border_width;
        */
	/* Since RowColumn adds entryBorder to its children's window fix it! */
	/* Do not want to subtract off borderWidth becuase RowColumn
	   is configuring it children with new border_width = entryborder.
	if (mvsIsParentRowColumn(mvsWidgetToWidgetInfo(widget))) {
            object->x -= border_width;
            object->y -= border_width;
	}
	*/
    }
    else {
        object->id_code = 0; /* Nonportable object not to be counted */
        object->x = object->y = (-10);
        object->width = object->height = 0;
        border_width = 0;
    }

    winfo = (MvsWidgetInfoRecord *)mvsWidgetToWidgetInfo(widget);
    if ((mvsInBehavior) && (winfo != NULL) && (!PreDefComposite)) {
        object->proc_InformExpectedActions =
                              winfo->widget_class_info->proc_SetExpectedActions;
    }
    else
        object->proc_InformExpectedActions = NULL;

    num_predefined = 0;

    if (widget == NULL)
        widget_class_code = mvsVendorShellWidgetClass;
    else
        widget_class_code = mvsGetClassCode(widget);
 
    switch (widget_class_code) {
        case mvsVendorShellWidgetClass:
        case mvsXmBulletinBoardWidgetClass:
        case mvsXmDesktopObjectClass:
        case mvsXmDialogShellWidgetClass:
        case mvsXmExtObjectClass:
        case mvsXmFormWidgetClass:
        case mvsXmFrameWidgetClass:
        case mvsXmManagerWidgetClass:
        case mvsXmMenuShellWidgetClass:
        case mvsXmProtocolObjectClass:
        case mvsXmShellExtObjectClass:
        case mvsXmVendorShellExtObjectClass:
	case mvsApplicationShellWidgetClass:
	case mvsCompositeWidgetClass:
	case mvsCoreWidgetClass:
	case mvsObjectClass:
	case mvsOverrideShellWidgetClass:
	case mvsRectObjClass:
	case mvsShellWidgetClass:
	case mvsTopLevelShellWidgetClass:
	case mvsTransientShellWidgetClass:
	case mvsWMShellWidgetClass:
            user_defined_num = 0;

            if(XtIsComposite(widget)) {
                num_children = (int)mvsGetResourceValue(widget,XmNnumChildren,
                                                        T_Cardinal);
                children = (Widget *)mvsGetResourceValue(widget,XmNchildren,
                                                         T_WidgetList);

                for (i=num_children-1; i>=0; i--) {
                    for (j=0; j<num_predefined; j++) {
                        if (children[i] == child[j])
                            break;
                    }
                    if (j < num_predefined)
                        continue;

                    child_obj = mvsGetSubObjects(object,children[i],
                                                 oUserDefined,
                                                 user_defined_num++,False);         
                }
            }
            break;

        case mvsXmArrowButtonWidgetClass:
        case mvsXmArrowButtonGadgetClass:
        case mvsXmCascadeButtonWidgetClass:
        case mvsXmCascadeButtonGadgetClass:
        case mvsXmDrawingAreaWidgetClass:
        case mvsXmDrawnButtonWidgetClass:
        case mvsXmGadgetClass:
        case mvsXmLabelWidgetClass:
        case mvsXmLabelGadgetClass:
        case mvsXmPrimitiveWidgetClass:
        case mvsXmPushButtonGadgetClass:
        case mvsXmSashWidgetClass:
        case mvsXmSeparatorGadgetClass:
        case mvsXmSeparatorWidgetClass:
        case mvsXmToggleButtonWidgetClass:
        case mvsXmToggleButtonGadgetClass:
	case mvsXmTearOffButtonWidgetClass:
            /* nothing to do! */
            break;


        case mvsXmCommandWidgetClass:

            i=0;
            child[i] = XmCommandGetChild(widget,XmDIALOG_COMMAND_TEXT);
            child_obj = mvsGetSubObjects(object,child[i],oSelectionText,0,True);
            i++;

            child[i] = XmCommandGetChild(widget,XmDIALOG_PROMPT_LABEL);
            child_obj = mvsGetSubObjects(object,child[i],oLabel,0,True);
            i++;
            
            child[i] = XmCommandGetChild(widget,XmDIALOG_HISTORY_LIST);
            child_obj = mvsGetSubObjects(object,child[i],
                                         oSelectionList,0,True);
            if(child_obj != NULL) {
                    child_obj->parent->id.object_type = oScrolledWindow;
                    child_obj->parent->id.instance = 0;
                    child_obj->id.object_type = oSelectionList;
            }
            i++;

            user_defined_num = 0;
            num_predefined = i;

            num_children = (int)mvsGetResourceValue(widget,XmNnumChildren,
                                                    T_Cardinal);
            children = (Widget *)mvsGetResourceValue(widget,XmNchildren,
                                                     T_WidgetList);

            for (i=num_children-1; i>=0; i--) {
                for (j=0; j<num_predefined; j++) {
                    if (children[i] == child[j])
                        break;
                }
                if (j < num_predefined)
                    continue;

                child_obj = mvsGetSubObjects(object,children[i],oUserDefined,
                                             user_defined_num++,True);         
            }
            break;


	case mvsXmNotebookWidgetClass:

	    i = 0;

	    child[i] = XtNameToWidget(widget,"MajorTabScrollerNext");
	    child_obj = mvsGetSubObjects(object, child[i], 
					 oMajorTabScrollerNext,0,True);
	    i++;

	    child[i] = XtNameToWidget(widget,"MajorTabScrollerPrev");
	    child_obj = mvsGetSubObjects(object, child[i], 
					 oMajorTabScrollerPrev,0,True);
	    i++;

	    child[i] = XtNameToWidget(widget,"MinorTabScrollerNext");
	    child_obj = mvsGetSubObjects(object, child[i], 
					 oMinorTabScrollerNext,0,True);
	    i++;

	    child[i] = XtNameToWidget(widget,"MinorTabScrollerPrev");
	    child_obj = mvsGetSubObjects(object, child[i], 
					 oMinorTabScrollerPrev,0,True);
	    i++;
            
	    separator_num = 0;
            user_defined_num = 0;
            num_predefined = i;

            num_children = (int)mvsGetResourceValue(widget,XmNnumChildren,
                                                    T_Cardinal);
            children = (Widget *)mvsGetResourceValue(widget,XmNchildren,
                                                     T_WidgetList);

            for (i=num_children-1; i>=0; i--) {
                for (j=0; j<num_predefined; j++) {
                    if (children[i] == child[j])
                        break;
                }
                if (j < num_predefined)
                    continue;

                /* Any other user defined children */
                child_obj= mvsGetSubObjects(object,children[i],oUserDefined,
                                            user_defined_num++,False);  
            }
            break;


	case mvsXmContainerWidgetClass:

	    num_icon_gadgets = 0;
	    num_pushb_tabs = 0;
            user_defined_num = 0;

            num_children = (int)mvsGetResourceValue(widget,XmNnumChildren,
                                                    T_Cardinal);
            children = (Widget *)mvsGetResourceValue(widget,XmNchildren,
                                                     T_WidgetList);

	    for (i=0; i < num_children; i++)  {
		cont_class_code = mvsGetClassCode(children[i]);

		/*
		switch (cont_class_code)
		    {
                    case mvsXmPushButtonGadgetClass:
                    	child_obj = mvsGetSubObjects(object,
                                           	     children[i],oListItem,
                                           	     num_pushb_tabs++,True);
                        break;
                    case mvsXmIconGadgetClass:
                        child_obj = mvsGetSubObjects(object,
                                           	     children[i],oIconGadget,
                                           	     num_icon_gadgets++,True);
                        break;
                    default:
                        child_obj = mvsGetSubObjects(object,
                                        	     children[i],oUserDefined,
                                                     user_defined_num++,False);
                    }       * end case *
		*/
	    }       /* end for statement */
            break;

        case mvsXmIconGadgetClass:

	    /*
	     * If the parent is not a Container, there
	     * are no children
	     */
	    if (strcmp (parent_obj->id.widget->core.widget_class->core_class.class_name,
			"XmContainer") != 0)
		break;

            /*
             * Get the Container widget x and y
             */
            ContainerWidgetX = parent_obj->x;
            ContainerWidgetY = parent_obj->y;

            found_IGObject = False;

            /*
             * Get the parent IG, if any exist, for this IG
             */
            n = 0;
            XtSetArg(args[n], XmNentryParent, &ParentIconGadget);  n++;
            XtGetValues(widget, args, n);

            /*
             * See if the outlineButtonPolicy is set to PRESENT
             * (i.e., if the outline buttons are displayed)
             */
            n = 0;
            XtSetArg(args[n], XmNoutlineButtonPolicy, &OutlineBtnPolicy);  n++;
            XtGetValues(widget->core.parent, args, n);

            /*
             * If a parent exists and the outlineButtonPolicy is set
             * to PRESENT, see if this widget is already in the
             * object hierarchy.  If not, put it in.
             */
            if ((ParentIconGadget != NULL) &&
                (OutlineBtnPolicy == XmOUTLINE_BUTTON_PRESENT))  {
                /*
                 * Get the Container constraint part from the IconGadget,
                 * and get the outline button widget from the constraint part.
                 */
                containerConstraint = GetContainerConstraint(ParentIconGadget);
                OutlineBtnWidget = containerConstraint->related_cwid;

		/*
		 * If the outline button is not visable, it might
		 * not be created.  We can ignore it for now because
		 * the object hierarchy will get recreated if anything
		 * gets expanded and any newly created outline buttons
		 * will get added into the object hiearchy at that time
		 */
		if (OutlineBtnWidget == NULL)
		    break;

                /*
                 * Get the parent IconGadget object
                 */
                ParentIGObject = xisFindObjectFromWidget(ParentIconGadget);

                /*
                 * Get the outline button object if it exists
                 */
                IG_current_object = ParentIGObject->first_child;
                while (IG_current_object != NULL_Object)  {
                    /*
                     * If the child object of the IconGadget is a
                     * PushButtonGadget, it's the outline button
                     */
                    if (strcmp(IG_current_object->id.widget->core.widget_class->core_class.class_name, "XmPushButtonGadget") == 0)  {
                        /*
                         * Set the geometry info needed for the Outline
                         * Button region (x, y, width and height)
                         */
                        /* Regions do not have a window */
                        IG_current_object->id.window = 0;
                        IG_current_object->x = ContainerWidgetX +
                                               OutlineBtnWidget->core.x;
                        IG_current_object->y = ContainerWidgetY +
                                               OutlineBtnWidget->core.y;
                        IG_current_object->width = OutlineBtnWidget->core.width;
                        IG_current_object->height =
                                               OutlineBtnWidget->core.height;

                        found_IGObject = True;
                        break;
                    }
                    else  {
                        IG_current_object = IG_current_object->next_sibling;
                    }
                }

                /*
                 * If we didn't find an object for the IG, create one
                 */
                if (found_IGObject == False)  {
                    /*
                     * Create a new object for the outline button
                     * as a region of the IconGadget
                     */
                     OutlineBtnRegion = xisAllocObject (ParentIGObject,
                                                        OutlineBtnWidget,
                                                        oOutlineBtn, 0);

                    /*
                     * Set the geometry info needed for the Outline
                     * Button region (x, y, width and height)
                     */
                    OutlineBtnRegion->id_code = next_avail_id_code++;
                    /* Regions do not have a window */
                    OutlineBtnRegion->id.window = 0;
                    OutlineBtnRegion->x = ContainerWidgetX +
                                          OutlineBtnWidget->core.x;
                    OutlineBtnRegion->y = ContainerWidgetY +
                                          OutlineBtnWidget->core.y;
                    OutlineBtnRegion->width = OutlineBtnWidget->core.width;
                    OutlineBtnRegion->height = OutlineBtnWidget->core.height;
                }  /* end if found_IGObject = False */

            }  /* end if ParentIconGadget != NULL */


            break;


        case mvsXmFileSelectionBoxWidgetClass:

            i = 0;

	    /* See if this is a CDE FileSelectionBox */
            n = 0;
            XtSetArg (args[n], XmNpathMode, &pathMode);   n++;
	    XtGetValues (widget, args, n);
	    if (pathMode == XmPATH_MODE_RELATIVE)  {
		child[i] = XtNameToWidget(widget,"DirText");
		child_obj = mvsGetSubObjects(object,child[i], 
					     oDirFilterText,0,True);
		i++;
		child[i] = XtNameToWidget(widget,"DirL");
		child_obj = mvsGetSubObjects(object,child[i], 
					     oDirFilterLabel,0,True);
		i++;
	    }

            child[i] = XmFileSelectionBoxGetChild(widget,XmDIALOG_APPLY_BUTTON);
            child_obj = mvsGetSubObjects(object,child[i], oFilterButton,0,True);
            i++;

            child[i] =XmFileSelectionBoxGetChild(widget,XmDIALOG_CANCEL_BUTTON);
            child_obj = mvsGetSubObjects(object,child[i], oCancelButton,0,True);
            i++;

            child[i] = XmFileSelectionBoxGetChild(widget,XmDIALOG_FILTER_LABEL);
            child_obj=mvsGetSubObjects(object,child[i],oFileFilterLabel,0,True);
            i++;

            child[i] = XmFileSelectionBoxGetChild(widget,XmDIALOG_FILTER_TEXT);
            child_obj=mvsGetSubObjects(object,child[i], oFileFilterText,0,True);
            i++;
            
            child[i] = XmFileSelectionBoxGetChild(widget,XmDIALOG_HELP_BUTTON);
            child_obj = mvsGetSubObjects(object,child[i], oHelpButton,0,True);
            i++;

            child[i] = XmFileSelectionBoxGetChild(widget,XmDIALOG_DIR_LIST);
            child_obj = mvsGetSubObjects(object,child[i], oDirList,0,True);
            if(child_obj != NULL) {
                child_obj->parent->id.object_type = oScrolledWindow;
                child_obj->parent->id.instance = 0;
                child_obj->id.object_type = oDirList;
            }
            i++;

            child[i] = XmFileSelectionBoxGetChild(widget,XmDIALOG_FILE_LIST);
            child_obj = mvsGetSubObjects(object,child[i], oFileList,0,True);
            if(child_obj != NULL) {
                child_obj->parent->id.object_type = oScrolledWindow;
                child_obj->parent->id.instance = 1;
                child_obj->id.object_type = oFileList;
            }
            i++;

            child[i] = XmFileSelectionBoxGetChild(widget,
                                                  XmDIALOG_FILE_LIST_LABEL);
            child_obj = mvsGetSubObjects(object,child[i],oFileListLabel,0,True);
            i++;
            
            child[i] = XmFileSelectionBoxGetChild(widget,XmDIALOG_OK_BUTTON);
            child_obj = mvsGetSubObjects(object,child[i], oOkButton,0,True);
            i++;

            child[i] = XmFileSelectionBoxGetChild(widget,XmDIALOG_SEPARATOR);
            child_obj = mvsGetSubObjects(object,child[i], oSeparator,0,True);
            i++;

            child[i] = XmFileSelectionBoxGetChild(widget,
                                                  XmDIALOG_SELECTION_LABEL);
            child_obj = mvsGetSubObjects(object,child[i],
                                         oFileSelectionLabel,0,True);
            i++;
            
            child[i] = XmFileSelectionBoxGetChild(widget,XmDIALOG_TEXT);
            child_obj = mvsGetSubObjects(object,child[i],
                                         oFileSelectionText,0,True);
            i++;

            child[i] = XmFileSelectionBoxGetChild(widget,
                                                  XmDIALOG_DIR_LIST_LABEL);
            child_obj = mvsGetSubObjects(object,child[i], oDirListLabel,0,True);
            i++;

            separator_num = 0;
            user_defined_num = 0;
            num_predefined = i;

            num_children = (int)mvsGetResourceValue(widget,XmNnumChildren,
                                                    T_Cardinal);
            children = (Widget *)mvsGetResourceValue(widget,XmNchildren,
                                                     T_WidgetList);

            for (i=num_children-1; i>=0; i--) {
                for (j=0; j<num_predefined; j++) {
                    if (children[i] == child[j])
                        break;
                }
                if (j < num_predefined)
                    continue;

                class_code = mvsGetClassCode(children[i]);
                
                if (class_code == mvsXmSeparatorGadgetClass)
                    child_obj = mvsGetSubObjects(object,children[i],
                                                 oSeparator,
                                                 separator_num++,True);

                /* Any other user defined children */
                else
                    child_obj= mvsGetSubObjects(object,children[i],oUserDefined,
                                                 user_defined_num++,False);  
            }
            break;


        case mvsXmSpinBoxWidgetClass:
	    {
		sb_x = object->x;
		sb_y = object->y;
		sb_width = object->width;
		sb_height = object->height;

		i = 0;

            	num_children = (int)mvsGetResourceValue(widget,XmNnumChildren,
                                                    T_Cardinal);
            	children = (Widget *)mvsGetResourceValue(widget,XmNchildren,
                                                     T_WidgetList);

                user_defined_num = 0; 
		num_predefined = 0;

            	for (i=num_children-1; i>=0; i--) {
                    /* Any user defined children */
                    child_obj= mvsGetSubObjects(object,children[i],oUserDefined,
                                                user_defined_num++,False);  
                }

		/* Create increment arrow object */
		SBIncrementBtn = xisAllocObject(object,widget,
					        oIncrementBtn,0);
		SBIncrementBtn->id_code = next_avail_id_code++;
		SBIncrementBtn->id.window = 0; /*Regions do not have a window*/

		/* Create decrement arrow object */
		SBDecrementBtn = xisAllocObject(object,widget,
					        oDecrementBtn,0);
		SBDecrementBtn->id_code = next_avail_id_code++;
		SBDecrementBtn->id.window = 0; /*Regions do not have a window*/

		/* Get required sizes for determining the location */
		arrowLayout=(unsigned char) mvsGetResourceValue(widget,
								XmNarrowLayout,
								T_unsigned_char);
		layoutDirection = (int)mvsGetResourceValue(widget,
							   XmNlayoutDirection,
							   T_XmDirection);
		arrowSize = (int)mvsGetResourceValue(widget,
						     XmNarrowSize,
						     T_Dimension);

		SBIncrementBtn->width = arrowSize;
		SBIncrementBtn->height = sb_height;

		SBDecrementBtn->width = arrowSize;
		SBDecrementBtn->height = sb_height;

		/* 
		 * Calculate the location for the
		 * Increment and Decrement arrows
		 */
		SBIncrementBtn->y = SBDecrementBtn->y = sb_y + border_width;

		if (layoutDirection == XmLEFT_TO_RIGHT)  {
		    if (arrowLayout == XmARROWS_SPLIT)  {
		    	SBIncrementBtn->x = sb_x + sb_width + border_width -
					    arrowSize;
		    	SBDecrementBtn->x = sb_x + border_width;
		    }
		    else if (arrowLayout == XmARROWS_BEGINNING)  {
		    	SBIncrementBtn->x = sb_x + border_width + arrowSize;
		    	SBDecrementBtn->x = sb_x + border_width;
		    }
		    else  {  /* arrowLayout == XmARROWS_END */
		    	SBIncrementBtn->x = sb_x + sb_width + border_width -
					     arrowSize;
		    	SBDecrementBtn->x = (sb_x + sb_width + border_width) -
					    (2 * arrowSize);
		    }
		}  /* end if */
		else if (layoutDirection == XmRIGHT_TO_LEFT)  {
		    if (arrowLayout == XmARROWS_SPLIT)  {
		    	SBIncrementBtn->x = sb_x + border_width;
		    	SBDecrementBtn->x = sb_x + sb_width + border_width -
					     arrowSize;
		    }
		    else if (arrowLayout == XmARROWS_BEGINNING)  {
		    	SBIncrementBtn->x = (sb_x + sb_width + border_width) -
					    (2 * arrowSize);
		    	SBDecrementBtn->x = (sb_x + sb_width + border_width) -
					    arrowSize; 
		    }
		    else  {  /* arrowLayout == XmARROWS_END */
		    	SBIncrementBtn->x = sb_x + border_width;
		    	SBDecrementBtn->x = sb_x + border_width + arrowSize;
		    }
		}  /* end if layoutDirection == XmRIGHT_TO_LEFT */

		else {  /* error */
		    printf ("Error: layoutDirection not XmLEFT_TO_RIGHT or XmRIGHT_TO_LEFT");
		}


	    }
            break;


        case mvsXmComboBoxWidgetClass:
	    {
		cb_x = object->x;
		cb_y = object->y;
		cb_width = object->width;

		i = 0;
		combobox_type=(unsigned char) 
				mvsGetResourceValue(widget, 
						    XmNcomboBoxType,
						    T_unsigned_char);
		child[i] = XtNameToWidget(widget,"*List");
		if(combobox_type != XmCOMBO_BOX)  {  /* need to create it */
		    child_obj = mvsGetSubObjects(object,child[i], 
						 oSelectionList,0,False);
		    if(child_obj != NULL) {
			child_obj->parent->id.object_type = oScrolledWindow;
			child_obj->parent->id.instance = 0;
			child_obj->id.object_type = oSelectionList;
		    }
		}
		else  {
		    child_obj = mvsGetSubObjects(object,child[i], 
						 oSelectionList,0,True);
		    if(child_obj != NULL) {
			child_obj->parent->id.object_type = oScrolledWindow;
			child_obj->parent->id.instance = 0;
			child_obj->id.object_type = oSelectionList;
		    }
		}
		i++;

		child[i] = XtNameToWidget(widget,"Text");
		child_obj = mvsGetSubObjects(object,child[i],
					     oSelectionText,0,True);
		i++;

		user_defined_num = 0;
		num_predefined = i;

		num_children = (int)mvsGetResourceValue(widget,XmNnumChildren,
							T_Cardinal);
		children = (Widget *)mvsGetResourceValue(widget,XmNchildren,
							 T_WidgetList);

		CBDropDwnBtn = xisAllocObject(object,widget,
					      oComboBoxDropDownButton,0);
		CBDropDwnBtn->id_code = next_avail_id_code++;
		CBDropDwnBtn->id.window = 0;  /* Regions do not have a window */

		stringDirection = (int)mvsGetResourceValue(widget,
							   XmNstringDirection,
							   T_XmDirection);
		cb_marginWidth = (int)mvsGetResourceValue(widget,
							  XmNmarginWidth,
							  T_Dimension);
		cb_marginHeight = (int)mvsGetResourceValue(widget,
							   XmNmarginHeight,
							   T_Dimension);
		cb_highlightThickness = (int)mvsGetResourceValue(widget,
							 XmNhighlightThickness,
							 T_Dimension);
		cb_shadowThickness = (int)mvsGetResourceValue(widget,
							 XmNshadowThickness,
							 T_Dimension);
		arrowSize = (int)mvsGetResourceValue(widget,
						     XmNarrowSize,
						     T_Dimension);

		CBDropDwnBtn->width = arrowSize;
		CBDropDwnBtn->height = arrowSize;

		CBDropDwnBtn->y = cb_y + cb_marginHeight + border_width +
				  cb_shadowThickness + cb_highlightThickness;

		if (stringDirection == XmSTRING_DIRECTION_L_TO_R)
		    CBDropDwnBtn->x = cb_x + cb_width - 
				      (cb_highlightThickness + border_width + 
				      cb_marginWidth + arrowSize +
				      cb_shadowThickness);
		    /*CBDropDwnBtn->x = cb_x + cb_width + border_width - 
				      arrowSize;*/
		else  /* XmSTRING_DIRECTION_R_TO_L */
		    CBDropDwnBtn->x = cb_x + cb_highlightThickness +
				      border_width + cb_marginWidth +
				      arrowSize + cb_shadowThickness;
		    /*CBDropDwnBtn->x = cb_x + cb_marginWidth + border_width; */

	    }
            break;


        case mvsXmMessageBoxWidgetClass:

            i = 0;
            child[i] = XmMessageBoxGetChild(widget,XmDIALOG_CANCEL_BUTTON);
            child_obj = mvsGetSubObjects(object,child[i],oCancelButton,0,True);
            i++;

            child[i] = XmMessageBoxGetChild(widget,XmDIALOG_HELP_BUTTON);
            child_obj = mvsGetSubObjects(object,child[i],oHelpButton,0,True);
            i++;

            child[i] = XmMessageBoxGetChild(widget,XmDIALOG_MESSAGE_LABEL);
            child_obj = mvsGetSubObjects(object,child[i],oMessageLabel,0,True);
            i++;

            child[i] = XmMessageBoxGetChild(widget,XmDIALOG_OK_BUTTON);
            child_obj = mvsGetSubObjects(object,child[i],oOkButton,0,True);
            i++;

            child[i] = XmMessageBoxGetChild(widget,XmDIALOG_SEPARATOR);
            child_obj = mvsGetSubObjects(object,child[i],oSeparator,0,True);
            i++;

            child[i] = XmMessageBoxGetChild(widget,XmDIALOG_SYMBOL_LABEL);
            child_obj = mvsGetSubObjects(object,child[i],oSymbolLabel,0,True);
            i++;

            user_defined_num = 0;
            num_predefined = i;

            num_children = (int)mvsGetResourceValue(widget,XmNnumChildren,
                                                    T_Cardinal);
            children = (Widget *)mvsGetResourceValue(widget,XmNchildren,
                                                     T_WidgetList);

            for (i=num_children-1; i>=0; i--) {
                for (j=0; j<num_predefined; j++) {
                    if (children[i] == child[j])
                        break;
                }
                if (j < num_predefined)
                    continue;

                child_obj = mvsGetSubObjects(object,children[i],oUserDefined,
                                             user_defined_num++,False);             
            }
            break;


        case mvsXmPanedWindowWidgetClass:

            separator_num = 0;
            sash_num = 0;
            user_defined_num = 0;
            num_predefined = 0;

            num_children = (int)mvsGetResourceValue(widget,XmNnumChildren,
                                                    T_Cardinal);
            children = (Widget *)mvsGetResourceValue(widget,XmNchildren,
                                                     T_WidgetList);

            for (i=num_children-1; i>=0; i--) {
                for (j=0; j<num_predefined; j++) {
                    if (children[i] == child[j])
                        break;
                }
                if (j < num_predefined)
                    continue;

                class_code = mvsGetClassCode(children[i]);

                if (class_code == mvsXmSashWidgetClass) {
                   child_obj = mvsGetSubObjects(object,children[i],
                                                 oSash,sash_num++,True);

	    	   if (child_obj != NULL && XtIsManaged(child_obj->id.widget)) {
			w_info = mvsWidgetToWidgetInfo(child_obj->id.widget);
			if (w_info != NULL) {
				mvs_resources = w_info->mvs_resources;
				mvsLoadResources(w_info, mvs_resources);
			}
	    	   }
		}

                else if (class_code == mvsXmSeparatorGadgetClass)
                    child_obj = mvsGetSubObjects(object,children[i],
                                                 oSeparator,
                                                 separator_num++,True);
                else
                    child_obj = mvsGetSubObjects(object,children[i],
                                                 oUserDefined,
                                                 user_defined_num++,False);
            }
            break;


        case mvsXmRowColumnWidgetClass:
	    { 

               user_defined_num = 0;
               num_predefined = 0;
   
               num_children = (int)mvsGetResourceValue(widget,XmNnumChildren,
                                                    T_Cardinal);
               children = (Widget *)mvsGetResourceValue(widget,XmNchildren,
                                                     T_WidgetList);
               rc_type = (unsigned char)mvsGetResourceValue(widget,
						     XmNrowColumnType,
                                                     T_unsigned_char);

#ifndef MOTIF1_1

	       tear_off = XmGetTearOffControl(widget);
	       if (tear_off != NULL)
			child_obj = mvsGetSubObjects(object, tear_off,
						     oTearOffButton, 0, True);

#endif /* MOTIF1_1 */
			
               for (i=num_children-1; i>=0; i--) {
                   for (j=0; j<num_predefined; j++) {
                       if (children[i] == child[j])
                           break;
                   }
                   if (j < num_predefined)
                       continue;
   
                   if (rc_type == XmMENU_OPTION) {
			class_code = mvsGetClassCode(children[i]);

                	if (class_code == mvsXmCascadeButtonGadgetClass)
			    child_obj = mvsGetSubObjects(object,children[i],
                                                 oOptionButton,0,True);
			else
                	if (class_code == mvsXmLabelGadgetClass)
			    child_obj = mvsGetSubObjects(object,children[i],
                                                 	 oLabel,0,True);
                	else
                    	    child_obj = mvsGetSubObjects(object,children[i],
                                                 oUserDefined,
                                                 user_defined_num++,False);
		   } else
                        child_obj = mvsGetSubObjects(object,children[i], 
						oUserDefined,
						user_defined_num++, False);
               }
               break;

	    }
        case mvsXmScaleWidgetClass:

            user_defined_num = 0;
            num_predefined = 0;

            num_children = (int)mvsGetResourceValue(widget,XmNnumChildren,
                                                    T_Cardinal);
            children = (Widget *)mvsGetResourceValue(widget,XmNchildren,
                                                     T_WidgetList);

            for (i=num_children-1; i>=0; i--) {
                for (j=0; j<num_predefined; j++) {
                    if (children[i] == child[j])
                        break;
                }
                if (j < num_predefined)
                    continue;

                class_code = mvsGetClassCode(children[i]);

                if (class_code == mvsXmScrollBarWidgetClass)
                    child_obj = mvsGetSubObjects(object,children[i],
                                                 oScrollBar,0,True);
                else if (class_code == mvsXmLabelGadgetClass)
                    child_obj = mvsGetSubObjects(object,children[i],
                                                 oScaleLabel,0,True);
                else if (class_code == mvsXmSeparatorGadgetClass)
                    child_obj = mvsGetSubObjects(object,children[i],
                                                 oSeparator,
                                                 separator_num++,True);
                else
                    child_obj = mvsGetSubObjects(object,children[i],
                                                 oUserDefined,
                                                 user_defined_num++,False);

            }
            break;

        case mvsXmMainWindowWidgetClass:

            i= 0;
            child[i] = (Widget)mvsGetResourceValue(widget,XmNcommandWindow,
                                                   T_Widget_ref);
            child_obj= mvsGetSubObjects(object,child[i],oCommandWindow,0,False);
            i++;

            child[i] = (Widget)mvsGetResourceValue(widget,XmNmenuBar,
                                                   T_Widget_ref);
            child_obj = mvsGetSubObjects(object,child[i],oMenuBar,0,False);
            i++;

            child[i] = (Widget)mvsGetResourceValue(widget,XmNmessageWindow,
                                                   T_Widget_ref);
            child_obj= mvsGetSubObjects(object,child[i],oMessageWindow,0,False);
            i++;

            child[i] = (Widget)mvsGetResourceValue(widget,XmNworkWindow,
                                                   T_Widget_ref);
            child_obj = mvsGetSubObjects(object,child[i],oWorkWindow,0,False);
            i++;

            child[i] = XmMainWindowSep1(widget);
            child_obj = mvsGetSubObjects(object,child[i],oSeparator,1,True);
	    if (child_obj != NULL && XtIsManaged(child_obj->id.widget)) {
		w_info = mvsWidgetToWidgetInfo(child_obj->id.widget);
		if (w_info != NULL) {
			mvs_resources = w_info->mvs_resources;
			mvsLoadResources(w_info, mvs_resources);
		}
	    }
            i++;

            child[i] = XmMainWindowSep2(widget);
            child_obj = mvsGetSubObjects(object,child[i],oSeparator,2,True);
	    if (child_obj != NULL && XtIsManaged(child_obj->id.widget)) {
		w_info = mvsWidgetToWidgetInfo(child_obj->id.widget);
		if (w_info != NULL) {
			mvs_resources = w_info->mvs_resources;
			mvsLoadResources(w_info, mvs_resources);
		}
	    }
            i++;

            child[i] = XmMainWindowSep3(widget);
            child_obj = mvsGetSubObjects(object,child[i],oSeparator,3,True);
	    if (child_obj != NULL && XtIsManaged(child_obj->id.widget)) {
		w_info = mvsWidgetToWidgetInfo(child_obj->id.widget);
		if (w_info != NULL) {
			mvs_resources = w_info->mvs_resources;
			mvsLoadResources(w_info, mvs_resources);
		}
	    }
            i++;

            /**** FALL THROUGH TO mvsXmScrolledWindowWidgetClass ****/
            /*  Use value of 'i' as initialized to 0 or set above   */
            /**** FALL THROUGH TO mvsXmScrolledWindowWidgetClass ****/

        case mvsXmScrolledWindowWidgetClass:
            scroll_p =(unsigned char)mvsGetResourceValue(widget,
                                                         XmNscrollingPolicy,
                                                         T_unsigned_char);

            child[i] =(Widget)mvsGetResourceValue(widget,XmNhorizontalScrollBar,
                                                   T_Widget_ref);
            if(scroll_p != XmAUTOMATIC) { /* We needed to create it */
                child_obj =
                      mvsGetSubObjects(object,child[i],oHorizScrollBar,0,False);
		if (child_obj != NULL && XtIsManaged(child_obj->id.widget)) {
			w_info = mvsWidgetToWidgetInfo(child_obj->id.widget);
			if (w_info != NULL) {
				mvs_resources = w_info->mvs_resources;
				mvsLoadResources(w_info, mvs_resources);
			}
		}
            }
            else {
                child_obj =
                      mvsGetSubObjects(object,child[i],oHorizScrollBar,0,True);
		if (child_obj != NULL && XtIsManaged(child_obj->id.widget)) {
			w_info = mvsWidgetToWidgetInfo(child_obj->id.widget);
			if (w_info != NULL) {
				mvs_resources = w_info->mvs_resources;
				mvsLoadResources(w_info, mvs_resources);
			}
		}
            }
            i++;

            child[i] = (Widget)mvsGetResourceValue(widget,XmNverticalScrollBar,
                                                   T_Widget_ref);
            if(scroll_p != XmAUTOMATIC) { /* We needed to create it */
                child_obj =
                       mvsGetSubObjects(object,child[i],oVertScrollBar,0,False);
		if (child_obj != NULL && XtIsManaged(child_obj->id.widget)) {
			w_info = mvsWidgetToWidgetInfo(child_obj->id.widget);
			if (w_info != NULL) {
				mvs_resources = w_info->mvs_resources;
				mvsLoadResources(w_info, mvs_resources);
			}
		}
            }
            else {
                child_obj =
                       mvsGetSubObjects(object,child[i],oVertScrollBar,0,True);
		if (child_obj != NULL && XtIsManaged(child_obj->id.widget)) {
			w_info = mvsWidgetToWidgetInfo(child_obj->id.widget);
			if (w_info != NULL) {
				mvs_resources = w_info->mvs_resources;
				mvsLoadResources(w_info, mvs_resources);
			      }
		      }
	      }
            i++;

            user_defined_num = 0;
            num_predefined = i;

            num_children = (int)mvsGetResourceValue(widget,XmNnumChildren,
                                                    T_Cardinal);
            children = (Widget *)mvsGetResourceValue(widget,XmNchildren,
                                                     T_WidgetList);

            for (i=num_children-1; i>=0; i--) {
                for (j=0; j<num_predefined; j++) {
                    if (children[i] == child[j])
                        break;
                }
                if (j < num_predefined)
                    continue;

                child_obj = mvsGetSubObjects(object,children[i],oUserDefined,
                                             user_defined_num++,False);
            }
            break;

        case mvsXmSelectionBoxWidgetClass:
            i = 0;
            child[i] = XmSelectionBoxGetChild(widget,XmDIALOG_APPLY_BUTTON);
            child_obj = mvsGetSubObjects(object,child[i],oApplyButton,0,True);
	    if (child_obj != NULL && XtIsManaged(child_obj->id.widget)) {
		w_info = mvsWidgetToWidgetInfo(child_obj->id.widget);
		if (w_info != NULL) {
			mvs_resources = w_info->mvs_resources;
			mvsLoadResources(w_info, mvs_resources);
		}
	    }
            i++;

            child[i] = XmSelectionBoxGetChild(widget,XmDIALOG_CANCEL_BUTTON);
            child_obj = mvsGetSubObjects(object,child[i],oCancelButton,0,True);
            i++;

            child[i] = XmSelectionBoxGetChild(widget,XmDIALOG_HELP_BUTTON);
            child_obj = mvsGetSubObjects(object,child[i],oHelpButton,0,True);
            i++;

            child[i] = XmSelectionBoxGetChild(widget,XmDIALOG_LIST);
            child_obj = mvsGetSubObjects(object,child[i],oSelectionList,0,True);
            if(child_obj != NULL) {
                child_obj->parent->id.object_type = oScrolledWindow;
                child_obj->parent->id.instance = 0;
                child_obj->id.object_type = oSelectionList;
            }
            i++;

            child[i] = XmSelectionBoxGetChild(widget,XmDIALOG_LIST_LABEL);
            child_obj =
                   mvsGetSubObjects(object,child[i],oSelectionListLabel,0,True);
            i++;
            
            child[i] = XmSelectionBoxGetChild(widget,XmDIALOG_OK_BUTTON);
            child_obj = mvsGetSubObjects(object,child[i],oOkButton,0,True);
            i++;

            child[i] = XmSelectionBoxGetChild(widget,XmDIALOG_SEPARATOR);
            child_obj = mvsGetSubObjects(object,child[i],oSeparator,0,True);
            i++;

            child[i] = XmSelectionBoxGetChild(widget,XmDIALOG_SELECTION_LABEL);
            child_obj= mvsGetSubObjects(object,child[i],oSelectionLabel,0,True);
            i++;
            
            child[i] = XmSelectionBoxGetChild(widget,XmDIALOG_TEXT);
            child_obj = mvsGetSubObjects(object,child[i],oSelectionText,0,True);
            i++;

            separator_num = 0;
            user_defined_num = 0;
            num_predefined = i;

            num_children = (int)mvsGetResourceValue(widget,XmNnumChildren,
                                                    T_Cardinal);
            children = (Widget *)mvsGetResourceValue(widget,XmNchildren,
                                                     T_WidgetList);

            for (i=num_children-1; i>=0; i--) {
                for (j=0; j<num_predefined; j++) {
                    if (children[i] == child[j])
                        break;
                }
                if (j < num_predefined)
                    continue;

                child_obj = mvsGetSubObjects(object,children[i],oUserDefined,
                                             user_defined_num++,False);
            }
            break;

        case mvsXmScrollBarWidgetClass: 
	    {
                sb_x = object->x; 
				sb_y = object->y;
                sb_w = object->width; 
				sb_h = object->height;
                w = widget;
                showArrows = (int)mvsGetResourceValue(w,XmNshowArrows,
													  T_Boolean);
                orientation = (int)mvsGetResourceValue(w,XmNorientation,
													  T_unsigned_char);
                sliderSize = (int)mvsGetResourceValue(w,XmNsliderSize,
													  T_Integer);
                processingDirection = (int)mvsGetResourceValue(w, 
				            XmNprocessingDirection,
					    T_unsigned_char);
                hl = (int)mvsGetResourceValue(w,XmNhighlightThickness, 
					      T_Dimension);
                sh = (int)mvsGetResourceValue(w,XmNshadowThickness,T_Dimension);
                user_min = (int)mvsGetResourceValue(w,XmNminimum,T_Integer);
                user_max = (int)mvsGetResourceValue(w,XmNmaximum,T_Integer);
                user_value = (int)mvsGetResourceValue(w,XmNvalue,T_Integer);

                slider = xisAllocObject(object,widget,oSlider,0);
                slider->id_code = next_avail_id_code++;
                slider->id.window = 0;  /* Regions do not have a window */
                trough[1] = xisAllocObject(object,widget,oUpOrLeftSliderArea,0);
                trough[1]->id_code = next_avail_id_code++;
                trough[1]->id.window = 0; /* Regions do not have a window */
                trough[0] = xisAllocObject(object,widget,oDownOrRightSliderArea,0);
                trough[0]->id_code = next_avail_id_code++;
                trough[0]->id.window = 0;  /* Regions do not have a window */
                arrow[1] = xisAllocObject(object,widget,oUpOrLeftArrow,0);
                arrow[1]->id_code = next_avail_id_code++;
                arrow[1]->id.window = 0;  /* Regions do not have a window */
                arrow[0] = xisAllocObject(object,widget,oDownOrRightArrow,0);
                arrow[0]->id_code = next_avail_id_code++;
                arrow[0]->id.window = 0;  /* Regions do not have a window */

                if (showArrows != XmNONE) {
                    if (orientation == XmVERTICAL) {

/* Note added border_width to get correct arrow size */

                        arrow_size = sb_w - 2*(hl + sh + border_width);

                        arrow[0]->width = arrow[0]->height =
                        arrow[1]->width = arrow[1]->height = arrow_size;

                        crunched_arrow_size 
                          = (sb_h - 
			     (MIN_SLIDER_SIZE + 2 + 2*(hl+sh+border_width)))/2;

                        if (crunched_arrow_size < arrow_size)
                            arrow[0]->height = arrow[1]->height = 
							crunched_arrow_size;
                        arrow_length = arrow[0]->height;

                        arrow[0]->x = arrow[1]->x = sb_x +  hl + 
						    sh + border_width;

                        trough_length = sb_h - 2 * ( arrow[0]->height + hl + sh
						     + border_width );

                        trough_breadth = arrow_size;

                        trough_x = arrow[0]->x;

			/* Starting point for trough_y and arrow Y's */
			trough_y = arrow[0]->y = arrow[1]->y
			  = sb_y + hl + sh + border_width;
     
			if (showArrows == XmEACH_SIDE) {
			  trough_y += arrow[0]->height+1;
			  arrow[0]->y += arrow[0]->height+1 + trough_length+1;
			}
			else if (showArrows == XmMAX_SIDE) {
			  if (processingDirection == XmMAX_ON_TOP) {
			    arrow[0]->y += arrow[0]->height+1;
			    trough_y += 2*(arrow[0]->height+1);
			  }
			  else { /* XmMAX_ON_BOTTOM */
			    arrow[1]->y += trough_length+1;
			    arrow[0]->y += arrow[0]->height+1 + trough_length+1;
			  }
			}
			else if (showArrows == XmMIN_SIDE) {
			  if (processingDirection == XmMAX_ON_BOTTOM) {
			    arrow[0]->y += arrow[0]->height+1;
			    trough_y += 2*(arrow[0]->height+1);
			    }
			  else { /* XmMAX_ON_TOP */
			    arrow[1]->y += trough_length+1;
			    arrow[0]->y += arrow[0]->height+1 + trough_length+1;
			  }
			}
		      }
                    else {  /* XmHORIZONTAL */

                        arrow_size = sb_h - 2*(hl + sh + border_width);

                        arrow[0]->width = arrow[0]->height =
                        arrow[1]->width = arrow[1]->height = arrow_size;

                        crunched_arrow_size 
                           = (sb_w - 
			      (MIN_SLIDER_SIZE + 2 + 2*(hl+sh+border_width)))/2;

                        if (crunched_arrow_size < arrow_size)
                            arrow[0]->width = arrow[1]->width = 
							crunched_arrow_size;
                        arrow_length = arrow[0]->width;

                        arrow[0]->y = arrow[1]->y = sb_y + hl + 
						    sh + border_width;

                        trough_length = sb_w - 2*(arrow[0]->width + hl + sh 
						  + border_width);

                        trough_breadth = arrow_size;

                        trough_y = arrow[0]->y;
     
			/* Starting point for trough_x and arrow X's */
			trough_x = arrow[0]->x = arrow[1]->x
			  = sb_x + hl + sh + border_width;
     
			if (showArrows == XmEACH_SIDE) {
			  trough_x += arrow[0]->width+1;
			  arrow[0]->x += arrow[0]->width+1 + trough_length+1;
			}
			else if (showArrows == XmMAX_SIDE) {
			  if (processingDirection == XmMAX_ON_LEFT) {
			    arrow[0]->x += arrow[0]->width+1;
			    trough_x += 2*(arrow[0]->width+1);
			    }
			  else { /* XmMAX_ON_RIGHT */
			    arrow[1]->x += trough_length+1;
			    arrow[0]->x +=  arrow[0]->width+1 + trough_length+1;
			  }
			}
			else if (showArrows == XmMIN_SIDE) {
			  if (processingDirection == XmMAX_ON_RIGHT) {
			    arrow[0]->x += arrow[0]->width+1;
			    trough_x += 2*(arrow[0]->width+1);
			    }
			  else { /* XmMAX_ON_LEFT */
			    arrow[1]->x += trough_length+1;
			    arrow[0]->x += arrow[0]->width+1 + trough_length+1;
			  }
			}
		      }
		  }
                else { /* showArrow == XmNONE */
                    arrow_length = arrow_size = 0;
                    arrow[0]->width = arrow[0]->height = 0;
                    arrow[1]->width = arrow[1]->height = 0;
                    arrow[0]->x = arrow[0]->y = (-1);
                    arrow[1]->x = arrow[1]->y = (-1);
                    if (orientation == XmVERTICAL) {
                        trough_breadth = sb_w - 2*(hl + sh + border_width);
                        trough_length = sb_h - 2*(hl + sh + border_width);
		    }
                    else {
                        trough_breadth = sb_h - 2*(hl + sh + border_width);
                        trough_length = sb_w - 2*(hl + sh + border_width);
                    }
                    trough_x = sb_x + hl + sh + border_width;
                    trough_y = sb_y + hl + sh + border_width;
                }
                    
                trueSize = trough_length;
                userSize = user_max - user_min;

                if (arrow_length == 0)
                    arrow_length = (-1);

                factor = trueSize / userSize; 

		slider_offset = hl + sh + border_width;

		switch (showArrows) {
		case XmEACH_SIDE:
		  slider_offset += arrow_length+1;
		  break;
		case XmMAX_SIDE:
		  if ((processingDirection == XmMAX_ON_TOP) ||
		      (processingDirection == XmMAX_ON_LEFT)) {
		    slider_offset += 2*(arrow_length+1);
		  }
		  break;
		case XmMIN_SIDE:
		  if ((processingDirection == XmMAX_ON_BOTTOM) ||
		      (processingDirection == XmMAX_ON_RIGHT)) {
		    slider_offset += 2*(arrow_length+1);
		  }
		  break;
		case XmNONE:
		  break;
		}

  		if ( processingDirection == XmMAX_ON_BOTTOM || 
			processingDirection == XmMAX_ON_RIGHT )
		    slideStart = (float)(user_value - user_min) * factor +
				(orientation == XmVERTICAL ? sb_y : sb_x) + 
				slider_offset;
		else
		    slideStart = (float)(user_max - user_value - sliderSize)
				 * factor + 
				 (orientation == XmVERTICAL ? sb_y : sb_x) + 
				slider_offset;
  
                slideSize = (float)(sliderSize)*factor;

                if (orientation == XmVERTICAL) {
                    slider_x = trough_x;
                    slider_y = (int)(slideStart + 0.5);
                    slider_width = trough_breadth;
                    slider_height = (int)(slideSize + 0.5);
                }
                else {
                    slider_x = (int)(slideStart + 0.5);
                    slider_y = trough_y;
                    slider_width = (int)(slideSize + 0.5);
                    slider_height = trough_breadth;
                }

                if (slider_width < MIN_SLIDER_SIZE) {
                    slider_width = MIN_SLIDER_SIZE;
                    if ( orientation == XmHORIZONTAL && 
                         slider_x + slider_width > trough_x + trough_length )
                        slider_x = trough_x + trough_length - slider_width;
                }

                if (slider_height < MIN_SLIDER_SIZE) {
                    slider_height = MIN_SLIDER_SIZE;
                    if ( orientation == XmVERTICAL && 
                         slider_y + slider_height > trough_y + trough_length )
                        slider_y = trough_y + trough_length - slider_height;
                }

                slider->x = slider_x;
                slider->y = slider_y;
                slider->width = slider_width;
                slider->height = slider_height;
                
                if (orientation == XmVERTICAL) {
                    trough[1]->x = trough_x;
                    trough[1]->y = trough_y;
                    trough[1]->width = trough_breadth;
                    trough[1]->height = slider_y - trough_y - 1;
                    trough[0]->x = trough_x;
                    trough[0]->y = slider_y + slider_height + 1;
                    trough[0]->width = trough_breadth;
                    trough[0]->height = trough_y + trough_length 
                                            - (slider_y + slider_height) - 1;
                }
                else { /* XmHORIZONTAL */
                    trough[1]->x = trough_x;
                    trough[1]->y = trough_y;
                    trough[1]->width = slider_x - trough_x - 1;
                    trough[1]->height = trough_breadth;
                    trough[0]->x = slider_x + slider_width + 1;
                    trough[0]->y = trough_y;
                    trough[0]->width = trough_x + trough_length 
                                            - (slider_x + slider_width) - 1;
                    trough[0]->height = trough_breadth;
                }
	    }
            break;

        case mvsXmListWidgetClass: 
		{
            l_x = object->x; 
			l_y = object->y;
            l_w = object->width;
			l_h = object->height;
            w = widget;
            fontList = (XmFontList)mvsGetResourceValue(w,XmNfontList,
                                                       T_XmFontList);
            visibleItemsCount=(int)mvsGetResourceValue(w, XmNvisibleItemCount,
                                                       T_Integer);
            listMarginHeight=(int)mvsGetResourceValue(w,XmNlistMarginHeight,
                                                          T_Dimension);
            listMarginWidth = (int)mvsGetResourceValue(w,XmNlistMarginWidth,
                                                           T_Dimension);
            borderWidth = (int)mvsGetResourceValue(w,XmNborderWidth,
                                                       T_Dimension);
            listSpacing = (int)mvsGetResourceValue(w,XmNlistSpacing,
                                                       T_Dimension);
            topItemPosition = (int)mvsGetResourceValue(w,XmNtopItemPosition,
                                                           T_Integer);
            shadowThickness = (int)mvsGetResourceValue(w,XmNshadowThickness,
                                                           T_Dimension);
            highlightThickness=(int)mvsGetResourceValue(w,
                                                          XmNhighlightThickness,
                                                          T_Dimension);
            items = (XmStringTable)mvsGetResourceValue(w,XmNitems,
                                                               T_XmStringTable);
            itemCount = (int)mvsGetResourceValue(w,XmNitemCount,T_Integer);
            num_visible_items = itemCount - topItemPosition + 1;

            if (num_visible_items > visibleItemsCount)
                num_visible_items = visibleItemsCount;

            /*listMarginWidth + shadowThickness --> are these part of x?*/
            /*  borderWidth is *around* the list                        */
            curr_x = l_x;
            curr_y = l_y + listMarginHeight +
                         shadowThickness + highlightThickness;

            for (i=0; i<num_visible_items; i++) {
                child_obj = xisAllocObject(object,widget,oListItem,i);
                child_obj->id_code = next_avail_id_code++;
                child_obj->id.window = 0;  /* Regions do not have a window */
                child_obj->x = curr_x;
                child_obj->y = curr_y;
                s_h = XmStringHeight(fontList,items[i+topItemPosition-1]);
                child_obj->width = l_w;
                     /* Check if this is right.  Could be 1 pixel off */
                child_obj->height = s_h + listSpacing + highlightThickness+1;
                curr_y += s_h + listSpacing + highlightThickness+1;
                if(i==0) {
                    child_obj->height -= (listSpacing + highlightThickness-1);
                    curr_y -= (listSpacing + highlightThickness-1);
                }
            }
            /* List items appear to be one of the few XmString animals that
               are not copied when you do an XtGetValues(). Thus, do *NOT*
               free them here
            */
        }     /* End case XmList */
        break;

        case mvsXmTextWidgetClass:
		{

            t_x = object->x;
			t_y = object->y;
            t_w = object->width;
			t_h = object->height;
            w = widget;

	    /* 
	     * Added to get the font from text width's
	     * fontlist.
	     */
            fontList = (XmFontList)mvsGetResourceValue(w,XmNfontList, 
						       T_XmFontList);
	    if (XmFontListInitFontContext(&font_context, fontList) == True) {
		if (XmFontListGetNextFont(font_context, &charset, &fs) == False)
            		fs = mvsFontInfo[0].font_struct;
		XmFontListFreeFontContext(font_context);
		XtFree((char *) charset);
	    }
	    /*
	     * If a valid font can not be obtained from the text widget,
	     * then only use the fallback font from the font_info array.
	     */
	    else
            	fs = mvsFontInfo[0].font_struct;

            string = XmTextGetString(w);
            stringLength = strlen(string);
            marginHeight = (int)mvsGetResourceValue(w,XmNmarginHeight,
                                                        T_Dimension);
            marginWidth = (int)mvsGetResourceValue(w,XmNmarginWidth,
                                                       T_Dimension);
            borderWidth = (int)mvsGetResourceValue(w,XmNborderWidth,
                                                       T_Dimension);
            topCharacter = (int)mvsGetResourceValue(w,XmNtopCharacter,
                                                        T_XmTextPosition);
            shadowThickness = (int)mvsGetResourceValue(w,XmNshadowThickness,
                                                           T_Dimension);
            lineHeight = fs->ascent + fs->descent;

            highlightThickness=(int)mvsGetResourceValue(w,XmNhighlightThickness,
                                                        T_Dimension);
            curr_x = start_x = t_x + marginWidth + shadowThickness+ borderWidth;
            curr_y = t_y + marginHeight + shadowThickness + highlightThickness;

            i = topCharacter;
            while ((i != 0) && (string[i-1] != '\n')) {
                i--;
            }
            for (; i<stringLength; i++) {
                child_obj = xisAllocObject(object,widget,oTextChar,i);
                child_obj->id_code = next_avail_id_code++;
                child_obj->id.window = 0;  /* Regions do not have a window */
                child_obj->x = curr_x; /* May later want to use XmTextPosXY */
                child_obj->y = curr_y;
                child_obj->height = lineHeight;
                child_obj->width = XTextWidth(fs,&string[i],1);

                curr_x += child_obj->width;
                if (string[i] == '\n') {
                    curr_x = start_x;
                    curr_y += lineHeight;
                }
            }
            XtFree(string);
        }     /* End case XmText */
        break;

        case mvsXmTextFieldWidgetClass:
 	{
             int t_x = object->x, t_y = object->y;
             Widget w = widget;
             char *string = XmTextFieldGetString(w);
             int stringLength = strlen(string);
             int marginHeight = (int)mvsGetResourceValue(w,XmNmarginHeight,
                                                         T_Dimension);
             int marginWidth = (int)mvsGetResourceValue(w,XmNmarginWidth,
                                                        T_Dimension);
             int borderWidth = (int)mvsGetResourceValue(w,XmNborderWidth,
                                                        T_Dimension);
             int shadowThickness = (int)mvsGetResourceValue(w,XmNshadowThickness,
                                                            T_Dimension);
             int highlightThickness = (int)mvsGetResourceValue(w,
							  XmNhighlightThickness,
                                                          T_Dimension);
             int lineHeight;
             int start_x,curr_y,curr_x,i;
 

	     /* 
	      * Added to get the font from text field width's
	      * fontlist.
	      */
             fontList = (XmFontList)mvsGetResourceValue(w,XmNfontList, 
	  					        T_XmFontList);
	     if (XmFontListInitFontContext(&font_context, fontList) == True) {
		if (XmFontListGetNextFont(font_context, &charset, &fs) == False)
            		fs = mvsFontInfo[0].font_struct;
		XmFontListFreeFontContext(font_context);
		XtFree((char *) charset);
	     }
	     /*
	      * If a valid font can not be obtained from the text widget,
	      * then only use the fallback font from the font_info array.
	      */
	     else
            	fs = mvsFontInfo[0].font_struct;

             lineHeight = fs->ascent + fs->descent;

             curr_x = start_x = t_x + marginWidth + shadowThickness+ borderWidth;
             curr_y = t_y + marginHeight + shadowThickness + highlightThickness;
 
             for (i=0; i<stringLength; i++) {
                 child_obj = xisAllocObject(object,widget,oTextChar,i);
                 child_obj->id_code = next_avail_id_code++;
                 child_obj->id.window = 0;  /* Regions do not have a window */
                 child_obj->x = curr_x; /* May later want to use XmTextPosXY */
                 child_obj->y = curr_y;
                 child_obj->height = lineHeight;
                 child_obj->width = XTextWidth(fs,&string[i],1);
 
                 curr_x += child_obj->width;
                 if (string[i] == '\n') {
                     curr_x = start_x;
                     curr_y += lineHeight;
                 }
             }
             XtFree(string);
         }     /* End case XmTextField */
         break;

        /* case MAX_WIDGET_CLASS: */
        default:
            msg_trace("Widget of unknown class passed to mvsGetSubObjects()\n");
            break;
    } /* switch (WidgetClassCode) */

    return(object);

} /* End mvsGetSubObjects() */
