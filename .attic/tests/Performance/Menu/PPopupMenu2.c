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
static char rcsid[] = "$XConsortium: PPopupMenu2.c /main/7 1995/07/14 12:01:40 drk $"
#endif
#endif
#include <testlib.h>

#define	MAX_BUTTONS	40

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

Widget	push_btns[MAX_BUTTONS];

char	*btn_labels[MAX_BUTTONS] = {

			"PushButton1",
			"PushButton2",
			"PushButton3",
			"PushButton4",
			"PushButton5",
			"PushButton6",
			"PushButton7",
			"PushButton8",
			"PushButton9",
			"PushButton10",
			"PushButton11",
			"PushButton12",
			"PushButton13",
			"PushButton14",
			"PushButton15",
			"PushButton16",
			"PushButton17",
			"PushButton18",
			"PushButton19",
			"PushButton20",
			"PushButton21",
			"PushButton22",
			"PushButton23",
			"PushButton24",
			"PushButton25",
			"PushButton26",
			"PushButton27",
			"PushButton28",
			"PushButton29",
			"PushButton30",
			"PushButton31",
			"PushButton32",
			"PushButton33",
			"PushButton34",
			"PushButton35",
			"PushButton36",
			"PushButton37",
			"PushButton38",
			"PushButton39",
			"PushButton40"

		};
Widget    PushBtn4;
Widget    PushBtn5;
Widget    PushBtn6;
Widget    PushBtn7;
Widget    PushBtn8;
Widget    PushBtn9;
Widget    PushBtn10;
Widget    PushBtn11;
Widget    PushBtn12;
Widget    PushBtn13;
Widget    PushBtn14;
Widget    PushBtn15;
Widget    PushBtn16;
Widget    PushBtn17;
Widget    PushBtn18;
Widget    PushBtn19;
Widget    PushBtn20;

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
	int			num_buttons;
	int			i;



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

	num_buttons = MAX_BUTTONS; /* to be modified to take from command line */
	for (i = 0; i < num_buttons; i++)
		push_btns[i] = CreatePushButton(btn_labels[i], PopupMenu1);
    ToggleBtn1   = CreateToggle("Toggle1", PopupMenu1);

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


