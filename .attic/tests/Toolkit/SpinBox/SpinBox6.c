/* $XConsortium: SpinBox6.c /main/5 1995/07/15 21:10:08 drk $ */
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

#include "testlib.h"

void
main(int argc, char **argv)
{

  Arg args[10];
  int n;
  Widget Frame, SpinBox, TextF;

		     
  

        CommonTestInit(argc,argv);

	Frame = XmCreateFrame(Shell1, "Frame", NULL, 0);
	XtManageChild (Frame);


        n = 0;
	XtSetArg( args[n], XmNwidth, 200 ); n++;
	XtSetArg( args[n], XmNheight, 100 ); n++;
	SpinBox  = XmCreateSpinBox(Frame, "SpinBox", args, n);
        XtManageChild (SpinBox);

        n = 0;
	TextF  = XmCreateTextField(SpinBox, "TextF", args, n);
        XtManageChild( TextF );

        XtRealizeWidget (Shell1);

        CommonPause();
        CommonPause();
	XtAppMainLoop (app_context);
}
