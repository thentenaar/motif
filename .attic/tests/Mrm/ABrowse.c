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
static char rcsid[] = "$XConsortium: ABrowse.c /main/8 1995/07/14 10:44:50 drk $"
#endif
#endif

/*******************************************
 * ABrowse.c: Source code for ABrowse Widget *
 *******************************************/

/*
 * Include files
 */

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xmu.h>

#include <Xm/Xm.h>

#include <X11/Label.h>
#include <X11/Command.h>

#include "ABrowseP.h"

#include <stdio.h>

/*
 * Callback Routines
 */

static void	ChangeImage();
static void	ButtonCallback();

/*
 * Convenience constants and macros
 */

#define PREV	0
#define NEXT	1

/*
 * Convenience routines
 */

static void	CreateChildren();

/*
 * Resource List
 */

#define Offset(field) XtOffset(ABrowseWidget, abrowse.field)

static XtResource	resources[] =
{
    {
	XtNimages,
	XtCImages,
	XtRPointer,
	sizeof(caddr_t),
	Offset(images),
	XtRString,
	(caddr_t) NULL,
    },

    {
	XtNnumImages,
	XtCNumImages,
	XtRInt,
	sizeof(int),
	Offset(num_images),
	XtRString,
	"0",
    },

    {
	XtNimageBackground,
	XtCBackground,
	XtRPixel,
	sizeof(Pixel),
	Offset(image_background),
	XtRString,
	"White",
    },
    
    {
	XtNimageForeground,
	XtCForeground,
	XtRPixel,
	sizeof(Pixel),
	Offset(image_foreground),
	XtRString,
	"Black",
    },

    {
	XtNbuttonBackground,
	XtCBackground,
	XtRPixel,
	sizeof(Pixel),
	Offset(button_background),
	XtRString,
	"White",
    },
    
    {
	XtNbuttonForeground,
	XtCForeground,
	XtRPixel,
	sizeof(Pixel),
	Offset(button_foreground),
	XtRString,
	"Black",
    },

    {
	XtNquitCallback,
	XtCCallback,
	XtRCallback,
	sizeof(XtCallbackList),
	Offset(quit_callback),
	XtRPointer,
	(caddr_t) NULL,
    },

    {
	XtNhelpCallback,
	XtCCallback,
	XtRCallback,
	sizeof(XtCallbackList),
	Offset(help_callback),
	XtRPointer,
	(caddr_t) NULL,
    },
};

#undef Offset

/*
 * Methods
 */

static void	Initialize();
static Boolean	SetValues();

/*
 * Initialize Class Record
 */

ABrowseClassRec	abrowseClassRec =
{

/*
 * CoreClassPart
 */

    {
	(WidgetClass) &formClassRec,	/* superclass			*/
	"ABrowse",			/* class_name			*/
	sizeof(ABrowseRec),		/* widget_size			*/
	NULL,				/* class_initialize		*/
        NULL,				/* class_part_initialize	*/
	FALSE,				/* class_inited			*/
	Initialize,			/* initialize			*/
	NULL,				/* initialize_hook		*/
	XtInheritRealize,		/* realize			*/
	NULL,				/* actions			*/
	0,				/* num_actions			*/
	resources,			/* resources			*/
	XtNumber(resources),		/* num_resources		*/
	NULLQUARK,			/* xrm_class			*/
	TRUE,				/* compress_motion		*/
	TRUE,				/* compress_exposure		*/
	TRUE,				/* compress_enterleave		*/
	FALSE,				/* visible_interest		*/
	NULL,				/* destroy			*/
	XtInheritResize,		/* resize			*/
	XtInheritExpose,		/* expose			*/
	SetValues,			/* set_values			*/
	NULL,				/* set_values_hook		*/
	XtInheritSetValuesAlmost,	/* set_values_almost		*/
	NULL,				/* get_values_hook		*/
	NULL,				/* accept_focus			*/
	XtVersion,			/* version			*/
	NULL,				/* callback private		*/
	XtInheritTranslations,		/* tm_table			*/
	NULL,				/* query_geometry		*/
	NULL,				/* display_accelerator		*/
	NULL,				/* extension			*/
    },

/*
 * CompositeClassPart
 */

    {
	XtInheritGeometryManager,	/* geometry_manager		*/
	XtInheritChangeManaged,		/* change_managed		*/
	XtInheritInsertChild,		/* insert_child			*/
	XtInheritDeleteChild,		/* delete_child			*/
	NULL,				/* extension			*/
    },

/*
 * ConstraintClassPart
 */
    
    {
	NULL,				/* constraint resource  	*/
	0,				/* number of constraints	*/
	sizeof(FormConstraintsRec),	/* size of constraint		*/
	NULL,				/* initialization       	*/
	NULL,				/* destroy proc         	*/
	NULL,				/* set_values proc      	*/
	NULL,				/* extension	        	*/
    },

/*
 * FormClassPart
 */

    {
	NULL,				/* empty			*/
    },

/*
 * ABrowseClassPart
 */

    {
	0,				/* empty			*/
    }
};

