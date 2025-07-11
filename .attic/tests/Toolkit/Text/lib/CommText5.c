/* $TOG: CommText5.c /main/7 1997/10/02 16:20:47 csn $ */
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
#include <stdlib.h>

static Widget BBoard1 = NULL;
static Widget Text1 = NULL;
static Arg args[MAX_ARGS];

static char string1[] = "Sample Text";
static char string2[] = "Welcome to OSF/Motif";
static char string3[] = "W";
static char string4[] = "I";

static char font_name1[] = "8x13bold";
static char font_name2[] = "fixed";
static char font_name3[] = "variable";

static char render_tag[] = "render1";


static void UpdateVerifyCBStruct( XEvent *event, XmTextPosition *startPos, 
                                  XmTextPosition *endPos, char *text )
{
     static char    c = 'A';
     static int     position = 12;

     if (event == NULL)
	printf("Null event in ModifyVerifycallback\n");

     if (text != NULL)
	{
	   /* Make sure that it is not a BS */
           *startPos = position++;
           *endPos = *startPos;

        text[0] = c;
        text[1] = '\0';
	}

	c += 1;
	if (c > 'Z')
		c = 'A';
}

static void ModifyVerifyCB (Widget w, XtPointer client_data,
	XtPointer call_data)

{
     int            num_chars, buffer_size;

     XmTextVerifyCallbackStruct  *modify_verify;
     char	    *text;

     modify_verify = (XmTextVerifyCallbackStruct *)call_data;

     text = modify_verify->text->ptr;
     UpdateVerifyCBStruct( modify_verify->event,
			  &modify_verify->startPos,
			  &modify_verify->endPos,
			  text );

     modify_verify->text->length = 1;
}

static void UpdateVerifyCBStruct2( XmTextPosition *startPos, 
                                   XmTextPosition *endPos, char **text )
{
    static char c[] =
    "A234567890 234567890 234567890 234567890 234567890 234567890 234567890\n";
    static int position = 15;
    int	c_length;

    /* Make sure that it is not a BS */
    c_length = strlen(c);
    *startPos = position;
    position += c_length;
    *endPos = *startPos;
    *text=XtMalloc(c_length+1);
    strcpy(*text, c);
    

    *c += 1;
    if (*c > 'Z')
	    *c = 'A';
}

static void ModifyVerifyCB2 (Widget w, XtPointer client_data,
	XtPointer call_data)
{
    char *old_ptr, *ptr = NULL;

    XmTextVerifyCallbackStruct  *modify_verify;

    modify_verify = (XmTextVerifyCallbackStruct *)call_data;

    old_ptr = modify_verify->text->ptr;
    UpdateVerifyCBStruct2( &modify_verify->startPos,
			  &modify_verify->endPos,
			  &ptr );
    if ( old_ptr )
      XtFree( old_ptr );

    modify_verify->text->ptr = ptr;
    modify_verify->text->length = strlen( ptr );
}

static void ChangeString (Widget w, XtPointer client_data,
	XtPointer call_data)
{
        XmString  tcs = (XmString)0;
	int n = 0;
	char *value;

        n = 0;

	value = XmTextGetString(w);
	XmTextSetInsertionPosition(w, strlen(value));
	XtFree(value);

	XtSetArg(args[n], XmNmarginWidth, 10);  n++;
	XtSetArg(args[n], XmNmarginHeight, 10);  n++;
	XtSetArg(args[n], XmNwordWrap, False); n++;
	XtSetArg(args[n], XmNrows, 5);  n++;
	XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
	XtSetArg(args[n], XmNforeground, CommonGetColor("navy"));  n++;
	XtSetArg(args[n], XmNvalue, string2 ); n++;
	XtSetArg(args[n], XmNfontList, CommonGetFontList(font_name1));  n++;
	XtSetValues(w, args, n);

        if ( tcs )
           XmStringFree(tcs);
}

void setwidth( Widget w, XtPointer client_data, XtPointer call_data )
{
   int  n;

     XmScaleCallbackStruct *scale_cb = (XmScaleCallbackStruct *)call_data;

     n = 0;
     XtSetArg( args[n], XmNwidth, scale_cb->value ); n++;
     XtSetValues( Text1, args, n );
}

void setstr( Widget w, XtPointer client_data, XtPointer call_data )
{
   int  n;
   XmString  string = (XmString)0;

     n = 0;

   XtSetArg( args[n], XmNvalue, string3 ); n++;
     XtSetValues( Text1, args, n );
     if ( string )
        XmStringFree(string);
}


void readout( Widget w, XtPointer client_data, XtPointer call_data )
{
  int  n;
  short col;
  Dimension wid;

     n = 0;
     XtSetArg(args[n], XmNcolumns, &col ); n++;
     XtSetArg( args[n], XmNwidth, &wid ); n++;
     XtGetValues( Text1, args, n );

     printf("%d columns, width %d\n", col, wid);
}


