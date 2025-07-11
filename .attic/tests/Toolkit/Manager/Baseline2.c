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
static char rcsid[] = "$XConsortium: Baseline2.c /main/5 1995/07/13 18:28:48 drk $"
#endif
#endif

#include <testlib.h>
#include "gnu.bmp"

char multi_value[] = "This is a\nmulti line text\nwidget\nthat appears on\n\
5 lines";
char textf_value[] = "This is a Text Field Widget - One Line";

Widget  Form, Frame, RowColumn;
Widget	PushButton1, PushButton2;
Widget	Label1, Label2;
Widget  PushButtonGad1, PushButtonGad2;
Widget  LabelG1, LabelG2;
Widget	ToggleButton1, ToggleButton2;
Widget	ToggleButtonGad1, ToggleButtonGad2;
Widget 	Text, TextF;

Widget  Frame2, RowColumn2, RadioBox1;
Widget	Toggle_Top, Toggle_Bottom;
Widget  Toggle_Con_Bottom, Toggle_Con_Top;
Widget  Toggle_Center;
Widget  Scale;

Pixmap  pixmap1;
Pixmap  pixmap2;

static void CreatePanel();
static void ColumnsCB();
static void AlignCB();

void  main (argc, argv)
    int     argc;
    char  **argv;
{
    register int       	n;
    Arg                	args[MAX_ARGS];
    XmString		tcs;

    CommonTestInit(argc, argv);

    n = 0;
    Form = XmCreateForm(Shell1, "Form", args, n);
    XtManageChild(Form);

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM);	n++;
    Frame = XmCreateFrame(Form, "Frame", args, n);
    XtManageChild(Frame);

    n = 0;
    XtSetArg(args[n], XmNnumColumns, 2);			n++;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN);		n++;
    RowColumn = XmCreateRowColumn(Frame, "RowColumn", args, n);
    XtManageChild(RowColumn);

    tcs = XmStringCreateSimple("PushButton1");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);	n++;
    PushButton1 = XmCreatePushButton(RowColumn, "PushButton1", args, n);
    XtManageChild(PushButton1);

    XmStringFree(tcs);

    tcs = XmStringCreateSimple("PushButtonGadget1");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);	n++;
    PushButtonGad1 = XmCreatePushButtonGadget(RowColumn, "PushButtonGad1", 
					      args, n);
    XtManageChild(PushButtonGad1);

    XmStringFree(tcs);

    tcs = XmStringCreateLtoR("PushButtonGadget2\n\n - A Longer\n   multi line\n   Label String",
			     XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);	n++;
    PushButtonGad2 = XmCreatePushButtonGadget(RowColumn, "PushButtonGad2", 
					      args, n);
    XtManageChild(PushButtonGad2);

    XmStringFree(tcs);

    tcs = XmStringCreateSimple("Label");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);	n++;
    Label1 = XmCreateLabel(RowColumn, "Label1", args, n);
    XtManageChild(Label1);

    XmStringFree(tcs);

    pixmap1 = XmGetPixmap(XtScreenOfObject(Shell1), "woman", 
			  CommonGetColor("black"), CommonGetColor("white"));

    n = 0;
    XtSetArg(args[n], XmNlabelType, XmPIXMAP);	n++;
    XtSetArg(args[n], XmNlabelPixmap, pixmap1);	n++;
    Label2 = XmCreateLabel(RowColumn, "Label2", args, n);
    XtManageChild(Label2);

    tcs = XmStringCreateSimple("LabelGadget");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);	n++;
    LabelG1 = XmCreateLabel(RowColumn, "LabelG1", args, n);
    XtManageChild(LabelG1);

    XmStringFree(tcs);

    pixmap2 = XCreatePixmapFromBitmapData(display,
                                DefaultRootWindow(display),
                                gnu_bits, gnu_width, gnu_height,
                                WhitePixel(display,DefaultScreen(display)),
                                BlackPixel(display,DefaultScreen(display)),
                                DefaultDepth(display,DefaultScreen(display)));

    n = 0;
    XtSetArg(args[n], XmNlabelType, XmPIXMAP);	n++;
    XtSetArg(args[n], XmNlabelPixmap, pixmap2);	n++;
    ToggleButton1 = XmCreateToggleButton(RowColumn, "ToggleButton1", args, n);
    XtManageChild(ToggleButton1);

    tcs = XmStringCreateLtoR("ToggleB\n   - Longer\n     Label\n     String",
                                                     XmSTRING_DEFAULT_CHARSET);
    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);	n++;
    ToggleButton2 = XmCreateToggleButton(RowColumn, "ToggleButton2", args, n);
    XtManageChild(ToggleButton2);

    XmStringFree(tcs);

    tcs = XmStringCreateSimple("ToggleButtonGadget");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);	n++;
    ToggleButtonGad1 = XmCreateToggleButtonGadget(RowColumn, "ToggleButtonGad1",
					    args, n);
    XtManageChild(ToggleButtonGad1);

    XmStringFree(tcs);

    n = 0;
    XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT);	n++;
    XtSetArg(args[n], XmNvalue, multi_value);		n++;
    Text = XmCreateText(RowColumn, "Text", args, n);
    XtManageChild(Text);

    n = 0;
    XtSetArg(args[n], XmNvalue, textf_value);		n++;
    TextF = XmCreateTextField(RowColumn, "TextF", args, n);
    XtManageChild(TextF);

    CreatePanel();

    XtRealizeWidget(Shell1);

    CommonPause();
    CommonPause();

    n = 0;
    XtSetArg(args[n], XmNpacking, XmPACK_TIGHT);	n++;
    XtSetArg(args[n], XmNresizeHeight, False);		n++;
    XtSetArg(args[n], XmNheight, 250);			n++;
    XtSetValues(RowColumn, args, n);

    CommonPause();


    XtAppMainLoop(app_context);

}