WidgetClass abrowseWidgetClass = (WidgetClass) &abrowseClassRec;


/*********************************
 ***** ABrowse Widget Methods *****
 *********************************/

/**************
 * Initialize *
 **************/

static void Initialize(request, new)
     ABrowseWidget	request,
			new;
{

/*
 * Window size must be > 0.  This is not done by Core Initialize() Method
 */

    if(request->core.width == 0)
    {
	new->core.width = 5;
    }

    if(request->core.height == 0)
    {
	new->core.height = 5;
    }

/*
 * Set default spacing
 */

    new->form.default_spacing = 10;

/*
 * Start with first image
 */

    new->abrowse.current_image = -1;

/*
 * Create child widgets
 */

    CreateChildren(new);
}

/*************
 * SetValues *
 *************/

static Boolean SetValues(current, request, new)
     ABrowseWidget	current,
			request,
     			new;
{
    Arg	args[2];
    int	nargs;
  
/*
 * If image or button colors have changed, call XtSetValues to make the change
 */    

    if((new->abrowse.image_background != current->abrowse.image_background) ||
       (new->abrowse.image_foreground != current->abrowse.image_foreground))
    {
	if(new->abrowse.image)
	{
	    nargs = 0;
	    XtSetArg(args[nargs], XtNbackground,
		     new->abrowse.image_background); nargs++;
	    XtSetArg(args[nargs], XtNforeground,
		     new->abrowse.image_foreground); nargs++;

	    XtSetValues(new->abrowse.image, args, nargs);
	}
    }
    
    if((new->abrowse.button_background !=
	current->abrowse.button_background) ||
       (new->abrowse.button_foreground != current->abrowse.button_foreground))
    {
	nargs = 0;
	XtSetArg(args[nargs], XtNbackground, new->abrowse.button_background);
	nargs++;
	XtSetArg(args[nargs], XtNforeground, new->abrowse.button_foreground);
	nargs++;

	if(new->abrowse.prev)
	{
	    XtSetValues(new->abrowse.prev, args, nargs);
	}

	if(new->abrowse.next)
	{
	    XtSetValues(new->abrowse.next, args, nargs);
	}

	if(new->abrowse.help)
	{
	    XtSetValues(new->abrowse.help, args, nargs);
	}

	if(new->abrowse.quit)
	{
	    XtSetValues(new->abrowse.quit, args, nargs);
	}
    }

/*
 * If images have changed, place first image in image label widget
 */
    
    if(new->abrowse.images != current->abrowse.images)
    {
	new->abrowse.current_image = -1;

	if(new->abrowse.image)
	{
	    ChangeImage(new->abrowse.image, NEXT, NULL);
	}
    }
    
    return(FALSE);
}


/********************************
 ***** Convenience routines *****
 ********************************/

/******************
 * CreateChildren *
 ******************/

