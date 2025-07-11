/* $TOG: I18N1.c /main/6 1997/09/15 14:24:41 cshi $ */
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

/*
 *  Declare local functions.
 */


/*
 *  Declare global variables.
 */
#define MY_OFFSET 30
/* 
 *  Callback functions 
 */

/*
 *  Main
 */
void  main (argc, argv)
    int     argc;
    char  **argv;
{
    Widget           Form1;
    Widget           TextField1, TextFieldLabel;
    Widget           Text1, TextLabel, Text2;
    Arg              args[20];
    int              n, test_num;
    char             TestName[20], test_char;

    TestName[0] = '\0';
    test_num = 1;
    test_char = 'a';

    CommonTestI18NInit(argc, argv, NULL, NULL);

    if (UserData != NULL) {

       if (strcmp(UserData, "a") == 0)
	       test_num = 1;
       else if (strcmp(UserData, "b") == 0)
               test_num = 2;
       else if (strcmp(UserData, "c") == 0)
               test_num = 3;

       test_char = *UserData;

       free(UserData);
    }
    
    sprintf(TestName, "I18N1%c", test_char);

    n = 0;
    XtSetArg(args[n], XmNmarginHeight, 15); n++;
    XtSetArg(args[n], XmNmarginWidth, 15); n++;
    Form1 = XmCreateForm(Shell1, TestName, args, n);
    XtManageChild (Form1);

    /*
     *  Creates TextFieldLabel and TextField1.
     */

    n = 0;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING );   n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM );       n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM );      n++;
    TextFieldLabel = XmCreateLabelGadget(Form1, "TextFieldLabel", args, n);

    XtManageChild(TextFieldLabel);

    n = 0;
    XtSetArg(args[n], XmNcolumns, 40); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET );      n++;
    XtSetArg(args[n], XmNtopWidget, TextFieldLabel );           n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM );      n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM );       n++;
    XtSetArg(args[n], XmNleftOffset, MY_OFFSET );               n++;
    TextField1 = XmCreateTextField(Form1, "TextField1", args, n);

    XtManageChild(TextField1);

    /*
     *  Creates TextLabel, Text1 and Text2.
     */

    n = 0;

    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING );    n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET );      n++;
    XtSetArg(args[n], XmNtopWidget, TextField1 );               n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM );       n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING );    n++;
    TextLabel = XmCreateLabelGadget(Form1, "TextLabel", args, n );
   
    XtManageChild(TextLabel);

    n = 0;

    XtSetArg(args[n], XmNcolumns, 20); n++;
    XtSetArg(args[n], XmNrows, 5);			        n++;
    XtSetArg(args[n], XmNscrollVertical, True);                 n++;
    XtSetArg(args[n], XmNscrollHorizontal, True);               n++;
    XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); 		n++;
    XtSetArg(args[n], XmNwordWrap, True); 			n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET );      n++;
    XtSetArg(args[n], XmNtopWidget, TextLabel );                n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM );       n++;
    XtSetArg(args[n], XmNleftOffset, MY_OFFSET );               n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM );      n++;
    Text1 = XmCreateScrolledText(Form1, "Text1", args, n);

    XtManageChild(Text1);

    n = 0;

    XtSetArg(args[n], XmNcolumns, 20); n++;
    XtSetArg(args[n], XmNrows, 5);			        n++;
    XtSetArg(args[n], XmNscrollVertical, True);                 n++;
    XtSetArg(args[n], XmNscrollHorizontal, True);               n++;
    XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); 		n++;
    XtSetArg(args[n], XmNwordWrap, True); 			n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET );      n++;
    XtSetArg(args[n], XmNtopWidget, Text1 );                    n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM );       n++;
    XtSetArg(args[n], XmNleftOffset, MY_OFFSET );               n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM );      n++;
    Text2 = XmCreateScrolledText(Form1, "Text2", args, n);

    XtManageChild(Text2);

    XtRealizeWidget(Shell1);

    CommonPause();

    if (test_num == 2) {

	/* Get the Text1 value, convert it to wide char and redisplay it 
	 * in Text2.
	 */

	char *text_value;
	wchar_t text_wc_value[151];
	size_t len_wcs;

	n = 0;
	XtSetArg(args[n], XmNvalue, &text_value); n++;
	XtGetValues(Text1, args, n);

	len_wcs = mbstowcs(text_wc_value, text_value, 150);

	if (len_wcs == 100 || len_wcs == (size_t) -1)
	    printf("Multibyte to wide char conversion failed.\n");

	n = 0;
	XtSetArg(args[n], XmNvalueWcs, text_wc_value); n++;
	XtSetValues(Text2, args, n);
	
	CommonPause();

    }

    CommonPause();
 
    XtAppMainLoop(app_context);
}


