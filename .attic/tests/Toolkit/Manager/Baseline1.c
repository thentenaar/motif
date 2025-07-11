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
static char rcsid[] = "$XConsortium: Baseline1.c /main/7 1995/07/13 18:28:00 drk $"
#endif
#endif
#include <testlib.h>

char val_string[] = "Thomas, Lofton, Reed, and Kelly, spells victory\nfor \
the Bills in Minneapolis.\nSo I was wrong - so shoot me!!\nGo Bills!!";

Widget TitleWidget, Frame1;

void CreateChangeDialog();
static void ChangeVertAlign();
static void ChangeHorizAlign();
static void ChangeShadowType();
static void ChangeShadowThickness();
static void SetAlignmentString();

void  main (argc, argv)
    int     argc;
    char  **argv;
{
    register int       	n;
    Arg                	args[MAX_ARGS];
    XmString		tcs;
    Widget		Label2;
    Widget		BulletinB1, Text1;

    CommonTestInit(argc, argv);

    n = 0;
    XtSetArg(args[n], XmNshadowType, XmSHADOW_OUT);			n++;
    XtSetArg(args[n], XmNshadowThickness, 10);				n++;
    Frame1 = XmCreateFrame(Shell1, "Frame1", args, n);
    XtManageChild(Frame1);

    tcs = XmStringCreateSimple("Alignment String");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);				n++;
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD);		n++;
    XtSetArg(args[n], XmNchildHorizontalAlignment, XmALIGNMENT_CENTER);	n++;
    TitleWidget = XmCreateLabel(Frame1, "TitleWidget", args, n);
    XtManageChild(TitleWidget);

    n = 0;
    BulletinB1 = XmCreateBulletinBoard(Frame1, "BulletinB1", args, n);
    XtManageChild(BulletinB1);

    XmStringFree(tcs);

    XtRealizeWidget(Shell1);
    
    CommonPause();

    tcs = XmStringCreateSimple("Another Label");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);				n++;
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD);		n++;
    XtSetArg(args[n], XmNchildHorizontalAlignment, XmALIGNMENT_CENTER);	n++;
    Label2 = XmCreateLabel(Frame1, "Label2", args, n);
    XtManageChild(Label2);

    XmStringFree(tcs);

    CommonPause();

    XtDestroyWidget(Label2);

    /* 
       reset the original label to be XmFRAME_TITLE_CHILD, it was
       reset when Label2 was set as XmFRAME_TITLE_CHILD. 
    */

    n = 0;
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD);		n++;
    XtSetValues(TitleWidget, args, n);

    n = 0;
    XtSetArg(args[n], XmNcolumns, 50);					n++;
    XtSetArg(args[n], XmNrows, 5);					n++;
    XtSetArg(args[n], XmNvalue, val_string);				n++;
    XtSetArg(args[n], XmNchildType, XmFRAME_WORKAREA_CHILD);		n++;
    XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT);			n++;
    Text1 = XmCreateScrolledText(BulletinB1, "ScrolledText1", args, n);
    XtManageChild(Text1);

    CommonPause();

    n = 0;
    XtSetArg(args[n], XmNmarginWidth, 20);				n++;
    XtSetArg(args[n], XmNmarginHeight, 20);				n++;
    XtSetValues(Frame1, args, n);

    CommonPause();

    CreateChangeDialog(Frame1);

    CommonPause();

    n = 0;
    XtSetArg(args[n], XmNfontList, CommonGetFontList("9x15bold"));	n++;
    XtSetValues(TitleWidget, args, n);

    CommonPause();

    tcs = XmStringLtoRCreate("Motif\nScrolled Text Widget", 
			     XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);				n++;
    XtSetValues(TitleWidget, args, n);

    XmStringFree(tcs);

    CommonPause();

    n = 0;
    XtSetArg(args[n], XmNchildType, XmFRAME_GENERIC_CHILD);		n++;
    XtSetValues(TitleWidget, args, n);

    CommonPause();

    XtDestroyWidget(TitleWidget);

    n = 0;
    XtSetArg(args[n], XmNvalue, "You can even use a Text widget");	n++;
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD);		n++;
    XtSetArg(args[n], XmNchildHorizontalAlignment, XmALIGNMENT_END);	n++;
    XtSetArg(args[n], XmNwordWrap, True);				n++;
    XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT);			n++;
    XtSetArg(args[n], XmNrows, 2);					n++;
    TitleWidget = XmCreateText(Frame1, "Text1", args, n);
    XtManageChild(TitleWidget);

    CommonPause();

    XtAppMainLoop(app_context);

}


void CreateChangeDialog(widget)
    Widget widget;

