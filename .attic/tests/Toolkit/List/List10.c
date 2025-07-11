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
static char rcsid[] = "$XConsortium: List10.c /main/8 1995/07/13 18:14:46 drk $"
#endif
#endif

#include "testlib.h"
#include "List10.h"

void  main (argc, argv)
    int     argc;
    char  **argv;
{
    register int       n;
    Arg                args[MAX_ARGS];
    XmString           tcs, tcs2;

    CommonTestInit(argc, argv);

#ifndef MOTIF1_1

    font_list = CommonGetFontList("fixed");

    n = 0;
    XtSetArg(args[n], XmNdefaultFontList, CommonGetFontList("8x13"));	n++;
    XtSetValues(Shell1, args, n);

    for (n = 0; n < NITEMS; n++)
	ItemList[n] = XmStringLtoRCreate(Istrings[n],
			  XmSTRING_DEFAULT_CHARSET);

    my_selected[0] = XmStringCreateSimple("DumpStupidItem1");
    my_selected[1] = XmStringCreateSimple("DumpStupidItem2");
    my_selected[2] = XmStringCreateSimple("DumpStupidItem3");
    my_selected[3] = XmStringCreateSimple("DumpStupidItem4");
    my_selected[4] = XmStringCreateSimple("DumpStupidItem5");

    /* Create Outer Form */

    n = 0;
    XtSetArg(args[n], XmNbackground, CommonGetColor("ForestGreen"));	n++;
    Form = XmCreateForm(Shell1, "Form", args, n);
    XtManageChild(Form);

    /* Create List Label */

    tcs = XmStringLtoRCreate("Action List:",  XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
    XtSetArg(args[n], XmNleftOffset, 5);				n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);    		n++;
    XtSetArg(args[n], XmNtopOffset, 5);					n++;
    XtSetArg(args[n], XmNlabelString, tcs);				n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("ForestGreen"));	n++;
    XtSetArg(args[n], XmNfontList, CommonGetFontList("variable"));	n++;
    Label_List1 = XmCreateLabel(Form, "Label_List1", args, n);
    XtManageChild(Label_List1);

    XmStringFree(tcs);

    /* Create Action List */

    n = 0;
    XtSetArg(args[n], XmNlistSpacing, 5);  			n++;
    XtSetArg(args[n], XmNlistMarginWidth, 2);  			n++;
    XtSetArg(args[n], XmNlistMarginHeight, 2);  		n++;
    XtSetArg(args[n], XmNlistSizePolicy, XmVARIABLE);  		n++;
    XtSetArg(args[n], XmNitems, ItemList); 			n++;
    XtSetArg(args[n], XmNitemCount, NITEMS); 			n++;
    XtSetArg(args[n], XmNvisibleItemCount, NITEMS - 10);	n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
    XtSetArg(args[n], XmNleftOffset, 5);			n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);    	n++;
    XtSetArg(args[n], XmNtopWidget, Label_List1);		n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("Wheat"));	n++;
    XtSetArg(args[n], XmNfontList, CommonGetFontList("fixed"));	n++;
    XtSetArg(args[n], XmNselectedItemCount, 5);			n++;
    XtSetArg(args[n], XmNselectedItems, my_selected);		n++;
    List1 = XmCreateScrolledList(Form, "List1", args, n);
    XtManageChild(List1);

    /* Add Action ListCallbacks */

    XtAddCallback(List1, XmNsingleSelectionCallback, 
		  (XtCallbackProc) BrSiDeSelCB, NULL);
    XtAddCallback(List1, XmNmultipleSelectionCallback, 
		  (XtCallbackProc) MulSelCB, NULL);
    XtAddCallback(List1, XmNextendedSelectionCallback, 
		  (XtCallbackProc) ExtSelCB, NULL);
    XtAddCallback(List1, XmNbrowseSelectionCallback, 
		  (XtCallbackProc) BrSiDeSelCB, NULL);
    XtAddCallback(List1, XmNdefaultActionCallback, 
		  (XtCallbackProc) BrSiDeSelCB, NULL);

    /* Create Panel of Selected Items */

    CreateOptions();

    CreateCallbackInfo();

    XtRealizeWidget(Shell1);

    CommonDumpHierarchy(Shell1, stdout);

    CommonPause();

    XmTextFieldSetString(TextF_AddSL, NULL);
    XmTextFieldSetString(TextF_AddIL, NULL);

    CommonPause();

    XmTextFieldSetString(TextF_AddSL, NULL);
    XmTextFieldSetString(TextF_AddIL, NULL);

    CommonPause();

    XtAppMainLoop(app_context);

