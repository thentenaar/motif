/* $XConsortium: Color.c /main/4 1995/07/15 21:07:57 drk $ */
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



/* test app context destroy */

#include <testlib.h>


static void gobyebye();

int quitflag = 0;


/* test case for CR 2761. */

void main(argc, argv)
int argc;
char **argv;
{
	Widget fm;
	Widget pb;
	Arg args[32];
	int n, x, y;


	CommonTestInit (argc,argv);


	/* form */

	n=0;
	XtSetArg(args[n],XmNwidth,100); n++;
	XtSetArg(args[n],XmNheight,100); n++;
	fm = XmCreateForm(Shell1,"fm",args,n);

	/* add push button */

	n = 0;
	XtSetArg(args[n],XmNx,20); n++;
	XtSetArg(args[n],XmNy,20); n++;
	XtSetArg(args[n],XmNwidth,60); n++;
	XtSetArg(args[n],XmNheight,60); n++;
	pb = XmCreatePushButton(fm,"bye",args,n);
	XtAddCallback(pb, XmNactivateCallback, gobyebye, NULL);

	/* manage, realize, loop */

	XtManageChild(pb);
	XtManageChild(fm);

	XtRealizeWidget(Shell1);






	CommonPause();





}

static void
gobyebye(w, client, closure)
    Widget w;
    caddr_t client, closure;
{
	XtDestroyApplicationContext(app_context);
	printf ("exiting\n");
	exit(0);

}

