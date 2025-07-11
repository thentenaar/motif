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
static char rcsid[] = "$XConsortium: List2.c /main/9 1995/07/13 18:16:37 drk $"
#endif
#endif

#include "testlib.h"
#include "List2.h"

/* 
	Pirs Tested -

		Pir 2782 - Changing font after TopItemPosition does not
			   preserve TopItemPosition.
		Pir 2783 - Destroy and recreate corrupts display of List.
		Pir 4576 - selected_item_count set for SINGLE/BROWSE SELECT

       CRs Tested -

                CR 6117 - In ScrolledList, sensitivity of List should not
		          affect parent (scrollbar.)
		CR 5740 - Setting XmNtopItemPosition to 0 for Lists should not
		          cause a warning.

*/


void  main (argc, argv)
    int     argc;
    char  **argv;
{
    register int       n;
    Arg                args[MAX_ARGS];
    XmString           tcs, tcs2;

    CommonTestInit(argc, argv);

    font_list = CommonGetFontList("fixed");

    Init_Resources();
    
    n = 0;
    XtSetArg(args[n], XmNdefaultFontList, CommonGetFontList("9x15"));	n++;
    XtSetValues(Shell1, args, n);

    for (n = 0; n < NITEMS; n++)
	ItemList[n] = XmStringLtoRCreate(Istrings[n],
			  XmSTRING_DEFAULT_CHARSET);

    /* Create Outer Form */

    n = 0;
    XtSetArg(args[n], XmNbackground, CommonGetColor("ForestGreen"));	n++;
    Form = XmCreateForm(Shell1, "Form", args, n);
    XtManageChild(Form);

    /* Create List Label */

    tcs = XmStringLtoRCreate("Action Scale:",  XmSTRING_DEFAULT_CHARSET);

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
    List1 = XmCreateScrolledList(Form, "List1", args, n);
    XtManageChild(List1);

    /* Add Action ListCallbacks */

    XtAddCallback(List1, XmNsingleSelectionCallback, BrSiDeSelCB, NULL);
    XtAddCallback(List1, XmNmultipleSelectionCallback, MulSelCB, NULL);
    XtAddCallback(List1, XmNextendedSelectionCallback, ExtSelCB, NULL);
    XtAddCallback(List1, XmNbrowseSelectionCallback, BrSiDeSelCB, NULL);
    XtAddCallback(List1, XmNdefaultActionCallback, BrSiDeSelCB, NULL);

    /* Create Panel of Resource Options */

    CreateOptions();

    /* Create Paned with Callback Information */

    CreateCallbackInfo();

    XtRealizeWidget(Shell1);

    CommonDumpHierarchy(Shell1, stdout);

    CommonPause();

    /* Test for Pir 2782 */

    CommonPause();

    /* Test for Pir 2783 */

    CommonPause();

#ifndef MOTIF1_1

    /* Test KCopy and Scrolling features of 1.2 */

    CommonPause();
    CommonPause();
    CommonPause();

    /* Test CR6117 - Scrolled List sensitivity. */
    CommonPause();
    /* Test CR5740 - XmNtopPosition 0. */
    CommonPause();

#endif /* MOTIF1_1 */

   /*  Begin testing CR 5678 */
    CommonPause();
   /*  End testing CR 5678 */

    CommonPause();
    
    XtAppMainLoop(app_context);
}


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

    /* Visible Item Count Scale */

    tcs = XmStringLtoRCreate("Visible Item Count", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNmaximum, NITEMS);				n++;
    XtSetArg(args[n], XmNminimum, 1);					n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL);			n++;
    XtSetArg(args[n], XmNtitleString, tcs);				n++;
    XtSetArg(args[n], XmNshowValue, True);				n++;
    XtSetArg(args[n], XmNvalue, NITEMS - 10);				n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("ForestGreen"));	n++;
    VIC_Scale = XmCreateScale(RowCol1, "VIC_Scale", args, n);
    XtManageChild(VIC_Scale);

    XtAddCallback(VIC_Scale, XmNvalueChangedCallback, ChangeVICCB, NULL);

    XmStringFree(tcs);

    /* Margin Height Scale */

    tcs = XmStringLtoRCreate("Margin Height", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNmaximum, 50);					n++;
    XtSetArg(args[n], XmNminimum, 0);					n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL);			n++;
    XtSetArg(args[n], XmNtitleString, tcs);				n++;
    XtSetArg(args[n], XmNshowValue, True);				n++;
    XtSetArg(args[n], XmNvalue, 2);					n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("ForestGreen"));	n++;
    MH_Scale = XmCreateScale(RowCol1, "MH_Scale", args, n);
    XtManageChild(MH_Scale);

    XtAddCallback(MH_Scale, XmNvalueChangedCallback, ChangeMHCB, NULL);

    XmStringFree(tcs);

    /* Margin Width Scale */

    tcs = XmStringLtoRCreate("Margin Width", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNmaximum, 50);					n++;
    XtSetArg(args[n], XmNminimum, 0);					n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL);			n++;
    XtSetArg(args[n], XmNtitleString, tcs);				n++;
    XtSetArg(args[n], XmNshowValue, True);				n++;
    XtSetArg(args[n], XmNvalue, 2);					n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("ForestGreen"));	n++;
    MW_Scale = XmCreateScale(RowCol1, "MW_Scale", args, n);
    XtManageChild(MW_Scale);

    XtAddCallback(MW_Scale, XmNvalueChangedCallback, ChangeMWCB, NULL);

    XmStringFree(tcs);

    /* Spacing Scale */

    tcs = XmStringLtoRCreate("Spacing", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNmaximum, 50);					n++;
    XtSetArg(args[n], XmNminimum, 0);					n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL);			n++;
    XtSetArg(args[n], XmNtitleString, tcs);				n++;
    XtSetArg(args[n], XmNshowValue, True);				n++;
    XtSetArg(args[n], XmNvalue, 5);					n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("ForestGreen"));	n++;
    Space_Scale = XmCreateScale(RowCol1, "Space_Scale", args, n);
    XtManageChild(Space_Scale);

    XtAddCallback(Space_Scale, XmNvalueChangedCallback, ChangeSpaceCB, NULL);

    XmStringFree(tcs);

    /* Top Item Position Scale */

    tcs = XmStringLtoRCreate("Top Item Position", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNmaximum, NITEMS);				n++;
    /* CR 5740 - Setting XmNtopItemPosition to 0 for Lists should not
       cause a warning. */
    XtSetArg(args[n], XmNminimum, 0);					n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL);			n++;
    XtSetArg(args[n], XmNtitleString, tcs);				n++;
    XtSetArg(args[n], XmNshowValue, True);				n++;
    XtSetArg(args[n], XmNvalue, 2);					n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("ForestGreen"));	n++;
    TIP_Scale = XmCreateScale(RowCol1, "TIP_Scale", args, n);
    XtManageChild(TIP_Scale);

    XtAddCallback(TIP_Scale, XmNvalueChangedCallback, ChangeTIPCB, NULL);

    XmStringFree(tcs);

    /* ScrollBar Display Policy Pulldown */

    n = 0;
    XtSetArg(args[n], XmNbackground, CommonGetColor("ForestGreen"));	n++;
    XtSetArg(args[n], XmNfontList, CommonGetFontList("9x15"));		n++;
    SBDPolicy_Pulldown = XmCreatePulldownMenu(Shell1, "SBDPolicy_Pulldown",
					    args, n);

    Static = CreatePushButton(SBDPolicy_Pulldown, "XmSTATIC");
    AsNeeded = CreatePushButton(SBDPolicy_Pulldown, "XmAS_NEEDED");

    XtAddCallback(Static, XmNactivateCallback, CHSBDPolicyCB, 
						(XtPointer) XmSTATIC);
    XtAddCallback(AsNeeded, XmNactivateCallback, CHSBDPolicyCB, 
						(XtPointer) XmAS_NEEDED);


    /* List Size Policy Pulldown */

    n = 0;
    XtSetArg(args[n], XmNfontList, CommonGetFontList("9x15"));		n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("ForestGreen"));	n++;
    LSPolicy_Pulldown = XmCreatePulldownMenu(Shell1, "LSPolicy_Pulldown",
					    args, n);

    Constant = CreatePushButton(LSPolicy_Pulldown, "XmCONSTANT");
    VariableP = CreatePushButton(LSPolicy_Pulldown, "XmVARIABLE");
    Possible = CreatePushButton(LSPolicy_Pulldown, "XmRESIZE_IF_POSSIBLE");

    XtAddCallback(Constant, XmNactivateCallback, CHLSPolicyCB, 
						(XtPointer) XmCONSTANT);
    XtAddCallback(VariableP, XmNactivateCallback, CHLSPolicyCB, 
						(XtPointer) XmVARIABLE);
    XtAddCallback(Possible, XmNactivateCallback, CHLSPolicyCB, 	
					(XtPointer) XmRESIZE_IF_POSSIBLE);

    /* Fonts Pulldown Menu */

    n = 0;
    XtSetArg(args[n], XmNbackground, CommonGetColor("ForestGreen"));	n++;
    XtSetArg(args[n], XmNfontList, CommonGetFontList("9x15"));		n++;
    Fonts_Pulldown = XmCreatePulldownMenu(Shell1, "Fonts_Pulldown",
					   args, n);

    Fixed = CreatePushButton(Fonts_Pulldown, "Fixed");
    Variable = CreatePushButton(Fonts_Pulldown, "Variable");
    NineByFiveteen = CreatePushButton(Fonts_Pulldown, "9x15");
    helvetica = CreatePushButton(Fonts_Pulldown, "Helvetica");

    XtAddCallback(Fixed, XmNactivateCallback, ChangeFontCB, "fixed");
    XtAddCallback(Variable, XmNactivateCallback, ChangeFontCB, "variable");
    XtAddCallback(NineByFiveteen, XmNactivateCallback, ChangeFontCB, "9x15");
    XtAddCallback(helvetica, XmNactivateCallback, ChangeFontCB, "helvb18");

    /* String Direction Pulldown Menu */

    n = 0;
    XtSetArg(args[n], XmNbackground, CommonGetColor("ForestGreen"));	n++;
    XtSetArg(args[n], XmNfontList, CommonGetFontList("9x15"));		n++;
    Dirs_Pulldown = XmCreatePulldownMenu(Shell1, "Dirs_Pulldown",
					   args, n);

    LtoR = CreatePushButton(Dirs_Pulldown, "Left_to_Right");
    RtoL = CreatePushButton(Dirs_Pulldown, "Right_to_Left");

    XtAddCallback(LtoR, XmNactivateCallback, ChangeDirCB,
		  (XtPointer)XmSTRING_DIRECTION_L_TO_R);
    XtAddCallback(RtoL, XmNactivateCallback, ChangeDirCB,
		  (XtPointer)XmSTRING_DIRECTION_R_TO_L);

    /* Selection Policy Pulldown */

    n = 0;
    XtSetArg(args[n], XmNbackground, CommonGetColor("ForestGreen"));	n++;
    XtSetArg(args[n], XmNfontList, CommonGetFontList("9x15"));		n++;
    SPolicy_Pulldown = XmCreatePulldownMenu(Shell1, "SPolicy_Pulldown",
					   args, n);

    Single = CreatePushButton(SPolicy_Pulldown, "XmSINGLE_SELECT");
    Multiple = CreatePushButton(SPolicy_Pulldown, "XmMULTIPLE_SELECT");
    Browse = CreatePushButton(SPolicy_Pulldown, "XmBROWSE_SELECT");
    Extended = CreatePushButton(SPolicy_Pulldown, "XmEXTENDED_SELECT");

    XtAddCallback(Single, XmNactivateCallback, CHSPolicyCB, 
						(XtPointer) XmSINGLE_SELECT);
    XtAddCallback(Multiple, XmNactivateCallback, CHSPolicyCB, 
						(XtPointer) XmMULTIPLE_SELECT);
    XtAddCallback(Browse, XmNactivateCallback, CHSPolicyCB, 
						(XtPointer) XmBROWSE_SELECT);
    XtAddCallback(Extended, XmNactivateCallback, CHSPolicyCB,
						(XtPointer) XmEXTENDED_SELECT);
    
    /* List Size Policy Option Menu */

    tcs = XmStringLtoRCreate("List Size Policy         ", 
					XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);				n++;
    XtSetArg(args[n], XmNmenuHistory, VariableP);			n++;
    XtSetArg(args[n], XmNsubMenuId, LSPolicy_Pulldown);			n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("ForestGreen"));	n++;
    XtSetArg(args[n], XmNfontList, CommonGetFontList("9x15"));		n++;
    LSPolicy_Option = XmCreateOptionMenu(RowCol1, "LSPolicy_Option", args, n);
    XtManageChild(LSPolicy_Option);

    XmStringFree(tcs);

    /* ScrollBar Display Policy Option Menu */

    tcs = XmStringLtoRCreate("ScrollBar Display Policy ", 
					XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);				n++;
    XtSetArg(args[n], XmNmenuHistory, AsNeeded);			n++;
    XtSetArg(args[n], XmNsubMenuId, SBDPolicy_Pulldown);		n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("ForestGreen"));	n++;
    XtSetArg(args[n], XmNfontList, CommonGetFontList("9x15"));	n++;
    SBDPolicy_Option = XmCreateOptionMenu(RowCol1, "SBDPolicy_Option", args, n);
    XtManageChild(SBDPolicy_Option);

    XmStringFree(tcs);

    /* Fonts Option Menu */

    tcs = XmStringLtoRCreate("Available Fonts          ", 
					XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);				n++;
    XtSetArg(args[n], XmNmenuHistory, Fixed);				n++;
    XtSetArg(args[n], XmNsubMenuId, Fonts_Pulldown);			n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("ForestGreen"));	n++;
    XtSetArg(args[n], XmNfontList, CommonGetFontList("9x15"));		n++;
    Fonts_Option = XmCreateOptionMenu(RowCol1, "Fonts_Option", args, n);
    XtManageChild(Fonts_Option);

    XmStringFree(tcs);

    /* String Direction Option Menu */

    tcs = XmStringCreateLocalized("String Direction         ");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);				n++;
    XtSetArg(args[n], XmNmenuHistory, LtoR);				n++;
    XtSetArg(args[n], XmNsubMenuId, Dirs_Pulldown);		n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("ForestGreen"));	n++;
    XtSetArg(args[n], XmNfontList, CommonGetFontList("9x15"));		n++;
    Dirs_Option = XmCreateOptionMenu(RowCol1, "Dirs_Option", args, n);
    XtManageChild(Dirs_Option);

    XmStringFree(tcs);

    /* Selection Policy Option Menu */

    tcs = XmStringLtoRCreate("Selection Policy         ", 
					XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);				n++;
    XtSetArg(args[n], XmNmenuHistory, Browse);				n++;
    XtSetArg(args[n], XmNsubMenuId, SPolicy_Pulldown);			n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("ForestGreen"));	n++;
    XtSetArg(args[n], XmNfontList, CommonGetFontList("9x15"));	n++;
    SPolicy_Option = XmCreateOptionMenu(RowCol1, "SPolicy_Option", args, n);
    XtManageChild(SPolicy_Option);

    XmStringFree(tcs);

    tcs = XmStringCreateLtoR("List Sensitive", 
			     XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);				n++;
    XtSetArg(args[n], XmNselectColor, CommonGetColor("Yellow"));	n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("ForestGreen"));	n++;
    XtSetArg(args[n], XmNset, True);					n++;
    ToggleB1 = XmCreateToggleButton(RowCol1, "ToggleSensitive", args, n);
    XtManageChild(ToggleB1);

    XtAddCallback(ToggleB1, XmNvalueChangedCallback, ToggleB1CB, NULL);

    XmStringFree(tcs);

    tcs = XmStringCreateLtoR("Automatic Selection False", 
			     XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);				n++;
    XtSetArg(args[n], XmNselectColor, CommonGetColor("Yellow"));	n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("ForestGreen"));	n++;
    ToggleB = XmCreateToggleButton(RowCol1, "ToggleB", args, n);
    XtManageChild(ToggleB);

    XtAddCallback(ToggleB, XmNvalueChangedCallback, ToggleCB, NULL);

    XmStringFree(tcs);

    tcs = XmStringCreateLtoR("Text Paste Widget", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);				n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("ForestGreen"));    n++;
    Label_Text = XmCreateLabel(RowCol1, "Label_Text", args, n);
    XtManageChild(Label_Text);

    n = 0;
    XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT);			n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("goldenrod"));	n++;
    Text1 = XmCreateText(RowCol1, "Text1", args, n);
    XtManageChild(Text1);

    tcs = XmStringCreateLtoR("Destroy and recreate with current settings",
			     XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);				n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("orchid"));		n++;
    Destroy_Push = XmCreatePushButton(RowCol1, "Destroy_Push", args, n);
    XtManageChild(Destroy_Push);

    XtAddCallback(Destroy_Push, XmNactivateCallback, Destroy_and_Create, NULL);

    XmStringFree(tcs);
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
    XtSetArg(args[n], XmNlistSizePolicy, XmVARIABLE);  		n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("cyan"));	n++;
    Selected_List = XmCreateList(RowCol2, "Selected_List", args, n);
    XtManageChild(Selected_List);

}


