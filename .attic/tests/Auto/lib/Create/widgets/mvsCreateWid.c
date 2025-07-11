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
static char rcsid[] = "$XConsortium: mvsCreateWid.c /main/10 1995/07/14 11:25:25 drk $"
#endif
#endif
/***********************************************************************
  	@(#)mvsCreateWid.c	1.31.1.1	Date:1/22/91
  	Author: TAT
	History:
	   03/30/90 SJS Add to sccs tree
	   04/02/90 SJS Change level of messages
	   05/17/90 TAT Add code to allocate widget
           06/07/90 TAT Add code to support predefined composite creation
           06/25/90 SJS check for convenience function creates
           07/17/90 SJS do not try to manage the Shell that is serving as
                        our application context.
           07/29/90 PSN free arguments at end
           08/15/90 SJS try to get the hierarchy of menus correct
           09/18/90 SJS add intelligence to determine convenience parent
                        class
	Calls:

	Summary:
            Creates the specified widget and sets up the widget info record 
            associated with it. It returns a pointer to the widget info record.

            INPUTS
                widget_class_info  - Pointer to the mvs widget class info
                                     record that you want this widget info
                                     record to point to

                parent_widget_info - Pointer to the parent widget info record

                use_common         - When set to UseCommonArgs then use the
                                     common args found in the widget class
                                     record and merge them with those listed
                                     in the args parameter following.  When
                                     set to DontUseCommonArgs then only use
                                     those args listed in the parameter
                                     following.

                args               - The additional arguments that you wish
                                     to use.

                nargs              - The number of args listed in args (above).

            OUTPUTS
                none

            RETURNS
                Pointer to newly alloc'ed widget info record.
************************************************************************/

#include "mvslib.h" 
#include "xislib.h" 
#include <AutoMessages.h>
#include <Xm/DialogS.h>
#include <Xm/MenuShell.h>
#include <Xm/NotebookP.h>	/* Possible non-portability */

extern void NormalizePixmap();

extern Widget Shell1;

MvsWidgetInfoRecord *mvsCreateManageWidget(widget_class_info, parent_info, name,
					use_common, args_in, nargs_in, manage,
					xtclass)

