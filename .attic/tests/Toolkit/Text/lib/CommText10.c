/* $TOG: CommText10.c /main/8 1997/10/02 15:59:09 csn $ */
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

#define	TB_COUNT	9
#define	NUM_COLUMNS	20

typedef struct _FontCBData {
  Widget	widget;
  char        	*fontname;
} FontCBData;
  
/*
 *  Global variables.
 */
static char new_string2[] = "\
123456789012345678901234567890\n\
mmmmmmmmmmmmmmmmmmmmmmmmmmmmmm\n\
000000000000000000000000000000";

static char new_string3[] = "012345";

static char render_tag[] = "render1";

Widget RowCol1, Label1; 
Widget Text1, Text2;
Widget fonttitle1;
XFontStruct  *fontStruct0, *fontStruct1, *fontStruct2;

void printvalues();

static void fontTBCB1(Widget w, XtPointer client_data,
				XtPointer call_data)
{
	Arg args[MAX_ARGS];
	int n;
        XmRendition   rendition;
        XmRenderTable renderTab = NULL;
        FontCBData    *fontCBData = (FontCBData *)client_data; 
        Widget	      widget;
	char	      *fontname;

	widget = fontCBData->widget;
        fontname = fontCBData->fontname;

	n = 0;
	XtSetArg(args[n], XmNfontList, CommonGetFontList(fontname));  n++;
	XtSetValues(Text1, args, n);
	printvalues("after Text1 ->");
	XtSetValues(Text2, args, n);
	printvalues("after Text2 ->");

        XmRenderTableFree( renderTab );

}

static void fontTBCB2(Widget w, XtPointer client_data,
				XtPointer call_data)
{
	Arg args[MAX_ARGS];
	int n;
        XmRendition   rendition;
        XmRenderTable renderTab = NULL;
        FontCBData    *fontCBData = (FontCBData *)client_data; 
        Widget	      widget;
	char	      *fontname;
	unsigned int  em_space;
	char          buffer[25];
	XmToggleButtonCallbackStruct *toggle_data =
	  (XmToggleButtonCallbackStruct *)call_data;

	/* Don't do anything if this toggle is being turned off */
	if (!toggle_data->set) return;

	widget = fontCBData->widget;
        fontname = fontCBData->fontname;

        fontStruct2 = XLoadQueryFont( display, fontname ); 

        n = 0;
        XtSetArg(args[n], XmNfont, (XtPointer)fontStruct2);  n++;
        XtSetArg(args[n], XmNfontType, XmFONT_IS_FONT );  n++;
        XtSetArg(args[n], XmNloadModel, XmLOAD_IMMEDIATE );  n++;
        rendition = XmRenditionCreate( Text1, render_tag, args, n );

        renderTab = XmRenderTableAddRenditions( renderTab, &rendition, 1,
                      XmMERGE_REPLACE );

        XmRenditionFree( rendition ); 

	n = 0;
	XtSetArg(args[n], XmNrenderTable, renderTab);  n++;
	XtSetValues(Text1, args, n);

	XmRenderTableFree( renderTab );
}

void printvalues(char *tag)
{
	Arg args[MAX_ARGS];
	int n;
	short	cols;
	Dimension    width;

	printf("%s:\n", tag);
	n = 0;
	XtSetArg(args[n], XmNwidth, &width); n++;
	XtSetArg(args[n], XmNcolumns, &cols); n++;

	XtGetValues(Text1, args, n);
	printf("  Text1  width %d  columns %d\n", width, cols);
	XtGetValues(Text2, args, n);
	printf("  Text2  width %d  columns %d\n\n", width, cols);
}