{ 
    int 	n;
    Arg 	args[MAX_ARGS];
    Widget 	Toggle1, Toggle2, Toggle3,
		Toggle4, Toggle5;
    Widget	RadioBox1, PopupS;
    Widget	RadioBox2, RadioBox3, RowCol;
    Widget 	Toggle1a, Toggle2a, Toggle3a, Toggle4a;
    Widget 	Toggle1b, Toggle2b, Toggle3b;
    Widget 	Scale1;
    Widget      Text, ApplyBtn;
    XmString 	tcs;


    tcs = XmStringCreateSimple("Control Panel");

    n = 0;
    XtSetArg(args[n], XmNx, 0);				n++;
    XtSetArg(args[n], XmNy, 300);			n++;
    XtSetArg(args[n], XmNdefaultPosition, False);	n++;
    XtSetArg(args[n], XmNdialogTitle, tcs);		n++;
    PopupS = XmCreateBulletinBoardDialog(Shell1, "BulletinBD", args, n);

    XmStringFree(tcs);

    n = 0;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL);	n++;
    XtSetArg(args[n], XmNnumColumns, 2);		n++;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN);	n++;
    RowCol = XmCreateRowColumn(PopupS, "RowCol1", args, n);
    XtManageChild(RowCol);

    n = 0;
    RadioBox1 = XmCreateRadioBox(RowCol, "RadioBox1", args, n);
    XtManageChild(RadioBox1);

    tcs = XmStringCreateSimple("XmALIGNMENT_BASELINE_BOTTOM");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    Toggle1 = XmCreateToggleButton(RadioBox1, "Toggle1", args, n);
    XtManageChild(Toggle1);

    XtAddCallback(Toggle1, XmNarmCallback, (XtCallbackProc) ChangeVertAlign, 
		  (XtPointer) XmALIGNMENT_BASELINE_BOTTOM);

    XmStringFree(tcs);

    tcs = XmStringCreateSimple("XmALIGNMENT_BASELINE_TOP");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    Toggle2 = XmCreateToggleButton(RadioBox1, "Toggle2", args, n);
    XtManageChild(Toggle2);

    XtAddCallback(Toggle2, XmNarmCallback, (XtCallbackProc) ChangeVertAlign, 
		  (XtPointer)  XmALIGNMENT_BASELINE_TOP);

    XmStringFree(tcs);

    tcs = XmStringCreateSimple("XmALIGNMENT_WIDGET_TOP");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    Toggle3 = XmCreateToggleButton(RadioBox1, "Toggle3", args, n);
    XtManageChild(Toggle3);

    XtAddCallback(Toggle3, XmNarmCallback, (XtCallbackProc) ChangeVertAlign,
		  (XtPointer) XmALIGNMENT_WIDGET_TOP);

    XmStringFree(tcs);

    tcs = XmStringCreateSimple("XmALIGNMENT_CENTER");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    Toggle4 = XmCreateToggleButton(RadioBox1, "Toggle4", args, n);
    XtManageChild(Toggle4);

    XtAddCallback(Toggle4, XmNarmCallback, (XtCallbackProc) ChangeVertAlign, 
		  (XtPointer) XmALIGNMENT_CENTER);

    XmStringFree(tcs);
	
    tcs = XmStringCreateSimple("XmALIGNMENT_WIDGET_BOTTOM");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    Toggle5 = XmCreateToggleButton(RadioBox1, "Toggle5", args, n);
    XtManageChild(Toggle5);

    XtAddCallback(Toggle5, XmNarmCallback, (XtCallbackProc) ChangeVertAlign, 
		  (XtPointer) XmALIGNMENT_WIDGET_BOTTOM);

    XmStringFree(tcs);

    n = 0;
    RadioBox2 = XmCreateRadioBox(RowCol, "RadioBox1", args, n);
    XtManageChild(RadioBox2);

    tcs = XmStringCreateSimple("XmSHADOW_IN");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    Toggle1a = XmCreateToggleButton(RadioBox2, "Toggle1a", args, n);
    XtManageChild(Toggle1a);

    XtAddCallback(Toggle1a, XmNarmCallback, (XtCallbackProc) ChangeShadowType,
		  (XtPointer)XmSHADOW_IN);

    XmStringFree(tcs);

    tcs = XmStringCreateSimple("XmSHADOW_OUT");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    Toggle2a = XmCreateToggleButton(RadioBox2, "Toggle2a", args, n);
    XtManageChild(Toggle2a);

    XtAddCallback(Toggle2a, XmNarmCallback, (XtCallbackProc) ChangeShadowType, 
		  (XtPointer) XmSHADOW_OUT);

    XmStringFree(tcs);

    tcs = XmStringCreateSimple("XmSHADOW_ETCHED_IN");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    Toggle3a = XmCreateToggleButton(RadioBox2, "Toggle3a", args, n);
    XtManageChild(Toggle3a);

    XtAddCallback(Toggle3a, XmNarmCallback, (XtCallbackProc) ChangeShadowType, 
		  (XtPointer) XmSHADOW_ETCHED_IN);

    XmStringFree(tcs);

    tcs = XmStringCreateSimple("XmSHADOW_ETCHED_OUT");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    Toggle4a = XmCreateToggleButton(RadioBox2, "Toggle4a", args, n);
    XtManageChild(Toggle4a);

    XtAddCallback(Toggle4a, XmNarmCallback, ChangeShadowType, 
		  (XtPointer) XmSHADOW_ETCHED_OUT);

    XmStringFree(tcs);

    n = 0;
    RadioBox3 = XmCreateRadioBox(RowCol, "RadioBox3", args, n);
    XtManageChild(RadioBox3);

    tcs = XmStringCreateSimple("XmALIGNMENT_BEGINNING");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    Toggle1b = XmCreateToggleButton(RadioBox3, "Toggle1b", args, n);
    XtManageChild(Toggle1b);

    XtAddCallback(Toggle1b, XmNarmCallback, (XtCallbackProc) ChangeHorizAlign, 
		  (XtPointer) XmALIGNMENT_BEGINNING);

    XmStringFree(tcs);

    tcs = XmStringCreateSimple("XmALIGNMENT_CENTER");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    Toggle2b = XmCreateToggleButton(RadioBox3, "Toggle2b", args, n);
    XtManageChild(Toggle2b);

    XtAddCallback(Toggle2b, XmNarmCallback, (XtCallbackProc) ChangeHorizAlign, 
		  (XtPointer) XmALIGNMENT_CENTER);

    XmStringFree(tcs);

    tcs = XmStringCreateSimple("XmALIGNMENT_END");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    Toggle3b = XmCreateToggleButton(RadioBox3, "Toggle3b", args, n);
    XtManageChild(Toggle3b);

    XtAddCallback(Toggle3b, XmNarmCallback, (XtCallbackProc) ChangeHorizAlign, 
		  (XtPointer) XmALIGNMENT_END);

    XmStringFree(tcs);

    n = 0;
    XtSetArg (args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
    XtSetArg (args[n], XmNrows, 2); n++;
    Text = XmCreateText (RowCol, "Text", args, n);
    XtManageChild (Text);

    tcs = XmStringCreateSimple ("Apply String");

    n = 0;
    XtSetArg (args[n], XmNlabelString, tcs); n++;
    ApplyBtn = XmCreatePushButton (RowCol, "ApplyBtn", args, n);
    XtManageChild (ApplyBtn);

    XtAddCallback (ApplyBtn, XmNactivateCallback, SetAlignmentString, Text);

    XmStringFree(tcs);

    tcs = XmStringCreateSimple("Frame's ShadowThickness");

    n = 0;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL);			n++;
    XtSetArg(args[n], XmNmaximum, 50);					n++;
    XtSetArg(args[n], XmNminimum, 0);					n++;
    XtSetArg(args[n], XmNvalue, 10);					n++;
    XtSetArg(args[n], XmNshowValue, True);				n++;
    XtSetArg(args[n], XmNtitleString, tcs);				n++;
    Scale1 = XmCreateScale(RowCol, "Scale1", args, n);
    XtManageChild(Scale1);

    XmStringFree(tcs);

    XtAddCallback(Scale1, XmNvalueChangedCallback, 
		  (XtCallbackProc) ChangeShadowThickness, NULL);

    XtManageChild(PopupS);

}


