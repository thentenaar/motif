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
static char rcsid[] = "$XConsortium: InitFonts.c /main/7 1995/07/14 11:50:52 drk $"
#endif
#endif
/***********************************************************************
  	@(#)InitFonts.c	1.9.1.1	Date:1/22/91
  	Author: TAT
	History:
            06/13/90 SJS init font_list struct
            08/28/90 SJS make non-existance of fonts an error condition
	Calls:

	Summary:
            This function initializes the font_info array and mvsFontList.

************************************************************************/

#include <AutoMessages.h>
#include "mvslib.h"

/* 
 * Reduced the size of the font_info array to just 1 for the final fallback
 * font for automation.
 */
void mvsInitFonts()
{


    XmFontList font_list;
    char msg_string[125];

    font_list = (XmFontList)NULL;

    mvsFontInfo[0].font_struct = XLoadQueryFont(mvsDisplay,
                                                mvsFontInfo[0].font_name);

    if (mvsFontInfo[0].font_struct == NULL) {
        sprintf (msg_string, _AutoMessages[VISMSG41], mvsFontInfo[0].font_name);
        AutoMessage(msg_string);
                    
        AutoError (_AutoMessages[VISMSG13]);

	}
	else {

    	mvsNumLoadedFonts = 1; 
    	font_list = XmFontListCreate(mvsFontInfo[0].font_struct,
                                 	 mvsFontInfo[0].char_set_name);
    	mvsFontList = font_list;

	}

} /* End mvsInitFonts() */
