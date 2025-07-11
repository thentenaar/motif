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
/*   $XConsortium: List7.h /main/9 1995/07/13 18:20:42 drk $ */

#ifndef MOTIF1_1
#define NITEMS	 14
#else
#define NITEMS	 13
#endif /* MOTIF1_1 */

#define NITEMS2  15

#define	AddItems		1
#define	AddItem			2
#define	AddItemUnselected	3
#define	DeleteItem		4
#define	DeleteItems		5
#define	DeleteItemsPos		6
#define	DeletePos		7
#define	DeleteAllItems		8
#define	DeselectAllItems	9
#define	DeselectItem		10
#define	DeselectPos		11
#define	SelectItem		12
#define	SelectPos		13

#ifndef MOTIF1_1
#define	AddItemsUnselected	14
#endif /* MOTIF1_1 */

/*  Global Variables  */

char *Istrings[NITEMS] = {
	"XmListAddItems()",
	"XmListAddItem()",
	"XmListAddItemUnselected()",
	"XmListDeleteItem()",
	"XmListDeleteItems()",
	"XmListDeleteItemsPos()",
	"XmListDeletePos()",
	"XmListDeleteAllItems()",
	"XmListDeselectAllItems()",
	"XmListDeselectItem()",
	"XmListDeselectPos()",
	"XmListSelectItem()",
	"XmListSelectPos()",
#ifndef MOTIF1_1
	"XmListAddItemsUnselected()"
#endif /* MOTIF1_1 */
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
Widget	  ToggleB;
Widget 	  ApplyB, ClearB;

XmString  CS_ItemList[50];

int	  function;
int	  count = 0;
int 	  arg_count = 0;
int       position = 0;

Boolean	  notify = True;

/*  Private Functions  */


static void  SSelCB_List1(Widget w, XtPointer client_data,
				    	XtPointer call_data);
static void  MulSelCB_List2(Widget w, XtPointer client_data,
				     	XtPointer call_data);
static void  ApplyCB(Widget w, XtPointer client_data,
			       		XtPointer call_data);
static void  ClearCB(Widget w, XtPointer client_data,
			       		XtPointer call_data);
static void  ScaleCB(Widget w, XtPointer client_data,
			       		XtPointer call_data);
static void  NotifyCB(Widget w, XtPointer client_data,
			       		XtPointer call_data);

