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
static char rcsid[] = "$XConsortium: List4.c /main/9 1995/07/13 18:18:17 drk $"
#endif
#endif

#include <testlib.h>
#include "List4.h"


void  main (argc, argv)
    int     argc;
    char  **argv;
{
    int       		n;
    Arg                	args[MAX_ARGS];
    XmString           	tcs, ItemList[NITEMS],
    			ItemList2[NITEMS2], ItemList3[NITEMS3];
    XmFontList         	newfontlist;


    CommonTestInit(argc, argv);
    
    newfontlist = CommonGetFontList("variable");

    n = 0;
    Form = XmCreateForm(Shell1, "Form", args, n);
    XtManageChild (Form);

    tcs = XmStringCreateLtoR("Functions:", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
    XtSetArg(args[n], XmNtopOffset, 15);			n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
    XtSetArg(args[n], XmNleftOffset, 15);			n++;
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

    for (n = 0; n < NITEMS3; n++)
	ItemList3[n] = XmStringLtoRCreate(Istrings3[n],
			  XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);        n++;
    XtSetArg(args[n], XmNleftOffset, 15);                       n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNtopWidget, Label1);			n++;
    XtSetArg(args[n], XmNtopOffset, 15);			n++;
    XtSetArg(args[n], XmNselectionPolicy, XmSINGLE_SELECT);	n++;
    XtSetArg(args[n], XmNhighlightOnEnter, False); 		n++;
    XtSetArg(args[n], XmNfontList, newfontlist);  		n++;
    XtSetArg(args[n], XmNitems, ItemList); 			n++;
    XtSetArg(args[n], XmNitemCount, NITEMS); 			n++;
    XtSetArg(args[n], XmNvisibleItemCount, NITEMS);		n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("cyan"));	n++;
    List1 = XmCreateList(Form, "List1", args, n);
    XtManageChild(List1);

    XtAddCallback(List1, XmNsingleSelectionCallback, SSelCB_List1, NULL);

    tcs = XmStringCreateLtoR("Call Function", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);        n++;
    XtSetArg(args[n], XmNleftOffset, 15);                       n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNtopWidget, List1);			n++;
    XtSetArg(args[n], XmNtopOffset, 15);			n++;
    XtSetArg(args[n], XmNfontList, newfontlist); 		n++;
    XtSetArg(args[n], XmNy, 100);				n++;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    ApplyB = XmCreatePushButton(Form, "ApplyB", args, n);
    XtManageChild(ApplyB);

    XtAddCallback(ApplyB, XmNactivateCallback, ApplyCB, NULL);

    XmStringFree(tcs);

    tcs = XmStringCreateLtoR("Clear Args", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);        n++;
    XtSetArg(args[n], XmNleftOffset, 15);                       n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNtopWidget, ApplyB);			n++;
    XtSetArg(args[n], XmNtopOffset, 15);			n++;
    XtSetArg(args[n], XmNfontList, newfontlist);  		n++;
    XtSetArg(args[n], XmNy, 150);				n++;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    ClearB = XmCreatePushButton(Form, "ClearB", args, n);
    XtManageChild(ClearB);

    XtAddCallback(ClearB, XmNactivateCallback, ClearCB, NULL);

    XmStringFree(tcs);

    tcs = XmStringCreateLtoR("List1:", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNleftWidget, List1);			n++;
    XtSetArg(args[n], XmNleftOffset, 15); 			n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
    XtSetArg(args[n], XmNtopOffset, 15);			n++;
    XtSetArg(args[n], XmNfontList, newfontlist); 		n++;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    Label2 = XmCreateLabel(Form, "Label2", args, n);
    XtManageChild(Label2);

    XmStringFree(tcs);

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNleftWidget, List1);			n++;
    XtSetArg(args[n], XmNleftOffset, 15); 			n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNtopWidget, Label2);			n++;
    XtSetArg(args[n], XmNtopOffset, 15);			n++;
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

    XtAddCallback(List2, XmNmultipleSelectionCallback, MulSelCB_List1, NULL);

    tcs = XmStringCreateLtoR("List2:", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNleftWidget, List2);			n++;
    XtSetArg(args[n], XmNleftOffset, 15); 			n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
    XtSetArg(args[n], XmNtopOffset, 15);			n++;
    XtSetArg(args[n], XmNfontList, newfontlist);        	n++;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    Label3 = XmCreateLabel(Form, "Label3", args, n);
    XtManageChild(Label3);

    XmStringFree(tcs);

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNleftWidget, List2);			n++;
    XtSetArg(args[n], XmNleftOffset, 15); 			n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
    XtSetArg(args[n], XmNrightOffset, 15); 			n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNtopWidget, Label3);			n++;
    XtSetArg(args[n], XmNtopOffset, 15);			n++;
    XtSetArg(args[n], XmNlistMarginWidth, 5);			n++;
    XtSetArg(args[n], XmNselectionPolicy, XmMULTIPLE_SELECT);	n++;
    XtSetArg(args[n], XmNhighlightOnEnter, False); 		n++;
    XtSetArg(args[n], XmNfontList, newfontlist);  		n++;
    XtSetArg(args[n], XmNitems, ItemList3); 			n++;
    XtSetArg(args[n], XmNitemCount, NITEMS3); 			n++;
    XtSetArg(args[n], XmNvisibleItemCount, NITEMS3);		n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("wheat"));	n++;
    List3 = XmCreateScrolledList(Form, "List3", args, n);
    XtManageChild(List3);

    XtAddCallback(List3, XmNmultipleSelectionCallback, MulSelCB_List2, NULL);

    tcs = XmStringLtoRCreate("#Args/Position", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); 		n++;
    XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT);	n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNtopWidget, List2);			n++;
    XtSetArg(args[n], XmNtopOffset, 15);			n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNleftWidget, List1);			n++;
    XtSetArg(args[n], XmNleftOffset, 15);			n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM);	n++;
    XtSetArg(args[n], XmNbottomOffset, 15);			n++;
    XtSetArg(args[n], XmNfontList, newfontlist);  		n++;
    XtSetArg(args[n], XmNtitleString, tcs);			n++;
    XtSetArg(args[n], XmNshowValue, True);			n++;
    XtSetArg(args[n], XmNmaximum, 10);				n++;
    Scale = XmCreateScale(Form, "Scale", args, n);
    XtManageChild(Scale);

    XtAddCallback(Scale, XmNvalueChangedCallback, ScaleCB, NULL);

    XmStringFree(tcs);

    XtRealizeWidget(Shell1);

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
	case 1:
		printf("XmListReplaceItems chosen\n");
		break;
	case 2:
		printf("XmListReplaceItemsPos chosen\n");
		break;
