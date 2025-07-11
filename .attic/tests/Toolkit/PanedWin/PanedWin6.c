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
static char rcsid[] = "$XConsortium: PanedWin6.c /main/4 1995/07/13 19:07:23 drk $"
#endif
#endif


#include <testlib.h>

/* Tests that multiple buttons in paned window are all */
/* created with the same height. */

void  main(argc, argv)
int     argc;
char  **argv;
{
    Boolean       	trace = False;
    register int  	n, i;
    Arg			args[MAX_ARGS];
    Widget		PW, PB1, PB2, PB3;
    
    CommonTestInit(argc, argv);
    
    n = 0;
    XtSetArg(args[n], XmNgeometry, "50x50+0+0");  n++;
    XtSetValues(Shell1, args, n); 
    XtRealizeWidget(Shell1);

    n = 0;
    PW = XmCreatePanedWindow(Shell1, "PW", args, n);
    XtManageChild (PW);

    
    n = 0;
    PB1 = XmCreatePushButton(PW, "PB1", args, n);
    XtManageChild(PB1);

    n=0;
    PB2 = XmCreatePushButton(PW, "PB2", args, n);
    XtManageChild(PB2);

    n=0;
    PB3 = XmCreatePushButton(PW, "PB3", args, n);
    XtManageChild(PB3);



    CommonPause();


    /* Begin test for CR 4886 */
   
    n=0;
    XtSetArg (args[n], XmNsashWidth, 30); n++;
    XtSetArg (args[n], XmNsashHeight, 30); n++;
    XtSetValues (PW, args, n);   

    /* End test for CR 4886 */

    CommonPause();
    
    XtAppMainLoop(app_context);
}



