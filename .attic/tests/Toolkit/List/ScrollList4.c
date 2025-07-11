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
static char rcsid[] = "$XConsortium: ScrollList4.c /main/10 1995/07/13 18:24:32 drk $"
#endif
#endif

#include <testlib.h>
#include "ScrollList4.h"


void  main (argc, argv)
    int     argc;
    char  **argv;
{
    register int       	n, i;
    Widget		Form, Push1;
    Arg                	args[MAX_ARGS];
    XmString           	tcs, ItemList[NITEMS];
    XmFontList         	newfontlist;
    int 	       	position, position_count = 0;
    int 		*position_list = NULL;
    int			item_count = NITEMS;
    char		temp[10];

    CommonTestInit(argc, argv);

    newfontlist = CommonGetFontList("9x15");

    n = 0;
    XtSetArg(args[n], XmNtextFontList, newfontlist);  		n++;
    Form = XmCreateForm (Shell1, "Form", args, n);
    XtManageChild (Form);

    /* convert the plain strings to Xstrings */
    for (n = 0; n < (NITEMS - 1); n++)
	ItemList[n] = XmStringLtoRCreate(Istrings[n],
			  XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM);	n++;
    XtSetArg(args[n], XmNselectionPolicy, XmMULTIPLE_SELECT);	n++;
    XtSetArg(args[n], XmNhighlightOnEnter, True); 		n++;
    XtSetArg(args[n], XmNlistSpacing, 5);  			n++;
    XtSetArg(args[n], XmNlistMarginWidth, 2);  			n++;
    XtSetArg(args[n], XmNlistMarginHeight, 2);  		n++;
    XtSetArg(args[n], XmNlistSizePolicy, XmVARIABLE);  		n++;
    XtSetArg(args[n], XmNitems, ItemList); 			n++;
    XtSetArg(args[n], XmNitemCount, NITEMS - 1);		n++;
    List1 = XmCreateScrolledList(Form, "List1", args, n);
    XtManageChild(List1);

    n = 0;
    XtSetArg(args[n], XmNvisibleItemCount, NITEMS - 1);		n++;
    XtSetValues(List1, args, n);

    XtRealizeWidget(Shell1);

    CommonPause();

    /*
       Testing XmListItemPos() with valid and non-valid input 
    */

    tcs = XmStringLtoRCreate("Item5", XmSTRING_DEFAULT_CHARSET);
    printf("Determining what Position Item5 is in....\n");
    if (position = XmListItemPos(List1, tcs))
    	printf("    - Item5 is found in Position %d\n", position);
    else
	printf("    - Item5 could not be found in the List\n");
    XmStringFree(tcs);

    tcs = XmStringLtoRCreate("Item0", XmSTRING_DEFAULT_CHARSET);
    printf("Determining what Position Item0 is in....\n");
    if (position = XmListItemPos(List1, tcs))
    	printf("    - Item0 is found in Position %d\n\n", position);
    else
	printf("    - Item0 could not be found in the List\n\n");
    XmStringFree(tcs);

    fflush(stdout);
 
    CommonPause();

    /* 
       Add another Item1 to the List
    */

    Istrings[item_count - 1] = "Item1";
    tcs = XmStringLtoRCreate("Item1", XmSTRING_DEFAULT_CHARSET);
    XmListAddItem(List1, tcs, 0);

    CommonPause();

    /* 
       Testing XmListGetMatchPos() with valid and invalid input 
    */

    printf("Determining what Positions Item1 is in....\n");
    if (XmListGetMatchPos(List1, tcs, &position_list, &position_count))
    {
	printf("    - Item1 was found in %d slots ( slots ", position_count); 
	for (i = 0 ; i < position_count; i++)
		printf("%d, ", position_list[i]);
	printf(")\n");
    }
    else
	printf("    - No match for Item1 in the List\n");

    fflush(stdout);
	
    tcs = XmStringLtoRCreate("Item10", XmSTRING_DEFAULT_CHARSET);
    printf("Determining what Positions Item10 is in....\n");
    if (XmListGetMatchPos(List1, tcs, &position_list, &position_count))
    {
	printf("    - Item10 is found in %d slots ( slots ", position_count); 
	for (i = 0 ; i < position_count; i++)
		printf("%d, ", position_list[i]);
	printf(")\n\n");
    }
    else
	printf("    - No match for Item10 in the List\n\n");

    fflush(stdout);
    XmStringFree(tcs);

    CommonPause();

    /*
       Testing XmListGetSelectedPos() with valid and invalid input 
    */

    position_count = 0;
    printf("Determining what Items are Selected....\n");
    if (XmListGetSelectedPos(List1, &position_list, &position_count))
    {
	printf("    - %d items found to be selected.\n", position_count);
	printf("    - The following items have been selected:\n");
	for (i = 0 ; i < position_count; i++)
		printf("      %s\n", Istrings[position_list[i] - 1]);
    }
    else
	printf("    - No Items were found to be selected\n");

    fflush(stdout);
    	
    CommonPause();

    printf("\n");
    position_count = 0;
    printf("Determining what Items are Selected....\n");
    if (XmListGetSelectedPos(List1, &position_list, &position_count))
    {
	printf("    - %d items found to be selected.\n", position_count);
	printf("    - The following items have been selected:\n");
	for (i = 0 ; i < position_count; i++)
		printf("      %s\n", Istrings[position_list[i] - 1]);
    }
    else
	printf("    - No Items were found to be selected\n");
    	
    fflush(stdout);
    printf("\n");

    CommonPause();

    XmListDeselectAllItems(List1);
    XtAddCallback(List1, XmNmultipleSelectionCallback, ExtOrMulSelCB, NULL);

    CommonPause();

    XmListDeselectAllItems(List1);

    n = 0;
    XtSetArg(args[n], XmNselectionPolicy, XmEXTENDED_SELECT);  	n++;
    XtSetValues(List1, args, n);

    XtAddCallback(List1, XmNextendedSelectionCallback, ExtOrMulSelCB, NULL);

    CommonPause();

    n = 0;
    XtSetArg(args[n], XmNtopItemPosition, 2);			n++;
    XtSetValues(List1, args, n);

    CommonPause();

    /* Test for Pir 2798 and 3705 */

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_NONE);		n++;
    XtSetValues(List1, args, n);

    tcs = XmStringCreateSimple("Push To Delete Selected Item");

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    Push1 = XmCreatePushButton(Form, "Push1", args, n);
    XtManageChild(Push1);

    XtAddCallback(Push1, XmNactivateCallback, DeleteSelected, NULL);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNtopWidget, Push1);			n++;
    XtSetArg(args[n], XmNselectionPolicy, XmBROWSE_SELECT);	n++;
    XtSetArg(args[n], XmNvisibleItemCount, NITEMS);		n++;
    XtSetValues(List1, args, n);

    CommonPause();

    /* End Test for Pir 2798 and 3705 */

    XtAppMainLoop(app_context);
}


