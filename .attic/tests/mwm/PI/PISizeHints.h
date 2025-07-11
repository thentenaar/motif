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
/*   $XConsortium: PISizeHints.h /main/6 1995/07/13 20:24:41 drk $ */

/*  X headers  */
#include <X11/Xlib.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>

/*  Xm headers  */
#include <Xm/MwmUtil.h>


Widget			test_top,
				test1_btn1,
				control_panel,
				controlBB,
				min_widthScale,
				min_heightScale,
				max_widthScale,
				max_heightScale,
				base_widthScale,
				base_heightScale,
				width_incScale,
				height_incScale,
				min_aspect_xScale,
				min_aspect_yScale,
				max_aspect_xScale,
				max_aspect_yScale,
				applyButton;

Atom			new_type;
int				new_format;
unsigned long	new_nitems,
				new_bytes_after;
unsigned char	*new_data;
Atom			test_atom;

Window			test_wdw, top_wdw;

Pixmap			test1_btn1_icon;

XSizeHints  wm_set_size, wm_get_size;
XWMHints    wm_set_hints, wm_get_hints;

int		screenWidth, screenHeight;
