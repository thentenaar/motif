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
/*   $XConsortium: FontList.h /main/7 1995/07/13 19:52:11 drk $ */

#define NUM_FONTS 4

XmFontList	fontlist[10] =
{
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

XFontStruct	*font[NUM_FONTS] =
{
    NULL, 
    NULL,
    NULL,
    NULL,
};

char		*fontname[NUM_FONTS] =
{
    "NULL",
    "8x13bold",
    "fixed",
    "9x15"
};

XmStringCharSet	charset[4] =
{
    (XmStringCharSet) XmSTRING_DEFAULT_CHARSET,
    NULL,
    "ISOLatin1",
    "MyCharset"
};

char		*charsetname[4] =
{
    "XmSTRING_DEFAULT_CHARSET",
    "NULL",
    "ISOLatin1",
    "MyCharset"
};

char		error_string[120];

int		fontlist_num,
    	font_num,
    	charset_num;

int		errors;

