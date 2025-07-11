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
/*   $XConsortium: PINames.h /main/6 1995/07/13 20:23:15 drk $ */

/*  X headers  */
#include <X11/Xlib.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>

/*  Xm headers  */
#include <Xm/MwmUtil.h>


/* 
 * Data for summary
 */

#define NUM_TESTS		5
#define TEST_DESCRIP_LENGTH	60

struct TestInfo PIInfo[NUM_TESTS] = 
{
	{"Change the window name of a window (WM_NAME)          ",
		True,	0},
	{"Check for a ***** default name                        ",
		True,	0},
	{"Change the icon name of a window (WM_ICON_NAME)       ",
		True,	0},
	{"Change the window name of ***** (WM_NAME)             ",
		True,	0},
	{"Change the class name structure of a window (WM_CLASS)",
		True,	0},
};


Widget			test_top,
				label_box;

Atom			new_type;
int				new_format;
unsigned long	new_nitems,
				new_bytes_after;
unsigned char	*new_data;
Atom			test_atom;
Window			test_wdw, top_wdw;
Window          normal_wdw;

XClientMessageEvent     ClientMsg_ev;

String	new_winname = "New Window Name";
String	look_at_name = "Window name should be \"New Window Name\".";
String	new_iconname = "PI Icon";
String	look_at_icon = "Icon name should be \"PI Icon\".";
String	look_at_classname =
					"Window name should be \"NEWER Window Class Name\".";

char	class_buf[80];			/* string for WM_CLASS prop */
int		l_name, l_class;		/* temp length vars */

String	new_class_name = "NEWER Window Class Name";
String	new_class_class = "PINewClass";
String	new_winname2 = "Second New Name";
String	old_winname2 = "*****";
char	*new_name;
XClassHint	class_hints;
XWMHints	wm_set_hints;
