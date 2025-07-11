/* $TOG: CommText12.c /main/7 1997/10/02 16:08:32 csn $ */
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
#include <Text12.h>


extern char *_XmStringGetTextConcat();

int	n;
Arg	args[MAX_ARGS];

static char font_name1[] = "variable";

static char render_tag[] = "render1";

void main_text12( argc, argv)
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
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
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
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNleftWidget, List1); n++;
	XtSetArg(args[n], XmNleftOffset, 15); n++;
	XtSetArg(args[n], XmNhighlightThickness, 2); n++;
	XtSetArg(args[n], XmNcolumns, 32); n++;
	XtSetArg(args[n], XmNrows, 1); n++;
	XtSetArg(args[n], XmNeditMode, XmSINGLE_LINE_EDIT); n++;
	XtSetArg(args[n], XmNvalue, starting_string_text); n++;
	XtSetArg(args[n], XmNfontList, newfontlist); n++;
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


#ifndef	MOTIF1_1

	/* build the Num Arg 3 Scale */
	tcs = XmStringLtoRCreate("Num Arg 3", XmSTRING_DEFAULT_CHARSET);

	n = 0;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNleftWidget, List1); n++;
	XtSetArg(args[n], XmNleftOffset, 15); n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNtopWidget, Scale2); n++;
	XtSetArg(args[n], XmNfontList, newfontlist); n++;
	XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
	XtSetArg(args[n], XmNtitleString, tcs); n++;
	XtSetArg(args[n], XmNshowValue, True); n++;
	XtSetArg(args[n], XmNmaximum, 40); n++;
	Scale3 = XmCreateScale(Form, "Scale3", args, n);
	XtManageChild(Scale3);

	XtAddCallback(Scale3, XmNvalueChangedCallback, Scale3CB, NULL);

	XmStringFree(tcs);

#endif	/* MOTIF1_1 */


	/* build the True/False Label and ToggleButton */
	tcs = XmStringCreateLtoR("True", XmSTRING_DEFAULT_CHARSET);

	n = 0;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNleftWidget, List1); n++;
	XtSetArg(args[n], XmNleftOffset, 15); n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
#ifndef	MOTIF1_1
	XtSetArg(args[n], XmNtopWidget, Scale3); n++;
#else
	XtSetArg(args[n], XmNtopWidget, Scale2); n++;
#endif	/* MOTIF1_1 */
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


#ifndef	MOTIF1_1

	/* build the Forward/Backword Label and ToggleButton */
	tcs = XmStringCreateLtoR("Forward", XmSTRING_DEFAULT_CHARSET);

	n = 0;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNleftWidget, List1); n++;
	XtSetArg(args[n], XmNleftOffset, 15); n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNtopWidget, Label2); n++;
	XtSetArg(args[n], XmNtopOffset, 15); n++;
	XtSetArg(args[n], XmNlabelString, tcs); n++;
	XtSetArg(args[n], XmNselectColor, CommonGetColor("darkorchid")); n++;
	XtSetArg(args[n], XmNset, True); n++;
	XtSetArg(args[n], XmNfontList, newfontlist); n++;
	ToggleB2 = XmCreateToggleButton(Form, "ToggleB2", args, n);
	XtManageChild(ToggleB2);

	forb = XmTEXT_FORWARD;
	XtAddCallback(ToggleB2, XmNvalueChangedCallback, ForBCB, NULL);

	XmStringFree(tcs);

	tcs = XmStringCreateLtoR("Forward/Backward Arg",
		XmSTRING_DEFAULT_CHARSET);

	n = 0;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNleftWidget, List1); n++;
	XtSetArg(args[n], XmNleftOffset, 15); n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNtopWidget, ToggleB2); n++;
	XtSetArg(args[n], XmNtopOffset, 0); n++;
	XtSetArg(args[n], XmNfontList, newfontlist); n++;
	XtSetArg(args[n], XmNlabelString, tcs); n++;
	Label6 = XmCreateLabel(Form, "Label6", args, n);
	XtManageChild(Label6);

	XmStringFree(tcs);

