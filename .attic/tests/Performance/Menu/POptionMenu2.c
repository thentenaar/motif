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
static char rcsid[] = "$XConsortium: POptionMenu2.c /main/8 1995/07/14 12:01:19 drk $"
#endif
#endif
#include <testlib.h>

#define MAX_BUTTONS  40

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
			"**********PushButton110**********",
			"PushButton111",
			"PushButton112",
			"PushButton113",
			"PushButton114",
			"PushButton115",
			"PushButton116",
			"PushButton117",
			"PushButton118",
			"PushButton119",
			"**********PushButton120**********",
			"PushButton121",
			"PushButton122",
			"PushButton123",
			"PushButton124",
			"PushButton125",
			"PushButton126",
			"PushButton127",
			"PushButton128",
			"PushButton129",
			"**********PushButton130**********",
			"PushButton131",
			"PushButton132",
			"PushButton133",
			"PushButton134",
			"PushButton135",
			"PushButton136",
			"PushButton137",
			"PushButton138",
			"PushButton139",
			"**********PushButton140**********",

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
			"**********PushButton210**********",
			"PushButton211",
			"PushButton212",
			"PushButton213",
			"PushButton214",
			"PushButton215",
			"PushButton216",
			"PushButton217",
			"PushButton218",
			"PushButton219",
			"**********PushButton220**********",
			"PushButton221",
			"PushButton222",
			"PushButton223",
			"PushButton224",
			"PushButton225",
			"PushButton226",
			"PushButton227",
			"PushButton228",
			"PushButton229",
			"**********PushButton230**********",
			"PushButton231",
			"PushButton232",
			"PushButton233",
			"PushButton234",
			"PushButton235",
			"PushButton236",
			"PushButton237",
			"PushButton238",
			"PushButton239",
			"**********PushButton240**********"

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
	int			num_buttons;
	int			i;


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

	num_buttons = MAX_BUTTONS; /* to be modified to take from command line */
	for (i = 0; i < num_buttons; i++)
		push_btns1[i] = CreatePushButton(btn_labels1[i], PullDown1);

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
    
    PushBtn1      = CreatePushButton("PushButton1", PullDown1);

    PushBtn3      = CreatePushButton("*********PushButton3**********", 
				     PullDown1);

    XtAppMainLoop(app_context);
}


