/* $XConsortium: PComboBox1.c /main/5 1995/07/15 21:00:55 drk $ */
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
 * 
 */
/*
 * HISTORY
 */
#include <testlib.h>

#define VITEMS 		20
#define MAX_ITEMS 	20000
#define DEFAULT_ITEMS	5000

/* Global Variables */
Widget		ComboBox1;
int		nitems;
XmString	add_item;


void  
main (int argc, char **argv)
{
    register int	n;
    Arg			args[MAX_ARGS];
    char		Item[15];
    XmString		ItemList[MAX_ITEMS];

    CommonTestInit(argc, argv);

    if (UserData)  {
	nitems = atoi(UserData);
    }
    else
	nitems = DEFAULT_ITEMS;

    n = 0;
    XtSetArg(args[n], XmNwidth,  150);  			n++;
    XtSetArg(args[n], XmNheight, 300);  			n++;
    XtSetArg(args[n], XtNgeometry, "+0+0");  			n++;
    XtSetValues(Shell1, args, n);
    
    XtRealizeWidget(Shell1);

    /* 
     * generate items for list and 
     * convert the plain strings to Xmstrings 
     */
    for (n = 0; n < nitems; n++)  {
	sprintf(Item, "%s%d", "item", n);
	ItemList[n] = XmStringCreate(Item,
			  XmFONTLIST_DEFAULT_TAG);
    }

    add_item = XmStringCreate ("Added Item", XmFONTLIST_DEFAULT_TAG);

    CommonPause();

    /* now create a list with nitems and see performance */
    n = 0;
    XtSetArg(args[n], XmNlistSpacing, 5);  			n++;
    XtSetArg(args[n], XmNlistMarginWidth, 2);  			n++;
    XtSetArg(args[n], XmNlistMarginHeight, 2);  		n++;
    XtSetArg(args[n], XmNlistSizePolicy, XmVARIABLE);  		n++;
    XtSetArg(args[n], XmNitems, ItemList); 			n++;
    XtSetArg(args[n], XmNitemCount, nitems); 			n++;
    XtSetArg(args[n], XmNvisibleItemCount, VITEMS); 		n++;
    ComboBox1 = XmCreateComboBox(Shell1, "ComboBox1", args, n);
    XtManageChild(ComboBox1);

    CommonPause();

    /* 
     * test performance in .scr file by scrolling from top to bottom
     * and bottom to top
     */

    CommonPause();

    /* 
     * test the performance of adding one 
     * item to the botton when:
     *	- top is visible
     */
    XmListAddItem (XtNameToWidget (ComboBox1, "*List"), add_item, nitems+1);

    CommonPause();

    /* 
     * test the performance of adding one 
     * item to the botton when:
     *	- bottom is visible
     */
    XmListAddItem (XtNameToWidget (ComboBox1, "*List"), add_item, nitems+1);

    CommonPause();

    /* 
     * test the performance of deleting one 
     * item from the bottom when:
     *	- top is visible
     */

    XmListDeleteItemsPos(XtNameToWidget (ComboBox1, "*List"), 1, nitems+2);

    CommonPause();

    /* 
     * test the performance of deleting one
     * item from the bottom when:
     * - bottom is visible
     */

    XmListDeleteItemsPos (XtNameToWidget (ComboBox1, "*List"), 1, nitems+1);

    CommonPause();


    XtAppMainLoop(app_context);
}


