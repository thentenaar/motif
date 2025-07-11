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
/*   $XConsortium: TextFWcs1.h /main/7 1995/07/13 19:33:06 drk $ */


#ifndef MOTIF1_1

static char starting_string[] = "This is the starting string";

#define NITEMS	 6

#define	GetSelectionWcs		1
#define	InsertWcs		2
#define	GetStringWcs		3
#define	SetStringWcs		4
#define	ReplaceWcs		5
#define	GetSubstringWcs		6

/*  Global Variables  */

char *Istrings[NITEMS] = {
	"XmTextFieldGetSelectionWcs()",
	"XmTextFieldInsertWcs()",
	"XmTextFieldGetStringWcs()",
	"XmTextFieldSetStringWcs()",
	"XmTextFieldReplaceWcs()",
	"XmTextFieldGetSubstringWcs()"
};



Widget	  Scale1, Scale2, Scale3;
Widget    List1;
Widget    Text1, StatText;
Widget	  Label1, Label2, Label3, Label4, Label5, Label6;
Widget	  Form;
Widget	  ToggleB, ToggleB2;
Widget 	  ApplyB, ClearB;

static    int	  function;
static    int	  count = 0;
static    int 	  scale1_val = 0;
static    int 	  scale2_val = 0;
static    int 	  scale3_val = 0;
static    int       position = 0;

static    Boolean	  torf = True;

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
static void  Scale3CB(Widget w, XtPointer client_data,
		       		XtPointer call_data);
static void  TorFCB(Widget w, XtPointer client_data,
		       		XtPointer call_data);


#endif /* MOTIF1_1 */