MvsWidgetClassInfo *widget_class_info;
MvsWidgetInfoRecord *parent_info;
char *name;
int use_common, manage;
Arg args_in[];
Cardinal nargs_in;
WidgetClass xtclass;
{
    MvsWidgetInfoRecord *widget_info, *NB_child;
    static MvsWidgetInfoRecord *default_pg_scr_info;
    XSetWindowAttributes wattr;
    int i, j, n;
    Arg args[MAX_RESOURCES];
    Cardinal nargs=0;
    Arg color_args[50];
    Cardinal num_color_args;
    MvsResourceInfoRecord *resource_info = widget_class_info->resource_info;
    int num_res_colors = widget_class_info->num_res_colors;
    short *res_color_ref = widget_class_info->res_color_ref;
    short *res_pixmap_ref = widget_class_info->res_pixmap_ref;
    Widget tmp_widget;
    Widget save_widget;
    Widget created_widget;
    MvsWidgetInfoRecord *new_info=NULL;
    MvsWidgetClassInfo *convenience_class=NULL;
    Widget child;
    static int created_page_scroller = False;
    static int orig_page_scroller_destroyed = False;
    unsigned char NBChildType;
    XisObjectRecord *PagScr_object, *parent_xis_object, *cur_xis_object;
    XisObjectRecord *prev_xis_object;
    unsigned char nb_child_type;
    Cardinal num_children;

    msg_trace ("<mvsCreateWidget> Creating Widget.\n");

    /* Nullify args[] */
    for ( i=0 ; i<MAX_RESOURCES ; i++ ) {
	args[i].value = 0L;
	args[i].name = NULL;
    }

    if (parent_info == NULL)
        AutoMessage(_AutoMessages[WARNMSG40]);

    for (i = 0 ; i < nargs_in ; i++) {
	args[i].name  = args_in[i].name;
	args[i].value = args_in[i].value;
    }

    nargs = nargs_in;

    /* Allocate and setup widget info record */
    widget_info = mvsAllocWidgetInfo(widget_class_info, parent_info, 
                                     (Widget)NULL                    );

    /* Then if in generate mode, append "hard-wired" color settings */
    if (mvsGetVisualMode() == VISUAL_MODE_GENERATE_PIXMAPS) {
        num_color_args = 0;
        for (i=0; i<num_res_colors; i++) {
	    if (res_color_ref[i] >= 0) {
		XtSetArg(color_args[num_color_args],
			 resource_info[res_color_ref[i]].name,
			 widget_info->unique_color_ref[i]);

		num_color_args++;
	    }
        }

        mvsCopyReplaceArgs(widget_class_info, args, &nargs, color_args, 
			   num_color_args);
    }

    /* Create actual widget */
    widget_info->widget = (*widget_class_info->proc_CreateWidget)
             (parent_info->widget,name,args,nargs,xtclass);

    if (widget_info->widget == NULL) {
        AutoError (_AutoMessages[WARNMSG99]);
    }

    widget_info->window = 0;

    /* 
       Manage widget if it is not a shell, or a child of a MenuShell,
       (Pulldown, Popup), also, do not manage if parent is a Dialog
       Shell (ErrorDialog, FormDialog...).
   */

    XtRealizeWidget(widget_info->widget);

    /* Manage widget if it is not a Shell */

    if((parent_info != mvsRootWidgetInfo) && (manage == TRUE)   &&
       (! XmIsMenuShell(widget_info->widget))			&&
       (! XmIsMenuShell(XtParent(widget_info->widget)))		&&
       (! XmIsDialogShell(XtParent(widget_info->widget))))
        XtManageChild (widget_info->widget);

/* used in 1.1

    XtRealizeWidget(widget_info->widget);

*/

    /*
     * If the widget being created is a Notebook, we need
     * to test if the default PageScroller has been created
     */
    if ((widget_info->parent->widget != NULL ) && 
	(mvsGetClassCode(widget_info->parent->widget) 
		== mvsXmNotebookWidgetClass)) {
	child = XtNameToWidget(widget_info->parent->widget, "PageScroller");
	/*
	 * Add the PageScroller to the object hierarchy if it
	 * has not already been added
	 */
	if (created_page_scroller == False) {
	    default_pg_scr_info = mvsSetupNewWidgetInfo(widget_info->parent,
							child,
							iXmSpinBoxWidgetClass);
	    mvsCreateChildren(default_pg_scr_info);
	    created_page_scroller = True;
	}
	/*
	 * Remove the default PageScroller if a new user defined
	 * one is being created.
	 */
	n = 0;
	XtSetArg(args[n], XmNnotebookChildType, &nb_child_type);	n++;
	XtGetValues(widget_info->widget, args, n);
	if ((nb_child_type == XmPAGE_SCROLLER) && 
	    (orig_page_scroller_destroyed == False) &&
	    (created_page_scroller == True))  {
	    /*
	     * Destroy the default PageScroller in the mvs heirarchy
	     */
	    mvsDestroyWidget(default_pg_scr_info,DontCheckCallbacks);
	    orig_page_scroller_destroyed = True;
	}
    }

    /* 
	For Pixmap resources do the following:

	1) Find proper background and foreground pixel values. If
	   you are recording this will be the unique color info of
	   the widget. If you are playing back, get the forteground
	   and background from the parent (if foreground and background
	   is not specified in the argument list).

	2) Find out if a pixmap resource is present.

	3) If pixmap is present get image and do a XPutPixel
	   with first pixel being the background pixel and the
	   second pixel being the foreground pixel.

	This makes the pixmap contain the background and foreground
	color of the widget (if you are in playback), or the chosen
	color (if you are in record mode).

    */

    /* Get the Pixmap resources and see if they need to be normalized.
       They can have been set through the resource file, in that case 
       add them to the arg list so that we don't reload them when 
       loading the default resources */

    
    for (i =0; i< num_res_colors; i++) {
	if (res_pixmap_ref[i] >= 0) {
	    Arg pixmap_args[1];
	    Pixmap value_pixmap;

	    /* Check if this pixmap needs to be normalized */
	    
	    XtSetArg(pixmap_args[0],
		     resource_info[res_pixmap_ref[i]].name,
		     &value_pixmap);
	    
	    XtGetValues(widget_info->widget, pixmap_args, 1);
	    
	    if ((value_pixmap != XmUNSPECIFIED_PIXMAP) &&
		(value_pixmap != None)) {

		/* Normalize the pixmap only when in record or compare mode */
		
		if (mvsGetVisualMode() != VISUAL_MODE_DONT_CHECK)
		    NormalizePixmap(widget_info, value_pixmap);
		
		if (strcmp(resource_info[res_pixmap_ref[i]].name, 
			       "backgroundPixmap") == 0) {
	           
		    Mask    window_mask;
		    XSetWindowAttributes attributes;

		    window_mask = CWBackPixmap;
		    attributes.background_pixmap = value_pixmap;
						 
		    XChangeWindowAttributes(mvsDisplay, 
					    XtWindow(widget_info->widget), 
					    window_mask, &attributes);
		}

		/* Check if this needs to be added to the list of args */

		for (j = 0; j < nargs && 
		    strcmp(args[j].name,resource_info[res_pixmap_ref[i]].name);
		    j++);

		if (j == nargs) 
		    mvsCopyReplace1Arg(widget_class_info, args, &nargs, 
				       pixmap_args[0].name, value_pixmap);

	    }
	}
    }


    /* Set resources to default values */
    mvsSetResourcesToDefaults(widget_info,args,nargs);
    mvsSetIntToDefaults(widget_info);

    /* Store arg values in local copy of resources */
    if (nargs > 0) {
        if (mvsGetVisualMode() == VISUAL_MODE_GENERATE_PIXMAPS) {
            /* Make mvsSetLocalValues store color arguments locally */
            mvsSetVisualMode(VISUAL_MODE_DONT_CHECK);
            mvsSetLocalValues(widget_info,args,&nargs);
            mvsSetVisualMode(VISUAL_MODE_GENERATE_PIXMAPS);
        }
        else
            mvsSetLocalValues(widget_info,args,&nargs);
    }


    /* See if this was some sort of Convenience function that creates more   */
    /* than just the widget we asked for. If it is, then it is necessary to  */
    /* get all the new widgets and their infos into our widget hierarchy.    */
    /* See if children were created and alloc their widget_infos and colors. */

    tmp_widget = XtParent(widget_info->widget);
    if( tmp_widget != parent_info->widget) {

        save_widget = tmp_widget; 
        while ( (tmp_widget != NULL) &&
              (tmp_widget != parent_info->widget)) {

            save_widget = tmp_widget; 
            tmp_widget = XtParent(tmp_widget);
        }
        if(tmp_widget == NULL) {
            /* The parent we thought we had is nowhere in the ancestor chain
               above the parent we got. Look elsewhere for the widget
               in the hierarchy. Specifically, popup & pulldown menus
               look for the shell above them when they are children of
               a popup or pulldown.
            */
            tmp_widget = XtParent(widget_info->widget);
            save_widget = widget_info->widget;
            while (tmp_widget != NULL) {
                if(mvsWidgetToWidgetInfo(tmp_widget) !=
                                                  (MvsWidgetInfoRecord *)NULL) {                    parent_info = mvsWidgetToWidgetInfo(tmp_widget);
                    break;
                }
                save_widget = tmp_widget;
                tmp_widget = XtParent(tmp_widget);
            }
        }
        if(tmp_widget == NULL) {
        /* FUTURE: ADD IN call to determine class code. Use the return instead
           of blindly using iXmDialogShellWidgetClass.
        */
            AutoMessage(_AutoMessages[WARNMSG41]);
            new_info = mvsSetupNewWidgetInfo(mvsRootWidgetInfo,save_widget,
                                             iXmDialogShellWidgetClass);
        }
        else {
          /* The widget we are creating is not really created as a child   */
          /* of the requested parent. Rather, it has a parent which is the */
          /* child of the requested parent. (It is also possible that the  */
          /* parent is a widget somewhere else in the widget hierarchy)    */
          /* We basically, free the old widget info and construct a new    */
          /* one, including the dialog shell above (if a dialog shell is   */
          /* created. Popup & Pulldown can share common ancestor shells).  */
          /* The most important thing is to make sure that all widgets     */
          /* that were created end up in the hierarchy in a location that  */
          /* reflects the parent/child relations as seen by the user.      */
          /* The real window layout is handled by our object hierarchy.    */

            created_widget = widget_info->widget;
            mvsFreeWidgetInfo(widget_info);

            /* When the created widget & the saved widget are equal, it    */
            /* means that the widget we created was re-parented to         */
            /* somewhere else in the hierarchy. Thus, do not treat this    */
            /* case as a "Convenience" parent.                             */

            if(created_widget != save_widget) {
                new_info = mvsWidgetToWidgetInfo(save_widget);
                if(new_info == (MvsWidgetInfoRecord *)NULL) {
                    switch(mvsGetClassCode(save_widget)) {
                        case mvsXmDialogShellWidgetClass:
                            convenience_class = iXmDialogShellWidgetClass;
                            break;
                        case mvsXmMenuShellWidgetClass:
                            convenience_class = iXmMenuShellWidgetClass;
                            break;
                        default:
                            convenience_class = iXmScrolledWindowWidgetClass;
                            break;
                    }
                    new_info = mvsSetupNewWidgetInfo(parent_info,save_widget,
                                                     convenience_class);
                }
            }
            else {
                new_info = parent_info;
            }

            /* Allocate and setup widget info record */

            widget_info = mvsAllocWidgetInfo(widget_class_info, new_info, 
                                                 (Widget)NULL);
            widget_info->widget = created_widget;
            widget_info->window = 0;
            if(created_widget != save_widget) {
                widget_info->c_parent = True;
            }
            else {
                widget_info->c_parent = False;
            }

	    /* Set resources to default values */

            mvsSetResourcesToDefaults(widget_info,args,nargs);
            mvsSetIntToDefaults(widget_info);

            /* Store arg values in local copy of resources */

            if (nargs > 0) {
                if (mvsGetVisualMode() == VISUAL_MODE_GENERATE_PIXMAPS) {
                    /* Make mvsSetLocalValues store color arguments locally */
                    mvsSetVisualMode(VISUAL_MODE_DONT_CHECK);
                    mvsSetLocalValues(widget_info,args,&nargs);
                    mvsSetVisualMode(VISUAL_MODE_GENERATE_PIXMAPS);
                }
                else
                    mvsSetLocalValues(widget_info,args,&nargs);
            }
        }
    } /* End if(tmp_widget != parent_info->widget) */

    /* Create all the children of the widget we just created. We are    */
    /* *NOT* doing the parents of Convenience created widgets here.     */
    /* The reason is that right now we are unsure of the types of the   */
    /* chilren involved. This could possibly be improved upon.          */

    if (widget_info->parent != NULL) {

        if ((mvsGetClassCode(widget_info->widget) == mvsXmTextWidgetClass ||
       	     mvsGetClassCode(widget_info->widget) == mvsXmListWidgetClass) &&
	     mvsGetClassCode(XtParent(widget_info->widget)) == 
						mvsXmScrolledWindowWidgetClass)
    		mvsCreateChildren(widget_info->parent);
    }
		
    mvsCreateChildren(widget_info);
    
    /* Set up callback checking code if in behavior test set */

    XSync(mvsDisplay,0);

    if (widget_info->c_parent)
        new_info->window = XtWindowOfObject(new_info->widget);

    widget_info->window = XtWindowOfObject(widget_info->widget);
    xisCreateWidget(widget_info->widget);

    /* Make sure that window manager recognizes color map of this widget */
    wattr.colormap = mvsVisualInfo.colormap_id;
    XChangeWindowAttributes(mvsDisplay, XtWindowOfObject(widget_info->widget), 
                            CWColormap,&wattr);

    /* Process all events through expose event */
    xisProcessEvents(NULL, 0);
    xisUpdateObjectAttributes();

    /* Return pointer to widget info record */
    return(widget_info);

} /* End mvsCreateWidget() */


