/* $TOG: CommText9.c /main/9 1997/10/02 16:33:30 csn $ */
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
#include <Xm/Screen.h>

Widget Manager1;
Widget Text1, Text2, Text3;
Widget veriTB1, veriTB2, doitTB1, doitTB2;
Widget cursScale1, cursScale2;
Widget topScale1, topScale2;
Widget showPosPB1, showPosPB2;
Widget fonttitle1, fonttitle2;
Widget baselabel1, baselabel2, basevalue1, basevalue2;
Widget lengthlabel1, lengthlabel2, lengthvalue1, lengthvalue2;
Widget separator;
Boolean	doit_flag1 = False, doit_flag2 = False;

typedef	struct	_FontWidgetPair
	{
	char*	font;
	Widget	w;
	}	FontWidgetPair;

static char new_string2[] = "\
Jack Sprat could eat no fat\n\
His wife could eat no lean\n\
And so between them both\n\
They licked the platter clean";

static char new_string3[] = "\
Ends with newline.\n\
Ends with newline.\n\
This line of text does not end with a newline character. When word wrap is on, this long line should wrap around, displaying as nine lines within the text widget.";

static char new_string4[] =
"Some text to test   \n\
123456781234567812345\n\
the insertion of tabs\n\
123456781234567812345";

static char font_name1[] = "8x13";

static char render_tag[] = "render1";

static void HelpCB(Widget w, XtPointer client_data,
                                XtPointer call_data)
{
	Arg args[MAX_ARGS];
	int n;
	
	Widget info_box = NULL;
	XmString helpString, titleString, okString;
	 
	helpString = XmStringCreateLtoR("Help has arrived!",
		XmSTRING_DEFAULT_CHARSET);
	titleString = XmStringCreateLtoR("Info box", XmSTRING_DEFAULT_CHARSET);
	okString = XmStringCreateLtoR("Dismiss", XmSTRING_DEFAULT_CHARSET);
	n=0;
	XtSetArg(args[n], XmNmessageString, helpString); n++;
	XtSetArg(args[n], XmNokLabelString, okString); n++;
	XtSetArg(args[n], XmNdialogTitle, titleString); n++;
	info_box = XmCreateInformationDialog(Shell1, "info_box", args, n);
	XtManageChild(info_box);

        XmStringFree (helpString);
        XmStringFree (titleString);
        XmStringFree (okString);

}

static void veriTBCB(Widget w, XtPointer client_data,
                                XtPointer call_data)
{
	XmToggleButtonCallbackStruct *tb_calldata =
		(XmToggleButtonCallbackStruct *)call_data;
	Arg args[MAX_ARGS];
	int n;
	Boolean	curr_state;

	n = 0;
	XtSetArg(args[n], XmNverifyBell, tb_calldata->set); n++;
	XtSetValues((Widget)client_data, args, n);
}

static void doitTBCB(Widget w, XtPointer client_data,
                                XtPointer call_data)
{
	XmToggleButtonCallbackStruct *tb_calldata =
		(XmToggleButtonCallbackStruct *)call_data;
	if (((Widget)client_data) == Text2)
		{
		doit_flag2 = tb_calldata->set;
		}
	else
		{
		doit_flag1 = tb_calldata->set;
		}
}

static void showPosCB(Widget w, XtPointer client_data,
                                XtPointer call_data)
{
	XmTextShowPosition((Widget)client_data,
		XmTextGetInsertionPosition((Widget)client_data));
}

static void topScaleCB(Widget w, XtPointer client_data,
                                XtPointer call_data)
{
	XmScaleCallbackStruct *scale_calldata =
		(XmScaleCallbackStruct *)call_data;

	XmTextSetTopCharacter((Widget)client_data, scale_calldata->value);
}

static void cursScaleCB(Widget w, XtPointer client_data,
                                XtPointer call_data)
{
	XmScaleCallbackStruct *scale_calldata =
		(XmScaleCallbackStruct *)call_data;

	XmTextSetInsertionPosition((Widget)client_data, 
                                 scale_calldata->value);
}


