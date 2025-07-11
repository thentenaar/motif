/* $TOG: CommText8.c /main/7 1997/10/02 16:28:55 csn $ */
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

static Widget BBoard1 = NULL;
static Widget Form1 = NULL;
static Widget help_dialog = NULL;
static Arg args[MAX_ARGS];

char single_text[] = "Single Line Text Widget";
char multi_text[] = "Multi Line Text Widget"; 

static char font_name1[] = "fixed";
static char font_name2[] = "variable";

static char render_tag[] = "render1";

static char  *stat_msg[] = 
{
  "This is generic help for the application,\nor the widget you've selected has no help",
  "This is help for the Default Button.\nPress to make things happen",
  "This is help for the text type.",
  "This is help for the Other Button."
};

static void GenericHelp(Widget w, XtPointer client_data,
                                XtPointer call_data)
{	/*
	1 = bulletinBoard or a widget with no help 
	2 = Default button
	3 = Text1 (the single line text)
	4 = OtherButton
	*/
	XmString message;
	int n;

	message = XmStringLtoRCreate(stat_msg[((int)client_data) - 1],
				XmSTRING_DEFAULT_CHARSET);

	n = 0;
	XtSetArg(args[n], XmNmessageString, message); n++;
	XtSetValues(help_dialog, args, n);

	XtManageChild(help_dialog);
}                                            
	           

static void MyReturnRoutine(Widget w, XEvent *event,
				String *params, Cardinal *num_params)
{
	printf("The return key has been remapped to this routine.\n");
}

static void DblClickRoutine(Widget w, XEvent *event,
				String *params, Cardinal *num_params)
{
	printf("The user has double-clicked.\n");
}


static void ActivationDB(Widget w, XtPointer client_data,
                                XtPointer call_data)
{
	printf("The Default Button Activate callback was made.\n");
}

static void setInsensitive(Widget w, XtPointer client_data,
                                XtPointer call_data)
{
	XtSetArg(args[0], XmNsensitive, False);
	XtSetValues(w, args, 1);
}

Widget CreateTheBBoard(Widget parent, Boolean dialog);
Widget CreateTheForm(Widget parent);
Widget CreateSingleLineText(Widget parent);
Widget CreateOtherButton(Widget parent);
Widget CreateTheDefaultButton(Widget parent);
Widget CreateMultiLineText(Widget parent);
Widget CreateTextField(Widget parent);
Widget CreateToggleButton(Widget parent);
Widget CreateScrollBar(Widget parent);

