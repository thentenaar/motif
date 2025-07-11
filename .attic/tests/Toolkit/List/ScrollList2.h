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
/*   $XConsortium: ScrollList2.h /main/9 1995/07/13 18:23:39 drk $ */

#define NITEMS 62
#define VITEMS 20

/* Global Variables */

char *Istrings[] = {
    "btest1a", "btest1b", "btest2", "btest3",
    "decor00", "decor01", "decor02", "decor03", "decor04", "decor05",
    "decor06", "decor07", "decor08", "decor09",
    "func00", "func01", "func02", "func03", "func04",
    "func05", "func06", "func07", "func08", "func09",
    "func10", "func11", "func12", "func13", "func14",
    "iplac00", "iplac01", "iplac02", "iplac03", "iplac04",
    "iplac05", "iplac06", "iplac07", "iplac08",
    "menu00", "menu01", "menu02", "menu03",
    "menu04", "menu05", "menu06", "menu07",
    "mtest1", "mtest2", "mvrz00", "mvrz01",
    "parse00", "parse01", "parse02", "parse03", "pitest1",
    "prop00", "prop01", "prop02", "prop03", "prop04", "prop05",
    "rstest1"
    };


Widget    List1;

/* Private Functions */


static void  SinSelCB(Widget w, XtPointer client_data, XtPointer call_data);
static void  MulSelCB(Widget w, XtPointer client_data, XtPointer call_data);
static void  ExtSelCB(Widget w, XtPointer client_data, XtPointer call_data);

