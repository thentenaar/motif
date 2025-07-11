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
/*   $XConsortium: List4.h /main/9 1995/07/13 18:18:45 drk $ */

#ifndef MOTIF1_1
#define NITEMS	 4
#else
#define NITEMS	 2
#endif /* MOTIF1_1 */

#define NITEMS2  5
#define NITEMS3  4

/*  Global Variables  */

#ifndef MOTIF1_1
char *Istrings[NITEMS] = {
	"XmListReplaceItems()",
	"XmListReplaceItemPos()",
	"XmListReplaceItemsUnselected()",
	"XmListReplaceItemPosUnselected()"
};
#else
char *Istrings[NITEMS] = {
	"XmListReplaceItems()",
	"XmListReplaceItemPos()"
};
#endif /* MOTIF1_1 */

char *Istrings2[NITEMS2] = {
	"Item1",
	"Item2",
	"Item3",
	"Item4",
	"Item5"
	};

char *Istrings3[NITEMS3] = {
	"Item6",
	"Item7",
	"Item8",
	"Item9"
	};

Widget	  Scale;
Widget    List1;
Widget    List2;
Widget    List3;
Widget	  Label1;
Widget	  Label2;
Widget	  Label3;
Widget	  Form;
Widget 	  ApplyB;
Widget 	  ClearB;
XmString  CS_ItemList1[50];
XmString  CS_ItemList2[50];
int	  count1 = 0;
int	  count2 = 0;
int 	  arg_count = 0;
int	  function = 0;

/*  Private Functions  */


static void  SSelCB_List1(Widget w, XtPointer client_data,
				    XtPointer call_data);
static void  MulSelCB_List1(Widget w, XtPointer client_data,
				    XtPointer call_data);
static void  MulSelCB_List2(Widget w, XtPointer client_data,
				    XtPointer call_data);
static void  ApplyCB(Widget w, XtPointer client_data,
				    XtPointer call_data);
static void  ClearCB(Widget w, XtPointer client_data,
				    XtPointer call_data);
static void  ScaleCB(Widget w, XtPointer client_data,
				    XtPointer call_data);