static Widget  CreatePushButton(Widget parent, char *label)

{
    Widget        widget;
    register int  n;
    Arg           args[3];
    XmString      tcs;

    tcs = XmStringLtoRCreate(label, XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);  				n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("ForestGreen")); 	n++;
    XtSetArg(args[n], XmNfontList, CommonGetFontList("9x15"));		n++;
    widget = XmCreatePushButton(parent, label, args, n);
    XtManageChild(widget);

    XmStringFree(tcs);

    return(widget);
}


static void BrSiDeSelCB(Widget w, XtPointer client_data,
				  XtPointer call_data)

{
    XmString 	tcs;
    int		reason, item_length, item_pos, item_count, i;
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

    item_count = ((XmListCallbackStruct*)call_data)->selected_item_count;
    sprintf(string, "Selected Item Count = %d", item_count);
    ChangeLabel(Label_Selected_Count, string);

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


static void MulSelCB(Widget w, XtPointer client_data,
			       XtPointer call_data)

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


static void ExtSelCB(Widget w, XtPointer client_data,
			       XtPointer call_data)

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


static void ChangeFontCB(Widget w, XtPointer client_data,
				   XtPointer call_data)

{
    int 	n;
    Arg 	args[1];

    font_list = CommonGetFontList((char *)client_data);

    n = 0;
    XtSetArg(args[n], XmNfontList, font_list);			n++;
    XtSetValues(List1, args, n);
}
	
static void ChangeDirCB(Widget w, XtPointer client_data,
				   XtPointer call_data)

{
    int 	n;
    Arg 	args[1];

    n = 0;
    XtSetArg(args[n], XmNstringDirection, client_data);			n++;
    XtSetValues(List1, args, n);
}
	

static void CHSPolicyCB(Widget w, XtPointer client_data,
				  XtPointer call_data)

{
    int 	n;
    Arg 	args[1];

    selection_policy = (long)client_data;

    n = 0;
    XtSetArg(args[n], XmNselectionPolicy, client_data);		n++;
    XtSetValues(List1, args, n);

}

static void CHLSPolicyCB(Widget w, XtPointer client_data,
				   XtPointer call_data)

{
    int 	n;
    Arg 	args[1];

    list_policy = (long)client_data;

    n = 0;
    XtSetArg(args[n], XmNlistSizePolicy, client_data);		n++;
    XtSetValues(List1, args, n);

}


static void CHSBDPolicyCB(Widget w, XtPointer client_data,
				    XtPointer call_data)

{
    int 	n;
    Arg 	args[1];

    scroll_display_policy = (long)client_data;

    n = 0;
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, client_data);		n++;
    XtSetValues(List1, args, n);

}


