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
static char rcsid[] = "$XConsortium: TextWcs2.c /main/7 1995/07/13 19:38:18 drk $"
#endif
#endif


#include <testlib.h>

#ifndef	MOTIF1_1

Widget BBoard1 = NULL;
Arg args[MAX_ARGS];
static char starting_string[] = "Sample Text ";

static void ModifyVerifyCB (Widget w, XtPointer client_data,
	XtPointer call_data)
{
	XmTextVerifyCallbackStructWcs  *modify_verify;
	static char c = 'A';
	static wchar_t wc;
	static int position = 12;
	static int nbytes;

	modify_verify = (XmTextVerifyCallbackStructWcs *)call_data;

	if (modify_verify->event == NULL)
		printf("Null event in ModifyVerify callback\n");

	if (modify_verify->text->wcsptr != NULL)
	{
		/* Make sure that it is not a BS */
		nbytes = mbtowc(&wc, &c, 1);
		modify_verify->startPos = position++;
		modify_verify->endPos = modify_verify->startPos;
		modify_verify->text->wcsptr[0] = wc;
		modify_verify->text->length = nbytes;
	}

	c += 1;
	if (c > 'Z')
		c = 'A';
}

static void ModifyVerifyCB2 (Widget w, XtPointer client_data,
	XtPointer call_data)
{
	XmTextVerifyCallbackStructWcs  *modify_verify;
	static char c[] =
"A234567890 234567890 234567890 234567890 234567890 234567890 234567890";
	static int position = 12;
	wchar_t *wcsptr;
	int	c_length;

	modify_verify = (XmTextVerifyCallbackStructWcs *)call_data;

	if (modify_verify->text->wcsptr != NULL)
	{
		/* Make sure that it is not a BS */
		c_length = strlen(c);
		modify_verify->startPos = position;
		position += c_length;
		modify_verify->endPos = modify_verify->startPos;
		XtFree((char *)modify_verify->text->wcsptr);
		wcsptr = (wchar_t *)XtMalloc((unsigned)
			(c_length + 1) * sizeof(wchar_t));
		modify_verify->text->length =
			mbstowcs(wcsptr, c, c_length + 1);
		modify_verify->text->wcsptr = wcsptr;
	}

	*c += 1;
	if (*c > 'Z')
		*c = 'A';
}

#endif	/* MOTIF1_1 */

void main (argc, argv)
int argc;
char **argv;
{
	Widget	Text1 = NULL;
	int	n = 0;
	wchar_t	*starting_wchar;
	int	num_chars;

#ifndef	MOTIF1_1

	/*  initialize toolkit  */
	CommonTestInit(argc, argv);

	/* create bulletin board child to make allowresize work okay */

	n = 0;
	XtSetArg (args[n], XmNmarginHeight, 0); n++;
	XtSetArg (args[n], XmNmarginWidth, 0); n++;
	BBoard1 = XmCreateBulletinBoard(Shell1, "BBoard1", args, n);
	XtManageChild(BBoard1);

	XtRealizeWidget(Shell1);

	num_chars = strlen(starting_string);
	starting_wchar =
		(wchar_t *)XtMalloc((unsigned)
			(num_chars + 1) * sizeof(wchar_t));
	mbstowcs(starting_wchar, starting_string, num_chars + 1);

	n = 0;
	XtSetArg(args[n], XmNfontList, CommonGetFontList("fixed"));  n++;
	XtSetArg(args[n], XmNresizeWidth, False);  n++;
	XtSetArg(args[n], XmNresizeHeight, False);  n++;
	XtSetArg(args[n], XmNcolumns, 40);  n++;
	XtSetArg(args[n], XmNrows, 1);  n++;
	XtSetArg(args[n], XmNeditMode, XmSINGLE_LINE_EDIT); n++;
	XtSetArg(args[n], XmNcursorPosition, 12);  n++;
	XtSetArg(args[n], XmNvalueWcs, starting_wchar);  n++;
	XtSetArg(args[n], XmNforeground, CommonGetColor("Red"));  n++;
	XtSetArg(args[n], XmNbackground, CommonGetColor("White"));  n++;
	Text1 = XmCreateText(BBoard1, "Text1", args, n);
	XtFree((char *)starting_wchar);

	XtManageChild(Text1);

	XtAddCallback(Text1, XmNmodifyVerifyCallbackWcs, ModifyVerifyCB, NULL);

	CommonPause();

	XtRemoveAllCallbacks(Text1, XmNmodifyVerifyCallbackWcs);

	n = 0;
	XtSetArg(args[n], XmNwordWrap, True); n++;
	XtSetArg(args[n], XmNrows, 5);  n++;
	XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
	XtSetValues(Text1, args, n);

	XtAddCallback(Text1, XmNmodifyVerifyCallbackWcs, ModifyVerifyCB2, NULL);

	CommonPause();

	XtAppMainLoop(app_context);

#else	/* MOTIF1_1 */

	printf("This is not a Motif 1.1 test.\n");
	exit(0);

#endif	/* MOTIF1_1 */
}
