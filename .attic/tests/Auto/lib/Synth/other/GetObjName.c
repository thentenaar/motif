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
static char rcsid[] = "$XConsortium: GetObjName.c /main/7 1995/07/14 11:34:44 drk $"
#endif
#endif
/***********************************************************************
  	@(#)GetObjName.c	1.3.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/24/90 SJS add to sccs
	Calls:

	Summary:
            Returns a pointer to a statically declared string naming the
        specified object.

        INPUTS:
            object     - object pointer
        
        OUTPUTS:
            none

        RETURNS:
            string object name
************************************************************************/
#include "xislib.h"

char *xisGetObjectName(object)
XisObjectRecord *object;
{
    static char name[255];
    static char other_string[] = "Other";
    static char root_string[] = "Root Window";
    static char sync_string[] = "Sync Window";
    static char empty_string[] = "Windowless,Widgetless";

    if (object == NULL)
        return other_string;

    if (object->id.widget != NULL) {
        sprintf(name,"%s #%d %s of class %s",
                    (*xisGetObjectTypeName)(object->id.object_type),
                    object->id.instance,
                    XtName(object->id.widget),
                    (*xisGetWidgetClassName)(object->id.widget));

        return name;
    }

    else {
        if (!object->id.window)
            return empty_string;

        else {
            if (object->id.window == xisRootWindow)
                return root_string;
            else if (object->id.window == xisSyncWindow)
                return sync_string;
            else
                return other_string;
        }
    }

} /* End xisGetObjectName() */
