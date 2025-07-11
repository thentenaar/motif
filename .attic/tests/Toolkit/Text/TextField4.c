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
static char rcsid[] = "$XConsortium: TextField4.c /main/6 1995/07/13 19:35:51 drk $"
#endif
#endif


#include <testlib.h>

Arg args[MAX_ARGS];

void main (argc, argv)
int argc;
char **argv;
{
	Widget TextF1 = NULL;
	Widget TextF2 = NULL;
	Widget Text1 = NULL;
	Widget Label1, Label2, Label3;
	Widget BBoard1;
	int n = 0;
	XmString tmp_string;

	/*  initialize toolkit  */
	CommonTestInit(argc, argv);

	n = 0;
	BBoard1 = XmCreateBulletinBoard(Shell1, "BBoard1", args, n);
	XtManageChild(BBoard1);


	n = 0;
        tmp_string = XmStringCreate("TextField1 - resizeWidth: False",
		XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	Label1 = XmCreateLabel(BBoard1, "Label1", args, n);
	XtManageChild(Label1);
        XmStringFree (tmp_string);

	n = 0;
	XtSetArg(args[n], XmNresizeWidth, False); n++;
	XtSetArg(args[n], XmNcolumns, 30); n++;
	XtSetArg(args[n], XmNy, 30); n++;
	TextF1 = XmCreateTextField(BBoard1, "TextF1", args, n);
	XtManageChild(TextF1);


	n = 0;
        tmp_string = XmStringCreate("TextField2 - resizeWidth: True", 
		                    XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNy, 80); n++;
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	Label2 = XmCreateLabel(BBoard1, "Label2", args, n);
	XtManageChild(Label2);
        XmStringFree (tmp_string);

	n = 0;
	XtSetArg(args[n], XmNresizeWidth, True); n++;
	XtSetArg(args[n], XmNcolumns, 30); n++;
	XtSetArg(args[n], XmNy, 100); n++;
	TextF2 = XmCreateTextField(BBoard1, "TextF2", args, n);
	XtManageChild(TextF2);


	n = 0;
        tmp_string = XmStringCreate("Text1 - resizeWidth: True", 
                         	     XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNy, 140); n++;
	XtSetArg(args[n], XmNlabelString, tmp_string);
		
	Label3 = XmCreateLabel(BBoard1, "Label3", args, n);
	XtManageChild(Label3);
        XmStringFree (tmp_string);

	n = 0;
	XtSetArg(args[n], XmNresizeWidth, True); n++;
	XtSetArg(args[n], XmNcolumns, 30); n++;
	XtSetArg(args[n], XmNy, 160); n++;
	Text1 = XmCreateText(BBoard1, "Text1", args, n);
	XtManageChild(Text1);

	n = 0;
	XtSetArg(args[n], XmNallowShellResize, False); n++;
	XtSetValues(Shell1, args, n);


	XtRealizeWidget(Shell1);


	/* test case for PIR 2914, see .dat */
	CommonPause();

	CommonPause();

	/* test case for PIR 2898, see .dat */
	CommonPause();
	
	XtAppMainLoop(app_context);
}
