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
static char rcsid[] = "$XConsortium: mvsFreeRsc.c /main/7 1995/07/14 11:20:26 drk $"
#endif
#endif
/***********************************************************************
  	@(#)mvsFreeRsc.c	1.6.1.1	Date:1/22/91
  	Author: TAT
	History:
	   05/29/90 SJS Add to sccs tree
	   07/12/90 SJS free XmString type. These were XtMalloc'd upon
                        creation or loading
	   07/29/90 PSN do not do XtGetValues() for callbacks
	Calls:

	Summary:
		This function frees up all the resources associated with
	the current widget state. Only the resources which are pointers
	require any processing. The memory pointed to by the resources
	must be freed because it is dynamically allocated as each new
	widget state is entered.
************************************************************************/

#include <AutoMessages.h>
#include "mvslib.h"

void mvsFreeResource(widget_class_info, presource, i)
MvsWidgetClassInfo *widget_class_info;
MvsArgVal *presource;
int i;
{
    MvsResourceInfoRecord *resource_info;
    char msg_string[125];

    resource_info = widget_class_info->resource_info;

    switch (mvsTypeInfo[resource_info[i].type_code].ref_type) {

        case RefXmStringTable:  /* Here until handling of compound strings */
                                /* is determined */

	case RefValue:
        case RefGeneric:
        case RefBoolean:
	    if (presource)
	    	*presource = 0L;
            break;

	case RefXmString:     
	case RefString:
	case RefCallbackList:
            if (presource && *presource) {
                XtFree( (char *)(*presource));
	        *presource = 0L;
	    }
            break;

	default:
	    sprintf(msg_string,_AutoMessages[WARNMSG68], 
		    mvsTypeInfo[resource_info[i].type_code].ref_type);
            AutoMessage(msg_string);
                       
            break;
    }

} /* End mvsFreeResource() */

/***********************************************************************
  	Author: PSN
	History:

	Calls:

	Summary:
		This function frees up the memory associated with
	the current args array's value field depending on the type of
	resource it contains. 
************************************************************************/
void mvsFreeArgs(widget_class_info, args, nargs)
MvsWidgetClassInfo	*widget_class_info;
Arg			*args;
Cardinal		nargs;
{
    register int	i, j;
    register num_res = widget_class_info->num_resources;
    MvsResourceInfoRecord *res_info = widget_class_info->resource_info;

    for (i=0; i<nargs; i++) {
	if (args[i].name == NULL) continue;
	else for ( j=0; j<num_res; j++) {
	    if (!strcmp(args[i].name, res_info[j].name)) break;
	}

        if (j >= num_res)
		;
        else if (args[i].value)
            mvsFreeResource(widget_class_info, &(args[i].value), j);
    }
}