static void CreatePanel()
{
    Arg args[MAX_ARGS];
    int n;
    XmString tcs;

    Widget formdia;

    n = 0;
    XtSetArg (args[n], XmNgeometry, "+300+0"); n++;
    formdia = XmCreateFormDialog (Form, "formdia", args, n);

    n = 0;
    Frame2 = XmCreateFrame(formdia, "Frame2", args, n);

    n = 0;
    XtSetArg(args[n], XmNspacing, 10);				n++;
    RowColumn2 = XmCreateRowColumn(Frame2, "RowColumn2", args, n);
    XtManageChild(RowColumn2);

    tcs = XmStringCreateSimple("Number of Columns");

    n = 0;
    XtSetArg(args[n], XmNtitleString, tcs);			n++;
    XtSetArg(args[n], XmNminimum, 1);				n++;
    XtSetArg(args[n], XmNmaximum, 10);				n++;
    XtSetArg(args[n], XmNvalue, 2);				n++;
    XtSetArg(args[n], XmNshowValue, True);			n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL);		n++;
    Scale = XmCreateScale(RowColumn2, "Scale", args, n);
    XtManageChild(Scale);

    XtAddCallback(Scale, XmNvalueChangedCallback, ColumnsCB, NULL);

    n = 0;
    RadioBox1 = XmCreateRadioBox(RowColumn2, "RadioBox1", args, n);
    XtManageChild(RadioBox1);

    tcs = XmStringCreateSimple("XmALIGNMENT_BASELINE_BOTTOM");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    Toggle_Bottom = XmCreateToggleButton(RadioBox1, "Toggle_Bottom", args, n);
    XtManageChild(Toggle_Bottom);

    XtAddCallback(Toggle_Bottom, XmNarmCallback, AlignCB, 
		  (XtPointer) XmALIGNMENT_BASELINE_BOTTOM);

    XmStringFree(tcs);

    tcs = XmStringCreateSimple("XmALIGNMENT_BASELINE_TOP");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    Toggle_Top = XmCreateToggleButton(RadioBox1, "Toggle_Top", args, n);
    XtManageChild(Toggle_Top);

    XtAddCallback(Toggle_Top, XmNarmCallback, AlignCB, 
		  (XtPointer) XmALIGNMENT_BASELINE_TOP);

    XmStringFree(tcs);

    tcs = XmStringCreateSimple("XmALIGNMENT_CONTENTS_BOTTOM");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    Toggle_Con_Bottom = XmCreateToggleButton(RadioBox1, "Toggle_Con_Bottom",
					     args, n);
    XtManageChild(Toggle_Con_Bottom);

    XtAddCallback(Toggle_Con_Bottom, XmNarmCallback, AlignCB, 
		  (XtPointer) XmALIGNMENT_CONTENTS_BOTTOM);

    XmStringFree(tcs);

    tcs = XmStringCreateSimple("XmALIGNMENT_CONTENTS_TOP");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    Toggle_Con_Top = XmCreateToggleButton(RadioBox1, "Toggle_Con_Top", args, n);
    XtManageChild(Toggle_Con_Top);

    XtAddCallback(Toggle_Con_Top, XmNarmCallback, AlignCB, 
		  (XtPointer) XmALIGNMENT_CONTENTS_TOP);

    XmStringFree(tcs);

    tcs = XmStringCreateSimple("XmALIGNMENT_CENTER");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    Toggle_Center = XmCreateToggleButton(RadioBox1, "Toggle_Center", args, n);
    XtManageChild(Toggle_Center);

    XtAddCallback(Toggle_Center, XmNarmCallback, AlignCB, 
		  (XtPointer) XmALIGNMENT_CENTER);

    XmStringFree(tcs);
    XtManageChild(Frame2);
    XtManageChild (formdia);
}


static void AlignCB(widget, client_data, call_data)
Widget widget;
XtPointer client_data, call_data;
{
    Arg args[1];

    XtSetArg(args[0], XmNentryVerticalAlignment, (unsigned char) client_data);
    XtSetValues(RowColumn, args, 1);

}


static void ColumnsCB(widget, client_data, call_data)
Widget widget;
XtPointer client_data, call_data;
{
    Arg args[1];
    int value;

    XtSetArg(args[0], XmNvalue, &value);
    XtGetValues(Scale, args, 1);

    XtSetArg(args[0], XmNnumColumns, value);
    XtSetValues(RowColumn, args, 1);

}