#else

    printf("This test is for 1.2 only.\n");
    CommonPause();

#endif /* MOTIF1_1 */

}

#ifndef MOTIF1_1

static void CreateOptions()

{
    Arg		args[MAX_ARGS];
    int		n;
    XmString	tcs;

    tcs = XmStringCreateLtoR("List Resources:", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);				n++;
    XtSetArg(args[n], XmNfontList, CommonGetFontList("variable"));	n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("ForestGreen"));  	n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);			n++;
    XtSetArg(args[n], XmNtopOffset, 5);					n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);		n++;
    XtSetArg(args[n], XmNleftWidget, List1);				n++;
    XtSetArg(args[n], XmNleftOffset, 5);				n++;
    Label_Resources = XmCreateLabel(Form, "Label_Resources", args, n);
    XtManageChild(Label_Resources);

    XmStringFree(tcs);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
    XtSetArg(args[n], XmNtopWidget, Label_Resources);			n++;
    XtSetArg(args[n], XmNtopOffset, 5);					n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);		n++;
    XtSetArg(args[n], XmNleftWidget, List1);				n++;
    XtSetArg(args[n], XmNleftOffset, 5);				n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM);		n++;
    XtSetArg(args[n], XmNbottomOffset, 5);				n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("forestgreen"));	n++;
    XtSetArg(args[n], XmNshadowThickness, 5);				n++;
    Frame1 = XmCreateFrame(Form, "Frame1", args, n);
    XtManageChild(Frame1);

    n = 0;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER);		n++;
    XtSetArg(args[n], XmNspacing, 6);					n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL);			n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("forestgreen"));	n++;
    RowCol1 = XmCreateRowColumn(Frame1, "RowCol1", args, n);
    XtManageChild(RowCol1);

    n = 0;
    XtSetArg(args[n], XmNbackground, CommonGetColor("forestgreen"));	n++;
    RadioBox = XmCreateRadioBox(RowCol1, "RadioBox", args, n);
    XtManageChild(RadioBox);

    Single = CreateToggleButton(RadioBox, "XmSINGLE_SELECT");
    Multiple = CreateToggleButton(RadioBox, "XmMULTIPLE_SELECT");
    Browse = CreateToggleButton(RadioBox, "XmBROWSE_SELECT");
    Extended = CreateToggleButton(RadioBox, "XmEXTENDED_SELECT");

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

    tcs = XmStringCreateSimple("Dont Add Item Unselected");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);				n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("forestgreen"));	n++;
    XtSetArg(args[n], XmNselectColor, CommonGetColor("yellow"));	n++;
    XtSetArg(args[n], XmNfontList, CommonGetFontList("8x13"));		n++;
    Toggle_Selected = XmCreateToggleButton(RowCol1, "Toggle_Selected", args, n);
    XtManageChild(Toggle_Selected);
    XmStringFree(tcs);

    XtAddCallback(Toggle_Selected, XmNvalueChangedCallback, 
		  (XtCallbackProc) ToggleCB, NULL);

    tcs = XmStringCreateSimple("Add String to Selected List:");

    n = 0; 
    XtSetArg(args[n], XmNlabelString, tcs);				n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("forestgreen"));	n++;
    XtSetArg(args[n], XmNfontList, CommonGetFontList("8x13"));		n++;
    Label_AddSL = XmCreateLabel(RowCol1, "Label_AddSL", args, n);
    XtManageChild(Label_AddSL);
    XmStringFree(tcs);

    n = 0;
    XtSetArg(args[n], XmNbackground, CommonGetColor("Gray")); 		n++;
    XtSetArg(args[n], XmNeditable, True);				n++;
    TextF_AddSL = XmCreateTextField(RowCol1, "TextF_AddSL", args, n);
    XtManageChild(TextF_AddSL);

    XtAddCallback(TextF_AddSL, XmNactivateCallback, 
		  (XtCallbackProc) AddSLCB, NULL);

    tcs = XmStringCreateSimple("Add String to Items List:");

    n = 0; 
    XtSetArg(args[n], XmNlabelString, tcs);				n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("forestgreen"));	n++;
    XtSetArg(args[n], XmNfontList, CommonGetFontList("8x13"));		n++;
    Label_AddIL = XmCreateLabel(RowCol1, "Label_AddIL", args, n);
    XtManageChild(Label_AddIL);
    XmStringFree(tcs);

    n = 0;
    XtSetArg(args[n], XmNbackground, CommonGetColor("Gray")); 		n++;
    XtSetArg(args[n], XmNeditable, True);				n++;
    TextF_AddIL = XmCreateTextField(RowCol1, "TextF_AddIL", args, n);
    XtManageChild(TextF_AddIL);

    XtAddCallback(TextF_AddIL, XmNactivateCallback, 
		  (XtCallbackProc) AddILCB, NULL);

    tcs = XmStringCreateLtoR("Call XmListUpdateSelectedList", 
			     XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);				n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("ForestGreen"));	n++;
    XtSetArg(args[n], XmNfontList, CommonGetFontList("8x13"));		n++;
    PushB_Update = XmCreatePushButton(RowCol1, "PushB_Update", args, n);
    XtManageChild(PushB_Update);
    XmStringFree(tcs);

    XtAddCallback(PushB_Update, XmNactivateCallback, 
		  (XtCallbackProc) UpdateCB, NULL);

    tcs = XmStringCreateLtoR("Get SelectedItems", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);				n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("ForestGreen"));	n++;
    XtSetArg(args[n], XmNfontList, CommonGetFontList("8x13"));		n++;
    PushB_GetSelected = XmCreatePushButton(RowCol1, "PushB_GetSelected", 
					   args, n);
    XtManageChild(PushB_GetSelected);
    XmStringFree(tcs);

    XtAddCallback(PushB_GetSelected, XmNactivateCallback, 
		  (XtCallbackProc) SelectedCB, NULL);

    tcs = XmStringCreateLtoR("Selected Items:", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);				n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("ForestGreen"));	n++;
    XtSetArg(args[n], XmNfontList, CommonGetFontList("8x13"));		n++;
    Label_Selected = XmCreateLabel(RowCol1, "Label_Selected", args, n);
    XtManageChild(Label_Selected);
    XmStringFree(tcs);

    n = 0;
    XtSetArg(args[n], XmNlistSpacing, 5);  				n++;
    XtSetArg(args[n], XmNlistMarginWidth, 2);  				n++;
    XtSetArg(args[n], XmNlistMarginHeight, 2);  			n++;
    XtSetArg(args[n], XmNlistSizePolicy, XmVARIABLE);  			n++;
    XtSetArg(args[n], XmNvisibleItemCount, 10);				n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("goldenrod"));	n++;
    Selected_ListSV = XmCreateScrolledList(RowCol1, "Selected_ListSV", args, n);
    XtManageChild(Selected_ListSV);

}


