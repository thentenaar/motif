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
static char rcsid[] = "$XConsortium: Label6.c /main/8 1995/07/13 19:09:18 drk $"
#endif
#endif
/*
 *  Create labels with and without font to test the correct search
 *  path for default fonts.
 */

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include <testlib.h>

void main (argc, argv)
unsigned int argc;
char **argv;

{ 	
  Widget separator, menushell, bboard, bboard2, rowcolumn1, rowcolumn2;
  Widget rowcolumn3, label[15];
  Arg args[10];
  int j = 0, n;
  XmString tcs;
  XFontStruct *fnt;
  XmFontList  font1, font2, font3, font4;
  XmStringCharSet  cs = (char *) XmSTRING_DEFAULT_CHARSET;

  extern void post_menu_handler();

  /*  initialize toolkit  */
  CommonTestInit(argc, argv);

  font1 = CommonGetFontList("6x10");
  font2 = CommonGetFontList("8x13bold");
  font3 = CommonGetFontList("9x15bold");
  font4 = CommonGetFontList("8x13");

  n = 0;
  XtSetArg (args[n], XmNheight, 100 ); n++;
  XtSetArg (args[n], XmNwidth, 100 ); n++;
  bboard = XmCreateBulletinBoard(Shell1, "bboard", args, n);
  XtManageChild (bboard);

  n = 0;
  XtSetArg (args[n], XmNheight, 100 ); n++;
  XtSetArg (args[n], XmNwidth, 100 ); n++;
  XtSetArg (args[n], XmNdefaultFontList, font1);   n++;
  XtSetArg (args[n], XmNx, 300 ); n++; 
  XtSetArg (args[n], XmNy, 300 ); n++;  
  menushell= XmCreateMenuShell (bboard, "menushell", args, n);
  XtAddEventHandler(bboard, ButtonPressMask, FALSE, post_menu_handler,
		    menushell);

  n = 0;
  XtSetArg (args[n], XmNheight, 100 ); n++;
  XtSetArg (args[n], XmNwidth, 100 ); n++;
  XtSetArg (args[n], XmNx, 10 ); n++; 
  XtSetArg (args[n], XmNy, 50 ); n++;  
  rowcolumn1 = XmCreateRowColumn(bboard, "rowcolumn1", args, n);
  XtManageChild (rowcolumn1);

  n = 0;
  XtSetArg (args[n], XmNheight, 100 ); n++;
  XtSetArg (args[n], XmNwidth, 100 ); n++;
  rowcolumn2 = XmCreateRowColumn(menushell, "rowcolumn2", args, n);
  XtManageChild (rowcolumn2);

  /* labels in the menushell */

  n = 0;
  tcs = XmStringLtoRCreate("6x10", XmSTRING_DEFAULT_CHARSET);
  XtSetArg (args[n], XmNlabelString, tcs);	n++;
  label[0] = XmCreateLabel (rowcolumn2, "label[0]", args, n);
  XtManageChild (label[0]);
  XmStringFree (tcs);

  n = 0;
  tcs = XmStringLtoRCreate("8x13", XmSTRING_DEFAULT_CHARSET);
  XtSetArg (args[n], XmNlabelString, tcs);	n++;
  XtSetArg (args[n], XmNfontList, font4);      n++;
  label[1] = XmCreateLabel (rowcolumn2, "label[1]", args, n);
  XtManageChild (label[1]);
  XmStringFree (tcs);

  /* change fonts in the bulletinboard */
  n=0;
  XtSetArg (args[n], XmNseparatorType, XmSHADOW_ETCHED_OUT);	n++;
  separator = XmCreateSeparatorGadget (rowcolumn1, "separator", args, n);
  XtManageChild (separator);

  n = 0;
  tcs = XmStringLtoRCreate("fixed", XmSTRING_DEFAULT_CHARSET);
  XtSetArg (args[n], XmNlabelString, tcs);	n++;
  label[2] = XmCreateLabel (rowcolumn1, "label[2]", args, n);
  XtManageChild (label[2]);
  XmStringFree(tcs);

  n = 0;
  tcs = XmStringLtoRCreate("8x13", XmSTRING_DEFAULT_CHARSET);
  XtSetArg (args[n], XmNlabelString, tcs);	n++;
  XtSetArg (args[n], XmNfontList, font4);      n++;
  label[3] = XmCreateLabel (rowcolumn1, "label[3]", args, n);
  XtManageChild (label[3]);
  XmStringFree(tcs);

  n = 0;
  XtSetArg(args[n], XmNlabelFontList, font3); n++;
  XtSetValues(bboard, args, n);

  n = 0;
  tcs = XmStringLtoRCreate("9x15bold", XmSTRING_DEFAULT_CHARSET);
  XtSetArg (args[n], XmNlabelString, tcs);	n++;
  label[4] = XmCreateLabel (rowcolumn1, "label[4]", args, n);
  XtManageChild (label[4]);
  XmStringFree(tcs);

  /* now try changing in another level of bulletin-board */

  n = 0;
  XtSetArg(args[n], XmNheight, 100 ); n++;
  XtSetArg(args[n], XmNwidth, 100 ); n++;
  bboard2 = XmCreateBulletinBoard(rowcolumn1, "bboard2", args, n);
  XtManageChild(bboard2);
    
  n = 0;
  XtSetArg(args[n], XmNheight, 100 ); n++;
  XtSetArg(args[n], XmNwidth, 100 ); n++;
  rowcolumn3 = XmCreateRowColumn(bboard2, "rowcolumn3", args, n);
  XtManageChild(rowcolumn3);

  n = 0;
  tcs = XmStringLtoRCreate("9x15bold", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs);	n++;
  label[5] = XmCreateLabel (rowcolumn3, "label[5]", args, n);
  XtManageChild(label[5]);
  XmStringFree(tcs);

  n = 0;
  tcs = XmStringLtoRCreate("8x13", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs);	n++;
  XtSetArg(args[n], XmNfontList, font4);      n++;
  label[6] = XmCreateLabel (rowcolumn3, "label[6]", args, n);
  XtManageChild(label[6]);
  XmStringFree(tcs);

  n = 0;
  XtSetArg(args[n], XmNlabelFontList, font2); n++;
  XtSetValues(bboard2, args, n);

  n = 0;
  tcs = XmStringLtoRCreate("8x13bold", XmSTRING_DEFAULT_CHARSET);
  XtSetArg (args[n], XmNlabelString, tcs);	n++;
  label[7] = XmCreateLabel (rowcolumn3, "label[7]", args, n);
  XtManageChild(label[7]);
  XmStringFree(tcs);
  XtRealizeWidget(Shell1);

  CommonPause();

  for (j = 0; j <=6; j++)
    {
      XtDestroyWidget(label[j]);
    }
  XtDestroyWidget(separator);
  XtDestroyWidget(rowcolumn3);
  XtDestroyWidget(bboard2);
  XtDestroyWidget(rowcolumn1);
  XtDestroyWidget(rowcolumn2);
  XtDestroyWidget(menushell);
  XtDestroyWidget(bboard);
  CommonPause();

  XtAppMainLoop(app_context);
}


void post_menu_handler (w, menu, event)
Widget w;
Widget menu;
XEvent *event;

{
   XmMenuPosition (menu, (XButtonPressedEvent *)event);
   XtManageChild(menu);
 }









