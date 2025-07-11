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
/*   $XConsortium: ABrowseP.h /main/8 1995/07/14 10:45:22 drk $ */

/************************************************************
 * ABrowseP.h: Private header file for ABrowse Widget Class *
 ************************************************************/

#ifndef _ABrowseP_h
#define _ABrowseP_h

#include <X11/FormP.h>
#include <X11/ConstrainP.h>

#include "ABrowse.h"

/*
 * ABrowse widget part of class record
 */

typedef struct _ABrowseClassPart
{
    int	empty;
} ABrowseClassPart;

/*
 * ABrowse widget full class record
 */

typedef struct _ABrowseClassRec
{
    CoreClassPart		core_class;
    CompositeClassPart		composite_class;
    ConstraintClassPart		constraint_class;
    FormClassPart		form_class;
    ABrowseClassPart		abrowse_class;
} ABrowseClassRec;

extern ABrowseClassRec	abrowseClassRec;

/*
 * ABrowse widget part of instance record
 */

typedef struct _ABrowsePart
{
    Pixmap		*images;		/* images to view	  */
    Cardinal		num_images;		/* number of images	  */
    Pixel		image_background;	/* image background color */
    Pixel		image_foreground;	/* image foreground color */
    Pixel		button_background;	/* image background color */
    Pixel		button_foreground;	/* image foreground color */
    XtCallbackList	help_callback;		/* help callback proc	  */
    XtCallbackList	quit_callback;		/* quit callback proc	  */

    int			current_image;		/* image being displayed  */
    Widget		prev;			/* prev button		  */
    Widget		next;			/* next button		  */
    Widget		help;			/* help button		  */
    Widget		quit;			/* quit button		  */
    Widget		image;			/* image display	  */
} ABrowsePart;

/*
 * ABrowse widget full instance record
 */

typedef struct _ABrowseRec
{
    CorePart		core;
    CompositePart	composite;
    ConstraintPart	constraint;
    FormPart		form;
    ABrowsePart		abrowse;
} ABrowseRec;

#endif _ABrowseP_h
