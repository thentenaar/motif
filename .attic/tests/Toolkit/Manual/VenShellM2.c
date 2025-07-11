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
static char rcsid[] = "$TOG: VenShellM2.c /main/6 1999/01/29 16:34:55 jff $"
#endif
#endif



#include <testlib.h>
#include <Xm/Xm.h>


/*  Global Variables  */

void
main(Cardinal argc, char **argv)
{

    Display            *display;
    XtAppContext       app_context;
    Widget             vshell;
    Widget             manager, label, button, scale;

    XmFontListEntry     fl_0, fl_1, fl_2;
    XmFontList          dfl, bfl, lfl;
    XmString            string;
    Cardinal    	n;
    Arg         	args[MAX_ARGS];

    /* 
       NOTE: This test specifically does not call CommonTestInit() and 
       other Common functions. Its purpose is to test the creation of
       fontList resources outside of the normal flow of automated tests.
    */

    XtToolkitInitialize();
    app_context = XtCreateApplicationContext ();
    display = XtOpenDisplay (app_context, NULL, "VenShellM2", "XMCommon",
			     NULL, 0, (int *)&argc, argv);

    if (display == NULL) {
      printf ("Couldn't open display. Exiting.\n"); exit (1);
    }
    
    /*  Load some fonts using XmFontListEntryLoad */

    fl_0 = XmFontListEntryLoad (display, "fixed", 
				XmFONT_IS_FONT, XmFONTLIST_DEFAULT_TAG);
    fl_1 = XmFontListEntryLoad (display, "6x10",
				XmFONT_IS_FONT, "button_tag");
    fl_2 = XmFontListEntryLoad (display, "8x13bold",
				XmFONT_IS_FONT, "label_tag");

    /* create the fontLists to be passed to Vendor Shell */
    /* these will be used to set up the default fonts at create time */


    dfl = XmFontListAppendEntry (NULL, fl_0);
    bfl = XmFontListAppendEntry (NULL, fl_1);
    lfl = XmFontListAppendEntry (NULL, fl_2);
    
    XmFontListEntryFree (&fl_0);
    XmFontListEntryFree (&fl_1);
    XmFontListEntryFree (&fl_2);

    n = 0;
    XtSetArg (args[n], XmNdefaultFontList, dfl); n++;
    XtSetArg (args[n], XmNbuttonFontList, bfl); n++;
    XtSetArg (args[n], XmNlabelFontList, lfl); n++;
    vshell = XtAppCreateShell ("VenShellM2", "XMcommon", 
			       applicationShellWidgetClass,
			       display,
			       args, n);

    n = 0;
    XtSetArg (args[n], XmNheight, 100); n++;
    XtSetArg (args[n], XmNwidth, 200); n++;
    manager = XmCreateBulletinBoard (vshell, "manager", args, n);

    string = XmStringCreateLtoR ("button contents", "button_tag");
    n = 0;
    XtSetArg (args[n], XmNlabelString, string); n++;
    XtSetArg (args[n], XmNx, 10); n++;
    XtSetArg (args[n], XmNy, 10); n++;
    button = XmCreatePushButton (manager, "button", args, n);

    string = XmStringCreateLtoR ("label contents", "label_tag");
    n = 0;
    XtSetArg (args[n], XmNlabelString, string); n++;
    XtSetArg (args[n], XmNx, 50); n++;
    XtSetArg (args[n], XmNy, 50); n++;
    label = XmCreateLabel (manager, "label", args, n);

    XmStringFree (string);

    /* just print instructions to standard out for this simple test. */

    printf ("If the button uses 6x10 font and the label string 8x13bold,\n");
    printf ("   then this test has passed.\n");
    fflush (stdout);

    XtManageChild (label);
    XtManageChild (button);
    XtManageChild (manager);
    XtRealizeWidget(vshell);

    XtAppMainLoop(app_context);

}