void main_text8( argc, argv )
     int argc;
     char **argv;
{
	Widget Text1 = NULL;        /* single line text widget */
	Widget Text2 = NULL;        /* multi-line text widget */
	Widget TextF = NULL;        /* Text field widget */
	Widget DefaultB = NULL;
	Widget ToggleB = NULL;
	Widget OtherB = NULL;
	Widget ScrollB = NULL;
	int n = 0;

	/* the translation for BtnUp(2+) shows PIR 3113 */
	static XtActionsRec actions[] = {{ "MyReturnRoutine", MyReturnRoutine},
				    { "DblClickRoutine", DblClickRoutine}};
	static char translation_table[] = "<Key>Return: MyReturnRoutine()\n\
	                                   <Btn1Up>(2+): DblClickRoutine()";

	/*  initialize toolkit  */
	CommonTestInit(argc, argv);

	help_dialog = XmCreateMessageDialog(Shell1, "help_dialog", args, n);

	XtAppAddActions(app_context, actions, 2);

/* Test 1: Bulletin Board with default button. */

	BBoard1 = CreateTheBBoard(Shell1, False);

	DefaultB = CreateTheDefaultButton(BBoard1);
	n = 0;
	XtSetArg(args[n], XmNdefaultButton, DefaultB); n++;
	XtSetValues(BBoard1, args, n);

	Text1 = CreateSingleLineText(BBoard1);

	Text2 = CreateMultiLineText(BBoard1);

	TextF = CreateTextField(BBoard1);

	OtherB = CreateOtherButton(BBoard1);

	ToggleB = CreateToggleButton(BBoard1);

	ScrollB = CreateScrollBar(BBoard1);

	XtManageChild(BBoard1);
	XtManageChild(Text1);
	XtManageChild(Text2);
	XtManageChild(TextF);
	XtManageChild(DefaultB);
	XtManageChild(ToggleB);
	XtManageChild(OtherB);
	XtManageChild(ScrollB);
	XtRealizeWidget(Shell1);


	CommonPause();      /* #1: activate callback tests */
	CommonPause();      /* #2: ditto */
	CommonPause();      /* #3: more */
	CommonPause();      /* #4: help */

	XtOverrideTranslations(Text2,
		XtParseTranslationTable(translation_table));

	CommonPause();      /* #5: reset translations on text */

	XtRemoveCallback(OtherB, XmNactivateCallback, CommonGenericCB, NULL);
	XtAddCallback(OtherB, XmNactivateCallback, setInsensitive, NULL);

	XtRemoveCallback(DefaultB, XmNactivateCallback, CommonGenericCB, NULL);
	XtAddCallback(DefaultB, XmNactivateCallback, setInsensitive, NULL);

	CommonPause();     /* #6: button insensitivity */

	XtDestroyWidget(BBoard1);



/* Test 2: Bulletin Board Dialog with default button. */

	/* make Shell smaller, just for convenience */
	n = 0;
	XtSetArg(args[n], XmNheight, 50); n++;
	XtSetArg(args[n], XmNwidth, 50); n++;
	XtSetValues(Shell1, args, n);

	BBoard1 = CreateTheBBoard(Shell1, True);

	DefaultB = CreateTheDefaultButton(BBoard1);
	n = 0;
	XtSetArg(args[n], XmNdefaultButton, DefaultB); n++;
	XtSetValues(BBoard1, args, n);

	Text1 = CreateSingleLineText(BBoard1);

	Text2 = CreateMultiLineText(BBoard1);

	TextF = CreateTextField(BBoard1);

	OtherB = CreateOtherButton(BBoard1);

	ToggleB = CreateToggleButton(BBoard1);

	ScrollB = CreateScrollBar(BBoard1);

	XtManageChild(BBoard1);
	XtManageChild(Text1);
	XtManageChild(Text2);
	XtManageChild(TextF);
	XtManageChild(DefaultB);
	XtManageChild(ToggleB);
	XtManageChild(OtherB);
	XtManageChild(ScrollB);
	XtRealizeWidget(Shell1);

	CommonPause();   /* #7: check again */
	CommonPause();   /* #8: help */

	XtOverrideTranslations(Text2,
		XtParseTranslationTable(translation_table));

	CommonPause();   /* #9: override text translations */

	XtRemoveCallback(OtherB, XmNactivateCallback, CommonGenericCB, NULL);
	XtAddCallback(OtherB, XmNactivateCallback, setInsensitive, NULL);

	XtRemoveCallback(DefaultB, XmNactivateCallback, CommonGenericCB, NULL);
	XtAddCallback(DefaultB, XmNactivateCallback, setInsensitive, NULL);

	CommonPause();  /* #10: insensitive buttons */



/* Test 3: Form with with default button. */

	XtDestroyWidget(BBoard1);

	n = 0;

	Form1 = CreateTheForm(Shell1);

	DefaultB = CreateTheDefaultButton(Form1);
	n = 0;
	XtSetArg(args[n], XmNdefaultButton, DefaultB); n++;
	XtSetValues(Form1, args, n);

	Text1 = CreateSingleLineText(Form1);

	Text2 = CreateMultiLineText(Form1);

	TextF = CreateTextField(Form1);
	
	OtherB =  CreateOtherButton(Form1);

	ToggleB = CreateToggleButton(Form1);

	ScrollB = CreateScrollBar(Form1);

	XtManageChild(Form1);
	XtManageChild(Text1);
	XtManageChild(Text2);
	XtManageChild(TextF);
	XtManageChild(DefaultB);
	XtManageChild(ToggleB);
	XtManageChild(OtherB);
	XtManageChild(ScrollB);
	XtRealizeWidget(Shell1);
  
	CommonPause();  /* form */  /* number #11 */

	XtAppMainLoop(app_context);
}


