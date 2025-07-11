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
static char rcsid[] = "$XConsortium: POptionMenu1.c /main/8 1995/07/14 12:01:09 drk $"
#endif
#endif
/*  Standard C headers  */

/*  Xt headers  */

/*  Xm headers  */
#include <testlib.h>

/*  Global Variables  */
Widget    PullDown1;
Widget    Label1;
Widget    Separator1;
Widget    ToggleBtn1;
Widget    PushBtn1;
Widget    Separator2;
Widget    PushBtn2;
Widget    ToggleBtn2;
Widget    Separator3;
Widget    OptionMenu1;
Widget    OptionMenu3;
Widget    PushBtn3;

/*  Private Functions  */
static void  Quit();

static XtCallbackProc ButtonPressed(w, call_data, client_data)
Widget w;
caddr_t call_data, client_data;

{
printf ("Button pressed\n");
}

static Widget  CreateLabel(label, parent)
    char   *label;
    Widget  parent;
{
    Widget        widget;
    register int  n;
    Arg           args[MAX_ARGS];
    XmString      tcs;


    n = 0;
    tcs = XmStringLtoRCreate(label, XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, tcs);  n++;

    widget = XmCreateLabel(parent, "label", args, n);
    XtManageChild(widget);
    XmStringFree(tcs);

    return(widget);
}


static Widget  CreatePushButton(label, mnemonic, accel, accelText, parent)
    char   *label;
    char    mnemonic;
    Widget  parent;
    char *accel, *accelText;
{
    Widget        widget;
    register int  n;
    Arg           args[MAX_ARGS];
    XmString      tcs;

    n = 0;
    tcs = XmStringLtoRCreate(label, XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNaccelerator, accel); n++;
    XtSetArg(args[n], XmNacceleratorText,
      XmStringCreate(accelText, XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNlabelString, tcs);  n++;
    XtSetArg(args[n], XmNmnemonic, mnemonic);  n++;
    widget = XmCreatePushButton(parent, "pushButton", args, n);
    XtManageChild(widget);
    XmStringFree(tcs);

    return(widget);
}


static Widget  CreateToggle(label, mnemonic, parent)
    char   *label;
    char    mnemonic;
    Widget  parent;
{
    Widget        widget;
    register int  n;
    Arg           args[MAX_ARGS];
    XmString      tcs;


    n = 0;
    tcs = XmStringLtoRCreate(label, XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, tcs);  n++;
    XtSetArg(args[n], XmNmnemonic, mnemonic);  n++;
    widget = XmCreateToggleButton(parent, "toggle", args, n);
    XtManageChild(widget);
    XmStringFree(tcs);

    return(widget);
}


static Widget  CreateSeparator(parent)
Widget parent;
{
    Widget  widget;


    widget = XmCreateSeparator(parent, "separator", NULL, 0);
    XtManageChild(widget);

    return(widget);
}


static Widget  CreatePullDown(parent)
Widget parent;
{
    Widget  widget;
    

    widget = XmCreatePulldownMenu (parent, "pulldown", NULL, 0);

    return(widget);
}



void  main(argc, argv)
    int     argc;
    char  **argv;
{
    register int  n;
    Arg           args[MAX_ARGS];
    XmString      tcs;
    Widget Frame;


    CommonTestInit(argc, argv);
    
    n = 0;
    XtSetArg(args[n], XmNwidth,  200);  n++;
    XtSetArg(args[n], XmNheight, 40);   n++;
    XtSetArg(args[n], XtNgeometry, "+0+50");  n++;
    XtSetValues(Shell1, args, n);
    

    XtRealizeWidget(Shell1);



    /*
    **  PullDown1
    */
    PullDown1 = CreatePullDown(Shell1);


    /*
    **  Menu1
    */
    Label1        = CreateLabel("Menu1", PullDown1);

    Separator1    = CreateSeparator(PullDown1);

    ToggleBtn1    = CreateToggle("Toggle1", 'T', PullDown1);

    PushBtn1      = CreatePushButton("PushButton1", 'P', "Ctrl<Key>P",
		    "Ctrl-P",PullDown1);

    Separator2    = CreateSeparator(PullDown1);

    PushBtn2      = CreatePushButton("PushButton2", 'u', "Ctrl<Key>U",
		    "Ctrl-U", PullDown1);

    ToggleBtn2    = CreateToggle("Toggle2", 'o', PullDown1);

    Separator3    = CreateSeparator(PullDown1);


/* just for fun, put another manager in between */
    
    n = 0;
    Frame = XmCreateBulletinBoard (Shell1, "Frame", args, n);
    XtManageChild (Frame);

    /*
    **  OptionMenu1
    */
    n = 0;
    tcs = XmStringLtoRCreate("Option: ", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, tcs);         n++;
    XtSetArg(args[n], XmNmnemonic,    'O');         n++;
    XtSetArg(args[n], XmNsubMenuId,   PullDown1);   n++;
    XtSetArg(args[n], XmNmenuHistory, PushBtn2);    n++; 
    XtSetArg(args[n], XmNwhichButton, 2);	    n++;
    OptionMenu1 = XmCreateOptionMenu(Frame, "optionMenu", args, n);
    XtManageChild(OptionMenu1);
    XmStringFree(tcs);
    
    PushBtn1      = CreatePushButton("PushButton1", 'P', "Ctrl<Key>P",
			"Ctrl-P",PullDown1);


    PushBtn3      = CreatePushButton("*********PushButton3**********", 
				     's', "Ctrl<Key>S", "Ctrl-S",
				     PullDown1);

    XtAppMainLoop(app_context);
}


