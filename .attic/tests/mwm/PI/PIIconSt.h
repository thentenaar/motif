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
/*   $XConsortium: PIIconSt.h /main/7 1995/07/13 20:21:08 drk $ */

/*  X headers  */
#include <X11/Xlib.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>

/*  Xm headers  */
#include <Xm/MwmUtil.h>


struct
{

	int     state;
	Window  icon;

} *state_info;

XIconSize *icon_size;
/*
{

	int     min_width;
	int     min_height;
	int     max_width;
	int     max_height;
	int     width_inc;
	int     height_inc;

} *icon_size;
*/


/* 
 * Data for summary
 */

#define NUM_TESTS		2
#define TEST_DESCRIP_LENGTH	60

struct TestInfo PIInfo[NUM_TESTS] = 
{
	{"Get icon size information for root window       WM_ICON_SIZE",
		True,	0},
	{"Get state info for button window (NormalState)      WM_STATE",
		True,	0},

};

Widget			toplevel,
				test_top,
				test1_btn1;
Pixmap			test1_btn1_icon;

Atom			new_type;
int				new_format;
unsigned long	new_nitems,
				new_bytes_after;
unsigned char	*new_data;
Atom			test_atom;
Window			test_wdw, top_wdw;

XWMHints	wm_set_hints;
int             count_return;
