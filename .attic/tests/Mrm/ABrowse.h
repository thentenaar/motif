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
/*   $XConsortium: ABrowse.h /main/8 1995/07/14 10:45:01 drk $ */

/**********************************************************
 * ABrowse.h: Public header file for ABrowse Widget Class *
 **********************************************************/

#ifndef _ABrowse_h
#define _ABrowse_h

#include <X11/Constraint.h>

/* Parameters:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 background	     Background		Pixel		XtDefaultBackground
 border		     BorderColor	Pixel		XtDefaultForeground
 borderWidth	     BorderWidth	Dimension	1
 buttonBackground    Background		Pixel		White
 buttonForeground    Foreground		Pixel		Black
 defaultDistance     Thickness		int		4
 destroyCallback     Callback		Pointer		NULL
 height		     Height		Dimension	computed at realize
 helpCallback	     Callback		Pointer		NULL
 imageBackground     Background		Pixel		White
 imageForeground     Foreground		Pixel		Black
 images		     Images		Pointer		NULL
 mappedWhenManaged   MappedWhenManaged	Boolean		True
 numImages	     NumImages		Int		0
 quitCallback	     Callback		Pointer		NULL
 sensitive	     Sensitive		Boolean		True
 width		     Width		Dimension	computed at realize
 x		     Position		Position	0
 y		     Position		Position	0

*/

/* Constraint parameters:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 bottom		     Edge		XtEdgeType	XtRubber
 fromHoriz	     Widget		Widget		(left edge of form)
 fromVert	     Widget		Widget		(top of form)
 horizDistance	     Thickness		int		defaultDistance
 left		     Edge		XtEdgeType	XtRubber
 resizable	     Boolean		Boolean		False
 right		     Edge		XtEdgeType	XtRubber
 top		     Edge		XtEdgeType	XtRubber
 vertDistance	     Thickness		int		defaultDistance

*/

#define XtNimages		"images"
#define XtNnumImages		"numImages"
#define XtNimageBackground	"imageBackground"
#define XtNimageForeground	"imageForeground"
#define XtNbuttonBackground	"buttonBackground"
#define XtNbuttonForeground	"buttonForeground"
#define XtNhelpCallback		"helpCallback"
#define XtNquitCallback		"quitCallback"

#define XtCImages		"Images"
#define XtCNumImages		"NumImages"

typedef struct _ABrowseClassRec	*ABrowseWidgetClass;
typedef struct _ABrowseRec	*ABrowseWidget;

extern WidgetClass		abrowseWidgetClass;

extern Widget			XtCreateABrowse();

#endif _ABrowse_h
