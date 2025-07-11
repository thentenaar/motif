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
/*   $TOG: Text3.h /main/10 1997/09/18 11:00:29 csn $ */


static char starting_string_text[] = "This is the starting string";

/*  After Motif1.1 the function XmTextSetCursorPosition became obsolete. */
#ifndef MOTIF1_1
#define NITEMS	 11
#else
#define NITEMS 12
#endif

#define	GetEditable		1
#define	SetEditable		2
#define	GetMaxLength		3
#define	SetMaxLength		4
#define	GetLastPosition		5
#define	SetInsertionPosition	6
#ifndef MOTIF1_1
#define	GetBaseline		7
#define	XYToPos			8
#define	PosToXY		        9	
#define	GetTopCharacter		10
#define	SetTopCharacter		11
#else
#define	SetCursorPosition	7
#define	GetBaseline		8
#define	XYToPos			9
#define	PosToXY			10
#define	GetTopCharacter		11
#define	SetTopCharacter		12
#endif

/*  Global Variables  */

static char *Textstrings[NITEMS] = {
	"XmTextGetEditable()",
	"XmTextSetEditable()",
	"XmTextGetMaxLength()",
	"XmTextSetMaxLength()",
	"XmTextGetLastPosition()",
	"XmTextSetInsertionPosition()",
#ifdef MOTIF1_1
	"XmTextSetCursorPosition()",
#endif
	"XmTextGetBaseline()",
	"XmTextXYToPos()",
	"XmTextPosToXY()",
	"XmTextGetTopCharacter()",
	"XmTextSetTopCharacter()",
};

Widget	  Scale1, Scale2;
Widget    List1;
Widget    Text1, StatText;
Widget	  Label1, Label2, Label3, Label4, Label5;
Widget	  Form;
Widget	  ToggleB;
Widget 	  ApplyB, ClearB;

static    int	  function;
static    int	  count = 0;
static    int 	  scale1_val = 0;
static    int 	  scale2_val = 0;
static    int       position = 0;

static    Boolean	  torf = True;

/*  Private Functions  */


static void SSelCB_List1(Widget w, XtPointer client_data,
				    	XtPointer call_data);
static void MulSelCB_List2(Widget w, XtPointer client_data,
				     	XtPointer call_data);
static void ApplyCB(Widget w, XtPointer client_data,
			       		XtPointer call_data);
static void ClearCB(Widget w, XtPointer client_data,
			       		XtPointer call_data);
static void Scale1CB(Widget w, XtPointer client_data,
			       		XtPointer call_data);
static void Scale2CB(Widget w, XtPointer client_data,
			       		XtPointer call_data);
static void TorFCB(Widget w, XtPointer client_data,
			       		XtPointer call_data);