static void ExtOrMulSelCB(Widget w, XtPointer client_data,
				    XtPointer call_data)

{
    int *selectedItemPos;
    XmString *selectedItems;
    int i, num_selected = 0;

    selectedItemPos = 
		((XmListCallbackStruct*)call_data)->selected_item_positions;
    selectedItems = 
		((XmListCallbackStruct*)call_data)->selected_items;
    num_selected = ((XmListCallbackStruct*)call_data)->selected_item_count;
    printf("%d items found to be selected.\n", num_selected);
    printf("The following items (with there positions) have been selected:\n");
    for (i = 0 ; i < num_selected; i++)
    	printf("%s - position #%d\n", CommonCsToRs(selectedItems[i]),
				      selectedItemPos[i]);
    printf("\n");
}


static void DeleteSelected(Widget w, XtPointer client_data,
				     XtPointer call_data)

{
  Arg args[10];
  XmStringTable pos_tab;
  int n, pos_count;

  n = 0;
  XtSetArg(args[n], XmNselectedItemCount, &pos_count); n++;
  XtSetArg(args[n], XmNselectedItems, &pos_tab); n++;
  XtGetValues(List1, args, n);

  if (pos_count == 0) return;

  XmListDeleteItems(List1, pos_tab, pos_count);

  XmProcessTraversal(List1, XmTRAVERSE_CURRENT);

}
