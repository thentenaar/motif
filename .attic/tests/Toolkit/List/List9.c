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
static char rcsid[] = "$XConsortium: List9.c /main/8 1995/07/13 18:21:58 drk $"
#endif
#endif

#include <testlib.h>
#include "List9.h"

void  main (argc, argv)
    int     argc;
    char  **argv;
{
    register int       	n;
    Arg                	args[MAX_ARGS];
    XmString           	tcs, ItemList[NITEMS], ItemList2[NITEMS2];
    XmFontList         	newfontlist;

    CommonTestInit(argc, argv);

#ifndef MOTIF1_1
    newfontlist = CommonGetFontList("variable");

    n = 0;
    Form = XmCreateForm(Shell1, "Form", args, n);
    XtManageChild (Form);

    tcs = XmStringCreateLtoR("Functions:", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);         n++;
    XtSetArg(args[n], XmNtopOffset, 15);                        n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);        n++;
    XtSetArg(args[n], XmNleftOffset, 15);                       n++;
    XtSetArg(args[n], XmNfontList, newfontlist);  		n++;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    Label1 = XmCreateLabel(Form, "Label1", args, n);
    XtManageChild(Label1);

    XmStringFree(tcs);

    /* convert the plain strings to Xstrings */
    for (n = 0; n < NITEMS; n++)
	ItemList[n] = XmStringLtoRCreate(Istrings[n],
			  XmSTRING_DEFAULT_CHARSET);

    for (n = 0; n < NITEMS2; n++)
	ItemList2[n] = XmStringLtoRCreate(Istrings2[n],
			  XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);        n++;
    XtSetArg(args[n], XmNleftOffset, 15);                       n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
    XtSetArg(args[n], XmNtopWidget, Label1);                    n++;
    XtSetArg(args[n], XmNtopOffset, 15);                        n++;
    XtSetArg(args[n], XmNselectionPolicy, XmBROWSE_SELECT);	n++;
    XtSetArg(args[n], XmNhighlightOnEnter, False); 		n++;
    XtSetArg(args[n], XmNfontList, newfontlist);  		n++;
    XtSetArg(args[n], XmNitems, ItemList); 			n++;
    XtSetArg(args[n], XmNitemCount, NITEMS); 			n++;
    XtSetArg(args[n], XmNvisibleItemCount, NITEMS);		n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("cyan"));	n++;
    List1 = XmCreateList(Form, "List1", args, n);
    XtManageChild(List1);

    XtAddCallback(List1, XmNbrowseSelectionCallback, 
		  (XtCallbackProc) SSelCB_List1, NULL);

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);        n++;
    XtSetArg(args[n], XmNleftOffset, 15);                       n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
    XtSetArg(args[n], XmNtopWidget, List1);			n++;
    XtSetArg(args[n], XmNtopOffset, 50);                        n++;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    Frame = XmCreateFrame(Form, "Frame", args, n);
    XtManageChild(Frame);

    n = 0;
    XtSetArg(args[n], XmNspacing, 20);				n++;
    RowCol = XmCreateRowColumn(Frame, "RowCol", args, n);
    XtManageChild(RowCol);

    tcs = XmStringCreateSimple("XmListPosSelected() not called yet");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    XtSetArg(args[n], XmNfontList, newfontlist);		n++;
    Label_Selection = XmCreateLabel(RowCol, "Label_Selection", args, n);
    XtManageChild(Label_Selection);
    XmStringFree(tcs);

    tcs = XmStringCreateSimple("XmListGetKbdItemPos() not called yet");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    XtSetArg(args[n], XmNfontList, newfontlist);		n++;
    Label_Position = XmCreateLabel(RowCol, "Label_Position", args, n);
    XtManageChild(Label_Position);
    XmStringFree(tcs);

    tcs = XmStringCreateSimple("XmListPosToBounds() not called yet");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    XtSetArg(args[n], XmNfontList, newfontlist);		n++;
    Label_Bounds = XmCreateLabel(RowCol, "Label_Boundds", args, n);
    XtManageChild(Label_Bounds);
    XmStringFree(tcs);

    tcs = XmStringCreateLtoR("Call Function", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNleftWidget, Frame);			n++;
    XtSetArg(args[n], XmNleftOffset, 20);			n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNtopWidget, Label1);			n++;
    XtSetArg(args[n], XmNtopOffset, 15);			n++;
    XtSetArg(args[n], XmNfontList, newfontlist); 		n++;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    ApplyB = XmCreatePushButton(Form, "ApplyB", args, n);
    XtManageChild(ApplyB);

    XtAddCallback(ApplyB, XmNactivateCallback, (XtCallbackProc) ApplyCB, NULL);

    XmStringFree(tcs);

    tcs = XmStringCreateLtoR("Clear Args", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNleftWidget, Frame);			n++;
    XtSetArg(args[n], XmNleftOffset, 20);			n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNtopWidget, ApplyB);			n++;
    XtSetArg(args[n], XmNtopOffset, 15);			n++;
    XtSetArg(args[n], XmNfontList, newfontlist);  		n++;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    ClearB = XmCreatePushButton(Form, "ClearB", args, n);
    XtManageChild(ClearB);

    XtAddCallback(ClearB, XmNactivateCallback, (XtCallbackProc) ClearCB, NULL);

    XmStringFree(tcs);

    tcs = XmStringLtoRCreate("Position", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNleftWidget, Frame);			n++;
    XtSetArg(args[n], XmNleftOffset, 20);			n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNtopWidget, ClearB);			n++;
    XtSetArg(args[n], XmNtopOffset, 15);			n++;
    XtSetArg(args[n], XmNfontList, newfontlist);  		n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL);		n++;
    XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT);	n++;
    XtSetArg(args[n], XmNtitleString, tcs);			n++;
    XtSetArg(args[n], XmNshowValue, True);			n++;
    XtSetArg(args[n], XmNmaximum, NITEMS2 + 1);			n++;
    XtSetArg(args[n], XmNminimum, - 1);				n++;
    Scale = XmCreateScale(Form, "Scale", args, n);
    XtManageChild(Scale);
    XmStringFree(tcs);

    XtAddCallback(Scale, XmNvalueChangedCallback, (XtCallbackProc) ScaleCB,
		  NULL);

    tcs = XmStringCreateSimple("List AddMode Off");

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);		n++;
    XtSetArg(args[n], XmNleftWidget, Frame);				n++;
    XtSetArg(args[n], XmNleftOffset, 20);				n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
    XtSetArg(args[n], XmNtopWidget, Scale);				n++;
    XtSetArg(args[n], XmNtopOffset, 15);				n++;
    XtSetArg(args[n], XmNlabelString, tcs);				n++;
    XtSetArg(args[n], XmNselectColor, CommonGetColor("violet")); 	n++;
    Toggle_AddMode = XmCreateToggleButton(Form, "Toggle_AddMode", args, n);
    XtManageChild(Toggle_AddMode);
    XmStringFree(tcs);

    XtAddCallback(Toggle_AddMode, XmNvalueChangedCallback, 
		  (XtCallbackProc) AddModeCB, NULL);

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNleftWidget, Frame);			n++;
    XtSetArg(args[n], XmNleftOffset, 20);			n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNtopWidget, Toggle_AddMode);		n++;
    XtSetArg(args[n], XmNtopOffset, 15);			n++;
    RadioBox = XmCreateRadioBox(Form, "RadioBox", args, n);
    XtManageChild(RadioBox);

    Single = CreateToggle(RadioBox, "XmSINGLE_SELECT");
    Multiple = CreateToggle(RadioBox, "XmMULTIPLE_SELECT");
    Browse = CreateToggle(RadioBox, "XmBROWSE_SELECT");
    Extended = CreateToggle(RadioBox, "XmEXTENDED_SELECT");

    n = 0;
    XtSetArg(args[n], XmNset, True);				n++;
    XtSetValues(Browse, args, n);

    XtAddCallback(Single, XmNvalueChangedCallback, 
		  (XtCallbackProc) CHSPolicyCB, (XtPointer) XmSINGLE_SELECT);
    XtAddCallback(Multiple, XmNvalueChangedCallback, 
		  (XtCallbackProc) CHSPolicyCB, (XtPointer) XmMULTIPLE_SELECT);
    XtAddCallback(Browse, XmNvalueChangedCallback, 
		  (XtCallbackProc) CHSPolicyCB, (XtPointer) XmBROWSE_SELECT);
    XtAddCallback(Extended, XmNvalueChangedCallback, 
		  (XtCallbackProc) CHSPolicyCB, (XtPointer) XmEXTENDED_SELECT);

    tcs = XmStringCreateLtoR("Action List:", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);  	n++;
    XtSetArg(args[n], XmNtopOffset, 15);                 	n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
    XtSetArg(args[n], XmNrightOffset, 15);               	n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNleftWidget, RadioBox);			n++;
    XtSetArg(args[n], XmNleftOffset, 20);			n++;
    XtSetArg(args[n], XmNfontList, newfontlist); 		n++;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    Label2 = XmCreateLabel(Form, "Label2", args, n);
    XtManageChild(Label2);

    XmStringFree(tcs);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);  	n++;
    XtSetArg(args[n], XmNtopOffset, 15);                 	n++;
    XtSetArg(args[n], XmNtopWidget, Label2);			n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
    XtSetArg(args[n], XmNrightOffset, 15);               	n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNleftWidget, RadioBox);			n++;
    XtSetArg(args[n], XmNleftOffset, 20);			n++;
    XtSetArg(args[n], XmNlistMarginWidth, 5);			n++;
    XtSetArg(args[n], XmNselectionPolicy, XmBROWSE_SELECT);	n++;
    XtSetArg(args[n], XmNhighlightOnEnter, False); 		n++;
    XtSetArg(args[n], XmNfontList, newfontlist);  		n++;
    XtSetArg(args[n], XmNitems, ItemList2); 			n++;
    XtSetArg(args[n], XmNitemCount, NITEMS2); 			n++;
    XtSetArg(args[n], XmNvisibleItemCount, NITEMS2 - 5);	n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("wheat"));	n++;
    List2 = XmCreateScrolledList(Form, "List2", args, n);
    XtManageChild(List2);

    XtAddCallback(List2, XmNsingleSelectionCallback, 
		  (XtCallbackProc) CommonGenericCB, NULL);
    XtAddCallback(List2, XmNbrowseSelectionCallback, 
		  (XtCallbackProc) CommonGenericCB, NULL);
    XtAddCallback(List2, XmNextendedSelectionCallback, 
		  (XtCallbackProc) CommonGenericCB, NULL);
    XtAddCallback(List2, XmNmultipleSelectionCallback, 
		  (XtCallbackProc) CommonGenericCB, NULL);

    XtRealizeWidget(Shell1);

    CommonPause();
    CommonPause();
    CommonPause();

    /* Test for CR 5676 */
    CommonPause();
  
    XmListDeleteAllItems(List2);

    CommonPause();

    XtAppMainLoop(app_context);