static void CreateCallbackInfo()

{
    Arg 	args[MAX_ARGS];
    int 	n;
    XmString	tcs;

    tcs = XmStringCreateLtoR("Callback Information:", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);				n++;
    XtSetArg(args[n], XmNfontList, CommonGetFontList("variable"));	n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("ForestGreen"));  	n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);		n++;
    XtSetArg(args[n], XmNrightOffset, 5);				n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);			n++;
    XtSetArg(args[n], XmNtopOffset, 5);					n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);		n++;
    XtSetArg(args[n], XmNleftWidget, Frame1);				n++;
    XtSetArg(args[n], XmNleftOffset, 5);				n++;
    Label_RowCol = XmCreateLabel(Form, "Label_RowCol", args, n);
    XtManageChild(Label_RowCol);

    n = 0;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);		n++;
    XtSetArg(args[n], XmNrightOffset, 5);				n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
    XtSetArg(args[n], XmNtopWidget, Label_RowCol);			n++;
    XtSetArg(args[n], XmNtopOffset, 5);					n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);		n++;
    XtSetArg(args[n], XmNleftWidget, Frame1);				n++;
    XtSetArg(args[n], XmNleftOffset, 5);				n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("forestgreen"));	n++;
    XtSetArg(args[n], XmNshadowThickness, 5);				n++;
    Frame2 = XmCreateFrame(Form, "Frame2" , args, n);
    XtManageChild(Frame2);

    n = 0;
    XtSetArg(args[n], XmNorientation, XmVERTICAL);			n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("cyan"));		n++;
    RowCol2 = XmCreateRowColumn(Frame2, "RowCol2", args, n);
    XtManageChild(RowCol2);

    tcs = XmStringCreateLtoR("Reason = <Not Set>", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);				n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("cyan"));		n++;
    Label_Reason = XmCreateLabel(RowCol2, "Label_Reason", args, n);
    XtManageChild(Label_Reason);

    XmStringFree(tcs);

    tcs = XmStringCreateLtoR("Item = <Not Set>", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);				n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("cyan"));		n++;
    Label_Item = XmCreateLabel(RowCol2, "Label_Item", args, n);
    XtManageChild(Label_Item);

    XmStringFree(tcs);

    tcs = XmStringCreateLtoR("Item Length = <Not Set>", 
					XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);				n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("cyan"));		n++;
    Label_Item_Count = XmCreateLabel(RowCol2, "Label_Item_Count", args, n);
    XtManageChild(Label_Item_Count);

    XmStringFree(tcs);

    tcs = XmStringCreateLtoR("Item Position = <Not Set>", 
					XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);				n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("cyan"));		n++;
    Label_Item_Pos = XmCreateLabel(RowCol2, "Label_Item_Pos", args, n);
    XtManageChild(Label_Item_Pos);

    XmStringFree(tcs);

    tcs = XmStringCreateLtoR("Selection Type = <Not Set>", 
					XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);				n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("cyan"));		n++;
    Label_Select_Type = XmCreateLabel(RowCol2, "Label_Select_Type", args, n);
    XtManageChild(Label_Select_Type);

    XmStringFree(tcs);

    tcs = XmStringCreateLtoR("Selected Item Count = <Not Set>", 
					XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);				n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("cyan"));		n++;
    Label_Selected_Count = XmCreateLabel(RowCol2, "Label_Selected_Count", 
						args, n);
    XtManageChild(Label_Selected_Count);

    XmStringFree(tcs);

    tcs = XmStringCreateLtoR("Selection Item List:",
					XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);				n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("cyan"));		n++;
    Label_Selected_Items = XmCreateLabel(RowCol2, "Label_Selected_Items", 
						args, n);
    XtManageChild(Label_Selected_Items);

    XmStringFree(tcs);

    n = 0;
    XtSetArg(args[n], XmNlistSpacing, 5);  			n++;
    XtSetArg(args[n], XmNlistMarginWidth, 2);  			n++;
    XtSetArg(args[n], XmNlistMarginHeight, 2);  		n++;
    XtSetArg(args[n], XmNvisibleItemCount, 20);			n++;
    XtSetArg(args[n], XmNlistSizePolicy, XmVARIABLE);  		n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("cyan"));	n++;
    Selected_List = XmCreateScrolledList(RowCol2, "Selected_List", args, n);
    XtManageChild(Selected_List);

}