static void ChangeVICCB(Widget w, XtPointer client_data,
				  XtPointer call_data)

{
    int 	n;
    Arg 	args[1];
    int		value;

    value = ((XmScaleCallbackStruct*)call_data)->value;
    visible_item_count = value;

    n = 0;
    XtSetArg(args[n], XmNvisibleItemCount, value);		n++;
    XtSetValues(List1, args, n);

}


static void ChangeMHCB(Widget w, XtPointer client_data,
				 XtPointer call_data)

{
    int 	n;
    Arg 	args[1];
    int		value;

    value = ((XmScaleCallbackStruct*)call_data)->value;
    margin_height = value;

    n = 0;
    XtSetArg(args[n], XmNlistMarginHeight, value);		n++;
    XtSetValues(List1, args, n);

}


static void ChangeMWCB(Widget w, XtPointer client_data,
				 XtPointer call_data)

{
    int 	n;
    Arg 	args[1];
    int		value;

    value = ((XmScaleCallbackStruct*)call_data)->value;
    margin_width = value;

    n = 0;
    XtSetArg(args[n], XmNlistMarginWidth, value);		n++;
    XtSetValues(List1, args, n);

}


static void ChangeSpaceCB(Widget w, XtPointer client_data,
				    XtPointer call_data)

{
    int 	n;
    Arg 	args[1];
    int		value;

    value = ((XmScaleCallbackStruct*)call_data)->value;
    spacing = value;

    n = 0;
    XtSetArg(args[n], XmNlistSpacing, value);		n++;
    XtSetValues(List1, args, n);

}


