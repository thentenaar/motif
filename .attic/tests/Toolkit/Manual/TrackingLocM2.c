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
static char rcsid[] = "$XConsortium: TrackingLocM2.c /main/6 1995/07/13 18:42:17 drk $"
#endif
#endif

#include <testlib.h>
#include <X11/cursorfont.h>

/* forward refs */
static void Track();

Widget	BB;
Widget	Push1, Push2, Push3;
Widget	Push4, Push5, Push6;
Widget  submenu, menu, menubar;
Widget  submenu2, menu2;

Boolean confine = False;
int   	cur_num = 1;
char 	begin[50] = "Help has arrived (via XmTrackingLocate) for ";


void  main(argc, argv)
    int     argc;
    char  **argv;
{
    register int  n;
    Arg           args[MAX_ARGS];
    XmString      tcs;
    Pixel         colorVal;
    
    CommonTestInit(argc, argv);

    n = 0;

    BB = XmCreateBulletinBoard(Shell1, "BulletinBoard", args, n);
    XtManageChild(BB);

    XtAddCallback(BB, XmNhelpCallback, Track, NULL);

    n = 0;
    menubar = XmCreateMenuBar(BB, "MenuBar", NULL, 0);
    XtManageChild(menubar);

    submenu = XmCreatePulldownMenu(menubar, "Submenu1", NULL, 0);

    submenu2 = XmCreatePulldownMenu(menubar, "Submenu2", NULL, 0);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
             XmStringLtoRCreate("Menu1", XmSTRING_DEFAULT_CHARSET)); 	n++;
    XtSetArg(args[n], XmNsubMenuId, submenu); 				n++;
    menu = XmCreateCascadeButton(menubar, "Cascade_Menu1", args, n);
    XtManageChild(menu);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
             XmStringLtoRCreate("Menu2", XmSTRING_DEFAULT_CHARSET)); 	n++;
    XtSetArg(args[n], XmNsubMenuId, submenu2); 				n++;
    menu2 = XmCreateCascadeButton(menubar, "Cascade_Menu2", args, n);
    XtManageChild(menu2);

    n = 0;
    Push1 = XmCreatePushButton(submenu, "Push Button #1", args, n);
    XtManageChild(Push1);

    n = 0;
    Push2 = XmCreatePushButton(submenu, "Push Button #2", args, n);
    XtManageChild(Push2);

    n = 0;
    Push3 = XmCreatePushButton(submenu, "Push Button #3", args, n);
    XtManageChild(Push3);

    n = 0;
    Push4 = XmCreatePushButton(submenu2, "Push Button #4", args, n);
    XtManageChild(Push4);

    n = 0;
    Push5 = XmCreatePushButton(submenu2, "Push Button #5", args, n);
    XtManageChild(Push5);

    n = 0;
    Push6 = XmCreatePushButton(submenu2, "Push Button #6", args, n);
    XtManageChild(Push6);

    XtAddCallback(Push6, XmNactivateCallback, Track, NULL);

    XtRealizeWidget(Shell1);

    CommonPause();

    confine = True;

    CommonPause();

    XtAppMainLoop(app_context);
}

static void Track(w, client_data, call_data)
Widget   w;
XtPointer  client_data;
XtPointer  call_data;
{
    Cursor 	cursor;
    int 	n = 0;
    Arg 	args[5];
    char	to[150];
    XmString	tcs;
    Widget 	MessageB;
    Widget	widget;
	

    cursor = XCreateFontCursor (display, XC_pirate);

    if (confine)
	widget = XmTrackingLocate(BB,cursor, True);
    else
    	widget = XmTrackingLocate(Shell1, cursor, False);

    if (widget != NULL)
    { 
    	strcpy(to, begin);
    	strcat(to, XtName(widget));
    	tcs = XmStringCreateLtoR(to, XmSTRING_DEFAULT_CHARSET);
    }
    else
    {
	tcs = XmStringCreateLtoR("Error in Return Value from XmTrackingLocate()\nTry again with pointer in a widget",
			XmSTRING_DEFAULT_CHARSET);
    }

    n = 0;
    XtSetArg(args[n], XmNdefaultPosition, False);	n++;
    XtSetArg(args[n], XmNx, 0);				n++;
    XtSetArg(args[n], XmNy, 200);			n++;
    XtSetArg(args[n], XmNmessageString, tcs);		n++;
    MessageB = XmCreateInformationDialog(Shell1, "MessageB", args, n);
    XtManageChild(MessageB);

    cur_num++;
}