static Widget  CreateToggleButton(Widget parent, char *label)

{
    Widget        widget;
    register int  n;
    Arg           args[4];
    XmString      tcs;

    tcs = XmStringLtoRCreate(label, XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);  				n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("ForestGreen")); 	n++;
    XtSetArg(args[n], XmNfontList, CommonGetFontList("8x13"));		n++;
    XtSetArg(args[n], XmNselectColor, CommonGetColor("wheat"));		n++;
    widget = XmCreateToggleButton(parent, label, args, n);
    XtManageChild(widget);

    XmStringFree(tcs);

    return(widget);
}


static void BrSiDeSelCB(Widget w, caddr_t client_data, caddr_t call_data)

{
    XmString 	tcs;
    int		reason, item_length, item_pos, i;
    char	string[50];
    XmString	item;

    reason = ((XmListCallbackStruct*)call_data)->reason;

    item = ((XmListCallbackStruct*)call_data)->item;
    sprintf(string, "Item = \"%s\"", CommonCsToRs(item));
    ChangeLabel(Label_Item, string);

    item_length = ((XmListCallbackStruct*)call_data)->item_length;
    sprintf(string, "Item Length = %d", item_length);
    ChangeLabel(Label_Item_Count, string);

    item_pos = ((XmListCallbackStruct*)call_data)->item_position;
    sprintf(string, "Item Position = %d", item_pos);
    ChangeLabel(Label_Item_Pos, string);

    ChangeLabel(Label_Select_Type, "Selection Type = N/A");
    ChangeLabel(Label_Selected_Count, "Selected Item Count = N/A");

    printf("Callback Information:\n\tReason:\t\t");

    for (i = 0 ; i < 50 ; i ++) {
	string[i] = '\0';
    }
    strcat(string, "Reason =");

    switch (reason) {
	case XmCR_SINGLE_SELECT:	printf("XmCR_SINGLE_SELECT\n");
					strcat(string, " XmCR_SINGLE_SELECT");
					break;
	case XmCR_DEFAULT_ACTION:	printf("XmCR_DEFAULT_ACTION\n");
					strcat(string, " XmCR_DEFAULT_ACTION");
					break;
	case XmCR_BROWSE_SELECT:	printf("XmCR_BROWSE_SELECT\n");
					strcat(string, " XmCR_BROWSE_SELECT");
					break;
	case XmCR_MULTIPLE_SELECT:	printf("XmCR_MULTIPLE_SELECT\n");
					strcat(string, " XmCR_MULTIPLE_SELECT");
					break;
	case XmCR_EXTENDED_SELECT:	printf("XmCR_EXTENDED_SELECT\n");
					strcat(string, " XmCR_EXTENDED_SELECT");
					break;
	default:			printf("unknown reason\n");
					strcat(string, " unknown reason");
					break;
    };

    ChangeLabel(Label_Reason, string);

    printf("\tItem:\t\t%s\n\tItem Length:\t%d\n\tItem Position:\t%d\n\n",
		CommonCsToRs(item), item_length, item_pos);


    Update_selected_items(NULL, 0, True, NULL);
}


