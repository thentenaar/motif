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
/*   $XConsortium: PIMwmHints.h /main/6 1995/07/13 20:22:01 drk $ */

/*  X headers  */
#include <X11/Xlib.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>

/*  Xm headers  */
#include <Xm/MwmUtil.h>


/* 
 * Data for summary
 */

#define NUM_TESTS		7
#define TEST_DESCRIP_LENGTH	60

struct TestInfo PIInfo[NUM_TESTS] = 
{
	{"Change the decorations to title                             ",
		True,	0},
	{"Change the input_mode to system_modal                       ",
		True,	0},
	{"Change decorations to title/resize/menu/min/max             ",
		True,	0},
	{"Change the functions to resize/min                          ",
		True,	0},
	{"Change decorations/functions to ALL, input_mode to MODELESS ",
		True,	0},
	{"Change FUNC=~Close, DECOR=~Resize using ALL | CLOSE/RESIZEH ",
		True,	0},
	{"Change FUNC=~Close, DECOR=~Resize using |'d list            ",
		True,	0},
};

/*
 * Local declarations
 */

Widget			test_top,
				test_top2,
				test1_btn1,
				test2_btn1;
Pixmap			test1_btn1_icon, test2_btn1_icon;

Atom			new_type;
int				new_format;
unsigned long	new_nitems,
				new_bytes_after;
unsigned char	*new_data;
Atom			test_atom;
Window			test_wdw, test_wdw2, top_wdw;

XWMHints	wm_set_hints;
PropMwmHints	mwm_set_hints, *mwm_get_hints;
int			ret_val;

static void	test1_btn1_proc();
