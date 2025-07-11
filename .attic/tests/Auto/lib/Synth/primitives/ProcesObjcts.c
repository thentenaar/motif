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
static char rcsid[] = "$XConsortium: ProcesObjcts.c /main/7 1995/07/14 11:42:18 drk $"
#endif
#endif
/***********************************************************************
  	@(#)ProcesObjcts.c	1.6.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/30/90 SJS add to sccs
            08/28/90 SJS free child_windows initially allocated by XQueryTree()
	Calls:

	Summary:
            Regenerate the graphical objects data structure.

        INPUTS
            none

        OUTPUTS
            none

        RETURNS
            nothing
************************************************************************/

#include "xislib.h"

void xisProcessObjects()
{
    static char routine_name[] = "xisProcessObjects";
    int i;

    xisUseSessionInfo(routine_name);

    /* First free all objects below the root one */

    for (i=0; i<xisMaxObjectNum; i++) {
        if(xisObjects[i].id.num_children)
            XFree( (caddr_t)xisObjects[i].id.children_windows);
        xisObjects[i].in_use = 0;
    }

    xisObjects[0].first_child = NULL_Object;
    xisObjects[0].in_use = TRUE;
    xisMaxObjectNum = 1;

    xisObjects[0].id.window_under_parent = xisRootWindow;
    xisObjects[0].id.window = xisRootWindow;
    xisObjects[0].id.widget = NULL;

    XQueryTree(xisDisplay,xisObjects[0].id.window,
               &xisObjects[0].id.root_window,
               &xisObjects[0].id.parent_window,
               &xisObjects[0].id.children_windows,
               &xisObjects[0].id.num_children);

    /* Then call function to re-generate all the objects */

    (*xisGenerateObjects)(&xisObjects[0]);    

} /* End xisProcessObjects() */
