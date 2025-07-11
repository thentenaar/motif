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
static char rcsid[] = "$XConsortium: List7.c /main/9 1995/07/13 18:20:11 drk $"
#endif
#endif

#include <testlib.h>
#include "List7.h"


void  main (argc, argv)
    int     argc;
    char  **argv;
{
    register int       	n;
    Arg                	args[MAX_ARGS];
    XmString           	tcs, ItemList[NITEMS], ItemList2[NITEMS2];
    XmFontList         	newfontlist;

    CommonTestInit(argc, argv);

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

    XtAddCallback(List1, XmNbrowseSelectionCallback, SSelCB_List1, NULL);

    tcs = XmStringCreateLtoR("Call Function", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNleftWidget, List1);			n++;
    XtSetArg(args[n], XmNleftOffset, 20);			n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNtopWidget, Label1);			n++;
    XtSetArg(args[n], XmNtopOffset, 15);			n++;
    XtSetArg(args[n], XmNfontList, newfontlist); 		n++;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    ApplyB = XmCreatePushButton(Form, "ApplyB", args, n);
    XtManageChild(ApplyB);

    XtAddCallback(ApplyB, XmNactivateCallback, ApplyCB, NULL);

    XmStringFree(tcs);

    tcs = XmStringCreateLtoR("Clear Args", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNleftWidget, List1);			n++;
    XtSetArg(args[n], XmNleftOffset, 20);			n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNtopWidget, ApplyB);			n++;
    XtSetArg(args[n], XmNtopOffset, 15);			n++;
    XtSetArg(args[n], XmNfontList, newfontlist);  		n++;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    ClearB = XmCreatePushButton(Form, "ClearB", args, n);
    XtManageChild(ClearB);

    XtAddCallback(ClearB, XmNactivateCallback, ClearCB, NULL);

    XmStringFree(tcs);

    tcs = XmStringLtoRCreate("#Args/Position", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNleftWidget, List1);			n++;
    XtSetArg(args[n], XmNleftOffset, 20);			n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNtopWidget, ClearB);			n++;
    XtSetArg(args[n], XmNtopOffset, 15);			n++;
    XtSetArg(args[n], XmNfontList, newfontlist);  		n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL);		n++;
    XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT);	n++;
    XtSetArg(args[n], XmNtitleString, tcs);			n++;
    XtSetArg(args[n], XmNshowValue, True);			n++;
    XtSetArg(args[n], XmNmaximum, 10);				n++;
    Scale = XmCreateScale(Form, "Scale", args, n);
    XtManageChild(Scale);

    XtAddCallback(Scale, XmNvalueChangedCallback, ScaleCB, NULL);

    XmStringFree(tcs);

    tcs = XmStringCreateLtoR("Notify True", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNleftWidget, List1);			n++;
    XtSetArg(args[n], XmNleftOffset, 20);			n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNtopWidget, Scale);			n++;
    XtSetArg(args[n], XmNtopOffset, 15);			n++;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    XtSetArg(args[n], XmNselectColor, CommonGetColor("orchid"));n++;
    XtSetArg(args[n], XmNfontList, newfontlist);  		n++;
    ToggleB = XmCreateToggleButton(Form, "ToggleB", args, n);
    XtManageChild(ToggleB);

    XtAddCallback(ToggleB, XmNvalueChangedCallback, NotifyCB, NULL);

    XmStringFree(tcs);

    tcs = XmStringCreateLtoR("Action List:", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);  	n++;
    XtSetArg(args[n], XmNtopOffset, 15);                 	n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
    XtSetArg(args[n], XmNrightOffset, 15);               	n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNleftWidget, ApplyB);			n++;
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
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM);	n++;
    XtSetArg(args[n], XmNbottomOffset, 15);               	n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNleftWidget, ApplyB);			n++;
    XtSetArg(args[n], XmNleftOffset, 20);			n++;
    XtSetArg(args[n], XmNlistMarginWidth, 5);			n++;
    XtSetArg(args[n], XmNselectionPolicy, XmMULTIPLE_SELECT);	n++;
    XtSetArg(args[n], XmNhighlightOnEnter, False); 		n++;
    XtSetArg(args[n], XmNfontList, newfontlist);  		n++;
    XtSetArg(args[n], XmNitems, ItemList2); 			n++;
    XtSetArg(args[n], XmNitemCount, NITEMS2); 			n++;
    XtSetArg(args[n], XmNvisibleItemCount, NITEMS2);		n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("wheat"));	n++;
    List2 = XmCreateScrolledList(Form, "List2", args, n);
    XtManageChild(List2);

    XtAddCallback(List2, XmNmultipleSelectionCallback, MulSelCB_List2, NULL);

    XtRealizeWidget(Shell1);

    CommonPause();
    CommonPause();
    CommonPause();
    CommonPause();
    CommonPause();
    CommonPause();

    XtAppMainLoop(app_context);
}


static void ScaleCB(Widget w, XtPointer client_data,
			      XtPointer call_data)

