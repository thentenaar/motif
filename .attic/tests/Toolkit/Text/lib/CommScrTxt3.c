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
static char rcsid[] = "$TOG: CommScrTxt3.c /main/7 1997/10/02 15:48:46 csn $"
#endif
#endif
#include <testlib.h>


static char new_string1[] = "\
As I was going to St. Ives\n\
I met a man with seven wives\n\
How many were going to St. Ives?\n\
\n\
I don't know but...\n\
Jack Sprat could eat no fat\n\
His wife could eat no lean";

Widget BBoard1;
Widget PushB1;
Widget Text1, Text2, Text3, Text4;
Widget Label1, Label2, Label3, Label4;
Arg args[MAX_ARGS];
int n;

static char font_name1[] = "8x13";
static char render_tag[] = "render1";

static void ResetStrings(Widget w, XtPointer client_data,
                                XtPointer call_data)
{
	n = 0;
	XtSetArg(args[n], XmNvalue, new_string1); n++;
	XtSetValues(Text1, args, n);
	XtSetValues(Text2, args, n);
	XtSetValues(Text3, args, n);
	XtSetValues(Text4, args, n);
}

void main_scrtext3(argc, argv)
     int argc;
     char **argv;
{
    XmRendition   rendition;
    XmRenderTable renderTab = NULL;
    XtTranslations  new_table;
    static char     translations[] = 
      "Ctrl<Key>i:      toggle-overstrike() ";

	/* initialize toolkit */
        CommonTestInit(argc, argv);

        /*  Create Compound String version of new_string1. */
	n = 0;
	BBoard1 = XmCreateBulletinBoard(Shell1, "BBoard1", args, n);
	XtManageChild(BBoard1);

	/* ScrolledText widget */
	n = 0;
	XtSetArg(args[n], XmNlabelString,
	XmStringCreateLtoR("ScrolledText widget", XmSTRING_DEFAULT_CHARSET)); n++;
	Label1 = XmCreateLabel(BBoard1, "Label1", args, n);
	XtManageChild(Label1);


	n = 0;
	XtSetArg(args[n], XmNcolumns, 25); n++;
	XtSetArg(args[n], XmNrows, 5); n++;
	XtSetArg(args[n], XmNscrollVertical, True); n++;
	XtSetArg(args[n], XmNscrollHorizontal, True); n++;
	XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
	XtSetArg(args[n], XmNvalue, new_string1); n++;
	XtSetArg(args[n], XmNfontList, CommonGetFontList(font_name1)); n++;
	Text1 = XmCreateScrolledText(BBoard1, "Text1", args, n);
	XtManageChild(Text1);
	n = 0;
	XtSetArg(args[n], XmNy, 30); n++;
	XtSetValues(XtParent(Text1), args, n);


	/* Single Line Text widget */
	n = 0;
	XtSetArg(args[n], XmNy, 150); n++;
	XtSetArg(args[n], XmNlabelString,
		 XmStringCreateLtoR("SingleLine Text widget", 
				XmSTRING_DEFAULT_CHARSET)); n++;
	Label2 = XmCreateLabel(BBoard1, "Label2", args, n);
	XtManageChild(Label2);
	n = 0;
	XtSetArg(args[n], XmNy, 170); n++;
	XtSetArg(args[n], XmNcolumns, 25); n++;
	XtSetArg(args[n], XmNrows, 1); n++;
	XtSetArg(args[n], XmNeditMode, XmSINGLE_LINE_EDIT); n++;
	XtSetArg(args[n], XmNvalue, new_string1); n++;
        XtSetArg(args[n], XmNfontList, CommonGetFontList(font_name1)); n++;
	Text2 = XmCreateText(BBoard1, "Text2", args, n);

	XtManageChild(Text2);


	/* Multi Line Text widget */
	n = 0;
	XtSetArg(args[n], XmNy, 220); n++;
	XtSetArg(args[n], XmNlabelString,
	XmStringCreateLtoR("MultiLine Text widget", 
		       XmSTRING_DEFAULT_CHARSET)); n++;
	Label3 = XmCreateLabel(BBoard1, "Label3", args, n);
	XtManageChild(Label3);
	n = 0;
	XtSetArg(args[n], XmNy, 240); n++;
	XtSetArg(args[n], XmNcolumns, 25); n++;
	XtSetArg(args[n], XmNrows, 3); n++;
	XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
        XtSetArg(args[n], XmNvalue, new_string1); n++;
        XtSetArg(args[n], XmNfontList, CommonGetFontList(font_name1)); n++;
	Text3 = XmCreateText(BBoard1, "Text3", args, n);
	XtManageChild(Text3);


	/* TextField widget */
	n = 0;
	XtSetArg(args[n], XmNy, 320); n++;
	XtSetArg(args[n], XmNlabelString,
	XmStringCreateLtoR("TextField widget", XmSTRING_DEFAULT_CHARSET)); n++;
	Label4 = XmCreateLabel(BBoard1, "Label4", args, n);
	XtManageChild(Label4);
	n = 0;
	XtSetArg(args[n], XmNy, 340); n++;
	XtSetArg(args[n], XmNcolumns, 25); n++;
	XtSetArg(args[n], XmNvalue, new_string1); n++;
	XtSetArg(args[n], XmNfontList, CommonGetFontList("8x13")); n++;
	Text4 = XmCreateTextField(BBoard1, "Text4", args, n);
	XtManageChild(Text4);

	/* Reset String pushbutton */
	n = 0;
	XtSetArg(args[n], XmNy, 390); n++;
	XtSetArg(args[n], XmNlabelString,
	XmStringCreateLtoR("Reset Strings", XmSTRING_DEFAULT_CHARSET)); n++;
	PushB1 = XmCreatePushButton(BBoard1, "PushB1", args, n);
	XtAddCallback(PushB1, XmNactivateCallback, ResetStrings, NULL);
	XtManageChild(PushB1);


	XtRealizeWidget(Shell1);

	CommonPause();

	n = 0;
	XtSetArg(args[n], XmNvalue, new_string1); n++;
	XtSetValues(Text4, args, n);
	XtSetValues(Text1, args, n);
	XtSetValues(Text2, args, n);
	XtSetValues(Text3, args, n);

	CommonPause();

        CommonPause();  /*  Test case CR 5186 */

	n = 0;
	XtSetArg(args[n], XmNvalue, new_string1); n++;
	XtSetValues(Text4, args, n);

	XtSetValues(Text1, args, n);
	XtSetValues(Text2, args, n);
	XtSetValues(Text3, args, n);

        n = 0;
        XtSetArg( args[n], XmNsensitive, False ); n++;
	XtSetValues(Text1, args, n);
	XtSetValues(Text2, args, n);
	XtSetValues(Text3, args, n);
	XtSetValues(Text4, args, n);

	CommonPause(); 

	XtAddCallback(Text1, XmNfocusCallback, CommonGenericCB,
		"ScrolledText");
	XtAddCallback(Text1, XmNlosingFocusCallback, CommonGenericCB,
		"ScrolledText");

	XtAddCallback(Text2, XmNfocusCallback, CommonGenericCB,
		"SingleLine");
	XtAddCallback(Text2, XmNlosingFocusCallback, CommonGenericCB,
		"SingleLine");

	XtAddCallback(Text3, XmNfocusCallback, CommonGenericCB,
		"MultiLine");
	XtAddCallback(Text3, XmNlosingFocusCallback, CommonGenericCB,
		"MultiLine");

	XtAddCallback(Text4, XmNfocusCallback, CommonGenericCB,
		"TextField");
	XtAddCallback(Text4, XmNlosingFocusCallback, CommonGenericCB,
		"TextField");

        n = 0;
        XtSetArg( args[n], XmNsensitive, True ); n++;
	XtSetValues(Text1, args, n);
	XtSetValues(Text2, args, n);
	XtSetValues(Text3, args, n);
	XtSetValues(Text4, args, n);

	CommonPause();

	XtRemoveAllCallbacks(Text1, XmNfocusCallback);
	XtRemoveAllCallbacks(Text1, XmNlosingFocusCallback);
	XtAddCallback(Text1, XmNvalueChangedCallback, CommonGenericCB,
		"ScrolledText");
	XtAddCallback(Text1, XmNmodifyVerifyCallback, CommonGenericCB,
		"ScrolledText");
	XtAddCallback(Text1, XmNmotionVerifyCallback, CommonGenericCB,
		"ScrolledText");

	XtRemoveAllCallbacks(Text2, XmNfocusCallback);
	XtRemoveAllCallbacks(Text2, XmNlosingFocusCallback);
	XtAddCallback(Text2, XmNvalueChangedCallback, CommonGenericCB,
		"SingleLine");
	XtAddCallback(Text2, XmNmodifyVerifyCallback, CommonGenericCB,
		"SingleLine");
	XtAddCallback(Text2, XmNmotionVerifyCallback, CommonGenericCB,
		"SingleLine");

	XtRemoveAllCallbacks(Text3, XmNfocusCallback);
	XtRemoveAllCallbacks(Text3, XmNlosingFocusCallback);
	XtAddCallback(Text3, XmNvalueChangedCallback, CommonGenericCB,
		"MultiLine");
	XtAddCallback(Text3, XmNmodifyVerifyCallback, CommonGenericCB,
		"MultiLine");
	XtAddCallback(Text3, XmNmotionVerifyCallback, CommonGenericCB,
		"MultiLine");

	XtRemoveAllCallbacks(Text4, XmNfocusCallback);
	XtRemoveAllCallbacks(Text4, XmNlosingFocusCallback);
	XtAddCallback(Text4, XmNvalueChangedCallback, CommonGenericCB,
		"TextField");
	XtAddCallback(Text4, XmNmodifyVerifyCallback, CommonGenericCB,
		"TextField");
	XtAddCallback(Text4, XmNmotionVerifyCallback, CommonGenericCB,
		"TextField");

	CommonPause();

	XtRemoveAllCallbacks(Text1, XmNvalueChangedCallback);
	XtRemoveAllCallbacks(Text1, XmNmodifyVerifyCallback);
	XtRemoveAllCallbacks(Text1, XmNmotionVerifyCallback);
	XtAddCallback(Text1, XmNgainPrimaryCallback, CommonGenericCB,
		"ScrolledText");
	XtAddCallback(Text1, XmNlosePrimaryCallback, CommonGenericCB,
		"ScrolledText");
	XtAddCallback(Text1, XmNactivateCallback, CommonGenericCB,
		"ScrolledText");
	XtAddCallback(Text1, XmNhelpCallback, CommonGenericCB,
		"ScrolledText");

	XtRemoveAllCallbacks(Text2, XmNvalueChangedCallback);
	XtRemoveAllCallbacks(Text2, XmNmodifyVerifyCallback);
	XtRemoveAllCallbacks(Text2, XmNmotionVerifyCallback);
	XtAddCallback(Text2, XmNgainPrimaryCallback, CommonGenericCB,
		"SingleLine");
	XtAddCallback(Text2, XmNlosePrimaryCallback, CommonGenericCB,
		"SingleLine");
	XtAddCallback(Text2, XmNactivateCallback, CommonGenericCB,
		"SingleLine");
	XtAddCallback(Text2, XmNhelpCallback, CommonGenericCB,
		"SingleLine");

	XtRemoveAllCallbacks(Text3, XmNvalueChangedCallback);
	XtRemoveAllCallbacks(Text3, XmNmodifyVerifyCallback);
	XtRemoveAllCallbacks(Text3, XmNmotionVerifyCallback);
	XtAddCallback(Text3, XmNgainPrimaryCallback, CommonGenericCB,
		"MultiLine");
	XtAddCallback(Text3, XmNlosePrimaryCallback, CommonGenericCB,
		"MultiLine");
	XtAddCallback(Text3, XmNactivateCallback, CommonGenericCB,
		"MultiLine");
	XtAddCallback(Text3, XmNhelpCallback, CommonGenericCB,
		"MultiLine");

	XtRemoveAllCallbacks(Text4, XmNvalueChangedCallback);
	XtRemoveAllCallbacks(Text4, XmNmodifyVerifyCallback);
	XtRemoveAllCallbacks(Text4, XmNmotionVerifyCallback);
	XtAddCallback(Text4, XmNgainPrimaryCallback, CommonGenericCB,
		"TextField");
	XtAddCallback(Text4, XmNlosePrimaryCallback, CommonGenericCB,
		"TextField");
	XtAddCallback(Text4, XmNactivateCallback, CommonGenericCB,
		"TextField");
	XtAddCallback(Text4, XmNhelpCallback, CommonGenericCB,
		"TextField");

	CommonPause();

       /*  Remove remaining callbacks. */
	XtRemoveAllCallbacks(Text4, XmNgainPrimaryCallback);
	XtRemoveAllCallbacks(Text4, XmNlosePrimaryCallback);
	XtRemoveAllCallbacks(Text4, XmNactivateCallback);
	XtRemoveAllCallbacks(Text4, XmNhelpCallback);

       /*  Reset strings. */
	n = 0;
	XtSetArg(args[n], XmNvalue, new_string1); n++;
	XtSetValues(Text4, args, n);

        XtSetValues(Text1, args, n);
        XtSetValues(Text2, args, n);
        XtSetValues(Text3, args, n);

       /*  Override translation table with translation for toggle-overstrike. */
        new_table = XtParseTranslationTable( translations );
        XtOverrideTranslations( Text1, new_table );
        XtOverrideTranslations( Text2, new_table );
        XtOverrideTranslations( Text3, new_table );
        XtOverrideTranslations( Text4, new_table );

       /*  Test toggle-overstrike() */
	CommonPause();

	XtAppMainLoop(app_context);
}
