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
static char rcsid[] = "$XConsortium: InitRootInfo.c /main/7 1995/07/14 11:16:55 drk $"
#endif
#endif
/***********************************************************************
        @(#)InitRootInfo.c	1.3.1.1     Date:1/22/91
  	Author: TAT
	History:
	   05/14/90 TAT Create
	Calls:

	Summary:
		This functions initializes the Root widget Info
                record. 
************************************************************************/

#define DECLARE_MVSLIB_GLOBALS_HERE 
#include "mvslib.h"

MvsWidgetInfoRecord *mvsInitRootWidgetInfo()
{
    static MvsWidgetClassInfo rootWidgetClassInfo = { 0 };

    mvsNumWidgetInfos = 1;

    mvsWidgetInfos[0].in_use = 1;
    mvsWidgetInfos[0].widget_class_info = (&rootWidgetClassInfo);
    mvsWidgetInfos[0].widget_class_info->proc_SetExpectedActions = NULL;
    mvsWidgetInfos[0].widget_class_info->proc_CreateWidget = NULL;
    mvsWidgetInfos[0].widget_class_info->proc_TestBehavior = NULL;
    mvsWidgetInfos[0].widget_class_info->resource_info = NULL;
    mvsWidgetInfos[0].first_child = NULL;
    mvsWidgetInfos[0].next_sibling = NULL;
    mvsWidgetInfos[0].parent = NULL;
    mvsWidgetInfos[0].widget = NULL;

    return (&mvsWidgetInfos[0]);

} /* End mvsInitRootInfo() */