static void ChangeHorizAlign(w, client_data, call_data)
    Widget w;
    unsigned char client_data;
    caddr_t  call_data;

{

    int 	n;
    Arg		args[1];

    n = 0;
    XtSetArg(args[n], XmNchildHorizontalAlignment, client_data); 	n++;
    XtSetValues(TitleWidget, args, n);

}


static void ChangeShadowThickness(w, client_data, call_data)
    Widget w;
    caddr_t  client_data;
    caddr_t  call_data;

{

    int 	n;
    Arg		args[1];
    XmScaleCallbackStruct *eventStruct;
    int 	value;

    eventStruct = (XmScaleCallbackStruct *) call_data;
    value = eventStruct->value;

    n = 0;
    XtSetArg(args[n], XmNshadowThickness, value);		n++;
    XtSetValues(Frame1, args, n);

}


static void ChangeVertAlign(w, client_data, call_data)
    Widget w;
    unsigned char  client_data;
    caddr_t  call_data;

{

    int 	n;
    Arg		args[1];

    n = 0;
    XtSetArg(args[n], XmNchildVerticalAlignment, client_data); 	n++;
    XtSetValues(TitleWidget, args, n);

}


static void ChangeShadowType(w, client_data, call_data)
    Widget w;
    XtPointer  client_data;
    caddr_t  call_data;

{

    int 	n;
    Arg		args[1];

    n = 0;
    XtSetArg(args[n], XmNshadowType, client_data); 	n++;
    XtSetValues(Frame1, args, n);

}

static void SetAlignmentString(w, client_data, call_data)
    Widget w;
    XtPointer  client_data;
    XtPointer  call_data;

{
    /* this fn. sets the Baseline Alignment String to the contents
       of the Text widget */

    int n;
    Arg args[1];
    String value_string;
    XmString cs;

    n = 0;
    XtSetArg (args[n], XmNvalue, &value_string); n++;
    XtGetValues ((Widget) client_data, args, n);

    cs = XmStringCreateLtoR (value_string, XmFONTLIST_DEFAULT_TAG);

    n = 0;
    XtSetArg (args[n], XmNlabelString, cs); n++;
    XtSetValues (TitleWidget, args, n);

    XmStringFree (cs);

 }
    
