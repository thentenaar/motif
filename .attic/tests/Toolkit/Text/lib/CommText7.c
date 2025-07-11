/* $TOG: CommText7.c /main/7 1997/10/02 16:26:40 csn $ */
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

Widget manager = NULL;
Arg args[MAX_ARGS];
static char string1[] = "Sample Text";

static char font_name1[] = "fixed";

static char render_tag[] = "render1";

static Widget makeTextWidget(Widget mgr, char *name)
{
        XmString tcs = (XmString)0;
        Widget Text;
	int n = 0;

	XtSetArg(args[n], XmNresizeWidth, False);  n++;
	XtSetArg(args[n], XmNresizeHeight, False);  n++;
	XtSetArg(args[n], XmNcolumns, 40);  n++;
	XtSetArg(args[n], XmNrows, 1);  n++;
	XtSetArg(args[n], XmNeditMode, XmSINGLE_LINE_EDIT); n++;
	XtSetArg(args[n], XmNcursorPosition, 5);  n++;
	XtSetArg(args[n], XmNvalue, string1);  n++;
	XtSetArg(args[n], XmNfontList, CommonGetFontList("fixed"));  n++;
	XtSetArg(args[n], XmNforeground, CommonGetColor("Red"));  n++;
	XtSetArg(args[n], XmNbackground, CommonGetColor("White"));  n++;
        Text = XmCreateText(mgr, name, args, n);
        if ( tcs ) XmStringFree(tcs);
	return(Text);
}

void main_text7(argc, argv)
     int argc;
     char **argv;
{
	Widget Text1 = NULL;
	Widget PB1 = NULL;
	int n = 0;
	XmString tcs;

	/*  initialize toolkit  */
	CommonTestInit(argc, argv);
	 
	n = 0;
	XtSetArg (args[n], XmNwidth, 50); n++;
	XtSetArg (args[n], XmNheight, 50); n++;
	XtSetValues(Shell1, args, n);
	 
	XtRealizeWidget(Shell1);

	n = 0;
	XtSetArg (args[n], XmNwidth, 400); n++;
	XtSetArg (args[n], XmNheight, 400); n++;
	manager = XmCreateBulletinBoard (Shell1, "manager", args, n);


	Text1 = makeTextWidget(manager, "Text1");
	XtAddCallback(Text1, XmNactivateCallback, CommonGenericCB, NULL);

	XtManageChild(manager);
	XtManageChild(Text1);

	CommonPause();

	n = 0;
	XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
	XtSetArg(args[n], XmNrows, 4); n++;
	XtSetValues (Text1, args, n);

	CommonPause();

	XtDestroyWidget (manager);

	XtSetArg (args[n], XmNwidth, 400); n++;
	XtSetArg (args[n], XmNheight, 400); n++;
	manager = XmCreateForm(Shell1, "manager", args, n);

	Text1 = makeTextWidget(manager, "Text1");
	XtAddCallback(Text1, XmNactivateCallback, CommonGenericCB, NULL);

	XtManageChild(manager);
	XtManageChild(Text1);

	CommonPause ();

	n = 0;
	XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
	XtSetArg(args[n], XmNrows, 4); n++;
	XtSetValues (Text1, args, n);

	CommonPause();

	XtDestroyWidget (manager);

	XtSetArg (args[n], XmNwidth, 400); n++;
	XtSetArg (args[n], XmNheight, 400); n++;
	manager = XmCreateFormDialog(Shell1, "manager", args, n);

	Text1 = makeTextWidget(manager, "Text1");
	XtAddCallback(Text1, XmNactivateCallback, CommonGenericCB, NULL);

	XtManageChild(manager);
	XtManageChild(Text1);

	CommonPause();

	n = 0;
	XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
	XtSetArg(args[n], XmNrows, 4); n++;
	XtSetValues (Text1, args, n);

	CommonPause();

	XtDestroyWidget (manager);

	XtSetArg (args[n], XmNwidth, 400); n++;
	XtSetArg (args[n], XmNheight, 400); n++;
	manager = XmCreateBulletinBoardDialog(Shell1, "manager", args, n);

	Text1 = makeTextWidget(manager, "Text1");

	XtAddCallback(Text1, XmNactivateCallback, CommonGenericCB, NULL);

	XtManageChild(manager);
	XtManageChild(Text1);

	CommonPause();

	n = 0;
	XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); 		n++;
	XtSetArg(args[n], XmNrows, 4); 				n++;
	XtSetValues (Text1, args, n);

	CommonPause();

	/* Begin test for Pir 2693 */

	XtDestroyWidget (manager);

	tcs = XmStringCreateLtoR("Modeless Window", XmSTRING_DEFAULT_CHARSET);

	XtSetArg (args[n], XmNallowOverlap, False); 			n++;
	XtSetArg (args[n], XmNdialogStyle, XmDIALOG_MODELESS); 	n++;
	XtSetArg (args[n], XmNdialogTitle, tcs); 			n++;
	XtSetArg (args[n], XmNresizePolicy, XmRESIZE_GROW); 		n++;
	manager = XmCreateFormDialog(Shell1, "manager", args, n);
	XtManageChild(manager);

	XmStringFree(tcs);

	n = 0;
	XtSetArg(args[n], XmNforeground, CommonGetColor("Red"));  	n++;
	XtSetArg(args[n], XmNbackground, CommonGetColor("White"));  	n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
	XtSetArg(args[n], XmNtopOffset, 15);				n++;
	XtSetArg(args[n], XmNbottomAttachment, XmATTACH_WIDGET);	n++;
	XtSetArg(args[n], XmNbottomWidget, PB1);			n++;
	XtSetArg(args[n], XmNbottomOffset, 15);			n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
	XtSetArg(args[n], XmNleftOffset, 5);				n++;
	XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
	XtSetArg(args[n], XmNrightOffset, 5);			n++;
	XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT);		n++;
	XtSetArg(args[n], XmNcolumns, 20);				n++;
	XtSetArg(args[n], XmNrows, 2);				n++;
	XtSetArg(args[n], XmNmaxLength, 200);			n++;
	XtSetArg(args[n], XmNresizeWidth, True);			n++;
	XtSetArg(args[n], XmNresizeHeight, False);			n++;
	XtSetArg(args[n], XmNfontList, CommonGetFontList("fixed"));	n++;
	Text1 = XmCreateText(manager, "Text1", args, n);
	XtAddCallback(Text1, XmNactivateCallback, CommonGenericCB, NULL);
	XtManageChild(Text1);

	/* End test for Pir 2693 */

	CommonPause();

	XtAppMainLoop(app_context);
	/* end of test */
}