{

    arg_count = ((XmScaleCallbackStruct*)call_data)->value;

}


static void SSelCB_List1(Widget w, XtPointer client_data,
			           XtPointer call_data)

{
    function = ((XmListCallbackStruct*)call_data)->item_position;

    switch (function) {
	case AddItems:
		printf("XmListAddItems chosen\n");
		break;
#ifndef MOTIF1_1
	case AddItemsUnselected:
		printf("XmListAddItemsUnselected\n");
		break;
#endif /* MOTIF1_1 */
	case DeleteItems:
		printf("XmListDeleteItems chosen\n");
		break;
	case DeleteItemsPos:
		printf("XmListDeleteItemsPos chosen\n");
		break;
	case AddItem:
		printf("XmListAddItem chosen\n");
		break;
	case DeleteItem: 
		printf("XmListDeleteItem chosen\n");
		break;
	case DeleteAllItems:
		printf("XmListDeleteAllItems chosen\n");
		break;
	case AddItemUnselected:
		printf("XmListAddItemUnselected chosen\n");
		break;
	case DeletePos:
		printf("XmListDeletePos chosen\n");
		break;
	case DeselectAllItems:
		printf("XmListDeselectAllItems chosen\n");
		break;
	case DeselectItem:
		printf("XmListDeselectItem chosen\n");
		break;
	case DeselectPos:
		printf("XmListDeselectPos chosen\n");
		break;
	case SelectItem:
		printf("XmListSelectItem chosen\n");
		break;
	case SelectPos:
		printf("XmListSelectPos chosen\n");
		break;
	default:
		printf("Not a valid function\n");

	}
}


static void MulSelCB_List2(Widget w, XtPointer client_data,
			             XtPointer call_data)

{
    XmString 	tcs;
    char 	*reg_string;
    int 	i;

    position = ((XmListCallbackStruct*)call_data)->item_position;
    tcs = ((XmListCallbackStruct*)call_data)->item;
    reg_string = CommonCsToRs(tcs);

    CS_ItemList[count++] = XmStringLtoRCreate(reg_string,
			      XmSTRING_DEFAULT_CHARSET);
}


static void ClearCB(Widget w, XtPointer client_data,
			      XtPointer call_data)

{
    register int i;

    for (i = 0 ; i < count ; i++)
	CS_ItemList[i] = NULL;

    count = 0;

    XmListDeselectAllItems(List2);
    XmListDeselectAllItems(List1);
}


static void ApplyCB(Widget w, XtPointer client_data,
			      XtPointer call_data)

{

    switch (function) {
	case AddItems:	
		XmListAddItems(List2, CS_ItemList, count, arg_count);
		break;
#ifndef MOTIF1_1
	case AddItemsUnselected:	
		XmListAddItemsUnselected(List2, CS_ItemList, count, arg_count);
		break;
#endif /* MOTIF1_1 */
	case DeleteItems:
		XmListDeleteItems(List2, CS_ItemList, count);
		break;
	case DeleteItemsPos: 
		XmListDeleteItemsPos(List2, arg_count, position);
		break;
	case AddItem:
		if (count > 0) 
			XmListAddItem(List2, CS_ItemList[count - 1], 
				      arg_count);
		else
			printf("Please Choose a Item from the List\n");
		break;
	case DeleteItem:
		if (count > 0) 
			XmListDeleteItem(List2, CS_ItemList[count - 1]);
		else
			printf("Please Choose a Item from the List\n");

		break;
	case DeleteAllItems:
		XmListDeleteAllItems(List2);
		break;
	case AddItemUnselected:
		if (count > 0)
			XmListAddItemUnselected(List2, CS_ItemList[count - 1], 
					        arg_count);
		else
			printf("Please Choose a Item from the List\n");
		break;
	case DeletePos:
		XmListDeletePos(List2, arg_count);
		break;
	case DeselectAllItems:
		XmListDeselectAllItems(List2);
		break;
	case DeselectItem:
		if (count > 0)
			XmListDeselectItem(List2, CS_ItemList[count - 1]);
		else
			printf("Please Choose an Item from the List\n");
		break;
	case DeselectPos:
		XmListDeselectPos(List2, arg_count);
		break;
	case SelectItem:
		if (count > 0)
			XmListSelectItem(List2, CS_ItemList[count - 1], 
					 notify);
		else
			printf("Please Choose an Item from the List\n");
		break;
	case SelectPos:
		XmListSelectPos(List2, arg_count, notify);
		break;
	default:
		printf("Not a valid option\n");
    };
}


static void NotifyCB(Widget w, XtPointer client_data,
			       XtPointer call_data)

{
    Arg		args[1];
    int		n;
    XmString	tcs;


    notify = ((XmToggleButtonCallbackStruct*)call_data)->set;

    if (notify)
	tcs = XmStringCreateLtoR("Notify True", XmSTRING_DEFAULT_CHARSET);
    else
	tcs = XmStringCreateLtoR("Notify False", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    XtSetValues(w, args, n);

}
