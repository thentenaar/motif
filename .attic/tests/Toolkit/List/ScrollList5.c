/* $XConsortium: ScrollList5.c /main/5 1995/07/15 21:05:42 drk $ */
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

#include <testlib.h>
#include "List9.h"

static void SelCB(Widget w, XtPointer client_data, XtPointer call_data)

{
    char **text;
    XmString tcs;
    
    printf("Type of Selection was ");
    switch (((XmListCallbackStruct*)call_data)->auto_selection_type) {
	case XmAUTO_BEGIN   : printf("Auto Begin\n");
			      break;
	case XmAUTO_MOTION  : printf("Auto Motion\n");
			      break;
	case XmAUTO_CANCEL  : printf("Auto Cancel\n");
			      break;
	case XmAUTO_NO_CHANGE : printf("Auto No Change\n");
			      break;
	case XmAUTO_CHANGE  : printf("Auto Change\n");
			      break;
	default             : printf("not defined?\n");
    }
}


void main( argc, argv )
int	argc;
char	**argv;
{
    register int       	n;
    Arg                	args[MAX_ARGS];
    XmString		ItemList[NITEMS2];
    unsigned int*	positions;
    unsigned int*       tmp_pos;
    int			num_positions;
    int			i;

    CommonTestInit( argc, argv );

    n = 0;
    XtSetArg(args[n], XmNwidth,  100);  		n++;
    XtSetArg(args[n], XmNheight, 100);  		n++;
    XtSetValues(Shell1, args, n);

    /* convert the plain strings to fancy Xstrings */
    for (n = 0; n < NITEMS2; n++)
	ItemList[n] = XmStringLtoRCreate(Istrings2[n],
			  XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNlistSizePolicy, XmVARIABLE);  		n++;
    XtSetArg(args[n], XmNitems, ItemList); 			n++;
    XtSetArg(args[n], XmNitemCount, NITEMS2); 			n++;
    XtSetArg(args[n], XmNselectionMode, XmNORMAL_MODE);		n++;
    XtSetArg(args[n], XmNselectionPolicy, XmEXTENDED_SELECT);	n++;
    XtSetArg(args[n], XmNlistSpacing, 5);  			n++;
    XtSetArg(args[n], XmNlistMarginWidth, 2);  			n++;
    XtSetArg(args[n], XmNlistMarginHeight, 2);  		n++;
    List1 = XmCreateScrolledList(Shell1, "List1", args, n);
    XtManageChild(List1);

    XtRealizeWidget(Shell1);

    /* Select some items from the list. */

    CommonPause();

    /* Now we get the list of selected item positions and deselect
       each of them. */

    n = 0;
    XtSetArg(args[n], XmNselectedPositions, &positions);	n++;
    XtSetArg(args[n], XmNselectedItemCount, &num_positions);	n++;

    XtGetValues(List1, args, n);

    /* We have to go through this because positions points to the
     * actual list in the widget.  And as each item is deselected the
     * list gets smaller or potentially reallocated inside the widget.
     */
    tmp_pos = (unsigned int*)malloc(num_positions * sizeof(unsigned int));
    memcpy(tmp_pos, positions, num_positions * sizeof(unsigned int));

    for (i = 0; i < num_positions; i++)
	XmListDeselectPos(List1, tmp_pos[i]);

    free(tmp_pos);

    CommonPause();

    /* Clean up if need be. */
    XmListDeselectAllItems(List1);

    /* Select some items from the list using the keyboard. */

    CommonPause();

    /* Now we get the list of selected item positions and deselect
       each of them. */

    n = 0;
    XtSetArg(args[n], XmNselectedPositions, &positions);	n++;
    XtSetArg(args[n], XmNselectedItemCount, &num_positions);	n++;
    XtGetValues(List1, args, n);

    /* We have to go through this because positions points to the
     * actual list in the widget.  And as each item is deselected the
     * list gets smaller or potentially reallocated inside the widget.
     */
    tmp_pos = (unsigned int*)malloc(num_positions * sizeof(unsigned int));
    memcpy(tmp_pos, positions, num_positions * sizeof(unsigned int));

    for (i = 0; i < num_positions; i++)
	XmListDeselectPos(List1, tmp_pos[i]);

    free(tmp_pos);

    CommonPause();

    /* Clean up if need be. */
    XmListDeselectAllItems(List1);

    /* Use XmADD_MODE */

    n = 0;
    XtSetArg(args[n], XmNselectionMode, XmADD_MODE);		n++;
    XtSetValues(List1, args, n);

    /* Select some items from the list using the keyboard. */
    CommonPause();

    /* Now we get the list of selected item positions and deselect
       each of them. */

    n = 0;
    XtSetArg(args[n], XmNselectedPositions, &positions);	n++;
    XtSetArg(args[n], XmNselectedItemCount, &num_positions);	n++;
    XtGetValues(List1, args, n);

    /* We have to go through this because positions points to the
     * actual list in the widget.  And as each item is deselected the
     * list gets smaller or potentially reallocated inside the widget.
     */
    tmp_pos = (unsigned int*)malloc(num_positions * sizeof(unsigned int));
    memcpy(tmp_pos, positions, num_positions * sizeof(unsigned int));

    for (i = 0; i < num_positions; i++)
	XmListDeselectPos(List1, tmp_pos[i]);

    CommonPause();

    free(tmp_pos);

    /* Clean up if need be. */
    XmListDeselectAllItems(List1);

    /* Test auto_selection_type callback struct member when
       XmNautomaticSelectionMode is True and selectionMode
       is BROWSE_SELECT. */

    n = 0;
    XtSetArg(args[n], XmNautomaticSelection, True);		n++;
    XtSetArg(args[n], XmNselectionPolicy, XmBROWSE_SELECT);	n++;
    XtSetValues(List1, args, n);
    
    XtAddCallback(List1, XmNbrowseSelectionCallback, SelCB, NULL);

    CommonPause();

    /* Clean up if need be. */
    XmListDeselectAllItems(List1);

    /* Test auto_selection_type callback struct member when
       XmNautomaticSelectionMode is True and selectionMode
       is EXTENDED_SELECT. */

    n = 0;
    XtSetArg(args[n], XmNselectionPolicy, XmEXTENDED_SELECT);	n++;
    XtSetValues(List1, args, n);
    
    XtRemoveAllCallbacks(List1, XmNbrowseSelectionCallback);
    XtAddCallback(List1, XmNextendedSelectionCallback, SelCB, NULL);

    CommonPause();

    /* Clean up if need be. */
    XmListDeselectAllItems(List1);

    /* Test XmNprimaryOwnership resource. */

    n = 0;
    XtSetArg(args[n], XmNselectionPolicy, XmSINGLE_SELECT);	n++;
    XtSetArg(args[n], XmNprimaryOwnership, XmOWN_NEVER);	n++;
    XtSetValues(List1, args, n);
    
    XtRemoveAllCallbacks(List1, XmNextendedSelectionCallback);

    /* Select an item. */

    CommonPause();

    /* List1 should not be the owner of the primary selection. */

    if (List1 == XtWindowToWidget(display,
				  XGetSelectionOwner(display, XA_PRIMARY)))
	printf("FAILED: The list should not own the primary selection but does.\n");
    else
	printf("PASSED: The list does not own the primary selection.\n");

    CommonPause();

    /* Clean up if need be. */
    XtDisownSelection(List1, XA_PRIMARY, CurrentTime);
    XFlush(display);
    XSync(display, False);
    XmListDeselectAllItems(List1);

    n = 0;
    XtSetArg(args[n], XmNprimaryOwnership, XmOWN_ALWAYS);	n++;
    XtSetValues(List1, args, n);
    
    /* Select an item. */

    CommonPause();

    /* List1 should be the owner of the primary selection. */

    if (List1 == XtWindowToWidget(display,
				  XGetSelectionOwner(display, XA_PRIMARY)))
	printf("PASSED: The list owns the primary selection.\n");
    else
	printf("FAILED: The list should own the primary selection, but doesn't.\n");

    CommonPause();

    /* Clean up if need be. */
    XtDisownSelection(List1, XA_PRIMARY, CurrentTime);
    XFlush(display);
    XSync(display, False);
    XmListDeselectAllItems(List1);

    n = 0;
    XtSetArg(args[n], XmNprimaryOwnership, XmOWN_MULTIPLE);	n++;
    XtSetValues(List1, args, n);

    /* Select an item. */

    CommonPause();

    /* List1 should not be the owner of the primary selection. */

    if (List1 == XtWindowToWidget(display,
				  XGetSelectionOwner(display, XA_PRIMARY)))
	printf("FAILED: The list should not own the primary selection but does.\n");
    else
	printf("PASSED: The list does not own the primary selection.\n");

    CommonPause();

    /* Clean up if need be. */
    XtDisownSelection(List1, XA_PRIMARY, CurrentTime);
    XFlush(display);
    XSync(display, False);
    XmListDeselectAllItems(List1);

    n = 0;
    XtSetArg(args[n], XmNprimaryOwnership, XmOWN_POSSIBLE_MULTIPLE);	n++;
    XtSetValues(List1, args, n);

    /* Select an item. */

    CommonPause();

    /* List1 should not be the owner of the primary selection. */

    if (List1 == XtWindowToWidget(display,
				  XGetSelectionOwner(display, XA_PRIMARY)))
	printf("FAILED: The list should not own the primary selection but does.\n");
    else
	printf("PASSED: The list does not own the primary selection.\n");

    CommonPause();

    /* Clean up if need be. */
    XtDisownSelection(List1, XA_PRIMARY, CurrentTime);
    XFlush(display);
    XSync(display, False);
    XmListDeselectAllItems(List1);

    n = 0;
    XtSetArg(args[n], XmNselectionPolicy, XmEXTENDED_SELECT);	n++;
    XtSetArg(args[n], XmNprimaryOwnership, XmOWN_NEVER);	n++;
    XtSetValues(List1, args, n);
    
    /* Select some items. */

    CommonPause();

    /* List1 should not be the owner of the primary selection. */

    if (List1 == XtWindowToWidget(display,
				  XGetSelectionOwner(display, XA_PRIMARY)))
	printf("FAILED: The list owns the primary selection, but it shouldn't.\n");
    else
	printf("PASSED: The list does not own the primary selection.\n");

    CommonPause();

    /* Clean up if need be. */
    XtDisownSelection(List1, XA_PRIMARY, CurrentTime);
    XFlush(display);
    XSync(display, False);
    XmListDeselectAllItems(List1);

    n = 0;
    XtSetArg(args[n], XmNprimaryOwnership, XmOWN_ALWAYS);	n++;
    XtSetValues(List1, args, n);
    
    /* Select some items. */

    CommonPause();

    /* List1 should be the owner of the primary selection. */

    if (List1 == XtWindowToWidget(display,
				  XGetSelectionOwner(display, XA_PRIMARY)))
	printf("PASSED: The list owns the primary selection.\n");
    else
	printf("FAILED: The list should own the primary selection, but doesn't.\n");

    CommonPause();

    /* Clean up if need be. */
    XtDisownSelection(List1, XA_PRIMARY, CurrentTime);
    XFlush(display);
    XSync(display, False);
    XmListDeselectAllItems(List1);

    n = 0;
    XtSetArg(args[n], XmNprimaryOwnership, XmOWN_MULTIPLE);	n++;
    XtSetValues(List1, args, n);
    
    /* Panel 21		*/
    /* Select one item */

    CommonPause();

    /* List1 shouldn't be the owner of the primary selection. */

    if (List1 == XtWindowToWidget(display,
				  XGetSelectionOwner(display, XA_PRIMARY)))
	printf("FAILED: The list owns the primary selection, but it shouldn't.\n");
    else
	printf("PASSED: The list does not own the primary selection.\n");

    CommonPause();

    /* Clean up if need be. */
    XtDisownSelection(List1, XA_PRIMARY, CurrentTime);
    XFlush(display);
    XSync(display, False);
    XmListDeselectAllItems(List1);

    n = 0;
    XtSetArg(args[n], XmNprimaryOwnership, XmOWN_POSSIBLE_MULTIPLE);	n++;
    XtSetValues(List1, args, n);
    
    /* Select some items. */

    CommonPause();

    /* List1 should be the owner of the primary selection. */

    if (List1 == XtWindowToWidget(display,
              XGetSelectionOwner(display, XA_PRIMARY)))
   printf("PASSED: The list owns the primary selection.\n");
	else
   printf("FAILED: The list should own the primary selection, but doesn't.\n");

    CommonPause();

    /* Clean up if need be. */
    XtDisownSelection(List1, XA_PRIMARY, CurrentTime);
    XFlush(display);
    XSync(display, False);
    XmListDeselectAllItems(List1);

    n = 0;
    XtSetArg(args[n], XmNprimaryOwnership, XmOWN_POSSIBLE_MULTIPLE); n++;
    XtSetValues(List1, args, n);

    /* Select some items. */

    CommonPause();

    /* List1 should be the owner of the primary selection. */

    if (List1 == XtWindowToWidget(display,
				  XGetSelectionOwner(display, XA_PRIMARY)))
	printf("PASSED: The list owns the primary selection.\n");
    else
	printf("FAILED: The list should own the primary selection, but doesn't.\n");
    
    CommonPause();

    /* Clean up if need be. */
    XtDisownSelection(List1, XA_PRIMARY, CurrentTime);
    XFlush(display);
    XSync(display, False);
    XmListDeselectAllItems(List1);

    n = 0;
    XtSetArg(args[n], XmNprimaryOwnership, XmOWN_POSSIBLE_MULTIPLE); n++;
    XtSetValues(List1, args, n);

    /* Select nothing. */

    CommonPause();

    /* List1 should be the owner of the primary selection. */

    if (List1 == XtWindowToWidget(display,
              XGetSelectionOwner(display, XA_PRIMARY)))
	printf("FAILED: The list owns the primary selection, but it shouldn't.\n");
    else
	printf("PASSED: The list does not own the primary selection.\n");

    CommonPause();

    XtAppMainLoop(app_context);
}