static void
CreateChildren(parent)
     ABrowseWidget	parent;
{
    Arg		args[6];
    int		nargs;
    Widget	left_att;

/*
 * Create child widgets
 */

/*
 * Image: Label
 */

    nargs = 0;
    XtSetArg(args[nargs], XtNfromVert, NULL); nargs++;
    XtSetArg(args[nargs], XtNfromHoriz, NULL); nargs++;
	
    parent->abrowse.image = XtCreateWidget("Image",		/* name	     */
					   labelWidgetClass,	/* class     */
					   parent,		/* parent    */
					   args,		/* arguments */
					   nargs);		/* num_args  */

    if(parent->abrowse.images)
    {
	ChangeImage(parent->abrowse.image, NEXT, NULL);
    }
    

/*
 * Prev: PushButton
 */

    nargs = 0;
    XtSetArg(args[nargs], XtNfromVert, parent->abrowse.image); nargs++;
    XtSetArg(args[nargs], XtNfromHoriz, NULL); nargs++;
	
    parent->abrowse.prev = XtCreateWidget("Prev",		/* name	     */
					  commandWidgetClass,	/* class     */
					  parent,		/* parent    */
					  args,			/* arguments */
					  nargs);		/* num_args  */

    XtAddCallback(parent->abrowse.prev,
		  XtNcallback,
		  ChangeImage,
		  PREV);

/*
 * Next: PushButton
 */

    nargs = 0;
    XtSetArg(args[nargs], XtNfromVert, parent->abrowse.image); nargs++;
    XtSetArg(args[nargs], XtNfromHoriz, parent->abrowse.prev); nargs++;
	
    parent->abrowse.next = XtCreateWidget("Next",		/* name	     */
					  commandWidgetClass,	/* class     */
					  parent,		/* parent    */
					  args,			/* arguments */
					  nargs);		/* num_args  */

    XtAddCallback(parent->abrowse.next,
		  XtNcallback,
		  ChangeImage,
		  NEXT);

/*
 * Attachments for Help and Quit widgets depend on width
 * of Image widget versus widths of Prev and Next widgets
 */

    if(parent->abrowse.image->core.width >=
       (parent->abrowse.prev->core.width + parent->abrowse.next->core.width))
    {
	left_att = parent->abrowse.image;
    }
    else
    {
	left_att = parent->abrowse.next;
    }
    

/*
 * Help: PushButton
 */

    nargs = 0;
    XtSetArg(args[nargs], XtNfromVert, NULL); nargs++;
    XtSetArg(args[nargs], XtNfromHoriz, left_att); nargs++;
	
    parent->abrowse.help = XtCreateWidget("Help",		/* name	     */
					  commandWidgetClass,	/* class     */
					  parent,		/* parent    */
					  args,			/* arguments */
					  nargs);		/* num_args  */

    XtAddCallback(parent->abrowse.help,
		  XtNcallback,
		  ButtonCallback,
		  (caddr_t) parent);

/*
 * Quit: PushButton
 */

    nargs = 0;
    XtSetArg(args[nargs], XtNfromVert, parent->abrowse.help); nargs++;
    XtSetArg(args[nargs], XtNfromHoriz, left_att); nargs++;
	
    parent->abrowse.quit = XtCreateWidget("Quit",		/* name	     */
					  commandWidgetClass,	/* class     */
					  parent,		/* parent    */
					  args,			/* arguments */
					  nargs);		/* num_args  */

    XtAddCallback(parent->abrowse.quit,
		  XtNcallback,
		  ButtonCallback,
		  (caddr_t) parent);

/*
 * Manage children
 */

    XtManageChildren(parent->composite.children,
		     parent->composite.num_children);
}

/***************
 * ChangeImage *
 ***************/

static void
ChangeImage(w, which, call_data)
     Widget	w;
     int	which;
     caddr_t	call_data;
{
    Arg			args[1];
    int			nargs;
    ABrowseWidget	parent = (ABrowseWidget) w->core.parent;
    LabelWidget		image = (LabelWidget) parent->abrowse.image;
    int			*current_image = &parent->abrowse.current_image,
    			num_images = parent->abrowse.num_images;

    if(which == NEXT)
    {
	if(*current_image < num_images - 1)
	{
	    (*current_image)++;
	    
	    nargs = 0;
	    XtSetArg(args[nargs], XtNbitmap,
		     parent->abrowse.images[*current_image]); nargs++;
	    XtSetValues(image, args, nargs);
	}
    }
    else if(which == PREV)
    {
	if(*current_image > 0)
	{
	    (*current_image)--;

	    nargs = 0;
	    XtSetArg(args[nargs], XtNbitmap,
		     parent->abrowse.images[*current_image]); nargs++;
	    XtSetValues(image, args, nargs);
	}
    }
}

/******************
 * ButtonCallback *
 ******************/

static void
ButtonCallback (w, parent, call_data)
     Widget		w;
     ABrowseWidget	parent;
     caddr_t		call_data;
{
    if(w == parent->abrowse.help)
    {
	XtCallCallbacks(parent,
			XtNhelpCallback,
			NULL);
    }
    else if(w == parent->abrowse.quit)
    {
	XtCallCallbacks(parent,
			XtNquitCallback,
			NULL);
    }
    
}

/*******************
 *  XtCreateABrowse *
 *******************/

Widget
XtCreateABrowse (parent, name, args, nargs)
     Widget	parent;
     char	*name;
     Arg	*args;
     int	nargs;
{
    return (XtCreateWidget(name,
			   abrowseWidgetClass,
			   parent,
			   args,
			   nargs));
}
