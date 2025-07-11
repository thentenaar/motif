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
static char rcsid[] = "$XConsortium: List5.c /main/9 1995/07/13 18:18:57 drk $"
#endif
#endif


#include <testlib.h>
#include "List5.h"

Widget PushB;

void  main (argc, argv)
    int     argc;
    char  **argv;
{
    register int       	n;
    Arg                	args[MAX_ARGS];
    XmString           	tcs[1], ItemList[NITEMS];
    XmString		tcs2;
    Widget		BulletinB;
    XmFontList         	newfontlist;

    CommonTestInit(argc, argv);
    
    newfontlist = CommonGetFontList(
	  "-adobe-helvetica-bold-r-normal--14-140-75-75-p-82-iso8859-1");

    n = 0;
    XtSetArg(args[n], XmNtextFontList, newfontlist);			n++;
    BulletinB = XmCreateBulletinBoard (Shell1, "BulletinB", args, n);
    XtManageChild (BulletinB);

    for (n = 0; n < NITEMS; n++)
	ItemList[n] = XmStringLtoRCreate(Istrings[n],
			                 XmSTRING_DEFAULT_CHARSET);

    tcs[0] = XmStringLtoRCreate("Item1", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNwidth, 100);				n++;
    XtSetArg(args[n], XmNheight, 250);				n++;
    XtSetArg(args[n], XmNselectionPolicy, XmEXTENDED_SELECT);	n++;
    XtSetArg(args[n], XmNitems, ItemList); 			n++;
    XtSetArg(args[n], XmNitemCount, NITEMS); 			n++;
    XtSetArg(args[n], XmNvisibleItemCount, NITEMS);		n++;
    XtSetArg(args[n], XmNselectedItems, tcs);			n++;
    XtSetArg(args[n], XmNselectedItemCount, 1);			n++;
    List1 = XmCreateList(BulletinB, "List1", args, n);
    XtManageChild(List1);

    XtAddCallback(List1, XmNmultipleSelectionCallback, MSelCB, (XtPointer)NULL);
    XtAddCallback(List1, XmNextendedSelectionCallback, ESelCB, (XtPointer)NULL);
    XtAddCallback(List1, XmNdefaultActionCallback, DefACB, (XtPointer)NULL);
    XtAddCallback(List1, XmNsingleSelectionCallback, SSelCB, (XtPointer)NULL);
    XtAddCallback(List1, XmNbrowseSelectionCallback, BSelCB, (XtPointer)NULL);
    XtAddCallback(List1, XmNhelpCallback, HelpCB,(XtPointer)NULL);

    XtRealizeWidget(Shell1);

    CommonPause();
    CommonPause();
    CommonPause();

    /* Enter Add Mode with Selection Policy as EXTENDED SELECT */

    XmListSetAddMode(List1, True);  

    CommonPause();
    CommonPause();
    CommonPause();

    /* Begin Test for Pir4605 */

    n = 0;
    XtSetArg(args[n], XmNwidth, 400);                   	n++;
    XtSetValues(BulletinB, args, n);

    n = 0;
    XtSetArg(args[n], XmNx, 350);               		n++;
    XtSetArg(args[n], XmNy, 0);                 		n++;
    PushB = XmCreatePushButton(BulletinB, "CancelButton", args, n);
    XtManageChild(PushB);

    XtAddCallback(PushB, XmNactivateCallback, PushBCB, NULL);

    n = 0;
    XtSetArg(args[n], XmNcancelButton, PushB); 	              	n++;
    XtSetValues(BulletinB, args, n);

    CommonPause();

    /* End Test for Pir4605 */

    /* Begin Test for Pir2920 */

    XtRemoveCallback(PushB, XmNactivateCallback, PushBCB, NULL);
    XtAddCallback(PushB, XmNactivateCallback, SensitiveCB, NULL);

    tcs2 = XmStringCreateSimple("Set Insensitive");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs2);			n++;
    XtSetValues(PushB, args, n);

    XmStringFree(tcs2);

    /* End Test for Pir2920 */

    CommonPause();

    /* Begin Test Code for Pir2978 */

    XtDestroyWidget(List1);
    XtDestroyWidget(PushB);

    n = 0;
    XtSetArg(args[n], XmNy, 250);				n++;
    XtSetValues(Shell1, args, n);

    n = 0;
    XtSetArg(args[n], XmNwidth, 100);                  		n++;
    XtSetValues(BulletinB, args, n);

    n = 0;
    XtSetArg(args[n], XmNselectionPolicy, XmEXTENDED_SELECT);   n++;
    XtSetArg(args[n], XmNitems, ItemList);                      n++;
    XtSetArg(args[n], XmNitemCount, NITEMS);                    n++;
    XtSetArg(args[n], XmNvisibleItemCount, NITEMS-5);           n++;
    List1 = XmCreateScrolledList(BulletinB, "List1", args, n);
    XtManageChild(List1);

    XtAddCallback(List1, XmNdefaultActionCallback, DefACB2, (XtPointer)NULL);

    CommonPause();

    /* End Test Code for Pir2978 */

    XtAppMainLoop(app_context);
}


static void PushBCB(Widget w, XtPointer client_data,
			      XtPointer call_data)
{

    printf("Cancel button callback called - should not have been\n");

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


static void SensitiveCB(Widget w, XtPointer client_data,
			      XtPointer call_data)

{

	XtSetSensitive(PushB, False);
	XtSetSensitive(List1, False);

}

