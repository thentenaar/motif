/* $XConsortium: SpinBox4.c /main/5 1995/07/15 21:09:58 drk $ */
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



void
main (int argc, char **argv)
{
    Widget Parent;
    Widget SpinBox1, SpinBox2, SpinBox3, SpinBox4;
    Widget TextF1, TextF2, TextF3, TextF4;
    Arg    args[5];
    int    n;
    

    CommonTestInit (argc, argv);


    Parent = XmCreateRowColumn (Shell1, "Parent", NULL,0);
    XtManageChild (Parent);


    SpinBox1 = XmCreateSpinBox (Parent, "SpinBox1", NULL,0);
    XtManageChild (SpinBox1);

    TextF1 = XmCreateTextField (SpinBox1, "TextF1", NULL,0);
    XtManageChild (TextF1);

    TextF2 = XmCreateTextField (SpinBox1, "TextF2", NULL,0);
    XtManageChild (TextF2);


    TextF3 = XmCreateTextField (SpinBox1, "TextF3", NULL,0);
    XtManageChild (TextF3);

    TextF4 = XmCreateTextField (SpinBox1, "TextF4", NULL,0);
    XtManageChild (TextF4);

    XtRealizeWidget (Shell1);

    CommonPause();

    n = 0;
    XtSetArg( args[n], XmNdefaultArrowSensitivity, 
                       XmARROWS_DECREMENT_SENSITIVE ); n++; 
    XtSetValues( SpinBox1, args, n );
    CommonPause();

    n = 0;
    XtSetArg( args[n], XmNdefaultArrowSensitivity, 
                       XmARROWS_INCREMENT_SENSITIVE ); n++; 
    XtSetValues( SpinBox1, args, n );
    CommonPause();
    CommonPause();

    XtAppMainLoop (app_context);

    
}


