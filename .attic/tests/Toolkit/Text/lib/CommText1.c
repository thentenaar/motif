/* $XConsortium: CommText1.c /main/6 1995/07/15 21:10:44 drk $ */
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

static char font[] = "-adobe-courier-medium-r-normal--14-140-75-75-m-90-iso8859-1";

static char render_tag[] = "render1";

static char string0[] = "Some Text.";

static char string1[] = "\
The ittsie bittsie spider\n\
went up the water spout.\n\
Down came the rain and\n\
washed the spider out.\n\
Out came the sun and\n\
dried up all the rain.\n\
And the ittsie bittsie spider\n\
went up the spout again.";

static char string2[] = "Spider Lives!";
 
  
void
main_text1 (int argc, char **argv)
{
	Arg args[MAX_ARGS];
	int n;
	Widget SWin = NULL;
	Widget Text1 = NULL;
	unsigned char scrollingPolicy;
        XmString      tcs;
        XmRendition   rendition;
        XmRenderTable renderTab = NULL;
 	XFontStruct   *fontStruct;


	/*  initialize toolkit  */
	CommonTestInit(argc, argv);
	 
	n = 0;
	/* test case for CR 4356 */
	XtSetArg(args[n], XmNscrollingPolicy, (unsigned char) 42); n++;
	SWin = XmCreateScrolledWindow(Shell1, "SWin", args, n);
	XtManageChild (SWin);

	/* test case for CR 4356 */
	n = 0;
	XtSetArg(args[n], XmNscrollingPolicy, &scrollingPolicy); n++;
	XtGetValues(SWin, args, n);
	if (scrollingPolicy == XmAPPLICATION_DEFINED) 
	{
	    printf("GetValues of scrollingPolicy = XmAPPLICATION_DEFINED\n");
	}
	else
	{
	    printf("GetValues of scrollingPolicy = %d \n",
			scrollingPolicy);
	}


       /*  Load font used by XmCSText widget. */
        fontStruct = XLoadQueryFont( display, font );

	n = 0;
/*
 * Added as a test for PIR #768		03-14-1990
 */
        tcs = XmStringCreate( string0, XmFONTLIST_DEFAULT_TAG );
	XtSetArg(args[n], XmNrows, 4);  n++;
	XtSetArg(args[n], XmNcolumns, 28);  n++;
	XtSetArg(args[n], XmNscrollVertical, True);  n++;
	XtSetArg(args[n], XmNscrollHorizontal, True);  n++;
	XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT);  n++;
     
	XtSetArg(args[n], XmNvalue, string0); n++;
	XtSetArg(args[n], XmNfontList, CommonGetFontList(font)); n++;
	Text1 = XmCreateText(SWin, "Text1", args, n);
	XtManageChild (Text1);
        XmStringFree(tcs);

	XtRealizeWidget(Shell1);
	
	CommonPause();
	CommonPause();
	CommonPause();
	CommonPause();
	CommonPause();
       
       /*  Test case for CR 5166. */
        tcs = XmStringCreateLtoR( string1, XmFONTLIST_DEFAULT_TAG );

        n = 0;
        XtSetArg( args[n], XmNrows, 2 ); n++;
        XtSetArg( args[n], XmNeditable, False ); n++;
	XtSetArg(args[n], XmNvalue, string1 ); n++;
        XtSetValues( Text1, args, n );
        XmStringFree(tcs);

	CommonPause();

	XmTextSetString( Text1, string2 );
         
	CommonPause();
	CommonPause();

	XtAppMainLoop(app_context);

}
