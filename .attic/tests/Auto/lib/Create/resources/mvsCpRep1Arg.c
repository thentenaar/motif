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
static char rcsid[] = "$XConsortium: mvsCpRep1Arg.c /main/7 1995/07/14 11:19:44 drk $"
#endif
#endif
/***********************************************************************
  	@(#)mvsCpRep1Arg.c	1.8.1.1	Date:1/22/91
  	Author: TAT
	History:
	   03/30/90 SJS Add to sccs tree
	   07/31/90 SJS match index with resource by look up
	   08/01/90 SJS our internal XmStrings are regular strings and
                        XmStrings in Arg lists are *really* XmStrings
	   09/20/90 SJS use define for NON_EXISTENT
	Calls:

	Summary:
		This function places 1 argument into the args array
	used in XtSetValues() calls. If the named resource exists in
	the array, the new value replaces the old value. If the named
	resource is new, then it is added to the end of the list.
		Any resource that is replaced is appropriately freed.
	The argument being added to the list is copied into place and
	should be freed later.
************************************************************************/

#include <AutoMessages.h>
#include "mvslib.h"

void mvsCopyReplace1Arg(w_class_info, orig_args, orig_argcnt, new_arg_name,
			new_arg_value)
MvsWidgetClassInfo	*w_class_info;
Arg 			orig_args[];
int 			*orig_argcnt;
char 			new_arg_name[];
MvsArgVal 		new_arg_value;
{
    register int j;
    register int index;
    char msg_string[125];

    for (j=0; (j<*orig_argcnt) && strcmp(new_arg_name,orig_args[j].name); j++)
        ;

    if (j < *orig_argcnt) {
	mvsFreeResource(w_class_info, &(orig_args[j].value), 
		    mvsGetResourceNum(w_class_info, orig_args[j].name));
    }

    index = mvsGetResourceNum(w_class_info, new_arg_name);
    if(index != NON_EXISTENT) {

        /* XmStrings in Arg lists are XmStrings and needed to be treated
           as such. We can not call mvsCopyResource() because it is
           copying "internal" resources. XmStrings are represented as regular
           strings in mvs resources and are thus not valid for an Arg
           list.
        */
        if(w_class_info->resource_info[index].type_code != T_XmString) {
            XtSetArg(orig_args[j], new_arg_name,
                     mvsCopyResource(w_class_info, new_arg_value, index));
        }
        else {
            XtSetArg(orig_args[j], new_arg_name, 0); /* just fill in name */
            orig_args[j].value =
                               (MvsArgVal)XmStringCopy((XmString)new_arg_value);
        }

        if (j == *orig_argcnt) {
            (*orig_argcnt)++;
        }
    }
    else {
        sprintf (msg_string, _AutoMessages[WARNMSG66], new_arg_name);
	AutoMessage(msg_string);
    }

} /* End mvsCopyReplace1Arg() */
