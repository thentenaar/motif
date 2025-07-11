/* $TOG: CommText3.c /main/7 1997/10/02 16:15:13 csn $ */
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
#include <Text3.h>

int	n;
Arg	args[MAX_ARGS];

static char font_name1[] = "variable";

static char render_tag[] = "render1";

void main_text3(argc, argv)
     int	argc;
     char	**argv;
{
	XmString	tcs, ItemList[NITEMS];
	XmFontList 	newfontlist;

	CommonTestInit(argc, argv);

	newfontlist = CommonGetFontList(font_name1);

	n = 0;
	XtSetArg(args[n], XmNmarginHeight, 15); n++;
	XtSetArg(args[n], XmNmarginWidth, 15); n++;
	Form = XmCreateForm(Shell1, "Form", args, n);
	XtManageChild (Form);

	/* build the list of Functions Label and List */
	tcs = XmStringCreateLtoR("Functions:", XmSTRING_DEFAULT_CHARSET);

	n = 0;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNtopOffset, 15); n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNleftOffset, 15); n++;
	XtSetArg(args[n], XmNfontList, newfontlist); n++;
	XtSetArg(args[n], XmNlabelString, tcs); n++;
	Label1 = XmCreateLabel(Form, "Label1", args, n);
	XtManageChild(Label1);

	XmStringFree(tcs);

	n = 0;
	/* convert the plain strings to Xstrings */
	for (n = 0; n < NITEMS; n++)
	  ItemList[n] = XmStringLtoRCreate(Textstrings[n],
					   XmSTRING_DEFAULT_CHARSET);

	n = 0;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNleftOffset, 15); n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNtopWidget, Label1); n++;
	XtSetArg(args[n], XmNtopOffset, 15); n++;
	XtSetArg(args[n], XmNresizable, True); n++;
	XtSetArg(args[n], XmNselectionPolicy, XmBROWSE_SELECT); n++;
	XtSetArg(args[n], XmNhighlightOnEnter, False); n++;
	XtSetArg(args[n], XmNfontList, newfontlist); n++;
	XtSetArg(args[n], XmNitems, ItemList); n++;
	XtSetArg(args[n], XmNitemCount, NITEMS); n++;
	XtSetArg(args[n], XmNforeground, CommonGetColor("white")); n++;
	XtSetArg(args[n], XmNbackground, CommonGetColor("darkorchid")); n++;
	List1 = XmCreateScrolledList(Form, "List1", args, n);
	XtManageChild(List1);

	XtAddCallback(List1, XmNbrowseSelectionCallback, SSelCB_List1, NULL);


	/* build the Action Label and Text */
	tcs = XmStringCreateLtoR("Action Text:", XmSTRING_DEFAULT_CHARSET);

	n = 0;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNtopOffset, 15); n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNleftWidget, List1); n++;
	XtSetArg(args[n], XmNleftOffset, 15); n++;
	XtSetArg(args[n], XmNfontList, newfontlist); n++;
	XtSetArg(args[n], XmNlabelString, tcs); n++;
	Label3 = XmCreateLabel(Form, "Label3", args, n);
	XtManageChild(Label3);

	XmStringFree(tcs);

	n = 0;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNtopOffset, 15); n++;
	XtSetArg(args[n], XmNtopWidget, Label3); n++;
	XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNrightOffset, 15); n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNleftWidget, List1); n++;
	XtSetArg(args[n], XmNleftOffset, 15); n++;
	XtSetArg(args[n], XmNhighlightThickness, 2); n++;
	XtSetArg(args[n], XmNcolumns, 32); n++;
	XtSetArg(args[n], XmNrows, 1); n++;
	XtSetArg(args[n], XmNeditMode, XmSINGLE_LINE_EDIT); n++;
	XtSetArg(args[n], XmNvalue, starting_string_text); n++;
	XtSetArg(args[n], XmNfontList, newfontlist); n++;
	XtSetArg(args[n], XmNmaxLength, strlen(starting_string_text)); n++;
	XtSetArg(args[n], XmNbackground, CommonGetColor("wheat")); n++;
	Text1 = XmCreateText(Form, "Text1", args, n);
	XtManageChild(Text1);

	/* build the Control Panel Label */
	tcs = XmStringCreateLtoR("Control Panel:", XmSTRING_DEFAULT_CHARSET);

	n = 0;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNtopWidget, Text1); n++;
	XtSetArg(args[n], XmNtopOffset, 25); n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNleftWidget, List1); n++;
	XtSetArg(args[n], XmNleftOffset, 15); n++;
	XtSetArg(args[n], XmNfontList, newfontlist); n++;
	XtSetArg(args[n], XmNlabelString, tcs); n++;
	Label5 = XmCreateLabel(Form, "Label5", args, n);
	XtManageChild(Label5);

	XmStringFree(tcs);


	/* build the Call Function pushbutton */
	tcs = XmStringCreateLtoR("Call Function", XmSTRING_DEFAULT_CHARSET);

	n = 0;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNleftWidget, List1); n++;
	XtSetArg(args[n], XmNleftOffset, 15); n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNtopWidget, Label5); n++;
	XtSetArg(args[n], XmNtopOffset, 15); n++;
	XtSetArg(args[n], XmNfontList, newfontlist); n++;
	XtSetArg(args[n], XmNlabelString, tcs); n++;
	ApplyB = XmCreatePushButton(Form, "ApplyB", args, n);
	XtManageChild(ApplyB);

	XtAddCallback(ApplyB, XmNactivateCallback, ApplyCB, (XtPointer)0);

	XmStringFree(tcs);


	/* build the Clear Args pushbutton */
	tcs = XmStringCreateLtoR("Clear Args", XmSTRING_DEFAULT_CHARSET);

	n = 0;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNleftWidget, ApplyB); n++;
	XtSetArg(args[n], XmNleftOffset, 15); n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNtopWidget, Label5); n++;
	XtSetArg(args[n], XmNtopOffset, 15); n++;
	XtSetArg(args[n], XmNfontList, newfontlist); n++;
	XtSetArg(args[n], XmNlabelString, tcs); n++;
	ClearB = XmCreatePushButton(Form, "ClearB", args, n);
	XtManageChild(ClearB);

	XtAddCallback(ClearB, XmNactivateCallback, ClearCB, (XtPointer)0);

	XmStringFree(tcs);


	/* build the Num Arg 1 Scale */
	tcs = XmStringLtoRCreate("Num Arg 1", XmSTRING_DEFAULT_CHARSET);

	n = 0;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNleftWidget, List1); n++;
	XtSetArg(args[n], XmNleftOffset, 15); n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNtopWidget, ApplyB); n++;
	XtSetArg(args[n], XmNtopOffset, 15); n++;
	XtSetArg(args[n], XmNfontList, newfontlist); n++;
	XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
	XtSetArg(args[n], XmNtitleString, tcs); n++;
	XtSetArg(args[n], XmNshowValue, True); n++;
	XtSetArg(args[n], XmNmaximum, 40); n++;
	Scale1 = XmCreateScale(Form, "Scale1", args, n);
	XtManageChild(Scale1);

	XtAddCallback(Scale1, XmNvalueChangedCallback, Scale1CB, NULL);

	XmStringFree(tcs);


	/* build the Num Arg 2 Scale */
	tcs = XmStringLtoRCreate("Num Arg 2", XmSTRING_DEFAULT_CHARSET);

	n = 0;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNleftWidget, List1); n++;
	XtSetArg(args[n], XmNleftOffset, 15); n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNtopWidget, Scale1); n++;
	XtSetArg(args[n], XmNfontList, newfontlist); n++;
	XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
	XtSetArg(args[n], XmNtitleString, tcs); n++;
	XtSetArg(args[n], XmNshowValue, True); n++;
	XtSetArg(args[n], XmNmaximum, 40); n++;
	Scale2 = XmCreateScale(Form, "Scale2", args, n);
	XtManageChild(Scale2);

	XtAddCallback(Scale2, XmNvalueChangedCallback, Scale2CB, NULL);

	XmStringFree(tcs);


	/* build the True/False Label and ToggleButton */
	tcs = XmStringCreateLtoR("True", XmSTRING_DEFAULT_CHARSET);

	n = 0;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNleftWidget, List1); n++;
	XtSetArg(args[n], XmNleftOffset, 15); n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNtopWidget, Scale2); n++;
	XtSetArg(args[n], XmNtopOffset, 15); n++;
	XtSetArg(args[n], XmNlabelString, tcs); n++;
	XtSetArg(args[n], XmNselectColor, CommonGetColor("darkorchid")); n++;
	XtSetArg(args[n], XmNset, True); n++;
	XtSetArg(args[n], XmNfontList, newfontlist); n++;
	ToggleB = XmCreateToggleButton(Form, "ToggleB", args, n);
	XtManageChild(ToggleB);

	XtAddCallback(ToggleB, XmNvalueChangedCallback, TorFCB, NULL);

	XmStringFree(tcs);

	tcs = XmStringCreateLtoR("True/False Arg", XmSTRING_DEFAULT_CHARSET);

	n = 0;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNleftWidget, List1); n++;
	XtSetArg(args[n], XmNleftOffset, 15); n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNtopWidget, ToggleB); n++;
	XtSetArg(args[n], XmNtopOffset, 0); n++;
	XtSetArg(args[n], XmNfontList, newfontlist); n++;
	XtSetArg(args[n], XmNlabelString, tcs); n++;
	Label2 = XmCreateLabel(Form, "Label2", args, n);
	XtManageChild(Label2);

	XmStringFree(tcs);


	/* build the Status/Input Label and Text */
	n = 0;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNleftWidget, List1); n++;
	XtSetArg(args[n], XmNleftOffset, 15); n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNtopWidget, Label2); n++;
	XtSetArg(args[n], XmNtopOffset, 15); n++;
	XtSetArg(args[n], XmNcolumns, 20); n++;
	XtSetArg(args[n], XmNrows, 1); n++;
	XtSetArg(args[n], XmNeditMode, XmSINGLE_LINE_EDIT); n++;
	XtSetArg(args[n], XmNbackground, CommonGetColor("yellow")); n++;
	XtSetArg(args[n], XmNfontList, newfontlist); n++;
	StatText = XmCreateText(Form, "StatText", args, n);
	XtManageChild(StatText);

	tcs = XmStringCreateLtoR("Status/Input", XmSTRING_DEFAULT_CHARSET);

	n = 0;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNleftWidget, List1); n++;
	XtSetArg(args[n], XmNleftOffset, 15); n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNtopWidget, StatText); n++;
	XtSetArg(args[n], XmNtopOffset, 0); n++;
	XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNbottomOffset, 15); n++;
	XtSetArg(args[n], XmNfontList, newfontlist); n++;
	XtSetArg(args[n], XmNlabelString, tcs); n++;
	Label4 = XmCreateLabel(Form, "Label4", args, n);
	XtManageChild(Label4);

	XmStringFree(tcs);
	XtRealizeWidget(Shell1);

	/* test of:
		XmTextGetEditable()
		XmTextSetEditable()
	*/
	n = 0;
	XtSetArg(args[n], XmNvisibleItemCount, 2); n++;
	XtSetArg(args[n], XmNtopItemPosition, 1); n++;
	XtSetValues(List1, args, n);

	CommonPause();

	/* test of:
		XmTextGetMaxLength()
		XmTextSetMaxLength()
		XmTextGetLastPosition()
	*/
	n = 0;
	XtSetArg(args[n], XmNvisibleItemCount, 3); n++;
	XtSetArg(args[n], XmNtopItemPosition, 3); n++;
	XtSetValues(List1, args, n);

	CommonPause();

	/* test of:
		XmTextSetInsertionPosition()
	*/
	n = 0;
	XtSetArg(args[n], XmNvisibleItemCount, 1); n++;
	XtSetArg(args[n], XmNtopItemPosition, 6); n++;
	XtSetValues(List1, args, n);

	CommonPause();

	/* test of:
		XmTextGetBaseline()
		XmTextXYToPos()
		XmTextPosToXY()
	*/
	n = 0;
	XtSetArg(args[n], XmNvisibleItemCount, 3); n++;
