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
static char rcsid[] = "$XConsortium: List3.c /main/9 1995/07/13 18:17:25 drk $"
#endif
#endif

#include <testlib.h>
#include "List3.h"

/* 
   Test of PIR792 - When specifing selectedItems and items in a resource
		    file, overlaping of the arrays occured causing a memory
		    fault. 
   Test of PIR872 - Memory fault due to short allocation in ResConvert.c
*/

static void HowMany(Widget w, XtPointer client_data,
			      XtPointer call_data)

{
    short 		whichCallback;
    Arg 		args[2];
    int 		count, n;
    XmStringTable       my_items;

    n = 0;
    XtSetArg (args[n], XmNselectedItemCount, &count);		n++;
    XtSetArg (args[n], XmNselectedItems, &my_items);		n++;
    XtGetValues (w, args, n);

    whichCallback = (short) client_data;

    if (whichCallback) { 	/* default action */
   	printf ("Default Action callback, selectedItemCount is %d\n", count);
    }
    else 			/* selectionCallback */
   	printf ("Selection callback, selectedItemCount is %d\n", count);

}


void  main (argc, argv)
    int     argc;
    char  **argv;
{
    register int       n;
    Arg                args[MAX_ARGS];
    XmString           tcs, ItemList[NITEMS], SelectionList[NITEMS];
    XrmDatabase        new_db, save_db;

    CommonTestInit(argc, argv);

    XrmInitialize();

    new_db = XrmGetFileDatabase("List3DB");

#ifdef MOTIF1_1
    XrmMergeDatabases(new_db, &(display->db));
#else
    save_db = XtDatabase(display);
    XrmCombineDatabase(new_db, &save_db, True);
#endif

    for (n = 0; n < NITEMS; n++)
	ItemList[n] = XmStringLtoRCreate(Istrings[n],
			                 XmSTRING_DEFAULT_CHARSET);

    for (n = 0; n < NITEMS - 1; n++)
	SelectionList[n] = XmStringLtoRCreate(Istrings[n],
			              XmSTRING_DEFAULT_CHARSET);

    /* Create a List with no resources set. Control all resources
       in a separate resource file for this test */

    n = 0;
    List1 = XmCreateList(Shell1, "List1", args, n);
    XtManageChild(List1);

    /* 
       do this so Automation will work properly. ChangedManaged()
       routine in Shell is the problem.
    */

    n = 0;
    XtSetArg(args[n], XmNvisibleItemCount, 6);			n++;
    XtSetValues(List1, args, n);

    XtRealizeWidget(Shell1);

    CommonPause();

    n = 0;
    XtSetArg(args[n], XmNitems, ItemList); 			n++;
    XtSetArg(args[n], XmNselectedItems, SelectionList); 	n++;
    XtSetArg(args[n], XmNselectedItemCount, NITEMS - 1); 	n++;
    XtSetArg(args[n], XmNitemCount, NITEMS); 			n++;
    XtSetArg(args[n], XmNvisibleItemCount, NITEMS);		n++;
    XtSetArg(args[n], XmNshadowThickness, 10);			n++;
    XtSetArg(args[n], XmNlistSpacing, 10); 			n++;
    XtSetArg(args[n], XmNlistMarginWidth, 20);			n++;
    XtSetArg(args[n], XmNlistMarginHeight, 20);			n++;
    XtSetArg(args[n], XmNselectionPolicy, XmMULTIPLE_SELECT);	n++;
    XtSetArg(args[n], XmNheight, 200);				n++;
    XtSetArg(args[n], XmNwidth, 200);				n++;
    XtSetValues(List1, args, n);

    XtAddCallback (List1, XmNmultipleSelectionCallback, HowMany, (XtPointer)0);
    XtAddCallback (List1, XmNdefaultActionCallback, HowMany, (XtPointer)1);

    CommonPause();

    XtAppMainLoop(app_context);
}