/***********************************************************************
  	Author: PSN
	History:
           08/15/90 PSN add manage/unmanage
	Summary:
		Create & manage widget
***********************************************************************/

MvsWidgetInfoRecord *mvsCreateWidget(widget_class_info, parent_info, name,
				     use_common, args_in, nargs_in, xtclass)

MvsWidgetClassInfo *widget_class_info;
MvsWidgetInfoRecord *parent_info;
char *name;
int use_common;
Arg args_in[];
Cardinal nargs_in;
WidgetClass xtclass;
{
/**Debug**
    return(mvsCreateManageWidget(widget_class_info, parent_info, name,
    				 use_common, args_in, nargs_in, TRUE));	
**/
    return(mvsCreateManageWidget(widget_class_info, parent_info, name,
    				 use_common, args_in, nargs_in, TRUE, xtclass));	

} /* End mvsCreateWidget() */

/***********************************************************************
  	Author: PSN
	History:
           08/15/90 PSN add manage/unmanage
	Summary:
		Create but do not manage widget
***********************************************************************/

MvsWidgetInfoRecord *mvsCreateNotManageWidget(widget_class_info, parent_info, 
					 name, use_common, args_in, nargs_in,
					 xtclass)

MvsWidgetClassInfo *widget_class_info;
MvsWidgetInfoRecord *parent_info;
char *name;
int use_common;
Arg args_in[];
Cardinal nargs_in;
WidgetClass xtclass;
{
    return(mvsCreateManageWidget(widget_class_info, parent_info, name,
    				 use_common, args_in, nargs_in, FALSE,
				 xtclass));

} /* End mvsCreateNotManageWidget() */


