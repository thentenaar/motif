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
static char rcsid[] = "$XConsortium: Color.c /main/9 1995/07/14 11:57:23 drk $"
#endif
#endif
#include "testlib.h"

/*************************************************************************
*									 *
*	Name:		CommonGetColor()				 *
*									 *
*	Purpose:	To get the Pixel representation corresponding to *
*			the color name given as input to this function.  *
*			If there is no match, NULL is returned.		 *
*									 *
*	Returns:	Pixel value corresponding to input string        *
*									 *
*************************************************************************/

Pixel CommonGetColor(char *colorstr)

{
   XrmValue from, to;

   from.size = strlen(colorstr) +1;
   if (from.size < sizeof(String)) from.size = sizeof(String);
   from.addr = colorstr;
   to.addr = NULL;
   XtConvert(Shell1, XmRString, &from, XmRPixel, &to);
    if (to.addr != NULL)
      return ((Pixel) *((Pixel *) to.addr));
    else
      return ((Pixel) NULL);

}
