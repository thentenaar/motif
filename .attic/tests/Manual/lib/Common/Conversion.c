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
static char rcsid[] = "$XConsortium: Conversion.c /main/8 1995/07/14 11:58:32 drk $"
#endif
#endif
#include "testlib.h"

/*************************************************************************
*									 *
*	Name:		CommonCsToRs()					 *
*									 *
*	Function:	This function will receive as input a Compound   *
*			string and return to the user a Regular string   *
*			representation.					 *
*									 *
*	Returns:	A Regular string corresponding to the inputs     *
*			Compound String. Will return NULL if inputted    *
*			string is NULL					 *
*									 *
*************************************************************************/

char *CommonCsToRs(XmString cs)

{
    XmStringContext 	context;
    XmStringCharSet 	charset;
    XmStringDirection 	direction;
    Boolean 		separator;
    char 		*primitive_string;

    /* Don't do conversion if string is NULL */

    if (cs == NULL)
	return (NULL);

    XmStringInitContext(&context,cs);
    XmStringGetNextSegment(context,&primitive_string,
           &charset,&direction,&separator);
    XmStringFreeContext(context);
    return(primitive_string);
}