void NormalizePixmap(widget_info, value_pixmap)
MvsWidgetInfoRecord *widget_info;
Pixmap value_pixmap;
{
    Arg color_args[10];
    Cardinal num_color_args;
    Pixel foreground, background;
    Window root;
    int pix_x, pix_y, x, y;
    unsigned int pix_width, pix_height, pix_border, depth;
    XImage *pix_image = NULL;
    Pixel pixel1, pixel2;
    GC gc;
    
    if (XtParent(widget_info->widget) == NULL) {
	printf("mvsCreateWidget: NULL parent in visuals\n");
	exit(0);
    }

    num_color_args = 0;
    XtSetArg(color_args[num_color_args], XmNbackground, &background); 
    num_color_args++;
    XtSetArg(color_args[num_color_args], XmNforeground, &foreground); 
    num_color_args++;
    if (XmIsGadget(widget_info->widget) && 
	(mvsGetClassCode(widget_info->widget) != mvsXmIconGadgetClass)) 
	if (XtParent(widget_info->widget) != Shell1)
	    XtGetValues(XtParent(widget_info->widget), color_args, 
			num_color_args);
	else
	    AutoMessage(_AutoMessages[WARNMSG100]);
    else
	XtGetValues(widget_info->widget, color_args, num_color_args);

    if (! XGetGeometry(mvsDisplay, value_pixmap, &root,
		       &pix_x, &pix_y, &pix_width, &pix_height,
		       &pix_border, &depth)) {
	printf("mvsCreateWid: Bad return from XGetGeometry\n");
	exit(0);
    }

    pix_image = XGetImage(mvsDisplay, value_pixmap, 0, 0, pix_width,
			  pix_height, AllPlanes, ZPixmap);

    pixel1 = XGetPixel(pix_image,0,0);

    /* Don't assume pixmaps are only using 2 colors.
       Temporary fix, we'll need to improve it for checking real colors.
     */

    for (y=0; y<pix_height; y++) {
	for (x=0; x<pix_width; x++) {
	    if (XGetPixel(pix_image,x,y) != pixel1) {
		pixel2 = XGetPixel(pix_image,x,y);
		break;
	    }
	}
    }

    for (y=0; y<pix_height; y++) {
	for (x=0; x<pix_width; x++) {
	    if (XGetPixel(pix_image,x,y) == pixel1) 
		XPutPixel(pix_image, x, y, background);
	    else
		/*  if (XGetPixel(pix_image,x,y) == pixel2)    */
		XPutPixel(pix_image, x, y, foreground);
	}
    }

    gc = XCreateGC (mvsDisplay, value_pixmap, 0, (XGCValues *)NULL);

    XPutImage(mvsDisplay, value_pixmap, gc, pix_image,  0, 0,
		  0, 0, pix_width, pix_height);

    XFreeGC(mvsDisplay, gc);

    XDestroyImage(pix_image);
}
