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
/*   $XConsortium: ScrollList3.h /main/9 1995/07/13 18:24:18 drk $ */

#define NITEMS 10

/* Global Varaibles */

char *Istrings[] = {
    "One", "Two", "Three", "Four", "Five",
    "Six", "Seven", "Eight", "Nine", "Ten"
};

Widget Form1;
Widget    List1;

/* Private Functions */


static void  SinSelCB(Widget w, XtPointer client_data, XtPointer call_data);
static void  addCB(Widget w, XtPointer client_data, XtPointer call_data);
static void  add1CB(Widget w, XtPointer client_data, XtPointer call_data);