static void fontTBCB(Widget w, XtPointer client_data,
                                XtPointer call_data)
{
	FontWidgetPair	*fwp_clientdata = (FontWidgetPair *)client_data;
	XmToggleButtonCallbackStruct *tb_calldata =
		(XmToggleButtonCallbackStruct *)call_data;
	Arg args[MAX_ARGS];
	int n;
	XmTextPosition	curr_pos;
	char	baseval[5];
        XmString tmp_string;

	if (tb_calldata->set)
		{
		curr_pos = XmTextGetInsertionPosition(fwp_clientdata->w);
		n = 0;
 
		XtSetArg(args[n], XmNfontList,
			CommonGetFontList(fwp_clientdata->font));  n++;
		XtSetValues(fwp_clientdata->w, args, n);
		XmTextSetInsertionPosition(fwp_clientdata->w, curr_pos);

		sprintf(baseval, "%d", 
                    XmTextGetBaseline(fwp_clientdata->w)); 
                tmp_string=XmStringCreate(baseval, XmSTRING_DEFAULT_CHARSET);
		XtSetArg(args[0], XmNlabelString, tmp_string);
			
		if (fwp_clientdata->w == Text1)
			{
			XtSetValues(basevalue1, args, 1);
			}
		else
			{
			XtSetValues(basevalue2, args, 1);
			}
                XmStringFree (tmp_string);
		}
}

static void ModifyCB(Widget w, XtPointer client_data,
                                XtPointer call_data)
{
Arg args[MAX_ARGS];
int n;
	char	lengthval[5];
        XmString tmp_string;

	sprintf(lengthval, "%d", XmTextGetLastPosition((Widget)client_data)); 
        tmp_string = XmStringCreate(lengthval, XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[0], XmNlabelString, tmp_string);
		
	if (((Widget)client_data) == Text2)
		{
		XtSetValues(lengthvalue2, args, 1);
		}
	else
		{
		XtSetValues(lengthvalue1, args, 1);
		}
        XmStringFree (tmp_string);
}

static void wordWrapTBCB(Widget w, XtPointer client_data,
                                   XtPointer call_data)
{
        Widget text = (Widget)client_data;
	XmToggleButtonCallbackStruct *tb_calldata =
		(XmToggleButtonCallbackStruct *)call_data;
        int  n;
        Arg  args[5];

        n = 0;
	if (tb_calldata->set)
        {
        	XtSetArg(args[n], XmNwordWrap, TRUE); n++;
        }
        else
        {
        	XtSetArg(args[n], XmNwordWrap, FALSE); n++;
        }

        XtSetValues(text, args, n );
}

static void VerifyCB(Widget w, XtPointer client_data,
                                XtPointer call_data)
{
	XmTextVerifyCallbackStruct *tv_calldata =
		(XmTextVerifyCallbackStruct *)call_data;
	if (((Widget)client_data) == Text2)
		{
		tv_calldata->doit = doit_flag2;
		}
	else
		{
		tv_calldata->doit = doit_flag1;
		}
}

static void SetTextSelectionArray(Widget w, XtPointer client_data,
                                XtPointer call_data)
{
/*
 * Test for HaL DTS 9841 - make the selection_array a dynamic variable.
 *			   No errors should occur.
 */
	XmTextScanType selection_array[4];
	Arg args[MAX_ARGS];
	int n, item = *((int *)client_data);

	selection_array[0] = XmSELECT_POSITION;

	switch (item) {
	case 0:   selection_array[1] = XmSELECT_POSITION;
		break;
	case 1:   selection_array[1] = XmSELECT_WORD;
		break;
	case 2:   selection_array[1] = XmSELECT_LINE;
		break;
	case 3:   selection_array[1] = XmSELECT_ALL;
	break;
	default:  printf("Error: no selection change made to text widgets!\n");
	return;
	}

	n = 0;
	XtSetArg(args[n], XmNselectionArray, selection_array); n++;
	XtSetArg(args[n], XmNselectionArrayCount, 2); n++;
	XtSetValues(Text3, args, n);
	XtSetValues(Text2, args, n);

	fflush(stdout);
}

static void ParseUserData(char *string, Boolean *bug_flag1)
{
	if (string == NULL)
	{
		*bug_flag1 = False;
	}
	else
	{
		*bug_flag1 = True;
	}
	return;
}

#define	BTN_COUNT	11
#define	TB_COUNT	3
#define	TEXT1_TOPCHAR	35
#define	TEXT2_TOPCHAR	30
#define NUM_MENU_ITEMS  4