#ifndef MOTIF1_1
	XtSetArg(args[n], XmNtopItemPosition, 7); n++;
#else
	XtSetArg(args[n], XmNtopItemPosition, 8); n++;
#endif
	XtSetValues(List1, args, n);

	CommonPause();

	/* test of:
		XmTextGetTopCharacter()
		XmTextSetTopCharacter()
	*/
	n = 0;
	XtSetArg(args[n], XmNvisibleItemCount, 2); n++;
#ifndef MOTIF1_1
	XtSetArg(args[n], XmNtopItemPosition, 10); n++;
#else
	XtSetArg(args[n], XmNtopItemPosition, 11); n++;
#endif
	XtSetValues(List1, args, n);

	CommonPause();

	XtAppMainLoop(app_context);
}


static void Scale1CB(Widget w, XtPointer client_data,
				XtPointer call_data)
{
	scale1_val = ((XmScaleCallbackStruct*)call_data)->value;
}

static void Scale2CB(Widget w, XtPointer client_data,
				XtPointer call_data)
{
	scale2_val = ((XmScaleCallbackStruct*)call_data)->value;
}


static void SSelCB_List1(Widget w, XtPointer client_data,
				XtPointer call_data)
{
	function = ((XmListCallbackStruct*)call_data)->item_position;

	switch (function) {
	/* these don't use any controls */
	case GetEditable:
	case GetMaxLength:
	case GetLastPosition:
	case GetBaseline:
	case GetTopCharacter:
		n = 0;
		XtSetArg(args[n], XmNsensitive, False); n++;
		XtSetValues(Scale1, args, n);
		XtSetValues(Scale2, args, n);
		XtSetValues(ToggleB, args, n);

		break;

	/* these use only the Scale1 control */
	case SetMaxLength:
	case PosToXY:
	case SetInsertionPosition:
	case SetTopCharacter:
		n = 0;
		XtSetArg(args[n], XmNsensitive, True); n++;
		XtSetValues(Scale1, args, n);

		n = 0;
		XtSetArg(args[n], XmNsensitive, False); n++;
		XtSetValues(Scale2, args, n);
		XtSetValues(ToggleB, args, n);

		break;

	/* these use the Scale1 and Scale2 controls */
	case XYToPos:
		n = 0;
		XtSetArg(args[n], XmNsensitive, True); n++;
		XtSetValues(Scale1, args, n);
		XtSetValues(Scale2, args, n);

		n = 0;
		XtSetArg(args[n], XmNsensitive, False); n++;
		XtSetValues(ToggleB, args, n);

		break;

	/* these use only the ToggleB control */
	case SetEditable:
		n = 0;
		XtSetArg(args[n], XmNsensitive, True); n++;
		XtSetValues(ToggleB, args, n);

		n = 0;
		XtSetArg(args[n], XmNsensitive, False); n++;
		XtSetValues(Scale1, args, n);
		XtSetValues(Scale2, args, n);

		break;

	default:
		printf("Not a valid function\n");
	}
}