#endif	/* MOTIF1_1 */


	/* build the Status/Input Label and Text */
	n = 0;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNleftWidget, List1); n++;
	XtSetArg(args[n], XmNleftOffset, 15); n++;
	XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
#ifndef	MOTIF1_1
	XtSetArg(args[n], XmNtopWidget, Label6); n++;
#else
	XtSetArg(args[n], XmNtopWidget, Label2); n++;
#endif	/* MOTIF1_1 */
	XtSetArg(args[n], XmNtopOffset, 15); n++;
	XtSetArg(args[n], XmNcolumns, 25); n++;
	XtSetArg(args[n], XmNrows, 1); n++;
	XtSetArg(args[n], XmNeditMode, XmSINGLE_LINE_EDIT); n++;
	XtSetArg(args[n], XmNbackground, CommonGetColor("yellow")); n++;
	XtSetArg(args[n], XmNfontList, newfontlist); n++;
	StatText = XmCreateScrolledText(Form, "StatText", args, n);
	XtManageChild(StatText);

	XtAddCallback(StatText, XmNactivateCallback, ApplyCB, (XtPointer)0);

	tcs = XmStringCreateLtoR("Status/Input", XmSTRING_DEFAULT_CHARSET);

	n = 0;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNleftWidget, List1); n++;
	XtSetArg(args[n], XmNleftOffset, 15); n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNtopWidget, StatText); n++;
	XtSetArg(args[n], XmNtopOffset, 0); n++;
	XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNfontList, newfontlist); n++;
	XtSetArg(args[n], XmNlabelString, tcs); n++;
	Label4 = XmCreateLabel(Form, "Label4", args, n);
	XtManageChild(Label4);

	XmStringFree(tcs);
	XtRealizeWidget(Shell1);

	/* test of:
		XmTextGetSelectionPosition()
		XmTextGetSelection()
		XmTextSetSelection()
		XmTextClearSelection()
	*/
	n = 0;
	XtSetArg(args[n], XmNvisibleItemCount, 4); n++;
	XtSetArg(args[n], XmNtopItemPosition, 1); n++;
	XtSetValues(List1, args, n);

	CommonPause();

	/* test of:
		XmTextInsert()
	*/

	n = 0;
	XtSetArg(args[n], XmNvisibleItemCount, 1); n++;
	XtSetArg(args[n], XmNtopItemPosition, 5); n++;
	XtSetValues(List1, args, n);

	CommonPause();

	/* test of:
		XmTextSetAddMode()
	*/
	n = 0;
	XtSetArg(args[n], XmNvisibleItemCount, 1); n++;
	XtSetArg(args[n], XmNtopItemPosition, 6); n++;
	XtSetValues(List1, args, n);

	CommonPause();

	/* test of:
		XmTextGetString()
		XmTextSetString()
		XmTextReplace()
	*/
	n = 0;
	XtSetArg(args[n], XmNvisibleItemCount, 3); n++;
	XtSetArg(args[n], XmNtopItemPosition, 7); n++;
	XtSetValues(List1, args, n);

	CommonPause();

	/* test case for PIR 2878 */
	n = 0;
	XtSetArg(args[n], XmNvisibleItemCount, NITEMS); n++;
	XtSetArg(args[n], XmNtopItemPosition, 1); n++;
	XtSetValues(List1, args, n);

	CommonPause();
	/* end of test case for PIR 2878 */