static void MulSelCB(Widget w, caddr_t client_data, caddr_t call_data)

{
    int 	count, type;
    int		*selected_pos, i;
    XmString 	tcs;
    char	string[50];
    int		reason, item_length, item_pos, item_count;
    XmString	item;

    reason = ((XmListCallbackStruct*)call_data)->reason;

    item_count = ((XmListCallbackStruct*)call_data)->selected_item_count;
    sprintf(string, "Selected Item Count = %d", item_count);
    ChangeLabel(Label_Selected_Count, string);

    item = ((XmListCallbackStruct*)call_data)->item;
    sprintf(string, "Item = \"%s\"", CommonCsToRs(item));
    ChangeLabel(Label_Item, string);

    item_length = ((XmListCallbackStruct*)call_data)->item_length;
    sprintf(string, "Item Length = %d", item_length);
    ChangeLabel(Label_Item_Count, string);

    item_pos = ((XmListCallbackStruct*)call_data)->item_position;
    sprintf(string, "Item Position = %d", item_pos);
    ChangeLabel(Label_Item_Pos, string);

    ChangeLabel(Label_Select_Type, "Selection Type = N/A");

    selected_pos = ((XmListCallbackStruct*)call_data)->selected_item_positions;

    printf("Callback Information:\n\tReason:\t\t");

    for (i = 0 ; i < 50 ; i ++) {
	string[i] = '\0';
    }
    strcat(string, "Reason =");

    switch (reason) {
	case XmCR_SINGLE_SELECT:	printf("XmCR_SINGLE_SELECT\n");
					strcat(string, " XmCR_SINGLE_SELECT");
					break;
	case XmCR_DEFAULT_ACTION:	printf("XmCR_DEFAULT_ACTION\n");
					strcat(string, " XmCR_DEFAULT_ACTION");
					break;
	case XmCR_BROWSE_SELECT:	printf("XmCR_BROWSE_SELECT\n");
					strcat(string, " XmCR_BROWSE_SELECT");
					break;
	case XmCR_MULTIPLE_SELECT:	printf("XmCR_MULTIPLE_SELECT\n");
					strcat(string, " XmCR_MULTIPLE_SELECT");
					break;
	case XmCR_EXTENDED_SELECT:	printf("XmCR_EXTENDED_SELECT\n");
					strcat(string, " XmCR_EXTENDED_SELECT");
					break;
	default:			printf("unknown reason\n");
					strcat(string, " unknown reason");
					break;
    };

    ChangeLabel(Label_Reason, string);

    printf("\tItem:\t\t%s\n\tItem Length:\t%d\n\tItem Position:\t%d\n\n",
		CommonCsToRs(item), item_length, item_pos);

    Update_selected_items(((XmListCallbackStruct*)call_data)->selected_items,
		     ((XmListCallbackStruct*)call_data)->selected_item_count,
		     False, selected_pos);
}


static void ExtSelCB(Widget w, caddr_t client_data, caddr_t call_data)