#else

    printf("This test is for 1.2 only.\n");
    CommonPause();   /* Exit */

#endif /* MOTIF1_1 */

}

#ifndef MOTIF1_1

static void ScaleCB(Widget w, XtPointer client_data, XtPointer call_data)

{

    arg_count = ((XmScaleCallbackStruct*)call_data)->value;

}


static void SSelCB_List1(Widget w, XtPointer client_data, XtPointer call_data)

{
    function = ((XmListCallbackStruct*)call_data)->item_position;

    switch (function) {
	case GetKbdItemPos:
		printf("XmListGetKbdItemPos() chosen\n");
		break;
	case SetKbdItemPos:
		printf("XmListSetKbdItemPos() chosen\n");
		break;
	case PosSelected:
		printf("XmListPosSelected() chosen\n");
		break;
	case PosToBounds:
		printf("XmListPosToBounds() chosen\n");
		break;
	default:
		printf("Not a valid function\n");

	}
}


static void ClearCB(Widget w, XtPointer client_data, XtPointer call_data)

{
    XmListDeselectAllItems(List2);
    XmListDeselectAllItems(List1);
}


static void ApplyCB(Widget w, XtPointer client_data, XtPointer call_data)

{
    int 	keyboard_position;
    Boolean 	IsSelected, IsVisible;
    Arg 	args[1];
    XmString 	tcs;
    char 	s[50];
    Position	x, y;
    Dimension	width, height;

    switch (function) {
	case SetKbdItemPos:
		XmListSetKbdItemPos(List2, arg_count);
		break;
	case GetKbdItemPos:
		keyboard_position = XmListGetKbdItemPos(List2);
		sprintf(s, "Keyboard Position is %d", keyboard_position);
		tcs = XmStringCreateSimple(s);
		XtSetArg(args[0], XmNlabelString, tcs);
		XtSetValues(Label_Position, args, 1);
		XmStringFree(tcs);
		break;
	case PosSelected:
		IsSelected = XmListPosSelected(List2, arg_count);
		if (IsSelected)
			sprintf(s, "Position %d is Selected", arg_count);
		else
			sprintf(s, "Position %d is not Selected", arg_count);
		tcs = XmStringCreateSimple(s);
		XtSetArg(args[0], XmNlabelString, tcs);
		XtSetValues(Label_Selection, args, 1);
		XmStringFree(tcs);
		break;
	case PosToBounds:
		IsVisible = XmListPosToBounds(List2, arg_count,
					      &x, &y, &width, &height);
		if (IsVisible)
		  sprintf(s, "Position %d has bounding box (%d,%d)%dx%d",
			  arg_count, x, y, width, height);
		else sprintf(s, "Position %d is not visible", arg_count);
		tcs = XmStringCreateSimple(s);
		XtSetArg(args[0], XmNlabelString, tcs);
		XtSetValues(Label_Bounds, args, 1);
		XmStringFree(tcs);
		break;
	default:
		printf("Not a valid option\n");
    };
}


