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
/*   $XConsortium: Font.h /main/4 1995/07/13 20:33:53 drk $ */

#include <stdio.h>

#include <Mrm/MrmAppl.h>


/* FontList Structure from XmString.c */

typedef struct _XmFontListRec
{
    XFontStruct      *font;
    XmStringCharSet  charset;
}
    XmFontListRec;


/* Name of uid file
 */

String		filename_vec[] =
{
    "dataS.uid"
};

MrmCount	filename_num =
(sizeof(filename_vec) / sizeof(filename_vec[0]));

/*
 * Convenience routine
 */

void		error_proc();
void		compare_fonts();

/*
 * Local declarations
 */

MrmHierarchy	s_MrmHierarchy;
MrmType		class;
MrmCode		type;

Widget		toplevel,
		UIL_widget[4];

Display		*dsp;

XFontStruct	*X_font = NULL,
		*UIL_font[4] = 
		{
		    NULL,
		    NULL,
		    NULL,
		    NULL,
		};

XmFontList	UIL_fontlist[4],
		X_fontlist[4];

String		UIL_font_names[4] =
{
    "font1",
    "font2",
    "font3",
    "font4",
};

String		UIL_widget_names[4] =
{
    "widget1",
    "widget2",
    "widget3",
    "widget4",
};

String		X_font_names[2] =
{
    "8x13bold",
    "variable",
};

XmStringCharSet	X_charsets[2] =
{
    "ISO8859-1",
    "ISO8859-6",
};

char	err_msg[80],
    	error_message[12000];

int	errors;