static void ClearCB(Widget w, XtPointer client_data,
				XtPointer call_data)
{
	XmString	tcs;

	XmListDeselectAllItems(List1);
	function = -1;

	n = 0;
	XtSetArg(args[n], XmNvalue, 0); n++;
	XtSetValues(Scale1, args, n);
	XtSetValues(Scale2, args, n);

	tcs = XmStringCreateLtoR("True", XmSTRING_DEFAULT_CHARSET);
	n = 0;
	XtSetArg(args[n], XmNlabelString, tcs); n++;
	XtSetArg(args[n], XmNset, True); n++;
	XtSetValues(ToggleB, args, n);
	XmStringFree(tcs);

	torf = True;

	XmTextSetString( StatText, "");
}


static void ApplyCB(Widget w, XtPointer client_data,
				XtPointer call_data)
{
	char	stat_str[25];
	int	left, right;
	Position	x, y;
	char	*tmp_str;
	XmAnyCallbackStruct *c_d;
        static char editable[] = "GetEditable: True";
        static char not_editable[] = "GetEditable: False";
        static char no_position[] = "Position not found";
        XmString  string;

	c_d = (XmAnyCallbackStruct *) call_data;

	switch (function) {
	case GetEditable:
		if (XmTextGetEditable(Text1))
                {
		  XmTextSetString( StatText, editable);
                }
		else
                {
		  XmTextSetString( StatText, not_editable);
                }
		break;
	case SetEditable:
		XmTextSetEditable(Text1, torf);
		break;
	case GetMaxLength:
		sprintf(stat_str, "MaxLength = %d", XmTextGetMaxLength(Text1));
		XmTextSetString( StatText, stat_str);
		break;
	case SetMaxLength:
		XmTextSetMaxLength(Text1, scale1_val);
		break;
	case XYToPos:
		sprintf(stat_str, "Position = %d",
			XmTextXYToPos(Text1, scale1_val, scale2_val));
		XmTextSetString( StatText, stat_str);
		break;
	case PosToXY:
		if (XmTextPosToXY(Text1, (XmTextPosition)scale1_val, &x, &y))
		{
	        	sprintf(stat_str, "x = %d, y = %d", x, y);
			XmTextSetString( StatText, stat_str);
		}
		else
                {
		  XmTextSetString( StatText, no_position);
                }
		break;
	case GetLastPosition:
		sprintf(stat_str, "last position = %d",
			XmTextGetLastPosition(Text1));
		XmTextSetString( StatText, stat_str);
		break;
	case SetInsertionPosition:
		XmTextSetInsertionPosition(Text1, 
                                           (XmTextPosition)scale1_val);
		break;
	case GetBaseline:
		sprintf(stat_str, "baseline = %d", XmTextGetBaseline(Text1));
		XmTextSetString( StatText, stat_str);
		break;
	case GetTopCharacter:
		sprintf(stat_str, "top character = %d",
				XmTextGetTopCharacter(Text1));
		XmTextSetString( StatText, stat_str);
		break;
	case SetTopCharacter:
		XmTextSetTopCharacter(Text1, (XmTextPosition)scale1_val);
		break;
	default:
		printf("Please select a valid function.\n");
	};
}


static void TorFCB(Widget w, XtPointer client_data,
				XtPointer call_data)
{
	XmString	tcs;

	torf = ((XmToggleButtonCallbackStruct*)call_data)->set;
	if (torf)
		tcs = XmStringCreateLtoR("True", XmSTRING_DEFAULT_CHARSET);
	else
		tcs = XmStringCreateLtoR("False", XmSTRING_DEFAULT_CHARSET);

	n = 0;
	XtSetArg(args[n], XmNlabelString, tcs); n++;
	XtSetValues(w, args, n);

	XmStringFree(tcs);
}