void main_text5 (argc, argv)
     int argc;
     char **argv;
{
        Widget dialog, scale, setbut, readbut;
        int n = 0;
        Dimension wid;
        XmString cs_string1 = (XmString)0;

	/*  initialize toolkit  */
	 CommonTestInit(argc, argv);

	/* create bulletin board child to make allowresize work okay */

	n = 0;
	XtSetArg (args[n], XmNmarginHeight, 0); n++;
	XtSetArg (args[n], XmNmarginWidth, 0); n++;
	BBoard1 = XmCreateBulletinBoard(Shell1, "BBoard1", args, n);
	XtManageChild(BBoard1);

	XtRealizeWidget(Shell1);


	/* Test case for PIR 2757 */
	n = 0;
	XtSetArg(args[n], XmNrows, 1); n++;
	XtSetArg(args[n], XmNcolumns, 1); n++;
	XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
	XtSetArg(args[n], XmNwordWrap, False); n++;
	XtSetArg(args[n], XmNresizeHeight, True); n++;
	XtSetArg(args[n], XmNresizeWidth, True); n++;
	XtSetArg(args[n], XmNvalue, string1); n++;
	Text1 = XmCreateText(BBoard1, "Text1", args, n);
	XtManageChild(Text1);

	CommonPause();

	XtDestroyWidget(Text1);

	n = 0;
	XtSetArg(args[n], XmNresizeWidth, True);  n++;
	XtSetArg(args[n], XmNresizeHeight, True);  n++;
	XtSetArg(args[n], XmNcolumns, 40);  n++;
	XtSetArg(args[n], XmNrows, 1);  n++;
	XtSetArg(args[n], XmNeditMode, XmSINGLE_LINE_EDIT); n++;
	XtSetArg(args[n], XmNcursorPosition, 12);  n++;
	XtSetArg(args[n], XmNvalue, string1); n++;
	XtSetArg(args[n], XmNfontList, CommonGetFontList(font_name2));  n++;
	XtSetArg(args[n], XmNforeground, CommonGetColor("Red"));  n++;
	XtSetArg(args[n], XmNbackground, CommonGetColor("White"));  n++;
	Text1 = XmCreateText(BBoard1, "Text1", args, n);
	XtManageChild(Text1);


	CommonPause();

	XtAddCallback(Text1, XmNmodifyVerifyCallback, ModifyVerifyCB, 
                             (XtPointer)0);

	CommonPause();

	XtRemoveAllCallbacks (Text1, XmNmodifyVerifyCallback);
	XtAddCallback(Text1, XmNactivateCallback, ChangeString, 
                             (XtPointer)0);

	CommonPause();

	/* test case for PIR 2230 */
	XtRemoveAllCallbacks (Text1, XmNactivateCallback);
	XtAddCallback(Text1, XmNmodifyVerifyCallback, ModifyVerifyCB2, 
                             (XtPointer)0);

	CommonPause();
	/* end of test case for PIR 2230 */

        if ( cs_string1 )
           XmStringFree(cs_string1);

        /* test case for CR 5380 */
        n = 0;
        XtSetArg( args[n], XmNallowShellResize, False); n++;
        XtSetValues(Shell1, args, n);

        XtUnmanageChild(Text1);
        XtDestroyWidget(Text1);

        n = 0;
	XtSetArg( args[n], XmNvalue, string4 ); n++;
	XtSetArg( args[n], XmNfontList,
		 CommonGetFontList(font_name3));  n++;
        XtSetArg( args[n], XmNcolumns, 20 ); n++;
        XtSetArg( args[n], XmNeditMode, XmSINGLE_LINE_EDIT );  n++;
        Text1 = XmCreateText( BBoard1, "Text1", args, n );
        XtManageChild(Text1);

        XmStringFree( cs_string1 );

        n = 0;
        XtSetArg( args[n], XmNwidth, &wid ); n++;
        XtGetValues(Text1, args, n);

        n = 0;
        XtSetArg(args[n], XmNautoUnmanage, False); n++;
        dialog = XmCreateFormDialog(Shell1, "Controls", args, n);

        cs_string1 = XmStringCreateSimple("Width");

        n = 0;
        XtSetArg(args[n], XmNmaximum, wid ); n++;
        XtSetArg(args[n], XmNminimum, 0 ); n++;
        XtSetArg(args[n], XmNorientation, XmHORIZONTAL ); n++;
        XtSetArg(args[n], XmNshowValue, True ); n++;
        XtSetArg(args[n], XmNtitleString, cs_string1 ); n++;
        XtSetArg(args[n], XmNvalue, wid ); n++;
        XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM ); n++;


	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
        XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM ); n++;
        scale = XmCreateScale(dialog, "scale", args, n);
        XtManageChild(scale);

        XmStringFree(cs_string1);

        XtAddCallback(scale, XmNvalueChangedCallback, setwidth, (XtPointer)0);

        cs_string1 = XmStringCreateSimple("Set String");

        n = 0;
        XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
        XtSetArg(args[n], XmNtopWidget, scale ); n++;
        XtSetArg(args[n], XmNlabelString, cs_string1 ); n++;
        setbut = XmCreatePushButton(dialog, "set_string", args, n);
        XtManageChild(setbut);

        XtAddCallback(setbut, XmNactivateCallback, setstr, NULL);

        XmStringFree(cs_string1);

        cs_string1 = XmStringCreateSimple("Get Values");

        n = 0;
        XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
        XtSetArg(args[n], XmNtopWidget, scale ); n++;
        XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET ); n++;
        XtSetArg(args[n], XmNleftWidget, setbut ); n++;
        XtSetArg(args[n], XmNlabelString, cs_string1 ); n++;
        readbut = XmCreatePushButton(dialog, "get_values", args, n);
        XtManageChild(readbut);

        XtAddCallback(readbut, XmNactivateCallback, readout, NULL);

        XmStringFree(cs_string1);

        XtManageChild(dialog);

        CommonPause();
        /* end test case 5380 */

        CommonPause();
        XtAppMainLoop(app_context);
}