{
    int 	count, type;
    int 	i, *selected_pos;
    XmString 	tcs;
    char	string[50];
    int		reason, item_length, item_pos, item_count;
    XmString	item;

    reason = ((XmListCallbackStruct*)call_data)->reason;

    item_count = ((XmListCallbackStruct*)call_data)->selected_item_count;
    sprintf(string, "Selected Item Count = %d", item_count);
    ChangeLabel(Label_Selected_Count, string);

    item = ((XmListCallbackStruct*)call_data)->item;
    sprintf(string, "Item = \"%s\"", CommonCsToRs(item));
    ChangeLabel(Label_Item, string);

    item_length = ((XmListCallbackStruct*)call_data)->item_length;
    sprintf(string, "Item Length = %d", item_length);
    ChangeLabel(Label_Item_Count, string);

    item_pos = ((XmListCallbackStruct*)call_data)->item_position;
    sprintf(string, "Item Position = %d", item_pos);
    ChangeLabel(Label_Item_Pos, string);

    type = ((XmListCallbackStruct*)call_data)->selection_type;
    sprintf(string, "Selection Type = %d", type);
    ChangeLabel(Label_Select_Type, string);

    selected_pos = ((XmListCallbackStruct*)call_data)->selected_item_positions;

    printf("Callback Information:\n\tReason:\t\t");

    for (i = 0 ; i < 50 ; i ++) {
	string[i] = '\0';
    }

    strcat(string, "Reason =");

    switch (reason) {
	case XmCR_SINGLE_SELECT:	printf("XmCR_SINGLE_SELECT\n");
					strcat(string, " XmCR_SINGLE_SELECT");
					break;
	case XmCR_DEFAULT_ACTION:	printf("XmCR_DEFAULT_ACTION\n");
					strcat(string, " XmCR_DEFAULT_ACTION");
					break;
	case XmCR_BROWSE_SELECT:	printf("XmCR_BROWSE_SELECT\n");
					strcat(string, " XmCR_BROWSE_SELECT");
					break;
	case XmCR_MULTIPLE_SELECT:	printf("XmCR_MULTIPLE_SELECT\n");
					strcat(string, " XmCR_MULTIPLE_SELECT");
					break;
	case XmCR_EXTENDED_SELECT:	printf("XmCR_EXTENDED_SELECT\n");
					strcat(string, " XmCR_EXTENDED_SELECT");
					break;
	default:			printf("unknown reason\n");
					strcat(string, " unknown reason");
					break;
    };

    ChangeLabel(Label_Reason, string);

    printf("\tItem:\t\t%s\n\tItem Length:\t%d\n\tItem Position:\t%d\n",
		CommonCsToRs(item), item_length, item_pos);

    printf("\tSelection Type:\t");

    for (i = 0 ; i < 50 ; i ++) {
	string[i] = '\0';
    }
    strcat(string, "Selection Type =");

    switch (type) {
	case XmINITIAL: 	printf("Initial\n");
				strcat(string, " Initial");
			      	break;
	case XmADDITION: 	printf("Addition\n");
				strcat(string, " Addition");
			      	break;
	case XmMODIFICATION: 	printf("Modify\n");
				strcat(string, " Modify");
			      	break;
	default: 		printf("not defined\n");
				strcat(string, " not defined");
				break;
    }

    ChangeLabel(Label_Select_Type, string);

    printf("\n");

    Update_selected_items(((XmListCallbackStruct*)call_data)->selected_items,
		     ((XmListCallbackStruct*)call_data)->selected_item_count,
		     False, selected_pos);
}


static void CHSPolicyCB(Widget w, unsigned char client_data, caddr_t call_data)

{
    int 	n;
    Arg 	args[1];
    int		selection_policy;

    n = 0;
    XtSetArg(args[n], XmNselectionPolicy, client_data);		n++;
    XtSetValues(List1, args, n);

}

static void Update_selected_items(XmString *item_list, int item_count,
				  Boolean single_or_browse, int *selected_pos)

