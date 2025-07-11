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
static char rcsid[] = "$XConsortium: ScrollList1.c /main/9 1995/07/13 18:22:42 drk $"
#endif
#endif

#include <testlib.h>
#include "ScrollList1.h"


											/* Begin CR5080 */
/*
 * The following callback routine deletes (NITEMS - VITEMS) items
 * from the scrollList, resulting in the disappearence of the
 * vertical scrollbar. Also, if CR5080 has not been fixed, or
 * has reappeared, this will result in a core dump.
 */
static void delete_items(Widget w, XtPointer client_data,
			XtPointer call_data)

{
  XmListDeleteItemsPos (List1, (NITEMS - VITEMS), VITEMS);
}
											/* End CR5080 */

void  main (argc, argv)
    int     argc;
    char  **argv;
{
    register int       n;
    Arg                args[MAX_ARGS];
    XmString           tcs, ItemList[NITEMS];

    CommonTestInit(argc, argv);
    
    n = 0;
    XtSetArg(args[n], XmNwidth,  150);  			n++;
    XtSetArg(args[n], XmNheight, 300);  			n++;
    XtSetArg(args[n], XtNgeometry, "+0+0");  			n++;
    XtSetValues(Shell1, args, n);
    

    XtRealizeWidget(Shell1);

    /* convert the plain strings to fancy Xstrings */
    for (n = 0; n < NITEMS; n++)
	ItemList[n] = XmStringLtoRCreate(Istrings[n],
			  XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNlistSpacing, 5);  			n++;
    XtSetArg(args[n], XmNlistMarginWidth, 2);  			n++;
    XtSetArg(args[n], XmNlistMarginHeight, 2);  		n++;
    XtSetArg(args[n], XmNlistSizePolicy, XmVARIABLE);  		n++;
    XtSetArg(args[n], XmNitems, ItemList); 			n++;
    XtSetArg(args[n], XmNitemCount, NITEMS); 			n++;
    XtSetArg(args[n], XmNvisibleItemCount, VITEMS); 		n++;
    XtSetArg(args[n], XmNbackground,
    		WhitePixel(display,DefaultScreen(display))); 	n++;
    XtSetArg(args[n], XmNforeground,
    		BlackPixel(display,DefaultScreen(display))); 	n++;
    List1 = XmCreateScrolledList(Shell1, "List1", args, n);
    XtManageChild(List1);

    /* Add Callbacks */
    XtAddCallback(List1, XmNsingleSelectionCallback, SinSelCB, 
		  (XtPointer) NULL);
    XtAddCallback(List1, XmNmultipleSelectionCallback, MulSelCB, 
		  (XtPointer) NULL);
    XtAddCallback(List1, XmNextendedSelectionCallback, ExtSelCB, 
		  (XtPointer) NULL);
    XtAddCallback(List1, XmNbrowseSelectionCallback, SinSelCB, 
		  (XtPointer) NULL);

    CommonPause();

    n = 0;
    XtSetArg(args[n], XmNlistSpacing, 4);  			n++;
    XtSetArg(args[n], XmNlistMarginWidth, 2);  			n++;
    XtSetArg(args[n], XmNlistMarginHeight, 2);  		n++;
    XtSetArg(args[n], XmNselectionPolicy, XmMULTIPLE_SELECT);  	n++;
    XtSetValues(List1, args, n);

    CommonPause();

    n = 0;
    XtSetArg(args[n], XmNselectionPolicy, XmBROWSE_SELECT);  	n++;
    XtSetValues(List1, args, n);

    CommonPause();

    n = 0;
    XtSetArg(args[n], XmNselectionPolicy, XmEXTENDED_SELECT);  	n++;
    XtSetValues(List1, args, n);

    CommonPause();
											/* Begin CR5080 */

	XtAddCallback (List1, XmNdefaultActionCallback, delete_items, NULL);

    CommonPause();
											/* End CR5080 */

    XtAppMainLoop(app_context);
}


static void SinSelCB(Widget w, XtPointer client_data,
			       XtPointer call_data)

{
    char **text;
    XmString tcs;
    
    printf("One item selected : ");
    tcs = ((XmListCallbackStruct*)call_data)->item;
    printf("no. %d\n", ((XmListCallbackStruct*)call_data)->item_position);
}


static void MulSelCB(Widget w, XtPointer client_data,
			       XtPointer call_data)

{
    char **text;
    XmString tcs;
    
    printf("One or more items selected : ");
    tcs = ((XmListCallbackStruct*)call_data)->item;
    printf("no. of items is %d\n",
	   ((XmListCallbackStruct*)call_data)->selected_item_count);
    printf("    Last item selected has position no. %d\n",
	   ((XmListCallbackStruct*)call_data)->item_position);
}


static void ExtSelCB(Widget w, XtPointer client_data,
			       XtPointer call_data)

{
    char **text;
    XmString tcs;
    
    printf("One or more items selected : ");
    tcs = ((XmListCallbackStruct*)call_data)->item;
    printf("no. of items is %d\n",
	   ((XmListCallbackStruct*)call_data)->selected_item_count);
    printf("    Type of Selection was ");
    switch (((XmListCallbackStruct*)call_data)->selection_type) {
	case XmINITIAL      : printf("Initial\n");
			      break;
	case XmADDITION     : printf("Addition\n");
			      break;
	case XmMODIFICATION : printf("Modify\n");
			      break;
	default             : printf("not defined?\n");
    }
}