static void ChangeTIPCB(Widget w, XtPointer client_data,
				  XtPointer call_data)

{
    int 	n;
    Arg 	args[1];
    int		value;

    value = ((XmScaleCallbackStruct*)call_data)->value;
    top_item_pos = value;

    n = 0;
    XtSetArg(args[n], XmNtopItemPosition, value);		n++;
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
    XtSetArg(args[n], XmNvisibleItemCount, visible_count);		n++;
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


static void Destroy_and_Create(Widget w, XtPointer client_data,
					 XtPointer call_data)

{
    Arg		args[MAX_ARGS];
    int		n;

    XtDestroyWidget(List1);

    n = 0;
    XtSetArg(args[n], XmNlistSpacing, spacing);			n++;
    XtSetArg(args[n], XmNlistMarginWidth, margin_width);	n++;
    XtSetArg(args[n], XmNlistMarginHeight, margin_height);	n++;
    XtSetArg(args[n], XmNlistSizePolicy, list_policy); 		n++;
    XtSetArg(args[n], XmNvisibleItemCount, visible_item_count);	n++;
    XtSetArg(args[n], XmNtopItemPosition, top_item_pos);	n++;
    XtSetArg(args[n], XmNselectionPolicy, selection_policy);	n++;
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, 
					scroll_display_policy); n++;
    XtSetArg(args[n], XmNautomaticSelection, auto_selection);	n++;
    XtSetArg(args[n], XmNitems, ItemList); 			n++;
    XtSetArg(args[n], XmNitemCount, NITEMS); 			n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
    XtSetArg(args[n], XmNleftOffset, 5);			n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);    	n++;
    XtSetArg(args[n], XmNtopWidget, Label_List1);		n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("Wheat"));	n++;
    XtSetArg(args[n], XmNfontList, font_list);			n++;
    List1 = XmCreateScrolledList(Form, "List1", args, n);
    XtManageChild(List1);

    XtAddCallback(List1, XmNsingleSelectionCallback, BrSiDeSelCB, NULL);
    XtAddCallback(List1, XmNmultipleSelectionCallback, MulSelCB, NULL);
    XtAddCallback(List1, XmNextendedSelectionCallback, ExtSelCB, NULL);
    XtAddCallback(List1, XmNbrowseSelectionCallback, BrSiDeSelCB, NULL);
    XtAddCallback(List1, XmNdefaultActionCallback, BrSiDeSelCB, NULL);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);			n++;
    XtSetArg(args[n], XmNtopOffset, 5);					n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);		n++;
    XtSetArg(args[n], XmNleftWidget, List1);				n++;
    XtSetArg(args[n], XmNleftOffset, 5);				n++;
    XtSetValues(Label_Resources, args, n);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
    XtSetArg(args[n], XmNtopWidget, Label_Resources);			n++;
    XtSetArg(args[n], XmNtopOffset, 5);					n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);		n++;
    XtSetArg(args[n], XmNleftWidget, List1);				n++;
    XtSetArg(args[n], XmNleftOffset, 5);				n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM);		n++;
    XtSetArg(args[n], XmNbottomOffset, 5);				n++;
    XtSetValues(Frame1, args, n);

}


