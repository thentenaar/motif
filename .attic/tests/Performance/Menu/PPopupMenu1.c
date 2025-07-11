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
static char rcsid[] = "$XConsortium: PPopupMenu1.c /main/7 1995/07/14 12:01:30 drk $"
#endif
#endif
#include <testlib.h>

/*  Global Variables  */
Widget    Label;
Widget    PopupMenu1;
Widget    Label1;
Widget    Separator1;
Widget    PushBtn1;
Widget    ToggleBtn1;
Widget    PullDown2;
Widget    CascadeBtn1;
Widget    Label2;
Widget    Separator2;
Widget    ToggleBtn2;
Widget    PullDown3;
Widget    CascadeBtn2;
Widget    PushBtn2;
Widget    Label3;
Widget    PullDown4;
Widget    CascadeBtn3;
Widget    PushBtn3;
Widget    ToggleBtn3;
Widget    Label4;

/*  Private Functions  */
static void  Quit();
static XtCallbackProc  quitCB();

#define PUSHBUTTON 1
#define TOGGLEBUTTON 2
#define CASCADEBUTTON 3

XtCallbackProc CBHandler (w, client_data, call_data)
Widget w;
caddr_t client_data, call_data;

{
char *myname;
CoreWidget  *tmpwidget;

switch ( ((XmAnyCallbackStruct *)call_data)->reason)
  {    case XmCR_ARM:    printf ("Arm ");
                         break;
       case XmCR_ACTIVATE:    printf ("Activate ");
                         break;
       case XmCR_DISARM: printf ("Disarm ");
                         break;
       case XmCR_VALUE_CHANGED:  printf ("ValueChanged ");
                         break;
       case XmCR_CASCADING:  printf ("Cascading ");
                         break;
       default:           printf ("Other Callback ");
  }

printf ("%s\n", XtName(w));
XFlush(display);
fflush(stdout);

/*
tmpwidget = (CoreWidget)w;
*/

/* myname = ((CoreWidget *)w)->name;
   if (*myname != NULL) printf ("%s\n", myname);
*/

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


static Widget  CreatePushButton(label, parent)
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
    widget = XmCreatePushButton(parent, "pushButton", args, n);
    XtManageChild(widget);
    XmStringFree(tcs);

    return(widget);
}

static Widget  CreateToggle(label, parent)
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
    widget = XmCreateToggleButton(parent, "toggle", args, n);
    XtManageChild(widget);
    XmStringFree(tcs);

    return(widget);
}


static Widget  CreateSeparator(parent)
	Widget parent ;
{
    Widget  widget;


    widget = XmCreateSeparator(parent, "separator", NULL, 0);
    XtManageChild(widget);

    return(widget);
}


static Widget  CreatePullDown(parent)
	Widget parent ;
{
    Widget  widget;
    

    widget = XmCreatePulldownMenu (parent, "pulldown", NULL, 0);

    return(widget);
}


static Widget  CreateCascade(label, submenu, parent)
    char   *label;
    Widget  submenu;
    Widget  parent;
{
    Widget        widget;
    register int  n;
    Arg           args[MAX_ARGS];
    XmString      tcs;


    n = 0;
    tcs = XmStringLtoRCreate(label, XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, tcs);  n++;
    XtSetArg(args[n], XmNsubMenuId, submenu); n++;
    widget = XmCreateCascadeButton(parent, "cascade", args,n);
    XtManageChild(widget);
    XmStringFree(tcs);

    return(widget);
}


void  PostMenu (w, popup, event,c)
    Widget         w;
    XtPointer         popup;
    XEvent  *event;
    Boolean *c;
{
    if (event->xbutton.button != Button3)
        return;

    XmMenuPosition((Widget) popup, (XButtonPressedEvent *)event);
    XtManageChild ((Widget) popup);
}



void  main(argc, argv)
    int     argc;
    char  **argv;
{
    register int  n;
    Arg           args[MAX_ARGS];
    XmString      tcs;



    CommonTestInit(argc, argv);
    
    n = 0;
    XtSetArg(args[n], XmNwidth,  200);  n++;
    XtSetArg(args[n], XmNheight, 40);   n++;
    XtSetValues(Shell1, args, n);
    

    XtRealizeWidget(Shell1);


    Label = CreateLabel("Select Me", Shell1);


    /*
    **  PopupMenu1
    */
    PopupMenu1 = XmCreatePopupMenu(Label, "popMenu", NULL, 0);
    XtAddEventHandler(Label, ButtonPressMask, False, PostMenu, (XtPointer) PopupMenu1);


    /*
    **  Menu1
    */
    Label1       = CreateLabel("Menu1", PopupMenu1);

    Separator1   = CreateSeparator(PopupMenu1);

    PushBtn1     = CreatePushButton("PushButton1", PopupMenu1);

    ToggleBtn1   = CreateToggle("Toggle1", PopupMenu1);

    PullDown2    = CreatePullDown(PopupMenu1);

    CascadeBtn1  = CreateCascade("Cascade1", PullDown2, PopupMenu1);


    /*
    **  Menu2
    */
    Label2       = CreateLabel("Menu2", PullDown2);

    Separator2   = CreateSeparator(PullDown2);

    ToggleBtn2   = CreateToggle("Toggle2", PullDown2);

    Separator2   = CreateSeparator(PullDown2);

    PullDown3    = CreatePullDown(PullDown2);

    CascadeBtn2  = CreateCascade("Cascade2", PullDown3, PullDown2);

    Separator2   = CreateSeparator(PullDown2);

    PushBtn2     = CreatePushButton("PushButton2", PullDown2);


    /*
    **  Menu3
    */
    Label3       = CreateLabel("Menu3", PullDown3);

    PullDown4    = CreatePullDown(PullDown3);

    CascadeBtn3  = CreateCascade("Cascade3", PullDown4, PullDown3);

    PushBtn3     = CreatePushButton("PushButton3", PullDown3);

    ToggleBtn3   = CreateToggle("Toggle3", PullDown3);


    /*
    **  Menu4
    */
    Label4       = CreateLabel("Menu4", PullDown4);

    XtAppMainLoop(app_context);
}




static XtCallbackProc  quitCB(w, client_data, call_data)
    Widget   w;
    caddr_t  client_data;
    caddr_t  call_data;

{
	printf("Exiting...Please Standby...\n");
	exit(0);
}


