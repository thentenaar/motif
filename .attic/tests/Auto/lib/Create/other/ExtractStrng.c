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
static char rcsid[] = "$XConsortium: ExtractStrng.c /main/7 1995/07/14 11:15:34 drk $"
#endif
#endif
/***********************************************************************
  	@(#)ExtractStrng.c	1.7.1.2     Date:2/7/91
  	Author: SJS
	History:
            07/10/90 SJS Create
            07/12/90 SJS do proper malloc
            10/17/90 SJS check for NULL coming in
	Calls:

	Summary:
            This function extracts the "real" string out of an XmString.
        All the segments obtained are concatenated together into a single
        string. The required memory is allocated by this function
************************************************************************/

#include <AutoMessages.h>
#include "mvslib.h"

char *mvsExtractString(in_string, resource_name)
XmString in_string;
char *resource_name;
{
    XmStringContext context;
    XmStringCharSet charset;
    XmStringDirection direction;
    Boolean separator;
    int length=1;                 /* Init to 1 to count Null char */
    char *string;
    char real_string[MAX_STRING_LENGTH];
    char msg_string[100];

    if (in_string == (XmString)NULL) {
        return((char *)NULL);
    }

    if (!XmStringInitContext(&context,(XmString)in_string)) {
        sprintf (msg_string, _AutoMessages[WARNMSG23], resource_name);
	AutoMessage(msg_string);
	
#define EXTRACT_STRING_ERROR_MESS "ERROR"

        string = XtMalloc(sizeof(EXTRACT_STRING_ERROR_MESS)+1) ;
        strcpy(string,EXTRACT_STRING_ERROR_MESS) ;
        return(string);
    }
    else {
        real_string[0] = '\0';
        while (XmStringGetNextSegment(context,&string,&charset, &direction,
                                      &separator)) {
            length += strlen(string);
            if (length > MAX_STRING_LENGTH) {
	    /*
	       sprintf (msg_string, _AutoMessages[WARNMSG24], resource_name, 
	                                   length - strlen(string));

	       AutoMessage (msg_string);

	    */
                break;
            }
            strcat(real_string,string);
            XtFree((char *)string);
            XtFree((char *)charset);
        }

        /* use string to save the pointer from alloc'ing space */
        string = XtMalloc(strlen(real_string)+1);
        strcpy(string, real_string);
        XmStringFreeContext(context);

        return( string);
    }

} /* End mvsExtractString() */