#ifndef	MOTIF1_1

	/* test of:
		XmTextFindString()
	*/
	n = 0;
	XtSetArg(args[n], XmNvisibleItemCount, 1); n++;
	XtSetArg(args[n], XmNtopItemPosition, 10); n++;
	XtSetValues(List1, args, n);

	CommonPause();

	/* test of:
		XmTextGetSubstring()
	*/
	n = 0;
	XtSetArg(args[n], XmNvisibleItemCount, 1); n++;
	XtSetArg(args[n], XmNtopItemPosition, 11); n++;
	XtSetValues(List1, args, n);

	CommonPause();

	/* test of:
		XmTextDisableRedisplay()
		XmTextEnableRedisplay()
	(since we need other functions as well, we'll show
	 them all to test these functions)
	*/

	n = 0;
	XtSetArg(args[n], XmNvisibleItemCount, NITEMS); n++;
	XtSetArg(args[n], XmNtopItemPosition, 1); n++;
	XtSetValues(List1, args, n);

	CommonPause();

#endif	/* MOTIF1_1 */

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

#ifndef	MOTIF1_1

static void Scale3CB(Widget w, XtPointer client_data,
				XtPointer call_data)
{
	scale3_val = ((XmScaleCallbackStruct*)call_data)->value;
}

#endif	/* MOTIF1_1 */


