/* $XConsortium: ListM1.c /main/4 1995/07/15 21:06:03 drk $ */
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


#define NITEMS 9

char *Istrings[] = {
    "DrawnButton", 	"Label",
    "List",		"PushButton",
    "Separator",	"ArrowButton",
    "ScrollBar", 	"Text",
    "ToggleButton"
};

Widget    List1;



void  main (argc, argv)
    int     argc;
    char  **argv;
{
    register int       	n;
    Arg                	args[MAX_ARGS];
    XmString           	tcs, ItemList[NITEMS],
		       	new_items[2], old_items[2],
    	    		*selected_items;
    XmString		new_items_2[15];
    int		       	selected_item_count;
    int			i;
    int			*position_list, position_count;
    Boolean		selected;

    CommonTestInit(argc, argv);
    
    n = 0;
    XtSetArg(args[n], XmNwidth,  350);  		n++;
    XtSetArg(args[n], XmNheight, 300);  		n++;
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
    XtSetArg(args[n], XmNbackground,
    		WhitePixel(display,DefaultScreen(display))); 	n++;
    XtSetArg(args[n], XmNforeground,
    		BlackPixel(display,DefaultScreen(display))); 	n++;
	XtSetArg(args[n], XmNhighlightColor, CommonGetColor("green")); n++;
    List1 = XmCreateList(Shell1, "List1", args, n);
    XtManageChild(List1);

    CommonPause();

	/* Begin Test for CR5099 */

	n = 0;
	XtSetArg(args[n], XmNhighlightColor, CommonGetColor("red")); n++;
	XtSetValues(List1, args, n);

    CommonPause();

	/* End Test for CR5099 */

    XtAppMainLoop(app_context);
}
