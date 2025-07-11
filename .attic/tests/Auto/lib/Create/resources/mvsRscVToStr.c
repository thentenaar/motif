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
static char rcsid[] = "$XConsortium: mvsRscVToStr.c /main/7 1995/07/14 11:22:00 drk $"
#endif
#endif
/***********************************************************************
  	@(#)mvsRscVToStr.c	1.8.1.1	Date:1/22/91
  	Author: TAT
	History:
	   03/30/90 SJS Add to sccs tree
	   05/29/90 SJS composite changes
	   07/13/90 SJS change for XmStrings
	   10/16/90 SJS add XmStrings
	Calls:

	Summary:
		This function takes a value and converts it to a
	printable string based on the type of the resource associated
	with this particular value. "i" is an index into the resource_info
	array which contains the type code for the value being
	converted.
************************************************************************/

#include <AutoMessages.h>
#include <stdio.h>
#include "mvslib.h"

char *mvsResourceValueToString(widget_class_info, i ,value)
MvsWidgetClassInfo *widget_class_info;
int i;
MvsArgVal value;
{
    static char str[MAX_STRING_LENGTH];
    char *tmp_str;
    char msg_string[125];
    int t;

    t = widget_class_info->resource_info[i].type_code;
    switch (mvsTypeInfo[t].ref_type) {

    case RefBoolean:
        if (value)
            sprintf(str,"True");
        else
            sprintf(str,"False");
        break;

    case RefValue:
        sprintf(str,"%ld",value);
        break;

    case RefGeneric:
        sprintf(str,"%lx",value);
        break;
		    
    case RefXmString:
        tmp_str = mvsExtractString((XmString)value,
                                   widget_class_info->resource_info[i].name);
        sprintf(str,"%s", tmp_str);
        XtFree(tmp_str);
        break;
		    
    case RefString:
        sprintf(str,"%s", (char *)value);
        break;
		    
    case RefXmStringTable: /* until we know how to handle these */
        sprintf(str,"%ld",value);
        break;
		    
    case RefCallbackList:
        {
            XtCallbackList cb_list = (XtCallbackList)value;
            int k=0; 
 
            if (cb_list != NULL) {
                while (cb_list->callback != NULL) {
                    cb_list++;
                    k++;
	        }
            }
        sprintf(str,"%d callbacks",k);
	}
        break;
		    
    default:
	sprintf (msg_string,_AutoMessages[WARNMSG78],mvsTypeInfo[t].ref_type);
        AutoMessage(msg_string);
                  
        sprintf(str,"%ld",value);
        break; 
    }
    return str;

} /* End mvsResourceValueToString() */
