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
/*   $TOG: Text12.h /main/10 1997/09/18 11:00:08 csn $ */


static char starting_string_text[] = "This is the starting string";

#ifndef	MOTIF1_1
#define NITEMS	 13
#else
#define NITEMS	 9
#endif	/* MOTIF1_1 */

#define	GetSelectionPosition	1
#define	GetSelection		2
#define	SetSelection		3
#define	ClearSelection		4
#define	Insert			5
#define	SetAddMode		6
#define	GetString		7
#define	SetString		8
#define	Replace			9

#ifndef	MOTIF1_1
#define	FindString		10
#define	GetSubstring		11
#define	DisableRedisplay	12
#define	EnableRedisplay		13
#endif	/* MOTIF1_1 */

/*  Global Variables  */

char *Textstrings[NITEMS] = {
	"XmTextGetSelectionPosition()",
	"XmTextGetSelection()",
	"XmTextSetSelection()",
	"XmTextClearSelection()",
	"XmTextInsert()",
	"XmTextSetAddMode()",
	"XmTextGetString()",
	"XmTextSetString()",
#ifndef	MOTIF1_1
	"XmTextReplace()",
	"XmTextFindString()",
	"XmTextGetSubstring()",
	"XmTextDisableRedisplay()",
	"XmTextEnableRedisplay()"
#else
	"XmTextReplace()"
#endif	/* MOTIF1_1 */
};

Widget	Scale1, Scale2;
Widget 	List1;
Widget 	Text1, StatText;
Widget	Label1, Label2, Label3, Label4, Label5;
Widget	Form;
Widget	ToggleB;
Widget 	ApplyB, ClearB;

static  int	function;
static  int	count = 0;
static  int 	scale1_val = 0;
static  int 	scale2_val = 0;
static  int    	position = 0;

static  Boolean	torf = True;

#ifndef	MOTIF1_1

Widget	Scale3, ToggleB2, Label6;
int	scale3_val = 0;
XmTextDirection	forb = XmTEXT_FORWARD;

#endif	/* MOTIF1_1 */


/*  Private Functions  */


static void  SSelCB_List1(Widget w, XtPointer client_data,
				        XtPointer call_data);
static void  MulSelCB_List2(Widget w, XtPointer client_data,
				     	XtPointer call_data);
static void  ApplyCB(Widget w, XtPointer client_data,
			       		XtPointer call_data);
static void  ClearCB(Widget w, XtPointer client_data,
			       		XtPointer call_data);
static void  Scale1CB(Widget w, XtPointer client_data,
			       		XtPointer call_data);
static void  Scale2CB(Widget w, XtPointer client_data,
			       		XtPointer call_data);
static void  TorFCB(Widget w, XtPointer client_data,
			       		XtPointer call_data);
#ifndef	MOTIF1_1
static void  Scale3CB(Widget w, XtPointer client_data,
			       		XtPointer call_data);
static void  ForBCB(Widget w, XtPointer client_data,
					XtPointer call_data);
#endif	/* MOTIF1_1 */

