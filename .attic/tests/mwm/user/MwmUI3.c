/* $XConsortium: MwmUI3.c /main/5 1995/07/15 21:14:12 drk $ */
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
PushButtonCallback (Widget, XtPointer, XtPointer);

void 
main (unsigned int argc, char **argv)
{

	Widget TestShell1, TestShell2, TestShell3, TestShell4;
        Widget PB1, PB2, PB3, PB4;
        Widget MesDialog1;
        Arg args[10];
        int n;

    
        CommonTestInit (argc, argv);

	/* cautionary message to user */

	CommonPause();


        /* Create 4 top level shells */
   
        n=0;
        XtSetArg (args[n], XmNheight, 100);n++;
        XtSetArg (args[n], XmNwidth, 160);n++;
        TestShell1=XtAppCreateShell ("TestWindow1", "TestWindow1",
				topLevelShellWidgetClass,display, args,n);


        n=0;
        PB1 = XmCreatePushButton (TestShell1, "PB1", args, n);
        XtManageChild (PB1);

        XtRealizeWidget (TestShell1);


        
	n=0;
        XtSetArg (args[n], XmNheight, 100);n++;
        XtSetArg (args[n], XmNwidth, 160);n++;
        TestShell2=XtAppCreateShell ("TestWindow2", "TestWindow2",
				topLevelShellWidgetClass,display, args,n);




        
        n=0;
        PB2 = XmCreatePushButton (TestShell2, "PB2", args, n);
        XtManageChild(PB2);
        XtAddCallback (PB2, XmNactivateCallback, PushButtonCallback, NULL);
         
        XtRealizeWidget (TestShell2);


	n=0;
        XtSetArg (args[n], XmNheight, 100);n++;
        XtSetArg (args[n], XmNwidth, 160);n++;
        TestShell3=XtAppCreateShell ("TestWindow3", "TestWindow3",
				topLevelShellWidgetClass,display, args,n);




        
        n=0;
        PB3 = XmCreatePushButton (TestShell3, "PB3", args, n);
        XtManageChild(PB3);

        XtRealizeWidget (TestShell3);


        n=0;
        XtSetArg (args[n], XmNheight, 100);n++;
        XtSetArg (args[n], XmNwidth, 160);n++;
        TestShell4=XtAppCreateShell ("TestWindow4", "TestWindow4",
				topLevelShellWidgetClass,display, args,n);



	n=0;
        PB4 = XmCreatePushButton (TestShell4, "PB4", args, n);
        XtManageChild(PB4);

         
        XtRealizeWidget (TestShell4);

        CommonPause();

        /* have a dialog box pop up */

        n=0;
        MesDialog1=XmCreateMessageDialog (TestShell3, "MesDialog1", args, n);
        XtManageChild (MesDialog1);

    
        CommonPause();
        CommonPause();
        CommonPause();
        CommonPause(); 
        CommonPause();      
        CommonPause();      
        CommonPause();      

       	XtAppMainLoop(app_context);

}

void 
PushButtonCallback (Widget w, XtPointer client_data, XtPointer call_data)
{
    fprintf (stdout, "The button press event has been passed to the client\n");
}
