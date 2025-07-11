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
/*   $XConsortium: BrowseP.h /main/8 1995/07/14 10:46:03 drk $ */

/**********************************************************
 * BrowseP.h: Private header file for Browse Widget Class *
 **********************************************************/

#ifndef BROWSEP_H
#define BROWSEP_H

#include <Xm/BulletinBP.h>
#include <Xm/FormP.h>
#include <Xm/XmP.h>

#include "Browse.h"

/*
 * Browse widget part of class record
 */

typedef struct _BrowseClassPart
{
    int	empty;
} BrowseClassPart;

/*
 * Browse widget full class record
 */

typedef struct _BrowseClassRec
{
    CoreClassPart		core_class;
    CompositeClassPart		composite_class;
    ConstraintClassPart		constraint_class;
    XmManagerClassPart		manager_class;
    XmBulletinBoardClassPart	bulletin_board_class;
    XmFormClassPart		form_class;
    BrowseClassPart		browse_class;
} BrowseClassRec;

extern BrowseClassRec	browseClassRec;

/*
 * Browse widget part of instance record
 */

typedef struct _BrowsePart
{
    Pixmap		*images;		/* images to view	  */
    Cardinal		num_images;		/* number of images	  */
    Pixel		image_background;	/* image background color */
    Pixel		image_foreground;	/* image foreground color */
    Pixel		button_background;	/* image background color */
    Pixel		button_foreground;	/* image foreground color */
    XtCallbackList	quit_callback;		/* quit callback proc	  */

    int			current_image;		/* image being displayed  */
    Widget		prev;			/* prev button		  */
    Widget		next;			/* next button		  */
    Widget		help;			/* help button		  */
    Widget		quit;			/* quit button		  */
    Widget		image;			/* image display	  */
} BrowsePart;

/*
 * Browse widget full instance record
 */

typedef struct _BrowseRec
{
    CorePart		core;
    CompositePart	composite;
    ConstraintPart	constraint;
    XmManagerPart	manager;
    XmBulletinBoardPart	bulletin_board;
    XmFormPart		form;
    BrowsePart		browse;
} BrowseRec;

#endif /* BROWSEP_H */
