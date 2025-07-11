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
static char rcsid[] = "$XConsortium: mvsGetRscVal.c /main/7 1995/07/14 11:21:23 drk $"
#endif
#endif
/***********************************************************************
  	@(#)mvsGetRscVal.c	1.3.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/29/90 SJS add to sccs
	Calls:

	Summary:
            This function returns the current value of the requested
        resource. This value is obtained from the actual widget and
        is returned as an MvsArgVal type.
************************************************************************/

#include "mvslib.h"

MvsArgVal mvsGetResourceValue(widget, resource_name, type)
Widget widget;
String resource_name;
int type;
{
    Arg args[1];
    MvsArgVal value = 0;
   
    XtSetArg(args[0],resource_name,&value);
    XtGetValues(widget, args,1);

    value = mvsConvertToType(value, type);
    return(value);

} /* End mvsGetResourceValue() */