#define	TEXT1_X		10
#define	TEXT1_Y		130
#define	TEXT2_X		245
#define	TEXT2_Y		130

main_text9(argc, argv)
     int argc;
     char **argv;
{
	Arg args[MAX_ARGS];
	int n;
	Widget	SWin, ScreenID;
	Screen  *screen;
	Widget	Popup1Btn[BTN_COUNT], Popup1;
	Widget	fontTB1[TB_COUNT], fontTB2[TB_COUNT], fontRB1, fontRB2;
        Widget  wordWrapTB;
	Widget	selectionOption1;
	Widget	pd_menu, m_item[NUM_MENU_ITEMS];
        static int     item_num[NUM_MENU_ITEMS];
	FontWidgetPair	FWP[6];
	char	baseval[5];
	char	lengthval[5];
	XmString	optionString;
        XmString tmp_string = (XmString)0;
	Boolean	show_pir3701 = False;
	int	i;
        XmRendition   rendition;
        XmRenderTable renderTab = NULL;
	XFontStruct   *fontStruct;

	/*  initialize toolkit  */
	CommonTestInit(argc, argv);
    
	ParseUserData(UserData, &show_pir3701);

	if (show_pir3701)
	{
	/* test case for PIR 3701 */
#ifdef	MOTIF1_1
		n = 0; /* have to give width/height before realize */
		XtSetArg(args[n], XmNwidth, 10); n++;
		XtSetArg(args[n], XmNheight, 10); n++;
		XtSetValues(Shell1, args, n);
#endif /* MOTIF1_1 */
		XtRealizeWidget(Shell1);
	/* end of test case for PIR 3701 */
	}

	/* create main bulletin board */
	n = 0;
	/* uncommenting causes cursScale[12] to not show in PIR 3701
	XtSetArg(args[n], XmNallowOverlap, False); n++;
	*/
	Manager1 = XmCreateBulletinBoard(Shell1, "Manager1", args, n);
	XtManageChild(Manager1);
		
        n = 0;
        XtSetArg(args[n], XmNfontName, font_name1);  n++;
        XtSetArg(args[n], XmNfontType, XmFONT_IS_FONT );  n++;
        XtSetArg(args[n], XmNloadModel, XmLOAD_IMMEDIATE );  n++;
        rendition = XmRenditionCreate( Manager1, render_tag, args, n );

        renderTab = XmRenderTableAddRenditions( renderTab, &rendition, 1,
                      XmMERGE_REPLACE );

        XmRenditionFree( rendition ); 

	/* create scrolledtext widget */
	n = 0;
	XtSetArg(args[n], XmNbackground, CommonGetColor("goldenrod")); n++;
	XtSetArg(args[n], XmNtopCharacter, TEXT1_TOPCHAR); n++;
	XtSetArg(args[n], XmNrows, 4);  n++;
	XtSetArg(args[n], XmNwidth, 180); n++;
	XtSetArg(args[n], XmNscrollVertical, True);  n++;
	XtSetArg(args[n], XmNscrollHorizontal, True);  n++;
	XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
	XtSetArg(args[n], XmNvalue, new_string2); n++;
	XtSetArg(args[n], XmNrenderTable, renderTab);  n++;
	Text1 = XmCreateScrolledText(Manager1, "Text1", args, n);
	XtAddCallback(Text1, XmNmodifyVerifyCallback, VerifyCB,
				 (XtPointer) Text1);
	XtAddCallback(Text1, XmNvalueChangedCallback, ModifyCB,
				 (XtPointer) Text1);
	XtManageChild(Text1);
        if (tmp_string) XmStringFree(tmp_string);

	n = 0;
        tmp_string = XmStringCreate("verifyBell", XmSTRING_DEFAULT_CHARSET); 
	XtSetArg(args[n], XmNx, TEXT1_X); n++;
	XtSetArg(args[n], XmNy, TEXT1_Y); n++;
	XtSetArg(args[n], XmNset, True); n++;
	XtSetArg(args[n], XmNshadowThickness, 2); n++;
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	veriTB1 = XmCreateToggleButton(Manager1, "veriTB1", args, n);
	XtAddCallback(veriTB1, XmNvalueChangedCallback, veriTBCB, Text1);
	XtManageChild(veriTB1);
        XmStringFree (tmp_string);

	n = 0;
        tmp_string = XmStringCreate("doit flag", XmSTRING_DEFAULT_CHARSET); 
	XtSetArg(args[n], XmNx, TEXT1_X); n++;
	XtSetArg(args[n], XmNy, TEXT1_Y + 40); n++;
	XtSetArg(args[n], XmNset, False); n++;
	XtSetArg(args[n], XmNshadowThickness, 2); n++;
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	doitTB1 = XmCreateToggleButton(Manager1, "doitTB1", args, n);
	XtAddCallback(doitTB1, XmNvalueChangedCallback, doitTBCB, 
		      (XtPointer) Text1);
	XtManageChild(doitTB1);
        XmStringFree (tmp_string);

	n = 0;
        tmp_string = XmStringCreate("text length", XmSTRING_DEFAULT_CHARSET); 
	XtSetArg(args[n], XmNx, TEXT1_X + 110); n++;
	XtSetArg(args[n], XmNy, TEXT1_Y + 20); n++;
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	lengthlabel1 = XmCreateLabel(Manager1, "lengthlabel", args, n);
	XtManageChild(lengthlabel1);
	XmStringFree (tmp_string);



        n = 0;
	sprintf(lengthval, "%d", XmTextGetLastPosition(Text1)); 
        tmp_string = XmStringCreate(lengthval, XmSTRING_DEFAULT_CHARSET); 
	XtSetArg(args[n], XmNx, TEXT1_X + 140); n++;
	XtSetArg(args[n], XmNy, TEXT1_Y + 35); n++;
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	lengthvalue1 = XmCreateLabel(Manager1, "lengthvalue", args, n);
	XtManageChild(lengthvalue1);
        XmStringFree (tmp_string);

	n = 0;
        tmp_string = XmStringCreate("top character", 
				XmSTRING_DEFAULT_CHARSET); 
	XtSetArg(args[n], XmNx, TEXT1_X); n++;
	XtSetArg(args[n], XmNy, TEXT1_Y + 85); n++;
	XtSetArg(args[n], XmNhighlightThickness, 2); n++;
	XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
	XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg(args[n], XmNscaleHeight, 20); n++;
	XtSetArg(args[n], XmNscaleWidth, 180); n++;
	XtSetArg(args[n], XmNvalue, XmTextGetTopCharacter(Text1)); n++;
	XtSetArg(args[n], XmNshowValue, True); n++;
	XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
	XtSetArg(args[n], XmNtitleString, tmp_string); n++;
	topScale1 = XmCreateScale(Manager1, "topScale1", args, n);
	XtAddCallback(topScale1, XmNvalueChangedCallback, topScaleCB, 
		      (XtPointer) Text1);
	XtManageChild(topScale1);
        XmStringFree (tmp_string);

	n = 0;
        tmp_string = XmStringCreate("show position", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNx, TEXT1_X); n++;
	XtSetArg(args[n], XmNy, TEXT1_Y + 145); n++;
	XtSetArg(args[n], XmNshadowThickness, 2); n++;
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	showPosPB1 = XmCreatePushButton(Manager1, "showPosPB1", args, n);
	XtAddCallback(showPosPB1, XmNactivateCallback, showPosCB, Text1);
	XtManageChild(showPosPB1);
        XmStringFree (tmp_string);

	n = 0;
        tmp_string = XmStringCreate("cursor position", 
				XmSTRING_DEFAULT_CHARSET); 
	XtSetArg(args[n], XmNx, TEXT1_X); n++;
	XtSetArg(args[n], XmNy, TEXT1_Y + 170); n++;
	XtSetArg(args[n], XmNhighlightThickness, 2); n++;
	XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
	XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg(args[n], XmNscaleHeight, 20); n++;
	XtSetArg(args[n], XmNscaleWidth, 180); n++;
	XtSetArg(args[n], XmNvalue, XmTextGetInsertionPosition(Text1)); n++;
	XtSetArg(args[n], XmNshowValue, True); n++;
	XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
	XtSetArg(args[n], XmNtitleString, tmp_string); n++;
	cursScale1 = XmCreateScale(Manager1, "cursScale1", args, n);
	XtAddCallback(cursScale1, XmNvalueChangedCallback, cursScaleCB, Text1);
	XtManageChild(cursScale1);
        XmStringFree (tmp_string);

	n = 0;
        tmp_string = XmStringCreate("text font", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNx, TEXT1_X); n++;
	XtSetArg(args[n], XmNy, TEXT1_Y + 235); n++;
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	fonttitle1 = XmCreateLabel(Manager1, "fonttitle", args, n);
	XtManageChild(fonttitle1);
        XmStringFree (tmp_string);

	n = 0;
	XtSetArg(args[n], XmNx, TEXT1_X); n++;
	XtSetArg(args[n], XmNy, TEXT1_Y + 255); n++;
	XtSetArg(args[n], XmNborderWidth, 2); n++;
	XtSetArg(args[n], XmNisHomogeneous, False); n++;
	fontRB1 = XmCreateRadioBox(Manager1, "fontRB1", args, n);

	n = 0;
        tmp_string = XmStringCreate("6x12", XmSTRING_DEFAULT_CHARSET); 
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	fontTB1[0] = XmCreateToggleButtonGadget(fontRB1, "RB1_6x12", args, n);
        XmStringFree (tmp_string);
        
	

        n = 0;
        tmp_string = XmStringCreate("8x13", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNset, True); n++;
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	fontTB1[1] = XmCreateToggleButtonGadget(fontRB1, "RB1_8x13", args, n);
        XmStringFree (tmp_string);
	
        
        n = 0;
        tmp_string = XmStringCreate("9x15", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	fontTB1[2] = XmCreateToggleButtonGadget(fontRB1, "RB1_9x15", args, n);
        XmStringFree (tmp_string);

	FWP[0].font = "6x12"; FWP[0].w = Text1;
	XtAddCallback(fontTB1[0], XmNvalueChangedCallback, fontTBCB, &FWP[0]);
	FWP[1].font = "8x13"; FWP[1].w = Text1;
	XtAddCallback(fontTB1[1], XmNvalueChangedCallback, fontTBCB, &FWP[1]);
	FWP[2].font = "9x15"; FWP[2].w = Text1;
	XtAddCallback(fontTB1[2], XmNvalueChangedCallback, fontTBCB, &FWP[2]);

	XtManageChild(fontRB1);
	XtManageChildren(fontTB1, TB_COUNT);

	n = 0;
        tmp_string = XmStringCreate("text baseline",
				XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNx, TEXT1_X + 70); n++;
	XtSetArg(args[n], XmNy, TEXT1_Y + 255); n++;
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	baselabel1 = XmCreateLabel(Manager1, "baselabel", args, n);
	XtManageChild(baselabel1);
        XmStringFree (tmp_string);

	n = 0;
	sprintf(baseval, "%d", XmTextGetBaseline(Text1)); 
        tmp_string = XmStringCreate(baseval, XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNx, TEXT1_X + 110); n++;
	XtSetArg(args[n], XmNy, TEXT1_Y + 270); n++;
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
		 
	basevalue1 = XmCreateLabel(Manager1, "basevalue", args, n);
	XtManageChild(basevalue1);
        XmStringFree (tmp_string);


	optionString = XmStringCreateSimple("Double click selects...");

/* test case for PIR 3635 - MenuShell created with BBoard parent 
   and no fontlist specified */
	pd_menu = XmCreatePulldownMenu(Manager1, "pd_menu", NULL, 0);

	m_item[0] = XmCreatePushButton(pd_menu, "Position", NULL, 0);
	m_item[1] = XmCreatePushButton(pd_menu, "Word", NULL, 0);
	m_item[2] = XmCreatePushButton(pd_menu, "Line", NULL, 0);
	m_item[3] = XmCreatePushButton(pd_menu, "All", NULL, 0);
	for (i = 0; i < XtNumber(m_item); i++)
	{
                item_num[i] = i;
		XtAddCallback(m_item[i], XmNactivateCallback,
			SetTextSelectionArray, (XtPointer) &item_num[i]);
	}
	XtManageChildren(m_item, XtNumber(m_item));

	n = 0;
	XtSetArg(args[n], XmNsubMenuId, pd_menu); n++;
	XtSetArg(args[n], XmNmenuHistory, m_item[1]); n++;
	XtSetArg(args[n], XmNlabelString, optionString); n++;
	XtSetArg(args[n], XmNx, TEXT1_X + 50); n++;
	XtSetArg(args[n], XmNy, TEXT1_Y + 400); n++;
	selectionOption1 = XmCreateOptionMenu(Manager1, "selectionOption1",
		args, n);
	XtManageChild(selectionOption1);
/* end of test case for PIR 3635 */

	XmStringFree(optionString);

/*-------------------------*/
	n = 0;
	XtSetArg(args[n], XmNx,	230); n++;
	XtSetArg(args[n], XmNheight, 470); n++;
	XtSetArg(args[n], XmNshadowThickness, 4); n++;
	XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
	separator = XmCreateSeparator(Manager1, "separator", args, n);
	XtManageChild(separator);
/*-------------------------*/


	/* create single line text widget */
	n = 0;
	XtSetArg(args[n], XmNbackground, CommonGetColor("seagreen")); n++;
	XtSetArg(args[n], XmNx, 245); n++;
	XtSetArg(args[n], XmNverifyBell, False); n++;
	XtSetArg(args[n], XmNvalue, new_string2); n++;
	XtSetArg(args[n], XmNrenderTable, renderTab);  n++;
	Text2 = XmCreateText(Manager1, "Text2", args, n);
	XtAddCallback(Text2, XmNmodifyVerifyCallback, (XtCallbackProc)VerifyCB,
				 Text2);
	XtAddCallback(Text2, XmNvalueChangedCallback, (XtCallbackProc)ModifyCB,
				 Text2);
	XtManageChild(Text2);

	XmTextSetTopCharacter(Text2, (XmTextPosition)TEXT2_TOPCHAR);

	n = 0;
        tmp_string = XmStringCreate("verifyBell", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNx, TEXT2_X); n++;
	XtSetArg(args[n], XmNy, TEXT2_Y); n++;
	XtSetArg(args[n], XmNset, False); n++;
	XtSetArg(args[n], XmNshadowThickness, 2); n++;
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
		
	veriTB2 = XmCreateToggleButton(Manager1, "veriTB2", args, n);
	XtAddCallback(veriTB2, XmNvalueChangedCallback, veriTBCB, Text2);
	XtManageChild(veriTB2);
        XmStringFree (tmp_string);

	n = 0;
        tmp_string = XmStringCreate("doit flag", XmSTRING_DEFAULT_CHARSET); 
	XtSetArg(args[n], XmNx, TEXT2_X); n++;
	XtSetArg(args[n], XmNy, TEXT2_Y + 40); n++;
	XtSetArg(args[n], XmNset, False); n++;
	XtSetArg(args[n], XmNshadowThickness, 2); n++;
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	doitTB2 = XmCreateToggleButton(Manager1, "doitTB2", args, n);
	XtAddCallback(doitTB2, XmNvalueChangedCallback, doitTBCB, Text2);
	XtManageChild(doitTB2);
        XmStringFree (tmp_string);

	n = 0;
        tmp_string = XmStringCreate("text length", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNx, TEXT2_X + 110); n++;
	XtSetArg(args[n], XmNy, TEXT2_Y + 20); n++;
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	lengthlabel2 = XmCreateLabel(Manager1, "lengthlabel", args, n);
	XtManageChild(lengthlabel2);
	XmStringFree (tmp_string);


        n = 0;
	XtSetArg(args[n], XmNx, TEXT2_X + 140); n++;
	XtSetArg(args[n], XmNy, TEXT2_Y + 35); n++;
	sprintf(lengthval, "%d", XmTextGetLastPosition(Text2)); 
        tmp_string = XmStringCreate(lengthval, XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	lengthvalue2 = XmCreateLabel(Manager1, "lengthvalue", args, n);
	XtManageChild(lengthvalue2);
        XmStringFree (tmp_string);

	n = 0;
        tmp_string = XmStringCreate("top character", 
				XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNx, TEXT2_X); n++;
	XtSetArg(args[n], XmNy, TEXT2_Y + 85); n++;
	XtSetArg(args[n], XmNhighlightThickness, 2); n++;
	XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
	XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg(args[n], XmNscaleHeight, 20); n++;
	XtSetArg(args[n], XmNscaleWidth, 180); n++;
	XtSetArg(args[n], XmNvalue, XmTextGetTopCharacter(Text2)); n++;
	XtSetArg(args[n], XmNshowValue, True); n++;
	XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
	XtSetArg(args[n], XmNtitleString, tmp_string); n++;
	topScale2 = XmCreateScale(Manager1, "topScale2", args, n);
	XtAddCallback(topScale2, XmNvalueChangedCallback, topScaleCB, Text2);
	XtManageChild(topScale2);
        XmStringFree (tmp_string);

	n = 0;
        tmp_string = XmStringCreate("show position", 
				XmSTRING_DEFAULT_CHARSET); n++;
	XtSetArg(args[n], XmNx, TEXT2_X); n++;
	XtSetArg(args[n], XmNy, TEXT2_Y + 145); n++;
	XtSetArg(args[n], XmNshadowThickness, 2); n++;
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	showPosPB2 = XmCreatePushButton(Manager1, "showPosPB2", args, n);
	XtAddCallback(showPosPB2, XmNactivateCallback, showPosCB, Text2);
	XtManageChild(showPosPB2);
        XmStringFree (tmp_string);

	n = 0;
        tmp_string = XmStringCreate("cursor position", 
				XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNx, TEXT2_X); n++;
	XtSetArg(args[n], XmNy, TEXT2_Y + 170); n++;
	XtSetArg(args[n], XmNhighlightThickness, 2); n++;
	XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
	XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg(args[n], XmNscaleHeight, 20); n++;
	XtSetArg(args[n], XmNscaleWidth, 180); n++;
	XtSetArg(args[n], XmNvalue, XmTextGetInsertionPosition(Text2)); n++;
	XtSetArg(args[n], XmNshowValue, True); n++;
	XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
	XtSetArg(args[n], XmNtitleString, tmp_string); n++;
	cursScale2 = XmCreateScale(Manager1, "cursScale2", args, n);
	XtAddCallback(cursScale2, XmNvalueChangedCallback, cursScaleCB, Text2);
	XtManageChild(cursScale2);
        XmStringFree (tmp_string);

	n = 0;
        tmp_string = XmStringCreate("text font", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNx, TEXT2_X); n++;
	XtSetArg(args[n], XmNy, TEXT2_Y + 235); n++;
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	fonttitle2 = XmCreateLabel(Manager1, "fonttitle", args, n);
	XtManageChild(fonttitle2);
        XmStringFree (tmp_string);

	n = 0;
	XtSetArg(args[n], XmNx, TEXT2_X); n++;
	XtSetArg(args[n], XmNy, TEXT2_Y + 255); n++;
	XtSetArg(args[n], XmNborderWidth, 2); n++;
	XtSetArg(args[n], XmNisHomogeneous, False); n++;
	fontRB2 = XmCreateRadioBox(Manager1, "fontRB2", args, n);

	n = 0;
        tmp_string = XmStringCreate("6x12", XmSTRING_DEFAULT_CHARSET); n++;
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	fontTB2[0] = XmCreateToggleButtonGadget(fontRB2, "RB2_6x12", args, n);
	XmStringFree (tmp_string);


        n = 0;
        tmp_string = XmStringCreate("8x13", XmSTRING_DEFAULT_CHARSET); 
	XtSetArg(args[n], XmNset, True); n++;
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	fontTB2[1] = XmCreateToggleButtonGadget(fontRB2, "RB2_8x13", args, n);
	XmStringFree (tmp_string);

        n = 0;
        tmp_string = XmStringCreate("9x15", XmSTRING_DEFAULT_CHARSET); 
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	fontTB2[2] = XmCreateToggleButtonGadget(fontRB2, "RB2_9x15", args, n);
        XmStringFree (tmp_string);

	FWP[3].font = "6x12"; FWP[3].w = Text2;
	XtAddCallback(fontTB2[0], XmNvalueChangedCallback, fontTBCB, &FWP[3]);
	FWP[4].font = "8x13"; FWP[4].w = Text2;
	XtAddCallback(fontTB2[1], XmNvalueChangedCallback, fontTBCB, &FWP[4]);
	FWP[5].font = "9x15"; FWP[5].w = Text2;
	XtAddCallback(fontTB2[2], XmNvalueChangedCallback, fontTBCB, &FWP[5]);

	XtManageChild(fontRB2);
	XtManageChildren(fontTB2, TB_COUNT);

	n = 0;
        tmp_string = XmStringCreate("text baseline", 
				XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNx, TEXT2_X + 70); n++;
	XtSetArg(args[n], XmNy, TEXT2_Y + 255); n++;
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
		 
	baselabel2 = XmCreateLabel(Manager1, "baselabel", args, n);
	XtManageChild(baselabel2);
        XmStringFree (tmp_string);
  
	n = 0;
	XtSetArg(args[n], XmNx, TEXT2_X + 110); n++;
	XtSetArg(args[n], XmNy, TEXT2_Y + 270); n++;
	sprintf(baseval, "%d", XmTextGetBaseline(Text2)); 
        tmp_string = XmStringCreate(baseval, XmSTRING_DEFAULT_CHARSET); n++;
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	basevalue2 = XmCreateLabel(Manager1, "basevalue", args, n);
	XtManageChild(basevalue2);
        XmStringFree (tmp_string);

	XtRealizeWidget(Shell1);
	CommonPause();

	CommonPause();

	CommonPause();

        /*  Remove the original multi-line text widget from layout. */
        XtUnmanageChild( Text1 );

	/* create a new multiline scrolledtext widget */
	n = 0;
	XtSetArg(args[n], XmNbackground, CommonGetColor("goldenrod")); n++;
	XtSetArg(args[n], XmNtopCharacter, TEXT1_TOPCHAR); n++;
	XtSetArg(args[n], XmNrows, 4);  n++;
	XtSetArg(args[n], XmNwidth, 180); n++;
	XtSetArg(args[n], XmNscrollVertical, True);  n++;
	XtSetArg(args[n], XmNscrollHorizontal, False);  n++;
	XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
	XtSetArg(args[n], XmNwordWrap, False); n++;
	XtSetArg(args[n], XmNvalue, new_string3); n++;
	XtSetArg(args[n], XmNrenderTable, renderTab); n++;
	Text3 = XmCreateScrolledText(Manager1, "Text3", args, n);
	XtAddCallback(Text3, XmNmodifyVerifyCallback, VerifyCB,
				 (XtPointer) Text3);
	XtAddCallback(Text3, XmNvalueChangedCallback, ModifyCB,
				 (XtPointer) Text3);

        /* 
         *  Remove topScaleCB from topScale, 
         *  replace with new client data, Text3.
         */
	XtRemoveCallback(topScale1, XmNvalueChangedCallback, topScaleCB, 
		      (XtPointer) Text1);
	XtAddCallback(topScale1, XmNvalueChangedCallback, topScaleCB, 
		      (XtPointer) Text3);
	XtManageChild(Text3);

        n = 0;
        tmp_string = XmStringCreate("word wrap", XmSTRING_DEFAULT_CHARSET); n++;
	XtSetArg(args[n], XmNx, TEXT1_X); n++;
	XtSetArg(args[n], XmNy, TEXT1_Y + 350 ); n++;
	XtSetArg(args[n], XmNset, False); n++;
	XtSetArg(args[n], XmNshadowThickness, 2); n++;
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	wordWrapTB = XmCreateToggleButtonGadget(Manager1, "wordWrap", args, n);
        XtManageChild(wordWrapTB);

	XtAddCallback(wordWrapTB, XmNvalueChangedCallback, wordWrapTBCB, Text3);
        XmStringFree (tmp_string);

        CommonPause();  /*  Test word wrap in multiline text widget. */

	CommonPause();  /*  Test single line text widget. */

	CommonPause();

	CommonPause();

	CommonPause();

      /*  
       *  Assign the same font to the XmScreen object as
       *  to the Text/CSText widget to test that tabs
       *  inserted into an XmText widget are the same
       *  size as tabs inserted into an XmCSText widget.
       */
        screen = XtScreen( Shell1 );
	ScreenID = XmGetXmScreen(screen);

	fontStruct = (XFontStruct *)XLoadQueryFont( display, font_name1 );

	n = 0;
	XtSetArg( args[n], XmNfont, (XtPointer)fontStruct ); n++;
	XtSetValues( ScreenID, args, n );

        n = 0;
	XtSetArg( args[n], XmNvalue, new_string4 ); n++;
	XtSetArg( args[n], XmNcolumns, 21 ); n++;
	XtSetArg( args[n], XmNeditable, True ); n++;
	XtSetArg( args[n], XmNrenderTable, renderTab ); n++;
        XtSetValues( Text3, args, n );

	XmRenderTableFree( renderTab );

	CommonPause();
	CommonPause();

	XtAppMainLoop(app_context);
}
