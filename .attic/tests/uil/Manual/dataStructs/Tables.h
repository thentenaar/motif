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
/*   $XConsortium: Tables.h /main/4 1995/07/13 20:34:45 drk $ */

#include <Mrm/MrmAppl.h>

#include <Xm/PushB.h>

#define NUM_TESTS	6

/* TEMP FIX from XmString.c */

typedef struct _XmFontListRec
{
    XFontStruct      *font;
    XmStringCharSet  charset;
}
    XmFontListRec;

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
 * Callback routines
 */

void		integer_table_proc(),
		compound_string_table_proc(),
		asciz_string_table_proc(),
		font_table_proc(),
		identifier_proc();

/*
 * Convenience routine
 */

void		error_proc();
void		compare_fonts();

/*
 * Names to register with MrmRegisterNames
 */

static MrmRegisterArg reglist[] = 
{
    "integer_table_proc",		(caddr_t) integer_table_proc,
    "compound_string_table_proc",	(caddr_t) compound_string_table_proc,
    "asciz_string_table_proc",		(caddr_t) asciz_string_table_proc,
    "font_table_proc",			(caddr_t) font_table_proc,
    "identifier_proc",			(caddr_t) identifier_proc,
    "ident1",				(caddr_t) 50,
    "ident2",				(caddr_t) 2001
};

static int reglist_num =
(sizeof(reglist) / sizeof(reglist[0]));

/*
 * Local declarations
 */

MrmHierarchy	s_MrmHierarchy;
MrmType		class;

Display		*dsp;
Colormap	cmap;

Widget		toplevel = NULL,
		UIL_widgets[NUM_TESTS] =
		{
		    NULL, NULL, NULL, NULL, NULL, NULL
		};

String		UIL_widget_names[NUM_TESTS] = 
		{
		    "widget5",
		    "widget5a",
		    "widget6",
		    "widget7",
		    "widget8",
		    "widget9"
		};

String		test_result_headers[NUM_TESTS] =
		{
		    "Integer Table",
		    "Integer Table 2 (OSF CR 5691)",
		    "Compound String Table",
		    "Asciz String Table",
		    "Font Table",
		    "Identifier"
		};

int		X_integer_table[5] = 
		{
		    628,
		    0,
		    -90009,
		    4883,
		    -29
		};

String		X_compound_string_table[3] =
		{
		    "stRIng1",
		    "string number 2",
		    "Third string"
		};

String		X_asciz_string_table[4] = 
		{
		    "a_string",
		    "ThiS_stRing2",
		    "3_stringG",
		    "String_4_U"
		};

String		X_font_names[3] = 
		{
		    "fixed",
		    "6x10",
		    "8x13bold"
		};

int		expected_ident[2] =
{
    50,
    2001
};

String		ident_type[2] =
{
    "Identifier as widget resource value",
    "Identifier as callback tag value   "
};

int		ident_int = 0;

int		errors[NUM_TESTS] =
		{
		    0, 0, 0, 0, 0, 0
		};

int		ndx;
