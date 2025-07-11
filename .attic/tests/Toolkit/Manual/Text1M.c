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
static char rcsid[] = "$XConsortium: Text1M.c /main/6 1995/07/13 18:41:26 drk $"
#endif
#endif


#include <testlib.h>

void main (argc, argv)
int argc;
char **argv;
{
   Widget PanedWindow1;
   Widget Text1;
   Pixel red_pixel;
   Arg args[MAX_ARGS];
   int n;

   /*  initialize toolkit  */
   CommonTestInit(argc, argv);
    
   n = 0;
   PanedWindow1 = XmCreatePanedWindow(Shell1, "PanedWindow1", args, n);
   XtManageChild(PanedWindow1);

   red_pixel = CommonGetColor("red");

   n = 0;
   XtSetArg(args[n], XmNhighlightThickness, 2); n++;
   XtSetArg(args[n], XmNhighlightColor, red_pixel); n++;
   XtSetArg(args[n], XmNallowResize, True); n++;
   XtSetArg(args[n], XmNblinkRate, 0);  n++;
   XtSetArg(args[n], XmNresizeWidth, True); n++;
   XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
   XtSetArg(args[n], XmNrows, 5); n++;
   XtSetArg(args[n], XmNvalue, "One fish\nTwo fish\nRed fish\nBlue fish"); n++;
   XtSetArg(args[n], XmNcursorPosition, 5); n++;
   Text1 = XmCreateText(PanedWindow1, "Text1", args, n);
   XtManageChild(Text1);

   XtRealizeWidget(Shell1);

   CommonPause();              /* tab groups, losingFocus, test for PIR 3143 */

   XtAppMainLoop(app_context);
}
