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
static char rcsid[] = "$XConsortium: ChekNaColors.c /main/7 1995/07/14 11:47:05 drk $"
#endif
#endif
/***********************************************************************
  	@(#)ChekNaColors.c	1.5.1.1	Date:1/22/91
  	Author: TAT
	History:
	   03/30/90 SJS Add to sccs tree
	   09/11/90 SJS fix arg passing
	Calls:

	Summary:
            Check to make sure all the colors named in mvsColorTalbe are indeed
        installed on the user's server.

        Note: This is no longer a necessary requirement since 
              mvsAllocNamedColor now translates the color name 
              to the appropriate RGB value for the user and calls
              XAllocColor instead of XAllocNamedColor like it used to.

        INPUTS
            none

        OUTPUTS
            none
 
        RETURNS
            nothing
************************************************************************/

#include <AutoMessages.h>
#include "vislib.h"

void mvsCheckNamedColors PARAMS((void))
{
    Display *display2;
    Colormap cmap;
    int i;
    XColor nearest,exact;
    char *name;
    unsigned long pixels[1];
    char msg_string[125];

    display2 = XOpenDisplay(DisplayString(visDisplay));
    cmap = XDefaultColormap(visDisplay,DefaultScreen(visDisplay));

    for (i=0; i<mvsNumTableColors; i++) {
        if (!XAllocNamedColor(display2,cmap,name=mvsColorTable[i].name1,
                              &nearest,&exact)) {
	    sprintf (msg_string, _AutoMessages[VISMSG25], name);
            AutoMessage (msg_string);}
        else {
            if (exact.red != (mvsColorTable[i].red<<8)) {
	      sprintf(msg_string, _AutoMessages[VISMSG26],name,exact.red,mvsColorTable[i].red<<8); 
              AutoMessage(msg_string);}
                    
            if (exact.green != (mvsColorTable[i].green<<8)) {
	      sprintf (msg_string, _AutoMessages[VISMSG27],name,exact.green,mvsColorTable[i].green<<8); 
               AutoMessage(msg_string);}
                    
            if (exact.blue != (mvsColorTable[i].blue<<8)) {
	      sprintf(msg_string, _AutoMessages[VISMSG28],name,exact.blue,mvsColorTable[i].blue<<8); 
              AutoMessage(msg_string); }
                    
        }
        pixels[0] = nearest.pixel;
        XFreeColors(display2,cmap,pixels,1,0);

        if (!XAllocNamedColor(display2,cmap,name=mvsColorTable[i].name2,
                              &nearest,&exact)) {
	   sprintf(msg_string, _AutoMessages[VISMSG25], name);
	   AutoMessage(msg_string);}
        else {
            if (exact.red != (mvsColorTable[i].red<<8)){
	        sprintf(msg_string,_AutoMessages[VISMSG26],name,exact.red,mvsColorTable[i].red<<8); 
                AutoMessage(msg_string);}
                    
            if (exact.green != (mvsColorTable[i].green<<8)) {
	        sprintf (msg_string, _AutoMessages[VISMSG27], name,exact.green,mvsColorTable[i].green<<8);
                AutoMessage(msg_string);}
                    
            if (exact.blue != (mvsColorTable[i].blue<<8)) {
	      sprintf (msg_string, _AutoMessages[WARNMSG28], name,exact.blue,mvsColorTable[i].blue<<8); }
                
                    
        }
        pixels[0] = nearest.pixel;
        XFreeColors(display2,cmap,pixels,1,0);
    }

} /* End mvsCheckNamedColors() */
