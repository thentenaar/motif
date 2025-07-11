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
/*   $XConsortium: PIColormap.h /main/6 1995/07/13 20:18:50 drk $ */

/*  X headers  */
#include <X11/Xlib.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>

/*  Xm headers  */
#include <Xm/MwmUtil.h>


/* 
 * Data for summary
 */

#define	PIInfo_INIT		0
#define	PIInfo_CMstart	0

#define NUM_TESTS		4
#define TEST_DESCRIP_LENGTH	60

#define			VALID_TEST1	1
#define			VALID_TEST2	2
#define			VALID_TEST3	3
#define			FINISH		4

struct TestInfo PIInfo[NUM_TESTS] = 
{
	{"Check if def_cmap is initially installed WM_COLORMAP_WINDOWS",
		True,	0},
	{"Check if cmap1 is listed as installed    WM_COLORMAP_WINDOWS",
		True,	0},
	{"Check if cmap2 is listed as installed    WM_COLORMAP_WINDOWS",
		True,	0},
	{"Check if def_cmap is listed as installed WM_COLORMAP_WINDOWS",
		True,	0},
};


/*
 * Local declarations
 */

Widget			toplevel,
				test_top,
				test1_btn1,
				test2_btn1,
				test_box;
Pixmap			test1_btn1_icon, test2_btn1_icon;

Atom			new_type;
int				new_format;
unsigned long	new_nitems,
				new_bytes_after;
unsigned char	*new_data;
Atom			test_atom;
Window			test_wdw, top_wdw;
Window			button_wdw, button_wdw2,
				test_wdw;

Window			wdw_list[5];
int				def_scrn;

Colormap		*cm_list, def_cmap, cmap1, cmap2;
int				cm_num, i, ncolors;
XColor			colors[256], new_colors[256];

int				iteration = 1;

void	test1_btn1_proc();