static void SSelCB_List1(Widget w, XtPointer client_data,
				XtPointer call_data)
{
	function = ((XmListCallbackStruct*)call_data)->item_position;

	switch (function)
	{
	/* these don't use any controls */
	case GetSelectionPosition:
	case GetSelection:
	case ClearSelection:
#ifndef	MOTIF1_1
	case DisableRedisplay:
	case EnableRedisplay:
#endif	/* MOTIF1_1 */
		n = 0;
		XtSetArg(args[n], XmNsensitive, False); n++;
		XtSetValues(Scale1, args, n);
		XtSetValues(Scale2, args, n);
		XtSetValues(ToggleB, args, n);
#ifndef	MOTIF1_1
		XtSetValues(ToggleB2, args, n);
		XtSetValues(Scale3, args, n);
#endif	/* MOTIF1_1 */

		break;

	/* these use the Scale1 and Scale2 controls */
	case SetSelection:
		n = 0;
		XtSetArg(args[n], XmNsensitive, True); n++;
		XtSetValues(Scale1, args, n);
		XtSetValues(Scale2, args, n);

		n = 0;
		XtSetArg(args[n], XmNsensitive, False); n++;
		XtSetValues(ToggleB, args, n);
#ifndef	MOTIF1_1
		XtSetValues(ToggleB2, args, n);
		XtSetValues(Scale3, args, n);
#endif	/* MOTIF1_1 */

		break;

	/* these use only the ToggleB control */
	case SetAddMode:
		n = 0;
		XtSetArg(args[n], XmNsensitive, True); n++;
		XtSetValues(ToggleB, args, n);

		n = 0;
		XtSetArg(args[n], XmNsensitive, False); n++;
		XtSetValues(Scale1, args, n);
		XtSetValues(Scale2, args, n);
#ifndef	MOTIF1_1
		XtSetValues(ToggleB2, args, n);
		XtSetValues(Scale3, args, n);
#endif	/* MOTIF1_1 */

		break;

	/* these use the Scale1 and StatText controls */
	case Insert:
		n = 0;
		XtSetArg(args[n], XmNsensitive, True); n++;
		XtSetValues(Scale1, args, n);

		n = 0;
		XtSetArg(args[n], XmNsensitive, False); n++;
		XtSetValues(Scale2, args, n);
		XtSetValues(ToggleB, args, n);
#ifndef	MOTIF1_1
		XtSetValues(ToggleB2, args, n);
		XtSetValues(Scale3, args, n);
#endif	/* MOTIF1_1 */

		break;

	/* these use only the StatText control */
	case GetString:
	case SetString:
		n = 0;
		XtSetArg(args[n], XmNsensitive, False); n++;
		XtSetValues(Scale1, args, n);
		XtSetValues(Scale2, args, n);
		XtSetValues(ToggleB, args, n);
#ifndef	MOTIF1_1
		XtSetValues(ToggleB2, args, n);
		XtSetValues(Scale3, args, n);
#endif	/* MOTIF1_1 */

		break;

	/* these use the Scale1, Scale2 and StatText controls */
	case Replace:
		n = 0;
		XtSetArg(args[n], XmNsensitive, True); n++;
		XtSetValues(Scale1, args, n);
		XtSetValues(Scale2, args, n);

		n = 0;
		XtSetArg(args[n], XmNsensitive, False); n++;
		XtSetValues(ToggleB, args, n);
#ifndef	MOTIF1_1
		XtSetValues(ToggleB2, args, n);
		XtSetValues(Scale3, args, n);
#endif	/* MOTIF1_1 */

		break;

#ifndef	MOTIF1_1
	/* these use the Scale1, ToggleB2 and StatText controls */
	case FindString:
		n = 0;
		XtSetArg(args[n], XmNsensitive, True); n++;
		XtSetValues(Scale1, args, n);
		XtSetValues(ToggleB2, args, n);

		n = 0;
		XtSetArg(args[n], XmNsensitive, False); n++;
		XtSetValues(ToggleB, args, n);
		XtSetValues(Scale2, args, n);
		XtSetValues(Scale3, args, n);

		break;

	/* these use the Scale1, Scale2, Scale3 and StatText controls */
	case GetSubstring:
		n = 0;
		XtSetArg(args[n], XmNsensitive, True); n++;
		XtSetValues(Scale1, args, n);
		XtSetValues(Scale2, args, n);
		XtSetValues(Scale3, args, n);

		n = 0;
		XtSetArg(args[n], XmNsensitive, False); n++;
		XtSetValues(ToggleB, args, n);
		XtSetValues(ToggleB2, args, n);

		break;
#endif	/* MOTIF1_1 */

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
#ifndef	MOTIF1_1
	XtSetValues(Scale3, args, n);
#endif	/* MOTIF1_1 */

	tcs = XmStringCreateLtoR("True", XmSTRING_DEFAULT_CHARSET);
	n = 0;
	XtSetArg(args[n], XmNlabelString, tcs); n++;
	XtSetArg(args[n], XmNset, True); n++;
	XtSetValues(ToggleB, args, n);
	XmStringFree(tcs);

	torf = True;

#ifndef	MOTIF1_1

	tcs = XmStringCreateLtoR("Forward", XmSTRING_DEFAULT_CHARSET);
	n = 0;
	XtSetArg(args[n], XmNlabelString, tcs); n++;
	XtSetArg(args[n], XmNset, True); n++;
	XtSetValues(ToggleB2, args, n);
	XmStringFree(tcs);

	forb = XmTEXT_FORWARD;

#endif	/* MOTIF1_1 */

	XmTextSetString( StatText, "");
}

static void GetCharPtr( void *ret_val, char **tmp_str )
{
  *tmp_str = (char *)ret_val;
}

static void FreeCharPtr( void *value )
{
  XtFree((char *)value);
}

static void ApplyCB(Widget w, XtPointer client_data,
				XtPointer call_data)
{
	char	stat_str[50];
	XmTextPosition	left, right;
	Position	x, y;
	char	*tmp_str;
        void    *value;
        XmString string;
	XmAnyCallbackStruct *c_d;
#ifndef	MOTIF1_1
	XmTextPosition	tmp_pos;
	int	retval;
#endif	/* MOTIF1_1 */
        static char msg1[] = "No selection found.";

	c_d = (XmAnyCallbackStruct *) call_data;

	switch (function)
	{
	case GetSelectionPosition:
		if (XmTextGetSelectionPosition(Text1, &left, &right))
		{
			sprintf(stat_str,
				"left = %d, right = %d", left, right);
			XmTextSetString( StatText, stat_str);
		}
		else
		  XmTextSetString( StatText, msg1);
		break;
	case GetSelection:
 		tmp_str = NULL;
		tmp_str = XmTextGetSelection( Text1 );
		if (tmp_str == NULL)
                {
		  XmTextSetString( StatText, msg1);
                }
		else
                {
		  XmTextSetString( StatText, tmp_str );
                }
		break;
	case SetSelection:
		if (c_d->event != NULL)
			XmTextSetSelection(Text1, scale1_val, scale2_val,
				c_d->event->xbutton.time);
		break;
	case ClearSelection:
		if (c_d->event != NULL)
			XmTextClearSelection(Text1, c_d->event->xbutton.time);
		break;
	case Insert:
		value = XmTextGetString(StatText);
		XmTextInsert( Text1, (XmTextPosition)scale1_val, (char *)value);
		break;
	case SetAddMode:
		XmTextSetAddMode(Text1, torf);
		break;
	case GetString:
		value = XmTextGetString(Text1);
		XmTextSetString( StatText, (char *)value);
		break;
	case SetString:
		value = XmTextGetString(StatText);
		XmTextSetString( Text1, (char *)value);
		break;
	case Replace:
		value = XmTextGetString(StatText);
		XmTextReplace( Text1, scale1_val, scale2_val, (char *)value);
		break;

#ifndef	MOTIF1_1
	case FindString:
		value = XmTextGetString(StatText);
		tmp_str = XmTextGetString( StatText ); 
		if (XmTextFindString(Text1, (XmTextPosition)scale1_val,
			tmp_str, forb, &tmp_pos))
			{
			sprintf(stat_str,
				"String FOUND! See Action selection.");
			XmTextSetString( StatText, stat_str);
                     
			if (c_d->event != NULL)
				XmTextSetSelection(Text1,
					tmp_pos, tmp_pos + strlen(tmp_str),
					c_d->event->xbutton.time);
			}
		else
		{
			sprintf(stat_str,
				"String NOT Found.");
			XmTextSetString( StatText, stat_str);
		}
                  FreeCharPtr( value );
		break;
	case GetSubstring:
		tmp_str = (char *)XtMalloc(scale3_val);
		retval = XmTextGetSubstring(Text1, 
                                          (XmTextPosition)scale1_val,
			                  scale2_val, scale3_val, tmp_str);
		switch (retval)
		{
		case XmCOPY_SUCCEEDED:
		        XmTextSetString( StatText, tmp_str);
			break;
		case XmCOPY_FAILED:
			sprintf(stat_str, "RETURN VALUE: XmCOPY_FAILED");
			XmTextSetString( StatText, stat_str);
			break;
		case XmCOPY_TRUNCATED:
			sprintf(stat_str, "(XmCOPY_TRUNCATED) %s", tmp_str);
			XmTextSetString(StatText, stat_str);
			break;
		default:
			printf("XmTextGetSubstring() ");
			printf("returned bad value: %d\n", retval);
		};
                XtFree((char *)tmp_str);
		break;
	case DisableRedisplay:
		XmTextDisableRedisplay(Text1);
		break;
	case EnableRedisplay:
		XmTextEnableRedisplay(Text1);
		break;
#endif	/* MOTIF1_1 */

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


#ifndef	MOTIF1_1

static void ForBCB(Widget w, XtPointer client_data,
				XtPointer call_data)
{
	XmString	tcs;
	Boolean		forb_val;

	forb_val = ((XmToggleButtonCallbackStruct*)call_data)->set;
	if (forb_val)
		{
		tcs = XmStringCreateLtoR("Forward", XmSTRING_DEFAULT_CHARSET);
		forb = XmTEXT_FORWARD;
		}
	else
		{
		tcs = XmStringCreateLtoR("Backward", XmSTRING_DEFAULT_CHARSET);
		forb = XmTEXT_BACKWARD;
		}

	n = 0;
	XtSetArg(args[n], XmNlabelString, tcs); n++;
	XtSetValues(w, args, n);

	XmStringFree(tcs);
}

#endif	/* MOTIF1_1 */