Widget CreateTheBBoard(Widget parent, Boolean dialog)
{
	int n;
	Widget widget;

	n = 0;
#ifdef	MOTIF1_1
	XtSetArg(args[n], XmNwidth, 400); n++;
	XtSetArg(args[n], XmNheight, 400); n++;
#endif	/* MOTIF1_1 */
	/* test for PIR 2657 */
	XtSetArg(args[n], XmNtextFontList,
		CommonGetFontList("variable")); n++;
	if (dialog)
	{
		XtSetArg(args[n], XmNautoUnmanage, False); n++;
		XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
		widget = XmCreateBulletinBoardDialog(parent,
			"BBoard1", args, n);
	}
	else
	{
		widget = XmCreateBulletinBoard(parent, "BBoard1", args, n);
	}
	XtAddCallback(widget, XmNhelpCallback, GenericHelp, (XtPointer)1);
	return(widget);
}

Widget CreateTheForm(Widget parent)
{
	int n;
	Widget widget;
	
	n = 0;
#ifdef	MOTIF1_1
	XtSetArg(args[n], XmNwidth, 400); n++;
	XtSetArg(args[n], XmNheight, 400); n++;
#endif	/* MOTIF1_1 */
	/* test for PIR 2657 */
	XtSetArg(args[n], XmNtextFontList,
		CommonGetFontList("variable")); n++;
	widget = XmCreateForm(parent, "Form1", args, n);
	XtAddCallback(widget, XmNhelpCallback, GenericHelp, (XtPointer)1);
	return(widget);
}

Widget CreateSingleLineText(Widget parent)
{
	int n;
	Widget widget;
        XmString tcs = (XmString)0;

	n = 0;
	XtSetArg(args[n], XmNresizeWidth, False);  n++;
	XtSetArg(args[n], XmNresizeHeight, False);  n++;
	XtSetArg(args[n], XmNcolumns, 40);  n++;
	XtSetArg(args[n], XmNrows, 1);  n++;
	XtSetArg(args[n], XmNeditMode, XmSINGLE_LINE_EDIT); n++;
	XtSetArg(args[n], XmNforeground, CommonGetColor("Red"));  n++;
	XtSetArg(args[n], XmNbackground, CommonGetColor("White"));  n++;
	XtSetArg(args[n], XmNx, 10); n++;
	XtSetArg(args[n], XmNy, 10); n++;
	XtSetArg(args[n], XmNtraversalOn, True); n++;
	XtSetArg(args[n], XmNfontList, CommonGetFontList(font_name1));  n++;
	widget = XmCreateText(parent, "Text1", args, n);
	XtAddCallback(widget, XmNactivateCallback,
		(XtCallbackProc) CommonGenericCB, NULL);
	XtAddCallback(widget, XmNhelpCallback, GenericHelp, (XtPointer)3);

	/* test case for PIR 684: cannot set XmNvalue and XmNcursorPosition */
	/* simultaneosly at SetValues time or cursorPosition is ignored. */

	n = 0;
	XtSetArg(args[n], XmNvalue, single_text);  n++;
	XtSetArg(args[n], XmNcursorPosition, 5);  n++;
	XtSetValues(widget, args, n);
        if (tcs) XmStringFree(tcs);
	/* end of test case for PIR 684 */

	return(widget);
}

Widget CreateOtherButton(Widget parent)
{
	int n;
	Widget OtherB;
        XmString tmp_string;

        tmp_string = XmStringLtoRCreate("Other Button",
		                         XmSTRING_DEFAULT_CHARSET); n++;

	n = 0;
        XtSetArg(args[n], XmNx, 375); n++;
	XtSetArg(args[n], XmNy, 330); n++;
	XtSetArg(args[n], XmNlabelString, tmp_string); n++; 

	OtherB = XmCreatePushButtonGadget(parent, "OtherB", args, n);
	XtAddCallback(OtherB, XmNactivateCallback, CommonGenericCB, NULL);
	XtAddCallback(OtherB, XmNhelpCallback, GenericHelp, (XtPointer)4);

        XmStringFree (tmp_string);
	return(OtherB);
}

