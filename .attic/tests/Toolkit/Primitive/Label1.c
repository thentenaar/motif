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
static char rcsid[] = "$XConsortium: Label1.c /main/7 1995/07/13 19:08:18 drk $"
#endif
#endif

/****************************************
*   Automatic test program generation   *
*              Version 1.0              *
*       Fri Mar 31 10:55:08 1989        *
****************************************/

#include <testlib.h>

#include "gnu.bmp"
Pixmap gnumap;

/*  Global Variables  */
Widget    RowColumn1;
Widget    Label1;
Widget    Separator1;
Widget    Label2;

void  main (argc, argv)
int     argc;
char  **argv;
{
  Boolean            trace = False;
  register int       n;
  Arg                args[MAX_ARGS];
  XmString           tcs;
  XEvent             DummyEvent;

  CommonTestInit(argc, argv);
    
  n = 0;
  XtSetArg(args[n], XmNwidth,  400);  n++;
  XtSetArg(args[n], XmNheight, 300);  n++;
  XtSetValues(Shell1, args, n);
    
  XtRealizeWidget(Shell1);

  /* create and realize a rowcolumn container widget */
  n = 0;
  XtSetArg(args[n], XmNisAligned, FALSE);  n++;
  RowColumn1 = XmCreateRowColumn(Shell1, "RowColumn1", args, n);
  XtManageChild(RowColumn1);

  /* make a string label widget */
  n = 0;
  tcs = XmStringLtoRCreate("Multi-Line\nLabel String\n\n\n\nwith extra newlines",
			   XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs);  n++;
  XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
  XtSetArg(args[n], XmNrecomputeSize, TRUE); n++;
  Label1 = XmCreateLabel(RowColumn1, "Label1", args, n);
  XtManageChild(Label1);

  /* add a separator */
  n = 0;
  Separator1 = XmCreateSeparator(RowColumn1, "Separator1", args, n);
  XtManageChild(Separator1);
  
  /* stick in a pixmap label widget. 1st create the pixmap */
  gnumap = 
    XCreatePixmapFromBitmapData(display,
				DefaultRootWindow(display),
				gnu_bits, gnu_width, gnu_height,
				WhitePixel(display,DefaultScreen(display)),
				BlackPixel(display,DefaultScreen(display)),
				DefaultDepth(display,DefaultScreen(display)));

  n = 0;
  XtSetArg(args[n], XmNheight, 54);  n++;
  XtSetArg(args[n], XmNlabelType, XmPIXMAP);  n++;
  XtSetArg(args[n], XmNrecomputeSize, TRUE); n++;
  XtSetArg(args[n], XmNlabelPixmap, gnumap);  n++;
  XtSetArg(args[n], XmNlabelString, tcs);  n++;
  Label2 = XmCreateLabel(RowColumn1, "Label2", args, n);
  XtManageChild(Label2);
  XmStringFree(tcs);

  /* 
   * try to force contained widget to change size - do a SetValues
   * without changing anything
   */
  
  n = 0;
  XtSetValues(Label2,args,n);

  CommonPause();
  CommonPause(); 
  CommonPause();

  XtDestroyWidget(Label2);
  XtDestroyWidget(Label1);
  XtDestroyWidget(Separator1);
  XtDestroyWidget(RowColumn1);
  CommonPause();

  XtAppMainLoop(app_context);
}

