static void ToggleB1CB(Widget w, XtPointer client_data,
			       XtPointer call_data)

{
    Arg		args[MAX_ARGS];
    int		n;
    Boolean	set;
    XmString	tcs;

    set = ((XmToggleButtonCallbackStruct*)call_data)->set;

    if (set) {
	tcs = XmStringCreateLtoR("List Sensitive",
				 XmSTRING_DEFAULT_CHARSET);
	sensitive = True;
    }
    else {
	tcs = XmStringCreateLtoR("List Insensitive",
				 XmSTRING_DEFAULT_CHARSET);
	sensitive = False;
    }

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    XtSetValues(ToggleB1, args, n);

    n = 0;
/*    XtSetArg(args[n], XmNsensitive, sensitive);		n++; */
/*    XtSetValues(List1, args, n); */
    XtSetSensitive(List1, sensitive);		n++;

}


static void ToggleCB(Widget w, XtPointer client_data,
			       XtPointer call_data)

{
    Arg		args[MAX_ARGS];
    int		n;
    Boolean	set;
    XmString	tcs;

    set = ((XmToggleButtonCallbackStruct*)call_data)->set;

    if (set) {
	tcs = XmStringCreateLtoR("Automatic Selection True",
				 XmSTRING_DEFAULT_CHARSET);
	auto_selection = True;
    }
    else {
	tcs = XmStringCreateLtoR("Automatic Selection False",
				 XmSTRING_DEFAULT_CHARSET);
	auto_selection = False;
    }

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    XtSetValues(ToggleB, args, n);

    n = 0;
    XtSetArg(args[n], XmNautomaticSelection, auto_selection);	n++;
    XtSetValues(List1, args, n);

}


static void Init_Resources()

{
    spacing = 5;
    margin_width = 2;
    margin_height = 2;
    list_policy = XmVARIABLE;
    visible_item_count = NITEMS - 10;
    top_item_pos = 1;
    selection_policy = XmBROWSE_SELECT;
    scroll_display_policy = XmAS_NEEDED;
    auto_selection = False;
    sensitive = True;
}
