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
/*   $XConsortium: PIProto.h /main/6 1995/07/13 20:24:06 drk $ */

/*  X headers  */
#include <X11/Xlib.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>

/*  Xm headers  */
#include <Xm/MwmUtil.h>


/* 
 * Data for summary
 */

#ifdef SHOW_P899
#define NUM_TESTS		7
#else
#define NUM_TESTS		6
#endif /* P899 */

#define TEST_DESCRIP_LENGTH	60

#define PIInfo_DW1  0
#define PIInfo_DW2  1
#ifdef  DONT
#define PIInfo_SY   2
#define PIInfo_TF   3
#endif  /* DONT */
#define PIInfo_MM   2
#define PIInfo_CB   3

#ifdef SHOW_P899

#define PIInfo_CB2a 4
#define PIInfo_CB2r 5
#define PIInfo_CB3  6

#else

#define PIInfo_CB2r 4
#define PIInfo_CB3  5

#endif /* P899 */

struct TestInfo PIInfo[NUM_TESTS] = 
{
	{"Check if ClientMessage sent for f.kill      WM_DELETE_WINDOW",
		True,	0},
	{"Check if window is not deleted after f.kill WM_DELETE_WINDOW",
		True,	0},
#ifdef  DONT
	{"Check ClientMessage sent for state change   WM_SAVE_YOURSELF",
		True,   0},
	{"Check ClientMessage sent for take focus        WM_TAKE_FOCUS",
		True,   0},
#endif
	{"Check ClientMessage sent for motif message _MOTIF_WM_MESSAGE",
		True,	0},
	{"Check callback sent for motif message      _MOTIF_WM_MESSAGE",
		True,	0},
#ifdef SHOW_P899
	{"Check callback sent for menu item 2 (added)   _MOTIF_WM_MENU",
		True,	0},
#endif /* P899 */
	{"Check callback sent for menu item 2 (replaced)_MOTIF_WM_MENU",
		True,	0},
	{"Check callback sent for menu item 3 (grayed)  _MOTIF_WM_MENU",
		False,	0},
};

#define MSG_NUMBER      1024
#define MSG_NUMBER2     2024
#define MSG_NUMBER3     3024

#define			VALID_TEST1	1
#define			VALID_TEST2	2
#define			VALID_TEST3	3
#define			VALID_TEST4	4
#define			FINISH		5


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
Atom            proto_atom, delwin_atom, savylf_atom,
				takfoc_atom, menu_atom, motif_msg_atom;

XWMHints	wm_set_hints;
XWindowAttributes	win_attrs;
Status	ret_stat;
int		msg_num;
char	menu_item[80];
char	menu_item2[300];

int				iteration = 1;

char	*send_msg1 = "Send_Message _M Alt<Key>F6 f.send_msg 1024";
char	*send_msg2 = "Send_Message2 _e Alt<Key>F7 f.send_msg 2024";
char	*send_msg3 = "Send_Message _M Alt<Key>F6 f.send_msg 1024 \n Send_Message2 _e Alt<Key>F7 f.send_msg 2024 \n Send_Message3 _d Alt<Key>F8 f.send_msg 3024";
