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
static char rcsid[] = "$XConsortium: List11.c /main/5 1995/07/13 18:15:32 drk $"
#endif
#endif

#include <testlib.h>

#define NITEMS  10

/*  Global Variables  */

char *Istrings[NITEMS] = {
	"Item1",
	"Item2",
	"Item3",
	"Item4",
	"Item5",
	"Item6",
	"Item7",
	"Item8",
	"Item9",
	"Item10"
	};

Widget    List1, PushB;

/*  Private Functions  */


static void  DefACB(Widget w, XtPointer client_data,
			      XtPointer call_data);
static void  DefACB2(Widget w, XtPointer client_data,
			      XtPointer call_data);
static void  SSelCB(Widget w, XtPointer client_data,
			      XtPointer call_data);
static void  MSelCB(Widget w, XtPointer client_data,
			      XtPointer call_data);
static void  ESelCB(Widget w, XtPointer client_data,
			      XtPointer call_data);
static void  BSelCB(Widget w, XtPointer client_data,
			      XtPointer call_data);
static void  HelpCB(Widget w, XtPointer client_data,
			      XtPointer call_data);
static void  PushBCB(Widget w, XtPointer client_data,
		      	      XtPointer call_data);
static void  SensitiveCB(Widget w, XtPointer client_data,
		      	      XtPointer call_data);
static XtTimerCallbackProc PopupDown(XtPointer client_data);



void  main (argc, argv)
    int     argc;
    char  **argv;
{
    register int       	n;
    Arg                	args[MAX_ARGS];
    XmString           	tcs[1], ItemList[NITEMS];
    Widget		mWindow;
    Widget		Text;
    Widget		sWindow, pWindow;
    XmFontList         	newfontlist;
    XrmDatabase         new_db, save_db;

    CommonTestInit(argc, argv);

    n = 0;
    XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
    XtSetValues(Shell1, args, n);

    XrmInitialize();

    new_db = XrmGetFileDatabase("List11DB");

#ifdef MOTIF1_1
    XrmMergeDatabases(new_db, &(display->db));
#else
    save_db = XtDatabase(display);
    XrmCombineDatabase(new_db, &save_db, True);
#endif

    newfontlist = CommonGetFontList(
	  "-adobe-helvetica-bold-r-normal--14-140-75-75-p-82-iso8859-1");

    n = 0;
    mWindow = XmCreateMainWindow(Shell1, "mWindow", args, n);
    XtManageChild (mWindow);

    n = 0;
    pWindow = XmCreatePanedWindow(mWindow, "pWindow", args, n);
    XtManageChild (pWindow);

    n = 0;
    XtSetArg(args[n], XmNshadowThickness, 0);				n++;
    XtSetArg(args[n], XmNscrollingPolicy, XmAPPLICATION_DEFINED);	n++;
    sWindow = XmCreateScrolledWindow(pWindow, "sWindow", args, n);
    XtManageChild (sWindow);

    for (n = 0; n < NITEMS; n++)
	ItemList[n] = XmStringLtoRCreate(Istrings[n],
			                 XmSTRING_DEFAULT_CHARSET);

    tcs[0] = XmStringLtoRCreate("Item1", XmSTRING_DEFAULT_CHARSET);

    XtRealizeWidget(Shell1);

    n = 0;
    XtSetArg(args[n], XmNselectionPolicy, XmEXTENDED_SELECT);   n++;
    XtSetArg(args[n], XmNitems, ItemList);                      n++;
    XtSetArg(args[n], XmNitemCount, NITEMS);                    n++;
    XtSetArg(args[n], XmNvisibleItemCount, NITEMS-5);           n++;
    XtSetArg(args[n], XmNlistSizePolicy, XmRESIZE_IF_POSSIBLE); n++;
    XtSetArg(args[n], XmNfontList, newfontlist);		n++;
    List1 = XmCreateList(sWindow, "List1", args, n);
    XtManageChild(List1);

    XtAddCallback(List1, XmNextendedSelectionCallback, ESelCB, (XtPointer)NULL);
    XtAddCallback(List1, XmNdefaultActionCallback, DefACB, (XtPointer)NULL);

    n = 0;
    XtSetArg(args[n], XmNfontList, newfontlist);		n++;
    Text = XmCreateScrolledText(pWindow, "Text1", args, n);
    XtManageChild(Text);

    CommonPause();

    XtAppMainLoop(app_context);
}

/* Begin Test code for Pir2978 */

static void DefACB2(Widget w, XtPointer client_data,
			      XtPointer call_data)

{
    Widget 	InfoDia1;
    int		n;
    Arg		args[5];

    n = 0;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_PRIMARY_APPLICATION_MODAL); n++;
    XtSetArg(args[n], XmNx, 0);				           n++;
    XtSetArg(args[n], XmNy, 0);					   n++;
    XtSetArg(args[n], XmNdefaultPosition, False);		   n++;
    InfoDia1 = XmCreateInformationDialog(Shell1, "InfoDia1", args, n);
    XtManageChild(InfoDia1);

}


static XtTimerCallbackProc PopupDown(XtPointer client_data)

{

    if (XtIsManaged((Widget) client_data)) {
    	printf("Unmanaging Dialog\n");
    	XtUnmanageChild((Widget)client_data);
    }
    else
	printf("Dialog Already Unmanaged\n");

}

/* End Test Code for Pir2978 */

static void HelpCB(Widget w, XtPointer client_data,
			     XtPointer call_data)

{
    printf("Help Callback called.\n");
}


static void BSelCB(Widget w, XtPointer client_data,
			     XtPointer call_data)

{
    printf("BrowseSelect Callback called.\n");
}


static void SSelCB(Widget w, XtPointer client_data,
			     XtPointer call_data)

{
    printf("SingleSelection Callback called.\n");
}


static void DefACB(Widget w, XtPointer client_data,
			     XtPointer call_data)

{
    printf("DefaultAction Callback called.\n");
}


static void MSelCB(Widget w, XtPointer client_data,
			     XtPointer call_data)

{
    printf("MultipleSelection Callback called\n");
}


static void ESelCB(Widget w, XtPointer client_data,
			     XtPointer call_data)

{
    printf("ExtendedSelection Callback called\n");
}
