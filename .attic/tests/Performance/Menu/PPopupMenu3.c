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
static char rcsid[] = "$XConsortium: PPopupMenu3.c /main/7 1995/07/14 12:01:49 drk $"
#endif
#endif
#include <testlib.h>

#define	MAX_BUTTONS	40

/*  Global Variables  */
Widget    Label;
Widget    PopupMenu1;
Widget    Label1;
Widget    Separator1;
Widget    PullDown2;
Widget    CascadeBtn1;
Widget    Label2;
Widget    Separator2;
Widget    PullDown3;
Widget    CascadeBtn2;
Widget    Label3;
Widget    Separator3;
Widget    PullDown4;
Widget    CascadeBtn3;
Widget    Label4;

Widget	push_btns1[MAX_BUTTONS];
Widget	push_btns2[MAX_BUTTONS];
Widget	push_btns3[MAX_BUTTONS];

char	*btn_labels1[MAX_BUTTONS] = {

			"PushButton11",
			"PushButton12",
			"PushButton13",
			"PushButton14",
			"PushButton15",
			"PushButton16",
			"PushButton17",
			"PushButton18",
			"PushButton19",
			"PushButton110",
			"PushButton111",
			"PushButton112",
			"PushButton113",
			"PushButton114",
			"PushButton115",
			"PushButton116",
			"PushButton117",
			"PushButton118",
			"PushButton119",
			"PushButton120",
			"PushButton121",
			"PushButton122",
			"PushButton123",
			"PushButton124",
			"PushButton125",
			"PushButton126",
			"PushButton127",
			"PushButton128",
			"PushButton129",
			"PushButton130",
			"PushButton131",
			"PushButton132",
			"PushButton133",
			"PushButton134",
			"PushButton135",
			"PushButton136",
			"PushButton137",
			"PushButton138",
			"PushButton139",
			"PushButton140"

		};

char	*btn_labels2[MAX_BUTTONS] = {

			"PushButton21",
			"PushButton22",
			"PushButton23",
			"PushButton24",
			"PushButton25",
			"PushButton26",
			"PushButton27",
			"PushButton28",
			"PushButton29",
			"PushButton210",
			"PushButton211",
			"PushButton212",
			"PushButton213",
			"PushButton214",
			"PushButton215",
			"PushButton216",
			"PushButton217",
			"PushButton218",
			"PushButton219",
			"PushButton220",
			"PushButton221",
			"PushButton222",
			"PushButton223",
			"PushButton224",
			"PushButton225",
			"PushButton226",
			"PushButton227",
			"PushButton228",
			"PushButton229",
			"PushButton230",
			"PushButton231",
			"PushButton232",
			"PushButton233",
			"PushButton234",
			"PushButton235",
			"PushButton236",
			"PushButton237",
			"PushButton238",
			"PushButton239",
			"PushButton240"

		};

char	*btn_labels3[MAX_BUTTONS] = {

			"PushButton31",
			"PushButton32",
			"PushButton33",
			"PushButton34",
			"PushButton35",
			"PushButton36",
			"PushButton37",
			"PushButton38",
			"PushButton39",
			"PushButton310",
			"PushButton311",
			"PushButton312",
			"PushButton313",
			"PushButton314",
			"PushButton315",
			"PushButton316",
			"PushButton317",
			"PushButton318",
			"PushButton319",
			"PushButton320",
			"PushButton321",
			"PushButton322",
			"PushButton323",
			"PushButton324",
			"PushButton325",
			"PushButton326",
			"PushButton327",
			"PushButton328",
			"PushButton329",
			"PushButton330",
			"PushButton331",
			"PushButton332",
			"PushButton333",
			"PushButton334",
			"PushButton335",
			"PushButton336",
			"PushButton337",
			"PushButton338",
			"PushButton339",
			"PushButton340"

		};

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
		push_btns1[i] = CreatePushButton(btn_labels1[i], PopupMenu1);

    PullDown2    = CreatePullDown(PopupMenu1);

    CascadeBtn1  = CreateCascade("Cascade1", PullDown2, PopupMenu1);


    /*
    **  Menu2
    */
    Label2       = CreateLabel("Menu2", PullDown2);

    Separator2   = CreateSeparator(PullDown2);

	for (i = 0; i < num_buttons; i++)
		push_btns2[i] = CreatePushButton(btn_labels2[i], PullDown2);

    PullDown3    = CreatePullDown(PullDown2);

    CascadeBtn2  = CreateCascade("Cascade2", PullDown3, PullDown2);

    Label3       = CreateLabel("Menu3", PullDown3);

    Separator3   = CreateSeparator(PullDown3);

	for (i = 0; i < num_buttons; i++)
		push_btns3[i] = CreatePushButton(btn_labels3[i], PullDown3);

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