Widget CreateTheDefaultButton(Widget parent)
{
	int n;
	Widget DefaultB;
        XmString tmp_string;

	n = 0;
        tmp_string = XmStringLtoRCreate("Default Button",
		     XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	XtSetArg(args[n], XmNx, 375); n++;
	XtSetArg(args[n], XmNy, 375); n++;
	XtSetArg(args[n], XmNtraversalOn, True); n++;
	XtSetArg(args[n], XmNdefaultButtonShadowThickness, 2); n++;
	DefaultB = XmCreatePushButtonGadget(parent, "DefaultB", args, n);
	XtAddCallback(DefaultB, XmNactivateCallback, ActivationDB, NULL);
	XtAddCallback(DefaultB, XmNhelpCallback, GenericHelp, (XtPointer)2);
  
        XmStringFree (tmp_string);
	return(DefaultB);
}

Widget CreateMultiLineText( Widget parent)
{
	int n;
	Widget Text2;
        XmString tcs = (XmString)0;

	n = 0;
	XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
	XtSetArg(args[n], XmNrows, 4); n++;
	/* test case for PIR 2657
	XtSetArg(args[n], XmNfontList, CommonGetFontList("variable"));  n++;
	*/
	XtSetArg(args[n], XmNcolumns, 20);  n++;
	XtSetArg(args[n], XmNcursorPosition, 1);  n++;
	XtSetArg(args[n], XmNvalue, multi_text);  n++;
	XtSetArg(args[n], XmNforeground, CommonGetColor("Red"));  n++;
	XtSetArg(args[n], XmNbackground, CommonGetColor("White"));  n++;
	XtSetArg(args[n], XmNx, 10); n++;
	XtSetArg(args[n], XmNy, 150); n++;
	XtSetArg(args[n], XmNtraversalOn, True); n++;
	Text2 = XmCreateText(parent, "Text2", args, n);
        if ( tcs ) XmStringFree(tcs);
	XtAddCallback(Text2, XmNactivateCallback, CommonGenericCB, NULL);
	return(Text2);
}

Widget CreateTextField(Widget parent)
{
	int n;
	Widget TextF;

	n = 0;
	XtSetArg(args[n], XmNresizeWidth, False);  n++;
	XtSetArg(args[n], XmNcolumns, 40);  n++;
	XtSetArg(args[n], XmNcursorPosition, 5);  n++;
	XtSetArg(args[n], XmNvalue, "Text Field Widget");  n++;
	XtSetArg(args[n], XmNforeground, CommonGetColor("Red"));  n++;
	XtSetArg(args[n], XmNbackground, CommonGetColor("White"));  n++;
	XtSetArg(args[n], XmNx, 10); n++;
	XtSetArg(args[n], XmNy, 300); n++; 
	XtSetArg(args[n], XmNfontList, CommonGetFontList(font_name1));  n++;
	TextF = XmCreateTextField(parent, "TextF", args, n);
	XtAddCallback(TextF, XmNactivateCallback, CommonGenericCB, NULL);

	return(TextF);

}

Widget CreateToggleButton(Widget parent)
{
	int n;
	Widget ToggleB;

	n = 0;
	XtSetArg(args[n], XmNx, 375); n++;
	XtSetArg(args[n], XmNy, 300); n++;
	XtSetArg(args[n], XmNtraversalOn, True); n++;
	ToggleB = XmCreateToggleButton(parent, "Toggle Button", args, n);
	XtAddCallback(ToggleB, XmNvalueChangedCallback, CommonGenericCB, NULL);
	return(ToggleB);
}

Widget CreateScrollBar(Widget parent)
{
	int n;
	Widget ScrollB;

	n = 0;
	XtSetArg(args[n], XmNx, 300); n++;
	XtSetArg(args[n], XmNy, 150); n++;
	XtSetArg(args[n], XmNtraversalOn, True); n++;
	XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
	XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
	XtSetArg(args[n], XmNhighlightThickness, 2); n++;
	ScrollB = XmCreateScrollBar (parent, "ScrollB", args, n);   
	return(ScrollB);
}

