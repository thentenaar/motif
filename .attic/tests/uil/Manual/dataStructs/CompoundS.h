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
/*   $XConsortium: CompoundS.h /main/4 1995/07/13 20:33:36 drk $ */

#include <stdio.h>

#include <Mrm/MrmAppl.h>

/*
 * Name of uid file
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

/*
 * Local declarations
 */

MrmHierarchy	s_MrmHierarchy;
MrmType		class;
MrmCode		type;

Widget		toplevel,
		UIL_widgets[3],
		X_widgets[3];

Display		*dsp;

XmString	X_compound_string1[2],
		X_compound_string2[2],
		X_compound_string3[9],
		UIL_compound_string1[3],
		UIL_compound_string2[3],
                UIL_compound_string3[9];

String		X_string = "string1";

String          X_direction_strings[9] =
{
    "CBA",
    "FED",
    "IHG",
    "JKL",
    "MNO",
    "PQR",
    "STU",
    "VWX",
    "YZA",
};

Boolean         X_rtol_direction[9] =
{
    0,1,1,1,0,1,0,0,1,
};

String          X_direction_charset[9] =
{
    "ISO8859-8",
    "ISO8859-8",
    "ISO8859-8",
    "ISO8859-8",
    "ISO8859-8",
    "ISO8859-8",
    "ISO8859-1",
    "ISO8859-1",
    "ISO8859-1",
};

XmStringCharSet	X_charset = "ISO8859-1";

String		UIL_compound_string_names[3] =
{
    "compound_string1",
    "compound_string2",
    "compound_string3",
};

String		UIL_widget_names[3] =
{
    "widget1",
    "widget2",
    "widget3",
};

String          UIL_direction_names[9] =
{
    "direction1",
    "direction2",
    "direction3",
    "direction4",
    "direction5",
    "direction6",
    "direction7",
    "direction8",
    "direction9",
};



String		X_method[2] = 
{
    "XmStringCreate(string, charset)    ",
    "XmStringCreateLtoR(string, charset)",
};

String		UIL_method[3] =
{
    "compound_string(\"string\")                          ",
    "compound_string(#char_set\"string\")                 ",
    "compound_string(\"string\", character_set = char_set)",
};

char	err_msg[80],
    	error_message[12000];

int	errors;

