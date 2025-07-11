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
static char rcsid[] = "$XConsortium: GetCharSet.c /main/7 1995/07/14 11:50:14 drk $"
#endif
#endif
/***********************************************************************
  	@(#)GetCharSet.c	1.7.1.1	Date:1/22/91
  	Author: TAT
	History:
            07/31/90 SJS change arg to char pointer, some msg changes
	Calls:

	Summary:
            This function tries to select the most appropriate XmString
            char set given that the user requests one specific char set.
            If for some reason this request can't be satisfied, it returns
            a more appropriate char set.

************************************************************************/

#include <AutoMessages.h>
#include "mvslib.h"

XmStringCharSet mvsGetCharSet(requested_char_set)
char *requested_char_set;
{
    int i;
    char msg_string[125];

    if (mvsNumLoadedFonts == 0)
        return XmSTRING_DEFAULT_CHARSET;

    for (i=0; i < mvsNumFonts; i++) {

        if (strcmp(requested_char_set,mvsFontInfo[i].char_set_name) == 0)
            break;
    }

    if (i == mvsNumLoadedFonts) {
       sprintf(msg_string, _AutoMessages[VISMSG38], requested_char_set,mvsFontInfo[0].char_set_name);
       AutoMessage(msg_string);
                    
        i = 0;
    }

    if ((i < mvsNumFonts) && (mvsFontInfo[i].font_struct == NULL)) {
        sprintf (msg_string, _AutoMessages[VISMSG39], requested_char_set);
	AutoMessage(msg_string);
                    
        for (i=0; i<mvsNumFonts; i++) {
            if (mvsFontInfo[i].font_struct != NULL)
                break;
        }
        if (i == mvsNumFonts)
            return XmSTRING_DEFAULT_CHARSET;
    }

    return(mvsFontInfo[i].char_set_name);

} /* End mvsGetCharSet() */
