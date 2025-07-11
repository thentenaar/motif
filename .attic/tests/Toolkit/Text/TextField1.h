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
/*   $XConsortium: TextField1.h /main/9 1995/07/13 19:34:19 drk $ */


static char starting_string_text[] = "This is the starting string";

#define NITEMS	 10

#define	GetEditable		1
#define	SetEditable		2
#define	GetMaxLength		3
#define	SetMaxLength		4
#define	GetLastPosition		5
#define	SetInsertionPosition	6
#define	GetBaseline		7
#define	XYToPos			8
#define	PosToXY			9

/*  Global Variables  */

char *Istrings[NITEMS] = {
	"XmTextFieldGetEditable()",
	"XmTextFieldSetEditable()",
	"XmTextFieldGetMaxLength()",
	"XmTextFieldSetMaxLength()",
	"XmTextFieldGetLastPosition()",
	"XmTextFieldSetInsertionPosition()",
	"XmTextFieldGetBaseline()",
	"XmTextFieldXYToPos()",
	"XmTextFieldPosToXY()",
};


Widget	  Scale1, Scale2;
Widget    List1;
Widget    TextF1, StatText;
Widget	  Label1, Label2, Label3, Label4, Label5;
Widget	  Form;
Widget	  ToggleB;
Widget 	  ApplyB, ClearB;

static int	  function;
static int	  count = 0;
static int 	  scale1_val = 0;
static int 	  scale2_val = 0;
static int       position = 0;

static Boolean	  torf = True;

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

