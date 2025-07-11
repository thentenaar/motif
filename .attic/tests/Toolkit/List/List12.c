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
static char rcsid[] = "$XConsortium: List12.c /main/5 1995/07/13 18:16:08 drk $"
#endif
#endif
  
#include <testlib.h>

#define NITEMS 8

char *Istrings[] = {
    "Item1",  "Item2",
    "Item3",  "Item4",
    "Item5",  "Item6",
    "Item6",  "Item8"
};


static void PushButtonCB(Widget w, Widget l,
			 XtPointer call_data)
{
  int num;
  int* sel;
  int i;
  
  if (!XmListGetSelectedPos(l, &sel, &num))
    return;
  for (i = 0; i < num; i++)
    /* correct for having deleted some already */
    XmListDeletePos(l, sel[i] - i);
}

main(argc, argv)
     int    argc;
     char **argv;
{
  register int n;
  Arg args[MAX_ARGS];
  XmString ItemList[NITEMS];
  Widget RC, List1, PushButton;
  
  CommonTestInit(argc, argv);

  n = 0;
  XtSetArg(args[n], XmNwidth,  350);  		n++;
  XtSetArg(args[n], XmNheight, 300);  		n++;
  XtSetArg(args[n], XmNallowShellResize, TRUE); n++;
  XtSetValues(Shell1, args, n);
  XtRealizeWidget(Shell1);

  n = 0;
  RC = XmCreateRowColumn(Shell1, "RC", args, n);
  XtManageChild (RC);

  for (n = 0; n < NITEMS; n++)
    ItemList[n] = XmStringLtoRCreate(Istrings[n],
				     XmSTRING_DEFAULT_CHARSET);

  n = 0;
  XtSetArg(args[n], XmNlistSpacing, 5);  			n++;
  XtSetArg(args[n], XmNlistMarginWidth, 2);  			n++;
  XtSetArg(args[n], XmNlistMarginHeight, 2);  		        n++;
  XtSetArg(args[n], XmNlistSizePolicy, XmVARIABLE);  		n++;
  XtSetArg(args[n], XmNitems, ItemList); 			n++;
  XtSetArg(args[n], XmNitemCount, NITEMS); 			n++;
  XtSetArg(args[n], XmNvisibleItemCount, NITEMS); 		n++;
  XtSetArg(args[n], XmNselectionPolicy, XmEXTENDED_SELECT);     n++;
  XtSetArg(args[n], XmNbackground,
	   WhitePixel(display,DefaultScreen(display))); 	n++;
  XtSetArg(args[n], XmNforeground,
	   BlackPixel(display,DefaultScreen(display))); 	n++;
  List1 = XmCreateList(RC, "List1", args, n);
  XtManageChild(List1);

  n = 0;

  PushButton = XmCreatePushButton (RC, "PushButton", args, n);
  XtManageChild(PushButton);

  XtAddCallback(PushButton, XmNactivateCallback, 
		(XtCallbackProc) PushButtonCB, List1);

  CommonPause();
  CommonPause();  /* Exit */

  XtAppMainLoop(app_context);
}
