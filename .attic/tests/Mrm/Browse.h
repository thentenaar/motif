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
/*   $XConsortium: Browse.h /main/8 1995/07/14 10:45:40 drk $ */

/********************************************************
 * Browse.h: Public header file for Browse Widget Class *
 ********************************************************/

#ifndef BROWSE_H
#define BROWSE_H

#include <Xm/Xm.h>

extern WidgetClass		browseWidgetClass;

extern Widget			XmCreateBrowse();

typedef struct _BrowseClassRec	*BrowseWidgetClass;
typedef struct _BrowseRec	*BrowseWidget;

/*
 * Define resource strings for the Browse widget
 */

#define XmNimages		"images"
#define XmNnumImages		"numImages"
#define XmNimageBackground	"imageBackground"
#define XmNimageForeground	"imageForeground"
#define XmNbuttonBackground	"buttonBackground"
#define XmNbuttonForeground	"buttonForeground"
#define XmNquitCallback		"quitCallback"

#define XmCImages		"Images"
#define XmCNumImages		"NumImages"

#endif /* BROWSE_H */
