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
static char rcsid[] = "$XConsortium: ErrorDia1.c /main/8 1995/07/13 18:56:51 drk $"
#endif
#endif


#include <signal.h>

#include <X11/StringDefs.h>


#include <testlib.h>

static Widget message1, message2;

void
DestroyCall(w, client, call)
    Widget w;
    XtPointer client, call;
{
    printf("Destroyed MessageBox\n");
}

void
MapFunction (w,d1,d2)
    Widget w;
    XtPointer d1;
    XtPointer d2;
{
    static int test = 0;

    switch (test) {
	case 0: XtManageChild (message1);
		test++;
		break;
	case 1: XtManageChild (message2);
		test = 0;
		break;
    }
}

void
UnmapFunction (w,d1,d2)
    Widget w;
    XtPointer d1;
    XtPointer d2;
{
    XtUnmanageChild (w);
    printf ("Enter Message 1 \n");
}

void
UnmapFunction2 (w,d1,d2)
    Widget w;
    XtPointer d1;
    XtPointer d2;
{
    XtUnmanageChild (w);
    printf ("Cancel Message 2 \n");
}

void
UnmapFunction3 (w,d1,d2)
    Widget w;
    XtPointer d1;
    XtPointer d2;
{
    XtUnmanageChild (w);
    printf ("Enter Message 2 \n");
}

void main (argc, argv)
int argc;
char **argv;

{
    Widget w2;


    /*  initialize toolkit  */
    CommonTestInit(argc, argv);
 
    w2 = XmCreatePushButton (Shell1, "mapbutton", NULL, 0);
    XtManageChild (w2);

    message1 = XmCreateErrorDialog(w2, "message1", NULL, 0);

    XtAddCallback (w2, XmNactivateCallback, MapFunction, NULL);

    XtAddCallback (message1, XmNdestroyCallback, DestroyCall, NULL);
    XtAddCallback (message1, XmNokCallback, UnmapFunction, NULL);

    message2 = XmCreateErrorDialog(w2, "message2", NULL, 0);

    XtAddCallback (message2, XmNokCallback, UnmapFunction3, NULL);
    XtAddCallback (message2, XmNcancelCallback, UnmapFunction2, NULL);

    XtRealizeWidget(Shell1);

    CommonPause();

    XtDestroyWidget(message1);

    CommonPause();

    /* begin test for PIR 3698 */

    message1 = XmCreateErrorDialog(w2, NULL, NULL, 0);
    XtManageChild (message1);

    CommonPause();

    /* end test for PIR 3698 */

    XtAppMainLoop(app_context);
}