static Widget CreateToggle(Widget parent, char *label)

{
    Widget        widget;
    register int  n;
    Arg           args[3];
    XmString      tcs;

    tcs = XmStringLtoRCreate(label, XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);                             n++;
    XtSetArg(args[n], XmNselectColor, CommonGetColor("yellow"));	n++;
    widget = XmCreateToggleButton(parent, label, args, n);
    XtManageChild(widget);

    XmStringFree(tcs);

    return(widget);
}


static void CHSPolicyCB(Widget w, unsigned char client_data, 
			XtPointer call_data)

{
    int         n;
    Arg         args[1];

    n = 0;
    XtSetArg(args[n], XmNselectionPolicy, client_data);         n++;
    XtSetValues(List2, args, n);

}


static void AddModeCB(Widget w, XtPointer client_data, XtPointer call_data)

{

    XmString 	tcs, tcs2;
    Arg		args[1];

    tcs = XmStringCreateSimple("List AddMode Off");
    tcs2 = XmStringCreateSimple("List AddMode On");

    if (((XmToggleButtonCallbackStruct*)call_data)->set) {
	XtSetArg(args[0], XmNlabelString, tcs2);
	XmListSetAddMode(List2, True);
    }
    else {
	XtSetArg(args[0], XmNlabelString, tcs);
	XmListSetAddMode(List2, False);
    }
    XtSetValues(Toggle_AddMode, args, 1);
    XmStringFree(tcs);
    XmStringFree(tcs2);

}

#endif /* MOTIF1_1 */
