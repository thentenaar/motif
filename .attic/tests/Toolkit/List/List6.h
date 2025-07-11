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
/*   $XConsortium: List6.h /main/9 1995/07/13 18:19:53 drk $ */

#define NITEMS  12

/*  Global Variables  */

char *Istrings[NITEMS] = {
	"LongItem1",
	"LongItem2",
	"LongItem3",
	"LongItem4",
	"LongItem5",
	"LongItem6",
	"LongItem7",
	"LongItem8",
	"LongItem9",
	"LongItem10",
	"LongItem11",
	"LongItem12"
	};

char *Istrings2[NITEMS] = {
	"Item1",
	"Item2",
	"Item3",
	"Item4",
	"Item5",
	"Item6",
	"Item7",
	"Item8",
	"Item9",
	"Item10",
	"Item11",
	"Item12"
	};

Widget    List1;

/*  Private Functions  */


static XtCallbackProc  DefACB(Widget w, caddr_t client_data,
			      caddr_t call_data);
static XtCallbackProc  SSelCB(Widget w, caddr_t client_data,
			      caddr_t call_data);
static XtCallbackProc  MSelCB(Widget w, caddr_t client_data,
			      caddr_t call_data);
static XtCallbackProc  ESelCB(Widget w, caddr_t client_data,
			      caddr_t call_data);
static XtCallbackProc  BSelCB(Widget w, caddr_t client_data,
			      caddr_t call_data);
static XtCallbackProc  HelpCB(Widget w, caddr_t client_data,
			      caddr_t call_data);
static void 	       DeleteMe(Widget w, XEvent *event, 
			      String *params, Cardinal num_params);

