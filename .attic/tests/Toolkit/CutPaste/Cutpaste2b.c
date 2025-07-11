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
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: Cutpaste2b.c /main/7 1995/07/13 17:55:21 drk $"
#endif
#endif


#include <testlib.h>
#include "X11/Xatom.h"

Widget    Form1;
Widget    Text1;
Widget    RowCol1;
Widget    PushButton1;
Widget    PushButton2;
Widget    ToggleButton1;

void  main(argc, argv)
    int     argc;
    char  **argv;
{
    register int  n;
    Arg           args[MAX_ARGS];
    XmString      tcs;


    CommonTestInit(argc, argv);
    
    n = 0;
    XtSetArg(args[n], XmNwidth,  10);  n++;
    XtSetArg(args[n], XmNheight, 10);  n++;
    XtSetArg(args[n], XtNgeometry, "+500+0");  n++;
    XtSetValues(Shell1, args, n);
    

    XtRealizeWidget(Shell1);

    CommonPause();
    CommonPause();
    CommonPause();

    XtAppMainLoop(app_context);
}
