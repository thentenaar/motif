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
static char rcsid[] = "$XConsortium: TextField3.c /main/8 1995/07/13 19:35:15 drk $"
#endif
#endif


#include <testlib.h>

Arg args[MAX_ARGS];

static char string1[] = "This line is exactly 40 characters long.";

static void ModifyVerifyCB(Widget w, XtPointer client_data,
	XtPointer call_data)
{
	XmTextVerifyCallbackStruct  *modify_verify;
	static char c = 'A';
	static int position = 12;

	modify_verify = (XmTextVerifyCallbackStruct *)call_data;

	if (modify_verify->event == NULL)
		printf("Null event in ModifyVerify callback\n");

	if (modify_verify->text->ptr != NULL) 
	{  /* Make sure that it is not a BS */
		modify_verify->startPos = position++;
		modify_verify->endPos = modify_verify->startPos;
		modify_verify->text->ptr[0] = c;
		modify_verify->text->length = 1;
	}

	c += 1;
	if (c > 'Z')
		c = 'A';
}

static void ChangeString(Widget w, XtPointer client_data,
	XtPointer call_data)
{
	int n = 0;
	char *value;

	value = XmTextFieldGetString(w);
	XmTextFieldSetInsertionPosition(w, strlen(value));
	XtFree(value);

	XtSetArg(args[n], XmNfontList, CommonGetFontList("8x13bold"));  n++;
	XtSetArg(args[n], XmNmarginWidth, 10);  n++;
	XtSetArg(args[n], XmNmarginHeight, 10);  n++;
	XtSetArg(args[n], XmNforeground, CommonGetColor("navy"));  n++;
	XtSetArg(args[n], XmNvalue, "Fenway !!!"); n++;
	XtSetValues(w, args, n);
}

/* test case for PIR 2492
 *   Type in a string as per bug report;
 *   hit return;
 *   note that the new string is drawn properly (not off-screen).
 *   On even activates, change string with set string; on odd, use setvalues
 */

static void ChangeString2(Widget w, XtPointer client_data,
	XtPointer call_data)
{
	static Boolean toggle = False;
	int n = 0;
	
	if (!toggle)
	{
		XmTextFieldSetString((Widget)client_data,
		"This is a much longer string which overflows the display");
	}
	else
	{
		n = 0;
		XtSetArg(args[0],XmNvalue,"Now is the time");  n++;
		XtSetValues((Widget)client_data, args, n);
	}
	toggle = !toggle;
}

void main (argc, argv)
int argc;
char **argv;
{
	Widget Text1 = NULL;
        String str;
	int n = 0;
	
	/*  initialize toolkit  */
	CommonTestInit(argc, argv);
	
	/* test case for PIR 4031 */
	n = 0;
	XtSetArg(args[n], XmNwidth, 10); n++;
	XtSetArg(args[n], XmNheight, 10); n++;
	XtSetValues(Shell1, args, n);
	XtRealizeWidget(Shell1);
	/* end of test case for PIR 4031 */
	
	n = 0;
	XtSetArg(args[n], XmNfontList, CommonGetFontList("fixed"));  n++;
	XtSetArg(args[n], XmNcursorPosition, 12);  n++;
	XtSetArg(args[n], XmNvalue, "Sample Text ");  n++;
	XtSetArg(args[n], XmNforeground, CommonGetColor("Red"));  n++;
	XtSetArg(args[n], XmNbackground, CommonGetColor("White"));  n++;
	Text1 = XmCreateTextField(Shell1, "Text1", args, n);
	
	XtManageChild(Text1);
	XtRealizeWidget(Shell1);
	
	n = 0;
	XtSetArg(args[n], XmNresizeWidth, False);  n++;
	XtSetArg(args[n], XmNcolumns, 40);  n++;
	XtSetValues(Text1, args, n);
	
	CommonPause();
	
	/* Begin test for PIR 2959 */
	XtAddCallback(Text1, XmNmodifyVerifyCallback, ModifyVerifyCB, NULL);
	
	CommonPause();
	/* End test for PIR 2959 *
	
	/* Begin Test for PIR 2663 */
	XtRemoveAllCallbacks (Text1, XmNmodifyVerifyCallback);
	XtAddCallback(Text1, XmNactivateCallback, ChangeString, NULL);
	CommonPause();
	/* End Test for PIR 2663 */
	
	/* Begin Test for PIR 2942, 4077 */
	XtRemoveAllCallbacks(Text1, XmNactivateCallback);
	XtAddCallback(Text1, XmNactivateCallback, ChangeString2, Text1);
	CommonPause();
	/* End Test for PIR 2942, 4077 */

        /*  Begin test case for CR 4803 */

       /*  Turn bell on for Shell. */
         n = 0;
         XtSetArg(args[0], XmNaudibleWarning, XmBELL); n++;
         XtSetValues(Shell1, args, n);
         XtDestroyWidget(Text1);

         n = 0;
         XtSetArg(args[n], XmNmaxLength, 40); n++;
         XtSetArg(args[n], XmNfontList, CommonGetFontList("fixed"));  n++;
         XtSetArg(args[n], XmNcursorPosition, 12);  n++;
         XtSetArg(args[n], XmNvalue, "Sample Text ");  n++;
         XtSetArg(args[n], XmNforeground, CommonGetColor("Red"));  n++;
         XtSetArg(args[n], XmNbackground, CommonGetColor("White"));  n++;
         XtSetArg(args[n], XmNvalue, string1);  n++;
         XtSetArg(args[n], XmNeditable, True);  n++;
         Text1 = XmCreateTextField(Shell1, "Text1", args, n);
         XtManageChild(Text1);

         CommonPause();

       /*  Turn bell off for Shell. */
         XtSetArg(args[0], XmNaudibleWarning, XmNONE);
         XtSetValues(Shell1, args, 1);
         XtDestroyWidget(Text1);

         n = 0;
         XtSetArg(args[n], XmNfontList, CommonGetFontList("fixed"));  n++;
         XtSetArg(args[n], XmNcursorPosition, 12);  n++;
         XtSetArg(args[n], XmNvalue, "Sample Text ");  n++;
         XtSetArg(args[n], XmNforeground, CommonGetColor("Red"));  n++;
         XtSetArg(args[n], XmNbackground, CommonGetColor("White"));  n++;
         XtSetArg(args[n], XmNmaxLength, 40); n++;
         XtSetArg(args[n], XmNvalue, string1);  n++;
         XtSetArg(args[n], XmNeditable, True);  n++;
         Text1 = XmCreateTextField(Shell1, "Text1", args, n);

         XtManageChild(Text1);

         CommonPause();
        /* End test for OSF 4803 */

	XtDestroyWidget (Text1);

	/* begin test for CR 5258 */
	str = "The cursor should be here.  If not, an error occurred.";
	n = 0;
        XtSetArg (args[n], XmNcursorPosition, 26);n++;
	XtSetArg (args[n], XmNcolumns, 60); n++;
	XtSetArg (args[n], XmNeditable, False); n++;
	XtSetArg (args[n], XmNresizeWidth, True); n++;
	XtSetArg (args[n], XmNvalue, str); n++;
	Text1 = XmCreateTextField(Shell1, "Text1", args, n);
	XtManageChild(Text1);
 
 /* end test for CR 5258 */
       CommonPause();
       CommonPause();
	
	XtAppMainLoop(app_context);
}
