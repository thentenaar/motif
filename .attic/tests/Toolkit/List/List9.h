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
/*   $XConsortium: List9.h /main/8 1995/07/13 18:22:24 drk $ */

#define NITEMS	 4
#define NITEMS2  15

#define	GetKbdItemPos		1
#define	SetKbdItemPos		2
#define PosSelected		3
#define PosToBounds		4

/*  Global Variables  */

char *Istrings[NITEMS] = {
	"XmListGetKbdItemPos()",
	"XmListSetKbdItemPos()",
	"XmListPosSelected()",
	"XmListPosToBounds()"
};


char *Istrings2[NITEMS2] = {
	"1)  Item1",
	"2)  Item2",
	"3)  Item3",
	"4)  Item4",
	"5)  Item5",
	"6)  Item6",
	"7)  Item7",
	"8)  Item8",
	"9)  Item9",
	"10) Item10",
	"11) Item11",
	"12) Item12",
	"13) Item13",
	"14) Item14",
	"15) Item15"
	};

Widget	  Scale;
Widget    List1, List2;
Widget	  Label1, Label2;
Widget	  Form;
Widget	  Frame;
Widget	  RowCol;
Widget 	  ApplyB, ClearB;
Widget	  Toggle_AddMode;
Widget	  Label_Selection, Label_Position, Label_Bounds;
Widget    Single, Multiple, Browse, Extended;
Widget 	  RadioBox;

int	  function;
int	  count = 0;
int 	  arg_count = 0;
int       position = 0;

/*  Private Functions  */


static void 	SSelCB_List1(Widget w, XtPointer client_data, 
						XtPointer call_data);
static void  	ApplyCB(Widget w, XtPointer client_data, 
						XtPointer call_data);
static void  	ClearCB(Widget w, XtPointer client_data, 
						XtPointer call_data);
static void  	ScaleCB(Widget w, XtPointer client_data, 
						XtPointer call_data);
static void  	NotifyCB(Widget w, XtPointer client_data, 
						XtPointer call_data);
static void  	AddModeCB(Widget w, XtPointer client_data, 
						XtPointer call_data);
static void  	CHSPolicyCB(Widget w, unsigned char client_data, 
			    			XtPointer call_data);
static void  	MulSelCB_List2(Widget w, XtPointer client_data, 
			    			XtPointer call_data);
static Widget	CreateToggle(Widget parent, char *label);

