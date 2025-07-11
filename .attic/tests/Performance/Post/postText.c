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
static char rcsid[] = "$XConsortium: postText.c /main/7 1995/07/14 12:03:52 drk $"
#endif
#endif
/*
 *	file: postText.c
 *
 *	Program to post a Text widget under a bulletinboard dialog.
 */
#include	<testlib.h>

Widget dialog;
Widget Text;

static void postit()
{
	XtManageChild(Text);
	XtManageChild(dialog);
}

main(argc, argv) 
int argc;
char *argv[];
{
	Widget pb;
	Arg args[20];
	int n;

    CommonTestInit(argc, argv);
    
	
	n = 0;
	pb = XmCreatePushButton(Shell1, "pb", args, n);
	XtManageChild(pb);
	XtAddCallback(pb, XmNactivateCallback, postit, NULL);

	n = 0;
	XtSetArg(args[n], XmNwidth, 100); n++;
	XtSetArg(args[n], XmNheight, 100); n++;
	dialog = XmCreateBulletinBoardDialog(Shell1, "dialog", args, n);

	n = 0;
	XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
	Text = XmCreateText(dialog, "Text", args, n);

	XtRealizeWidget(Shell1);

	XtAppMainLoop(app_context);
}

