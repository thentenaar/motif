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
/*   $XConsortium: List5.h /main/9 1995/07/13 18:19:20 drk $ */


#define NITEMS  10

/*  Global Variables  */

char *Istrings[NITEMS] = {
	"Item1",
	"Item2",
	"Item3",
	"Item4",
	"Item5",
	"Item6",
	"Item7",
	"Item8",
	"Item9",
	"Item10"
	};

Widget    List1, PushB;

/*  Private Functions  */


static void  DefACB(Widget w, XtPointer client_data,
			      XtPointer call_data);
static void  DefACB2(Widget w, XtPointer client_data,
			      XtPointer call_data);
static void  SSelCB(Widget w, XtPointer client_data,
			      XtPointer call_data);
static void  MSelCB(Widget w, XtPointer client_data,
			      XtPointer call_data);
static void  ESelCB(Widget w, XtPointer client_data,
			      XtPointer call_data);
static void  BSelCB(Widget w, XtPointer client_data,
			      XtPointer call_data);
static void  HelpCB(Widget w, XtPointer client_data,
			      XtPointer call_data);
static void  PushBCB(Widget w, XtPointer client_data,
		      	      XtPointer call_data);
static void  SensitiveCB(Widget w, XtPointer client_data,
		      	      XtPointer call_data);
static XtTimerCallbackProc PopupDown(XtPointer client_data);