main_text10 (argc, argv)
     int argc;
     char **argv;
{
	Widget	fontTB1[TB_COUNT], fontRB1;
	char	*fontname[TB_COUNT];
	int	i;
	Arg	args[MAX_ARGS];
	int	n;
        XmString tmp_string;
        FontCBData  fontCB1, fontCB2, fontCB3[TB_COUNT];

	/*  initialize toolkit  */
	CommonTestInit(argc, argv);
    
	/* create main rowcolumn */
	n = 0;
	XtSetArg(args[n], XmNadjustLast, False);  n++; 
	/* fix for PIR 3317 */
	XtSetArg(args[n], XmNpacking, XmPACK_NONE);n++;  
	/* end of fix for PIR 3317 */
	RowCol1 = XmCreateRowColumn(Shell1, "RowCol1", args, n);
	XtManageChild(RowCol1);

#ifdef	MOTIF1_1
	n = 0;
	XtSetArg(args[n], XmNwidth, 350); n++;
	XtSetArg(args[n], XmNheight, 375); n++;
	XtSetValues(Shell1, args, n);
#endif /* MOTIF1_1 */

	n = 0;
	XtSetArg(args[n], XmNrows, 1);  n++;
	XtSetArg(args[n], XmNcolumns, NUM_COLUMNS);  n++;
	XtSetArg(args[n], XmNvalue, new_string3); n++;
	Text1 = XmCreateText(RowCol1, "Text1", args, n);
	XtManageChild(Text1);

	n = 0;
	XtSetArg(args[n], XmNy, 40);  n++;
	XtSetArg(args[n], XmNrows, 1);  n++;
	XtSetArg(args[n], XmNcolumns, NUM_COLUMNS);  n++;
	XtSetArg(args[n], XmNvalue, new_string3); n++;
	Text2 = XmCreateText(RowCol1, "Text2", args, n);
	XtManageChild(Text2);

	n = 0;
        tmp_string = XmStringCreate("text font", XmSTRING_DEFAULT_CHARSET); 
	XtSetArg(args[n], XmNy, 80);  n++;
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	fonttitle1 = XmCreateLabel(RowCol1, "fonttitle", args, n);
	XtManageChild(fonttitle1);
        XmStringFree (tmp_string);

	n = 0;
	XtSetArg(args[n], XmNy, 105);  n++;
	XtSetArg(args[n], XmNborderWidth, 2); n++;
	XtSetArg(args[n], XmNisHomogeneous, False); n++;
	fontRB1 = XmCreateRadioBox(RowCol1, "fontRB1", args, n);

	n = 0;
        tmp_string = XmStringCreate("helvetica1", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	XtSetArg(args[n], XmNset, True); n++;
	fontTB1[0] = XmCreateToggleButtonGadget(fontRB1, "RB1_helv1", args, n);
        XmStringFree (tmp_string);

	n = 0;
        tmp_string = XmStringCreate("helvetica2", XmSTRING_DEFAULT_CHARSET); 
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	fontTB1[1] = XmCreateToggleButtonGadget(fontRB1, "RB1_helv2", args, n);
        XmStringFree(tmp_string);

	fontname[0] =
	"-adobe-helvetica-bold-r-normal--10-100-75-75-p-60-iso8859-1";
	fontname[1] =
	"-adobe-helvetica-bold-r-normal--14-140-75-75-p-82-iso8859-1";


	/* test case for PIR 3317 */

        
        fontCB1.widget = Text1;
        fontCB1.fontname = fontname[0];
	XtAddCallback(fontTB1[0], XmNarmCallback,
		fontTBCB1, (XtPointer) &fontCB1);

        fontCB2.widget = Text1;
        fontCB2.fontname = fontname[1];
	XtAddCallback(fontTB1[1], XmNarmCallback,
		fontTBCB1, (XtPointer) &fontCB2);


	n = 0; /* set this as first font */
	XtSetArg(args[n], XmNfontList, CommonGetFontList(fontname[0])); n++;
	XtSetValues(Text1, args, n);
	XtSetValues(Text2, args, n);


	XtManageChild(fontRB1);
	XtManageChild(fontTB1[0]);
	XtManageChild(fontTB1[1]);

	XtRealizeWidget(Shell1);
	XSynchronize(XtDisplay(Shell1), True); 

	n = 0;
	XtSetArg(args[n], XmNwidth, 350); n++;
	XtSetArg(args[n], XmNheight, 375); n++;
	XtSetArg(args[n], XmNresizeHeight, False);n++;  
	XtSetArg(args[n], XmNresizeWidth, False);n++;  
	XtSetValues(RowCol1, args, n);

	CommonPause();
	/* end of test case for PIR 3317 */


	/* start with a new text widget and font radiobox */
	XtDestroyWidget(Text1);
	XtDestroyWidget(Text2);
	XtUnmanageChild(fontTB1[0]);
	XtUnmanageChild(fontTB1[1]);
	XtUnmanageChild(fontRB1);

	n = 0;
	XtSetArg(args[n], XmNpacking, XmPACK_NONE);n++;  
	XtSetValues(RowCol1, args, n);

	i = 2;
	n = 0;
        tmp_string = XmStringCreate("helvetica3", XmSTRING_DEFAULT_CHARSET); 
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
		 
	fontTB1[i] = XmCreateToggleButtonGadget(fontRB1, "RB1_helv3", args, n);
	i++;
        XmStringFree (tmp_string);

	n = 0;
        tmp_string = XmStringCreate("6x12", XmSTRING_DEFAULT_CHARSET); 
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
		 
	fontTB1[i] = XmCreateToggleButtonGadget(fontRB1, "RB1_6x12", args, n);
	i++;
        XmStringFree (tmp_string);

	n = 0;
        tmp_string = XmStringCreate("9x15", XmSTRING_DEFAULT_CHARSET); 
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	fontTB1[i] = XmCreateToggleButtonGadget(fontRB1, "RB1_9x15", args, n);
	i++;
        XmStringFree (tmp_string);

	n = 0;
        tmp_string = XmStringCreate("courier1", XmSTRING_DEFAULT_CHARSET); 
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	fontTB1[i] = XmCreateToggleButtonGadget(fontRB1, "RB1_cour1", args, n);
	i++;
        XmStringFree (tmp_string);

	n = 0;
        tmp_string = XmStringCreate("courier2", XmSTRING_DEFAULT_CHARSET); 
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	fontTB1[i] = XmCreateToggleButtonGadget(fontRB1, "RB1_cour2", args, n);
	i++;
        XmStringFree (tmp_string);

	n = 0;
        tmp_string = XmStringCreate("times1", XmSTRING_DEFAULT_CHARSET); 
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	fontTB1[i] = XmCreateToggleButtonGadget(fontRB1, "RB1_times1", args, n);
	i++;
        XmStringFree (tmp_string);

	n = 0;
        tmp_string = XmStringCreate("times2", XmSTRING_DEFAULT_CHARSET); 
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	fontTB1[i] = XmCreateToggleButtonGadget(fontRB1, "RB1_times2", args, n);
	i++;
        XmStringFree (tmp_string);

	i = 0;
	fontname[i] =
	"-adobe-helvetica-bold-r-normal--10-100-75-75-p-60-iso8859-1"; i++;
	fontname[i] =
	"-adobe-helvetica-bold-r-normal--14-140-75-75-p-82-iso8859-1"; i++;
	fontname[i] =
	"-adobe-helvetica-bold-r-normal--24-240-75-75-p-138-iso8859-1"; i++;
	fontname[i] = "6x12"; i++;
	fontname[i] = "9x15"; i++;
	fontname[i] =
	"-adobe-courier-bold-r-normal--10-100-75-75-m-60-iso8859-1"; i++;
	fontname[i] =
	"-adobe-courier-bold-r-normal--24-240-75-75-m-150-iso8859-1"; i++;
	fontname[i] =
	"-adobe-times-medium-r-normal--12-120-75-75-p-64-iso8859-1"; i++;
	fontname[i] =
	"-adobe-times-medium-r-normal--24-240-75-75-p-124-iso8859-1"; i++;

	XtManageChild(fontRB1);
	XtManageChildren(fontTB1, TB_COUNT);

	n = 0;
	XtSetArg(args[n], XmNwidth, 170); n++;
	XtSetArg(args[n], XmNheight, 375); n++;
	XtSetArg(args[n], XmNresizeHeight, True);n++;  
	XtSetArg(args[n], XmNresizeWidth, True);n++;  
	XtSetValues(RowCol1, args, n);

	/* create multi line text widget */
	n = 0;
	XtSetArg(args[n], XmNvalue, new_string2); n++;
	XtSetArg(args[n], XmNcolumns, NUM_COLUMNS);  n++;
	XtSetArg(args[n], XmNrows, 3);  n++;
	XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
	XtSetArg(args[n], XmNfontList,
		CommonGetFontList(fontname[0]));  n++;
	Text1 = XmCreateText(RowCol1, "Text1", args, n);
	XtManageChild(Text1);

	for (i=0; i<TB_COUNT; i++)
	{
		n = 0;
           if ( i == 0 )
           {
		XtSetArg(args[n], XmNset, True); n++;
           }
           else
           {
		XtSetArg(args[n], XmNset, False); n++;
           }
		XtSetValues(fontTB1[i], args, n);
		XtRemoveAllCallbacks(fontTB1[i], XmNarmCallback);
		/* test was using armCallback (PIR 3753), *
		 * which was operator error *
		XtAddCallback(fontTB1[i], XmNarmCallback,
		*/
                fontCB3[i].widget = Text1;
                fontCB3[i].fontname = fontname[i];
		XtAddCallback(fontTB1[i], XmNvalueChangedCallback,
			fontTBCB2, (XtPointer) &(fontCB3[i]));
	}

#ifdef	PIR_CLOSED
	/* test case for PIR 3753 */
	CommonPause();
	/* end of test case for PIR 3753 */
#endif /* PIR_CLOSED */

	/* test case for PIR 2648 */
	CommonPause();
	/* end of test case for PIR 2648 */

	XtAppMainLoop(app_context);
}