{
    int 	n, i;
    int		visible_count;
    Arg 	args[3];
    char	tmp_string[50];
    XmString	item_list2[NITEMS];

    /* Append to the end of the item string. The Items postion from 
       the callback field selected_item_position */


    if (selected_pos != NULL) {
	for (i = 0 ; i < item_count ; i++) {
		sprintf(tmp_string, "%s (%d)", CommonCsToRs(item_list[i]),
			selected_pos[i]);
		item_list2[i] = XmStringCreateLtoR(tmp_string,
						     XmSTRING_DEFAULT_CHARSET);
	}
    }

    /* If single, browse, or defaultAction callback, then get
       the ItemList and the ItemCount from the Selected Item
       List of the Action List */

    if (single_or_browse) {
    	n = 0;
    	XtSetArg(args[n], XmNselectedItemCount, &item_count);		n++;
    	XtSetArg(args[n], XmNselectedItems, &item_list);		n++;
    	XtGetValues(List1, args, n);
    }

    if (item_count == 0)
	visible_count = 1;
    else
	visible_count = item_count;

    n = 0;

    if (selected_pos != NULL) {
	XtSetArg(args[n], XmNitems, item_list2);			n++;
    }
    else {
    	XtSetArg(args[n], XmNitems, item_list);				n++;
    }

    XtSetArg(args[n], XmNitemCount, item_count);			n++;
    XtSetValues(Selected_List, args, n);

}


static void ChangeLabel(Widget widget, char string[50])

{
    XmString 	tcs;
    Arg		args[1];
    int		n;

    tcs = XmStringCreateLtoR(string, XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);				n++;
    XtSetValues(widget, args, n);
}


static void UpdateCB(Widget w, caddr_t client_data, caddr_t call_data)

{
    XmListUpdateSelectedList(List1); 
}


static void SelectedCB(Widget w, caddr_t client_data, caddr_t call_data)

{
    Arg 	args[5];
    int		n;
    XmString	*strings, tcs;
    char  	tmp_str[20];
    int		selected_count;
	
    XmListDeleteAllItems(Selected_ListSV);

    n = 0;
    XtSetArg(args[n], XmNselectedItemCount, &selected_count);		n++;
    XtSetArg(args[n], XmNselectedItems, &strings);			n++;
    XtGetValues(List1, args, n);

    sprintf(tmp_str, "Selected Items (%d):", selected_count);
    tcs = XmStringCreateSimple(tmp_str);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);				n++;
    XtSetValues(Label_Selected, args, n);
    XmStringFree(tcs);

    n = 0;
    XtSetArg(args[n], XmNitems, strings);				n++;
    XtSetArg(args[n], XmNitemCount, selected_count);			n++;
    XtSetValues(Selected_ListSV, args, n);

}


static void AddSLCB(Widget w, caddr_t client_data, caddr_t call_data)

{
    Arg		args[5];
    int		n, i;
    int		selected_item_count;
    char 	*string;
    XmString	*selected_items, *new_selected_items, item;

    string = XmTextFieldGetString(w);
    item = XmStringCreateSimple(string);

    n = 0;
    XtSetArg(args[n], XmNselectedItems, &selected_items);		n++;
    XtSetArg(args[n], XmNselectedItemCount, &selected_item_count);	n++;
    XtGetValues(List1, args, n);

    new_selected_items = (XmString *) 
		XtMalloc(sizeof(XmString) * selected_item_count + 1);

    for (i = 0 ; i < selected_item_count; i++)
	new_selected_items[i] = selected_items[i];

    new_selected_items[selected_item_count] = item;

    n = 0;
    XtSetArg(args[n], XmNselectedItems, new_selected_items);		n++;
    XtSetArg(args[n], XmNselectedItemCount, selected_item_count + 1);	n++;
    XtSetValues(List1, args, n);
}


static void AddILCB(Widget w, caddr_t client_data, caddr_t call_data)

{
    XmString 	item;
    char	*string;

    string = XmTextFieldGetString(w);
    item = XmStringCreateSimple(string);

    if (Add_Unselected)	
    	XmListAddItemUnselected(List1, item , 1);
    else
    	XmListAddItem(List1, item , 1);
}


static void ToggleCB(Widget w, caddr_t client_data, caddr_t call_data)

{
    Arg		args[1];
    XmString	tcs, tcs2;

    tcs = XmStringCreateSimple("Add Item Unselected");
    tcs2 = XmStringCreateSimple("Dont Add Item Unselected");

    Add_Unselected = ((XmToggleButtonCallbackStruct*)call_data)->set;

    if (Add_Unselected) 
	XtSetArg(args[0], XmNlabelString, tcs);
    else
	XtSetArg(args[0], XmNlabelString, tcs2);

    XtSetValues(w, args, 1);

    XmStringFree(tcs);
    XmStringFree(tcs2);

}

#endif /* MOTIF1_1 */