#ifndef MOTIF1_1
	case 3:
		printf("XmListReplaceItemsUnselected chosen\n");
		break;
	case 4:
		printf("XmListReplaceItemsPosUnselected chosen\n");
		break;
#endif /* MOTIF1_1 */
	default:
		printf("Not a valid function\n");

	}
}


static void MulSelCB_List1(Widget w, XtPointer client_data,
			             XtPointer call_data)

{
    XmString tcs;
    char *reg_string;
    int i;

    tcs = ((XmListCallbackStruct*)call_data)->item;
    reg_string = CommonCsToRs(tcs);
    CS_ItemList1[count1++] = XmStringLtoRCreate(reg_string,
			      XmSTRING_DEFAULT_CHARSET);
}


static void MulSelCB_List2(Widget w, XtPointer client_data,
			             XtPointer call_data)

{
    XmString tcs;
    char *reg_string;
    int i;

    tcs = ((XmListCallbackStruct*)call_data)->item;
    reg_string = CommonCsToRs(tcs);
    CS_ItemList2[count2++] = XmStringLtoRCreate(reg_string,
			      XmSTRING_DEFAULT_CHARSET);
}


static void ClearCB(Widget w, XtPointer client_data,
			      XtPointer call_data)

{
    register int i;

    for (i = 0 ; i < count1 ; i++)
	CS_ItemList1[i] = NULL;
    for (i = 0 ; i < count2 ; i++)
	CS_ItemList2[i] = NULL;

    count1 = 0;
    count2 = 0;

    XmListDeselectAllItems(List1);
    XmListDeselectAllItems(List2);
    XmListDeselectAllItems(List3);
}


static void ApplyCB(Widget w, XtPointer client_data,
			      XtPointer call_data)

{
    switch (function) {
	case 1:	
		XmListReplaceItems(List2, CS_ItemList1, count2, CS_ItemList2);
		break;
	case 2:
		XmListReplaceItemsPos(List2, CS_ItemList2, count2, arg_count);
		break;
#ifndef MOTIF1_1
	case 3:	
		XmListReplaceItemsUnselected
			(List2, CS_ItemList1, count2, CS_ItemList2);
		break;
	case 4:
		XmListReplaceItemsPosUnselected
			(List2, CS_ItemList2, count2, arg_count);
		break;
#endif /* MOTIF1_1 */
	default:
		printf("Not a valid option\n");
    };
}
