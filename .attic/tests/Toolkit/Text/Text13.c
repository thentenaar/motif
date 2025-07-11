/* $XConsortium: Text13.c /main/5 1995/07/15 21:10:14 drk $ */
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
 * 
 */
/*
 * HISTORY
 */

#include <testlib.h>

#define NUM_FONTS 3

static char *font_names[] = {
  "6x12", "fixed", "9x15" };

static char string1[] =
"Some meaningless text with which to test       \n\
123456781234567812345678123456781234567812345678\n\
the insertion of tabs into an XmText widget     \n\
123456781234567812345678123456781234567812345678\n\
totally, meaningless, garbage...................";

Widget Text1;

void
SetNewFont( Widget w, XtPointer client_data, XtPointer call_data )
{
	char *new_font = (char *)client_data;
	XmToggleButtonCallbackStruct *cb_data = 
                         (XmToggleButtonCallbackStruct *)call_data;
	Arg	args[25];
	int	n;

	if ( cb_data->set )
        {
           n = 0;
           XtSetArg( args[n], XmNfontList, CommonGetFontList( new_font )); n++;
           XtSetValues( Text1, args, n );
        }
}
           

void
ResetString( Widget w, XtPointer client_data, XtPointer call_data )
{
	Widget  text = (Widget)client_data;
	Arg	args[25];
	int	n;

        XmTextSetString( text, string1 );
	return;
}

void
main( int argc, char *argv[] )
{
	Widget	  Form, RowColumn, ToggleB[NUM_FONTS], PushB;
	Arg   	  args[25];
	int	  n, i;
	XmString  tmp_string;
	char      buffer[25];

	CommonTestInit( argc, argv );

	n = 0;
	XtSetArg( args[n], XmNallowShellResize, True ); n++;
        XtSetValues( Shell1, args, n );

	n = 0;
        Form = XmCreateForm( Shell1, "Form", args, n );
	XtManageChild( Form );

	n = 0;
	XtSetArg( args[n], XmNtopAttachment, XmATTACH_FORM ); n++;
	XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
	XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM ); n++;
	XtSetArg( args[n], XmNresizeWidth, True ); n++;
	XtSetArg( args[n], XmNcolumns, 48 ); n++;
	XtSetArg( args[n], XmNrows, 4 ); n++;
        XtSetArg( args[n], XmNvalue, string1 ); n++;
        XtSetArg( args[n], XmNeditMode, XmMULTI_LINE_EDIT ); n++;
        XtSetArg( args[n], XmNfontList, CommonGetFontList("6x12")); n++;
        Text1 = XmCreateScrolledText( Form, "Text1", args, n );
	XtManageChild( Text1 );

	n = 0;
	XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
	XtSetArg( args[n], XmNtopWidget, Text1 ); n++;
	XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
	XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM ); n++;
	XtSetArg( args[n], XmNradioBehavior, True ); n++;
	RowColumn = XmCreateRowColumn( Form, "RowColumn", args, n );
	XtManageChild( RowColumn );

	for ( i = 0; i < NUM_FONTS; i++ )
        {
	    tmp_string = XmStringGenerate( font_names[i], 
                                           XmFONTLIST_DEFAULT_TAG,
		                           XmCHARSET_TEXT, NULL );

	    n = 0;
            XtSetArg( args[n], XmNlabelString, tmp_string ); n++; 
            XtSetArg( args[n], XmNselectColor, CommonGetColor("green")); n++; 
            if ( i == 0 )
            {
              XtSetArg( args[n], XmNset, True ); n++; 
            }
            sprintf( buffer, "ToggleB%d", i );
	    ToggleB[i] = XmCreateToggleButton( RowColumn, buffer, args, n );
	    XtAddCallback( ToggleB[i], XmNvalueChangedCallback,
                           SetNewFont, (XtPointer)font_names[i] );
          
            XmStringFree( tmp_string );
        }

	XtManageChildren( ToggleB, NUM_FONTS );

	tmp_string = XmStringGenerate( "Reset String", 
                                       XmFONTLIST_DEFAULT_TAG,
	                               XmCHARSET_TEXT, NULL );

	n = 0;
	XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
	XtSetArg( args[n], XmNtopWidget, RowColumn ); n++;
	XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
	XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM ); n++;
        XtSetArg( args[n], XmNlabelString, tmp_string ); n++;
	PushB = XmCreatePushButton( RowColumn, "PushB", args, n );
	XtManageChild( PushB );

	XtAddCallback( PushB, XmNactivateCallback, ResetString, Text1 );

	XtRealizeWidget( Shell1 );

	CommonPause();

       /*
        *  Shrink the width of the Text widget.
        */
        n = 0; 
	XtSetArg( args[n], XmNcolumns, 20 ); n++;
	XtSetArg( args[n], XmNresizeWidth, False ); n++;
	XtSetValues( Text1, args, n );

	CommonPause();
	CommonPause();


	XtAppMainLoop( app_context );
}
