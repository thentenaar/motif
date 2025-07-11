/* $XConsortium: Container2.c /main/5 1995/07/15 21:03:28 drk $ */
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

#include "collapsedState.bmp"
#include "expandedState.bmp"


#define	NAME_LEN	20
#define	NUM_OBJ		9
#define	NUM_COL		4


static void		CreateControlPanel(void );
static void		SelectCB(Widget wid, XtPointer client_data, 
				 XtPointer call_data);
static void		OutlineCB(Widget wid, XtPointer client_data, 
				  XtPointer call_data);
static void		SetColumnsCB(Widget wid, XtPointer client_data, 
				     XtPointer call_data);
static Widget	        CreatePushButton(Widget parent, char *label);
static void		AutoSelectionCB(Widget wid, XtPointer client_data, 
					XtPointer call_data);
static void		EntryViewTypeCB(Widget wid, XtPointer client_data, 
					XtPointer call_data);
static void		LayoutTypeCB(Widget wid, XtPointer client_data, 
				     XtPointer call_data);
static void		PrimaryOwnerShipCB(Widget wid, XtPointer client_data, 
					   XtPointer call_data);
static void		SelectionPolicyCB(Widget wid, XtPointer client_data, 
					  XtPointer call_data);
static void		ShowColumnHeadingCB(Widget wid, XtPointer client_data, 
					    XtPointer call_data);
static void		SelectionTechniqueCB(Widget wid, XtPointer client_data,
					     XtPointer call_data);
static void		IncludeModelCB(Widget wid, XtPointer client_data, 
				       XtPointer call_data);
static void		OutlineLineStyleCB(Widget wid, XtPointer client_data, 
					   XtPointer call_data);
static void		OutlineButtonPolicyCB(Widget wid, 
					      XtPointer client_data, 
					      XtPointer call_data);
static void             IndentationCB(Widget wid, XtPointer client_data,
				      XtPointer call_data);
static void             ColumnWidthCB(Widget wid, XtPointer client_data,
				      XtPointer call_data);
static void		PlaceStyleCB(Widget wid, XtPointer client_data, 
				     XtPointer call_data);
static void		SnapModelCB(Widget wid, XtPointer client_data, 
				    XtPointer call_data);
static void		ResizeModelCB(Widget wid, XtPointer client_data, 
				      XtPointer call_data);
static void             LargeCellWidthCB(Widget wid, XtPointer client_data,
					 XtPointer call_data);
static void             LargeCellHeightCB(Widget wid, XtPointer client_data,
					  XtPointer call_data);
static void             SmallCellWidthCB(Widget wid, XtPointer client_data,
					 XtPointer call_data);
static void             SmallCellHeightCB(Widget wid, XtPointer client_data,
					  XtPointer call_data);


Widget	Container2;
Widget	IconGad1,IconGad2,IconGad3,IconGad4;
Widget	IconGad31,IconGad32,IconGad321,IconGad3211,IconGad33;
Widget  IndentationScale, ColumnWidthScale;
Widget  LargeCellWidthScale, LargeCellHeightScale;
Widget  SmallCellWidthScale, SmallCellHeightScale;

char	*ColumnHeadingText[NUM_COL] = { 
	"Icon",
    "Full Title",
    "Favorite Flavor",
    "Age"
};

char	*FullTitleText[NUM_OBJ] = { 
	"The First Object",
    "2nd Object, but still important",
    "Show",
    "Honorable Mention",
    "Fifth of Beethoven",
    "Snowflake",
    "The Little Dwarfs",
    "100 (in Octal)",
    "Cat Lives"
};

char	*FlavorText[NUM_OBJ] = { 
	"Chocolate",
    "Raspberry",
    "Blueberry",
    "Maple",
    "Vanilla",
    "Zucchini",
    "Licorice",
    "Potatoe",
    "Lemon"
};

char	*AgeText[NUM_OBJ] = { 
	"42",
	"10",
	"4",
	"76",
	"2",
    "None of your business","102","23","16"
};


static Cardinal NumColumns[NUM_COL] = { 0, 1, 2, 3 };

static unsigned int	AutomaticSelection[2] = { XmAUTO_SELECT, 
						  XmNO_AUTO_SELECT };
static unsigned int	EntryViewType[3] = { XmLARGE_ICON, XmSMALL_ICON, 
					     XmANY_ICON };
static unsigned int	LayoutType[3] = { XmOUTLINE, XmSPATIAL, XmDETAIL };
static unsigned int	PrimaryOwnerShip[4] = { XmOWN_NEVER, XmOWN_ALWAYS,
						XmOWN_MULTIPLE,
						XmOWN_POSSIBLE_MULTIPLE };
static unsigned int	SelectionPolicy[4] = { XmSINGLE_SELECT, 
					       XmMULTIPLE_SELECT,
					       XmEXTENDED_SELECT, 
					       XmBROWSE_SELECT };

static Boolean ShowColumnHeading[2] = { True, False };

static unsigned int	SelectionTechnique[5] = { XmMARQUEE, 
						  XmMARQUEE_EXTEND_START,
						  XmMARQUEE_EXTEND_BOTH,
						  XmTOUCH_ONLY,
						  XmTOUCH_OVER };
static unsigned char OutlineLineStyle[2] = { XmNO_LINE, XmSINGLE };
static unsigned char OutlineButtonPolicy[2] = { XmOUTLINE_BUTTON_PRESENT, 
					        XmOUTLINE_BUTTON_ABSENT };
static unsigned char IncludeModel[3] = { XmAPPEND, XmCLOSEST, XmFIRST_FIT };
static unsigned char PlaceStyle[3] = { XmNONE, XmGRID, XmCELLS };
static unsigned char SnapModel[3] = { XmNONE, XmSNAP_TO_GRID, XmCENTER };
static unsigned char ResizeModel[3] = { XmGROW_MINOR, XmGROW_MAJOR, 
					XmGROW_BALANCED };



XmStringTable	*EntryDetails;
XmStringTable	ColumnHeadings;


void
main(int argc, char **argv)
{

	XmStringTable	ColumnDetails;
	int		i,j;
	Cardinal	n;
	Arg		args[MAX_ARGS];
	int		test_num;
	char		test_char;
	char		ContainerName[NAME_LEN + 1];
	Pixmap	        CollapsedStatePixmap, ExpandedStatePixmap;


	ContainerName[0] = '\0';
	test_num = 0;
	test_char = '\0';

	CommonTestInit(argc, argv);

	if (UserData != NULL) {

		if (strcmp(UserData, "a") == 0)
			test_num = 1;
		else if (strcmp(UserData, "b") == 0)
			test_num = 2;
		else if (strcmp(UserData, "c") == 0)
			test_num = 3;
		else if (strcmp(UserData, "d") == 0)
			test_num = 4;

		test_char = *UserData;

		free(UserData);

	}
	sprintf(ContainerName, "Container2%c", test_char);

	CollapsedStatePixmap = 
	  XCreatePixmapFromBitmapData(display, rootWindow,
		collapsedState_bits, collapsedState_width, 
		collapsedState_height, CommonGetColor("black"),
		CommonGetColor("white"), 
		XDefaultDepth(display, XDefaultScreen(display)));

	ExpandedStatePixmap = 
	  XCreatePixmapFromBitmapData(display, rootWindow,
		expandedState_bits, expandedState_width, expandedState_height,
		CommonGetColor("white"), CommonGetColor("black"),
		XDefaultDepth(display, XDefaultScreen(display)));


	ColumnHeadings = (XmStringTable) XtMalloc(NUM_COL * sizeof(XmString));

	for (i = 0; i < NUM_COL; i++)
	     ColumnHeadings[i] = XmStringGenerate(ColumnHeadingText[i],
						  NULL,
						  XmCHARSET_TEXT,
						  NULL);


	n = 0;
	XtSetArg(args[n], XmNentryViewType, XmANY_ICON); n++;
	XtSetArg(args[n], XmNlayoutType, XmOUTLINE); n++;
	XtSetArg(args[n], XmNautomaticSelection, XmNO_AUTO_SELECT); n++;
	XtSetArg(args[n], XmNdetailColumnHeading, ColumnHeadings); n++;
	XtSetArg(args[n], XmNdetailColumnHeadingCount, NUM_COL); n++;
	XtSetArg(args[n], XmNdetailOrderCount, NUM_COL-1); n++;
	XtSetArg(args[n], XmNcollapsedStatePixmap, CollapsedStatePixmap); n++;
	XtSetArg(args[n], XmNexpandedStatePixmap, ExpandedStatePixmap); n++;

	Container2 = XmCreateContainer(Shell1, "Container2", args, n);
	XtManageChild(Container2);

	XtAddCallback(Container2, XmNselectionCallback, SelectCB, NULL);
	XtAddCallback(Container2, XmNoutlineChangedCallback, OutlineCB, NULL);

	EntryDetails = (XmStringTable *) XtMalloc(NUM_OBJ * 
						  sizeof(XmStringTable));

	for (i = 0; i < NUM_OBJ; i++) {

	     ColumnDetails = (XmStringTable)XtMalloc((NUM_COL-1) * 
							sizeof(XmString));
	     ColumnDetails[0] = XmStringGenerate(FullTitleText[i],
						 NULL,
						 XmCHARSET_TEXT,
						 NULL);
	     ColumnDetails[1] = XmStringGenerate(FlavorText[i],
						 NULL,
						 XmCHARSET_TEXT,
						 NULL);
	     ColumnDetails[2] = XmStringGenerate(AgeText[i],
						 NULL,
						 XmCHARSET_TEXT,
						 NULL);
	     EntryDetails[i] = ColumnDetails;

	}

	n = 0;
	XtSetArg(args[n], XmNx, 100); n++;
	XtSetArg(args[n], XmNy, 100); n++;
	XtSetArg(args[n], XmNviewType, XmSMALL_ICON); n++;
	XtSetArg(args[n], XmNdetailCount, NUM_COL-1); n++;
	XtSetArg(args[n], XmNdetail, EntryDetails[0]); n++;
	IconGad1 = XmCreateIconGadget(Container2, "IconGad1", args, n);
	XtManageChild(IconGad1);

	n = 0;
	XtSetArg(args[n], XmNx, 200); n++;
	XtSetArg(args[n], XmNy, 200); n++;
	XtSetArg(args[n], XmNviewType, XmSMALL_ICON); n++;
	XtSetArg(args[n], XmNdetailCount, NUM_COL-1); n++;
	XtSetArg(args[n], XmNdetail, EntryDetails[1]); n++;
	IconGad2 = XmCreateIconGadget(Container2, "IconGad2", args, n);
	XtManageChild(IconGad2);

	n = 0;
	XtSetArg(args[n], XmNx, 300); n++;
	XtSetArg(args[n], XmNy, 100); n++;
	XtSetArg(args[n], XmNviewType, XmLARGE_ICON); n++;
	XtSetArg(args[n], XmNdetailCount, NUM_COL-1); n++;
	XtSetArg(args[n], XmNdetail, EntryDetails[2]); n++;
	IconGad3 = XmCreateIconGadget(Container2, "IconGad3", args, n);
	XtManageChild(IconGad3);

	n = 0;
	XtSetArg(args[n], XmNx, 50); n++;
	XtSetArg(args[n], XmNy, 400); n++;
	XtSetArg(args[n], XmNviewType, XmSMALL_ICON); n++;
	XtSetArg(args[n], XmNdetailCount, NUM_COL-1); n++;
	XtSetArg(args[n], XmNdetail, EntryDetails[3]); n++;
	IconGad4 = XmCreateIconGadget(Container2, "IconGad4", args, n);
	XtManageChild(IconGad4);

	n = 0;
	XtSetArg(args[n], XmNentryParent, IconGad3); n++;
	XtSetArg(args[n], XmNviewType, XmSMALL_ICON); n++;
	XtSetArg(args[n], XmNdetailCount, NUM_COL-1); n++;
	XtSetArg(args[n], XmNdetail, EntryDetails[4]); n++;
	IconGad31 = XmCreateIconGadget(Container2, "IconGad31", args, n);
	XtManageChild(IconGad31);

	n = 0;
	XtSetArg(args[n], XmNentryParent, IconGad3); n++;
	XtSetArg(args[n], XmNviewType, XmSMALL_ICON); n++;
	XtSetArg(args[n], XmNdetailCount, NUM_COL-1); n++;
	XtSetArg(args[n], XmNdetail, EntryDetails[5]); n++;
	IconGad32 = XmCreateIconGadget(Container2, "IconGad32", args, n);
	XtManageChild(IconGad32);

	n = 0;
	XtSetArg(args[n], XmNentryParent, IconGad32); n++;
	XtSetArg(args[n], XmNviewType, XmLARGE_ICON); n++;
	XtSetArg(args[n], XmNdetailCount, NUM_COL-1); n++;
	XtSetArg(args[n], XmNdetail, EntryDetails[6]); n++;
	IconGad321 = XmCreateIconGadget(Container2, "IconGad321", args, n);
	XtManageChild(IconGad321);

	n = 0;
	XtSetArg(args[n], XmNentryParent, IconGad321); n++;
	XtSetArg(args[n], XmNviewType, XmLARGE_ICON); n++;
	XtSetArg(args[n], XmNdetailCount, NUM_COL-1); n++;
	XtSetArg(args[n], XmNdetail, EntryDetails[7]); n++;
	IconGad3211 = XmCreateIconGadget(Container2, "IconGad3211", args, n);
	XtManageChild(IconGad3211);

	n = 0;
	XtSetArg(args[n], XmNentryParent, IconGad3); n++;
	XtSetArg(args[n], XmNviewType, XmSMALL_ICON); n++;
	XtSetArg(args[n], XmNdetailCount, NUM_COL-1); n++;
	XtSetArg(args[n], XmNdetail, EntryDetails[8]); n++;
	IconGad33 = XmCreateIconGadget(Container2, "IconGad33", args, n);
	XtManageChild(IconGad33);

	for (i = 0; i < NUM_OBJ; i++) {

		ColumnDetails = EntryDetails[i];
		for (j = 0; j < NUM_COL-1; j++)
			XmStringFree(ColumnDetails[j]);
		XtFree((XtPointer)ColumnDetails);

	}
	XtFree((XtPointer)EntryDetails);

	CreateControlPanel();

	XtRealizeWidget(Shell1);

	CommonPause();

	CommonPause();

	CommonPause();

	CommonPause();

	CommonPause();

	CommonPause();

	CommonPause();

	CommonPause();

	CommonPause();

	CommonPause();

	CommonPause();

	CommonPause();

	for (i = 0; i < NUM_COL; i++)
		XmStringFree(ColumnHeadings[i]);
	XtFree((XtPointer)ColumnHeadings);

    /* Exit */
	CommonPause();

	XtAppMainLoop(app_context);

}


static void
CreateControlPanel(void )
{

   Arg		args[MAX_ARGS];
   Cardinal	n;
   XmString	tcs;
   Widget	PopupShell, FormDialog;
   Widget	CommonResources;
   Widget	AutoSelection_Pulldown;
   Widget	NoAutoSelect, AutoSelect;
   Widget	AutoSelection_Option;
   Widget	EntryViewType_Pulldown;
   Widget	LargeIcon, SmallIcon, AnyIcon;
   Widget	EntryViewType_Option;
   Widget	LayoutType_Pulldown;
   Widget	Spatial, Outline, Detail;
   Widget	LayoutType_Option;
   Widget	PrimaryOwnerShip_Pulldown;
   Widget	NeverOwn, AlwaysOwn, Multiple, Possible_Multiple;
   Widget	PrimaryOwnerShip_Option;
   Widget	SelectionPolicy_Pulldown;
   Widget	SingleSelect, MultipleSelect, ExtendedSelect, BrowseSelect;
   Widget	SelectionPolicy_Option;
   Widget	SelectionTechnique_Pulldown;
   Widget	Marquee, MarqueeStart, MarqueeBoth, TouchOnly, TouchOver;
   Widget	SelectionTechnique_Option;
   Widget       OutlineResources;
   Widget	OutlineLineStyle_Pulldown;
   Widget	XmNo_line, XmSingle;
   Widget	OutlineLineStyle_Option;
   Widget	OutlineButtonPolicy_Pulldown;
   Widget	XmPresent, XmAbsent;
   Widget	OutlineButtonPolicy_Option;
   Widget	DetailResources;
   Widget	Columns_Pulldown;
   Widget	ZeroColumn, OneColumn, TwoColumns, ThreeColumns;
   Widget	Columns_Option;
   Widget	SpatialResources;
   Widget	ShowColumnHeading_Pulldown;
   Widget	Show, NoShow;
   Widget	ShowColumnHeading_Option;
   Widget	IncludeModel_Pulldown;
   Widget	Append, Closest, First_Fit;
   Widget	IncludeModel_Option;
   Widget	PlaceStyle_Pulldown;
   Widget	PSNone, PSGrid, PSCells;
   Widget	PlaceStyle_Option;
   Widget	SnapModel_Pulldown;
   Widget	SMNone, SMToGrid, SMCenter;
   Widget	SnapModel_Option;
   Widget	ResizeModel_Pulldown;
   Widget	RMMinor, RMMajor, RMBalanced;
   Widget	ResizeModel_Option;
   Widget	RowColumn1, RowColumn2, RowColumn3, RowColumn4;


   n = 0;
   XtSetArg(args[n], XtNgeometry, "=10x10+0+200"); n++;
   XtSetArg(args[n], XtNallowShellResize, True); n++;
   PopupShell = XtCreatePopupShell("Container Resources", 
                                    topLevelShellWidgetClass, Shell1, 
                                    args, n);

   n = 0;
   XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
   FormDialog = XmCreateForm(PopupShell, "FormDialog", args, n);
   XtManageChild(FormDialog);

   tcs = XmStringGenerate("General Resources:", XmFONTLIST_DEFAULT_TAG,
			  XmCHARSET_TEXT, NULL);
   n = 0;
   XtSetArg(args[n], XmNlabelString, tcs); n++;
   XtSetArg(args[n], XmNbackground, CommonGetColor("white")); n++;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNtopOffset, 5); n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNleftOffset, 5); n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNrightOffset, 5); n++;
   CommonResources = XmCreateLabel(FormDialog, "CommonResources", args, n);
   XtManageChild(CommonResources);

   XmStringFree(tcs);

   n = 0;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNtopOffset, 5); n++;
   XtSetArg(args[n], XmNtopWidget, CommonResources); n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNleftOffset, 5); n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNrightOffset, 5); n++;
   XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
   XtSetArg(args[n], XmNspacing, 5); n++;
   XtSetArg(args[n], XmNnumColumns, 2); n++;
   XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
   XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
   XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
   RowColumn1 = XmCreateRowColumn(FormDialog, "RowColumn1", args, n);
   XtManageChild(RowColumn1);

   n = 0;
   XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen"));  n++;
   LayoutType_Pulldown = XmCreatePulldownMenu(RowColumn1, 
					      "LayoutType_Pulldown",
                                              args, n);
   Spatial = CreatePushButton(LayoutType_Pulldown, "Spatial");
   Outline = CreatePushButton(LayoutType_Pulldown, "Outline");
   Detail = CreatePushButton(LayoutType_Pulldown, "Detail");

   XtAddCallback(Outline, XmNactivateCallback, LayoutTypeCB, 
		 (XtPointer ) &LayoutType[0]);
   XtAddCallback(Spatial, XmNactivateCallback, LayoutTypeCB, 
		 (XtPointer ) &LayoutType[1]);
   XtAddCallback(Detail, XmNactivateCallback, LayoutTypeCB, 
		 (XtPointer ) &LayoutType[2]);

   tcs = XmStringGenerate("Layout Type             ",
                          XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL);

   n = 0;
   XtSetArg(args[n], XmNlabelString, tcs); n++;
   XtSetArg(args[n], XmNmenuHistory, Outline); n++;    
   XtSetArg(args[n], XmNsubMenuId, LayoutType_Pulldown); n++;
   XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
   LayoutType_Option = XmCreateOptionMenu(RowColumn1, "LayoutType_Option",
                                          args, n);    
   XtManageChild(LayoutType_Option);

   XmStringFree(tcs);

   n = 0;
   XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen"));  n++;
   EntryViewType_Pulldown = XmCreatePulldownMenu(RowColumn1, 
						 "EntryViewType_Pulldown",
                                              	 args, n);
   LargeIcon = CreatePushButton(EntryViewType_Pulldown, "LargeIcon");
   SmallIcon = CreatePushButton(EntryViewType_Pulldown, "SmallIcon");
   AnyIcon = CreatePushButton(EntryViewType_Pulldown, "AnyIcon");

   XtAddCallback(LargeIcon, XmNactivateCallback,
                 EntryViewTypeCB, (XtPointer ) &EntryViewType[0]);
   XtAddCallback(SmallIcon, XmNactivateCallback,
                 EntryViewTypeCB, (XtPointer ) &EntryViewType[1]);
   XtAddCallback(AnyIcon, XmNactivateCallback,
                 EntryViewTypeCB, (XtPointer ) &EntryViewType[2]);

   tcs = XmStringGenerate("Entry View Type         ",
                          XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL);

   n = 0;
   XtSetArg(args[n], XmNlabelString, tcs); n++;
   XtSetArg(args[n], XmNmenuHistory, AnyIcon); n++;
   XtSetArg(args[n], XmNsubMenuId, EntryViewType_Pulldown); n++;
   XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
   EntryViewType_Option = XmCreateOptionMenu(RowColumn1, 
					     "EntryViewType_Option",
					     args, n);
   XtManageChild(EntryViewType_Option);

   XmStringFree(tcs);

   n = 0;
   XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen"));  n++;
   PrimaryOwnerShip_Pulldown = XmCreatePulldownMenu(RowColumn1, 
						   "PrimaryOwnerShip_Pulldown",
						    args, n);
   NeverOwn = CreatePushButton(PrimaryOwnerShip_Pulldown, "NeverOwn");
   AlwaysOwn = CreatePushButton(PrimaryOwnerShip_Pulldown, "AlwaysOwn");
   Multiple = CreatePushButton(PrimaryOwnerShip_Pulldown, "Multiple");
   Possible_Multiple = CreatePushButton(PrimaryOwnerShip_Pulldown, 
					"Possible_Multiple");

   XtAddCallback(NeverOwn, XmNactivateCallback,
                 PrimaryOwnerShipCB, (XtPointer ) &PrimaryOwnerShip[0]);
   XtAddCallback(AlwaysOwn, XmNactivateCallback,
                 PrimaryOwnerShipCB, (XtPointer ) &PrimaryOwnerShip[1]);
   XtAddCallback(Multiple, XmNactivateCallback,
                 PrimaryOwnerShipCB, (XtPointer ) &PrimaryOwnerShip[2]);
   XtAddCallback(Possible_Multiple, XmNactivateCallback,
                 PrimaryOwnerShipCB, (XtPointer ) &PrimaryOwnerShip[3]);

   tcs = XmStringGenerate("Primary OwnerShip       ",
                          XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL);

   n = 0;
   XtSetArg(args[n], XmNlabelString, tcs); n++;
   XtSetArg(args[n], XmNmenuHistory, Possible_Multiple); n++;
   XtSetArg(args[n], XmNsubMenuId, PrimaryOwnerShip_Pulldown); n++;
   XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
   PrimaryOwnerShip_Option = XmCreateOptionMenu(RowColumn1, 
						"PrimaryOwnerShip_Option",
                                          	args, n);
   XtManageChild(PrimaryOwnerShip_Option);

   XmStringFree(tcs);

   n = 0;
   XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen"));  n++;
   AutoSelection_Pulldown = XmCreatePulldownMenu(RowColumn1, 
						 "AutoSelection_Pulldown",
                                              	 args, n);
   NoAutoSelect = CreatePushButton(AutoSelection_Pulldown, "NoAutoSelect");
   AutoSelect = CreatePushButton(AutoSelection_Pulldown, "AutoSelect");

   XtAddCallback(AutoSelect, XmNactivateCallback,
                 AutoSelectionCB, (XtPointer ) &AutomaticSelection[0]);
   XtAddCallback(NoAutoSelect, XmNactivateCallback,
                 AutoSelectionCB, (XtPointer ) &AutomaticSelection[1]);

   tcs = XmStringGenerate("Automatic Selection     ",
                          XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL);

   n = 0;
   XtSetArg(args[n], XmNlabelString, tcs); n++;
   XtSetArg(args[n], XmNmenuHistory, AutoSelect); n++;
   XtSetArg(args[n], XmNsubMenuId, AutoSelection_Pulldown); n++;
   XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
   AutoSelection_Option = XmCreateOptionMenu(RowColumn1, 
					     "AutoSelection_Option",
					     args, n);
   XtManageChild(AutoSelection_Option);

   XmStringFree(tcs);

   n = 0;
   XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen"));  n++;
   SelectionPolicy_Pulldown = XmCreatePulldownMenu(RowColumn1, 
						   "SelectionPolicy_Pulldown",
						   args, n);
   SingleSelect = CreatePushButton(SelectionPolicy_Pulldown, "SingleSelect");
   MultipleSelect = CreatePushButton(SelectionPolicy_Pulldown, 
				     "MultipleSelect");
   ExtendedSelect = CreatePushButton(SelectionPolicy_Pulldown, 
				     "ExtendedSelect");
   BrowseSelect = CreatePushButton(SelectionPolicy_Pulldown, "BrowseSelect");

   XtAddCallback(SingleSelect, XmNactivateCallback,
                 SelectionPolicyCB, (XtPointer ) &SelectionPolicy[0]);
   XtAddCallback(MultipleSelect, XmNactivateCallback,
                 SelectionPolicyCB, (XtPointer ) &SelectionPolicy[1]);
   XtAddCallback(ExtendedSelect, XmNactivateCallback,
                 SelectionPolicyCB, (XtPointer ) &SelectionPolicy[2]);
   XtAddCallback(BrowseSelect, XmNactivateCallback,
                 SelectionPolicyCB, (XtPointer ) &SelectionPolicy[3]);

   tcs = XmStringGenerate("Selection Policy        ",
                          XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL);

   n = 0;
   XtSetArg(args[n], XmNlabelString, tcs); n++;
   XtSetArg(args[n], XmNmenuHistory, ExtendedSelect); n++;
   XtSetArg(args[n], XmNsubMenuId, SelectionPolicy_Pulldown); n++;
   XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
   SelectionPolicy_Option = XmCreateOptionMenu(RowColumn1, 
					       "SelectionPolicy_Option",
					       args, n);
   XtManageChild(SelectionPolicy_Option);

   XmStringFree(tcs);

    n = 0;
    XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen"));  n++;
    SelectionTechnique_Pulldown = XmCreatePulldownMenu(RowColumn1, 
						"SelectionTechnique_Pulldown",
                                                args, n);
    Marquee = CreatePushButton(SelectionTechnique_Pulldown, "Marquee");
    MarqueeStart = CreatePushButton(SelectionTechnique_Pulldown, 
				    "MarqueeStart");
    MarqueeBoth = CreatePushButton(SelectionTechnique_Pulldown, "MarqueeBoth");
    TouchOnly = CreatePushButton(SelectionTechnique_Pulldown, "TouchOnly");
    TouchOver = CreatePushButton(SelectionTechnique_Pulldown, "TouchOver");

    XtAddCallback(Marquee, XmNactivateCallback,
                  SelectionTechniqueCB, (XtPointer ) &SelectionTechnique[0]);
    XtAddCallback(MarqueeStart, XmNactivateCallback,
                  SelectionTechniqueCB, (XtPointer ) &SelectionTechnique[1]);
    XtAddCallback(MarqueeBoth, XmNactivateCallback,
                  SelectionTechniqueCB, (XtPointer ) &SelectionTechnique[2]);
    XtAddCallback(TouchOnly, XmNactivateCallback,
                  SelectionTechniqueCB, (XtPointer ) &SelectionTechnique[3]);
    XtAddCallback(TouchOver, XmNactivateCallback,
                  SelectionTechniqueCB, (XtPointer ) &SelectionTechnique[4]);

    tcs = XmStringGenerate("Selection Technique     ",
                          XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs); n++;
    XtSetArg(args[n], XmNmenuHistory, TouchOver); n++;
    XtSetArg(args[n], XmNsubMenuId, SelectionTechnique_Pulldown); n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
    SelectionTechnique_Option = XmCreateOptionMenu(RowColumn1, 
						   "SelectionTechnique_Option",
						   args, n);
    XtManageChild(SelectionTechnique_Option);

    XmStringFree(tcs);

   tcs = XmStringGenerate("Outline Resources:",
                          XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL);

   n = 0;
   XtSetArg(args[n], XmNlabelString, tcs); n++;
   XtSetArg(args[n], XmNbackground, CommonGetColor("white")); n++;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNtopOffset, 5); n++;
   XtSetArg(args[n], XmNtopWidget, RowColumn1); n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNleftOffset, 5); n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNrightOffset, 5); n++;
   OutlineResources = XmCreateLabel(FormDialog, "OutlineResources", args, n);
   XtManageChild(OutlineResources);

   XmStringFree(tcs);

   n = 0;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNtopOffset, 5); n++;
   XtSetArg(args[n], XmNtopWidget, OutlineResources); n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNleftOffset, 5); n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNrightOffset, 5); n++;
   XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
   XtSetArg(args[n], XmNspacing, 5); n++;
   XtSetArg(args[n], XmNnumColumns, 2); n++;
   XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
   XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
   XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
   RowColumn2 = XmCreateRowColumn(FormDialog, "RowColumn2", args, n);
   XtManageChild(RowColumn2);

    n = 0;
    XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen"));  n++;
    OutlineLineStyle_Pulldown = XmCreatePulldownMenu(RowColumn2, 
						   "OutlineLineStyle_Pulldown",
						   args, n);
    XmNo_line = CreatePushButton(OutlineLineStyle_Pulldown, "XmNO_LINE");
    XmSingle = CreatePushButton(OutlineLineStyle_Pulldown, "XmSINGLE");

    XtAddCallback(XmNo_line, XmNactivateCallback,
                  OutlineLineStyleCB, (XtPointer ) &OutlineLineStyle[0]);
    XtAddCallback(XmSingle, XmNactivateCallback,
                  OutlineLineStyleCB, (XtPointer ) &OutlineLineStyle[1]);

    tcs = XmStringGenerate("Outline LineStyle       ",
                          XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs); n++;
    XtSetArg(args[n], XmNmenuHistory, XmSingle); n++;
    XtSetArg(args[n], XmNsubMenuId, OutlineLineStyle_Pulldown); n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
    OutlineLineStyle_Option = XmCreateOptionMenu(RowColumn2, 
						 "OutlineLineStyle_Option",
						 args, n);
    XtManageChild(OutlineLineStyle_Option);

    XmStringFree(tcs);

    n = 0;
    XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen"));  n++;
    OutlineButtonPolicy_Pulldown = XmCreatePulldownMenu(RowColumn2, 
					  "OutlineButtonPolicy_Pulldown",
					  args, n);
    XmPresent = CreatePushButton(OutlineButtonPolicy_Pulldown, 
				 "XmOUTLINE_BUTTON_PRESENT");
    XmAbsent = CreatePushButton(OutlineButtonPolicy_Pulldown, 
				"XmOUTLINE_BUTTON_ABSENT");

    XtAddCallback(XmPresent, XmNactivateCallback,
                  OutlineButtonPolicyCB, (XtPointer ) &OutlineButtonPolicy[0]);
    XtAddCallback(XmAbsent, XmNactivateCallback,
                  OutlineButtonPolicyCB, (XtPointer ) &OutlineButtonPolicy[1]);

    tcs = XmStringGenerate("Outline ButtonPolicy    ",
                             XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs); n++;
    XtSetArg(args[n], XmNmenuHistory, XmPresent); n++;
    XtSetArg(args[n], XmNsubMenuId, OutlineButtonPolicy_Pulldown); n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
    OutlineButtonPolicy_Option = XmCreateOptionMenu(RowColumn2, 
						 "OutlineButtonPolicy_Option",
						 args, n);
    XtManageChild(OutlineButtonPolicy_Option);

    XmStringFree(tcs);

   tcs = XmStringGenerate("Outline Indentation", 
                          XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL);

   n = 0;
   XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen"));  n++;
   XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
   XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
   XtSetArg(args[n], XmNscaleHeight, 20); n++;
   XtSetArg(args[n], XmNscaleWidth, 250); n++;
   XtSetArg(args[n], XmNmaximum, 500); n++;
   XtSetArg(args[n], XmNshowValue, True); n++;
   XtSetArg(args[n], XmNvalue, 100); n++;
   XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
   XtSetArg(args[n], XmNtitleString, tcs); n++;
   IndentationScale = XmCreateScale(RowColumn2, "IndentationScale", args, n);
   XtManageChild(IndentationScale);
   
   XtAddCallback(IndentationScale, XmNvalueChangedCallback, IndentationCB, 
		 NULL);
   
   XmStringFree(tcs);

   tcs = XmStringGenerate("Outline ColumnWidth", 
                          XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL);

   n = 0;
   XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen"));  n++;
   XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
   XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
   XtSetArg(args[n], XmNscaleHeight, 20); n++;
   XtSetArg(args[n], XmNscaleWidth, 250); n++;
   XtSetArg(args[n], XmNmaximum, 500); n++;
   XtSetArg(args[n], XmNshowValue, True); n++;
   XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
   XtSetArg(args[n], XmNtitleString, tcs); n++;
   ColumnWidthScale = XmCreateScale(RowColumn2, "ColumnWidthScale", args, n);
   XtManageChild(ColumnWidthScale);
   
   XtAddCallback(ColumnWidthScale, XmNvalueChangedCallback, ColumnWidthCB, 
		 NULL);

   XmStringFree(tcs);

   tcs = XmStringGenerate("Detail Resources:",
                          XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL);

   n = 0;
   XtSetArg(args[n], XmNlabelString, tcs); n++;
   XtSetArg(args[n], XmNbackground, CommonGetColor("white")); n++;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNtopOffset, 5); n++;
   XtSetArg(args[n], XmNtopWidget, RowColumn2); n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNleftOffset, 5); n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNrightOffset, 5); n++;
   DetailResources = XmCreateLabel(FormDialog, "DetailResources", args, n);
   XtManageChild(DetailResources);

   XmStringFree(tcs);

   n = 0;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNtopOffset, 5); n++;
   XtSetArg(args[n], XmNtopWidget, DetailResources); n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNleftOffset, 5); n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNrightOffset, 5); n++;
   XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
   XtSetArg(args[n], XmNspacing, 5); n++;
   XtSetArg(args[n], XmNnumColumns, 2); n++;
   XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
   XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
   XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
   RowColumn3 = XmCreateRowColumn(FormDialog, "RowColumn3", args, n);
   XtManageChild(RowColumn3);

   n = 0;
   XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen"));  n++;
   Columns_Pulldown = XmCreatePulldownMenu(RowColumn3, "Columns_Pulldown",
                                           args, n);
   ZeroColumn = CreatePushButton(Columns_Pulldown, "Zero");
   OneColumn = CreatePushButton(Columns_Pulldown, "One");
   TwoColumns = CreatePushButton(Columns_Pulldown, "Two");
   ThreeColumns = CreatePushButton(Columns_Pulldown, "Three");

   XtAddCallback(ZeroColumn, XmNactivateCallback,
                 SetColumnsCB, (XtPointer ) &NumColumns[0]);
   XtAddCallback(OneColumn, XmNactivateCallback,
                 SetColumnsCB, (XtPointer ) &NumColumns[1]);
   XtAddCallback(TwoColumns, XmNactivateCallback,
                 SetColumnsCB, (XtPointer ) &NumColumns[2]);
   XtAddCallback(ThreeColumns, XmNactivateCallback,
                 SetColumnsCB, (XtPointer ) &NumColumns[3]);

   tcs = XmStringGenerate("Number of Columns       ",
                          XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL);
                         
   n = 0;
   XtSetArg(args[n], XmNlabelString, tcs); n++;
   XtSetArg(args[n], XmNmenuHistory, ZeroColumn); n++;
   XtSetArg(args[n], XmNsubMenuId, Columns_Pulldown); n++;
   XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
   Columns_Option = XmCreateOptionMenu(RowColumn3, "Columns_Option",
                                       args, n);
   XtManageChild(Columns_Option);

   XmStringFree(tcs);

    n = 0;
    XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen"));  n++;
    ShowColumnHeading_Pulldown = XmCreatePulldownMenu(RowColumn3, 
						  "ShowColumnHeading_Pulldown",
                                              	  args, n);
    Show = CreatePushButton(ShowColumnHeading_Pulldown, "YES");
    NoShow = CreatePushButton(ShowColumnHeading_Pulldown, "NO");

    XtAddCallback(Show, XmNactivateCallback,
                  ShowColumnHeadingCB, (XtPointer ) &ShowColumnHeading[0]);
    XtAddCallback(NoShow, XmNactivateCallback,
                  ShowColumnHeadingCB, (XtPointer ) &ShowColumnHeading[1]);

    tcs = XmStringGenerate("Show ColumnHeading      ",
                           XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs); n++;
    XtSetArg(args[n], XmNmenuHistory, Show); n++;
    XtSetArg(args[n], XmNsubMenuId, ShowColumnHeading_Pulldown); n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
    ShowColumnHeading_Option = XmCreateOptionMenu(RowColumn3, 
						  "ShowColumnHeading_Option",
						  args, n);
    XtManageChild(ShowColumnHeading_Option);

    XmStringFree(tcs);

    tcs = XmStringGenerate("Spatial Resources:",
                          XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs); n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("white")); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopOffset, 5); n++;
    XtSetArg(args[n], XmNtopWidget, RowColumn3); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset, 5); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset, 5); n++;
    SpatialResources = XmCreateLabel(FormDialog, 
				     "SpatialResources", 
				     args, n);
    XtManageChild(SpatialResources);

    XmStringFree(tcs);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopOffset, 5); n++;
    XtSetArg(args[n], XmNtopWidget, SpatialResources); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset, 5); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset, 5); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNspacing, 5); n++;
    XtSetArg(args[n], XmNnumColumns, 2); n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
    RowColumn4 = XmCreateRowColumn(FormDialog, "RowColumn4", args, n);
    XtManageChild(RowColumn4);

   tcs = XmStringGenerate("LargeCell Width", 
                          XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL);

   n = 0;
   XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen"));  n++;
   XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
   XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
   XtSetArg(args[n], XmNscaleHeight, 20); n++;
   XtSetArg(args[n], XmNscaleWidth, 250); n++;
   XtSetArg(args[n], XmNmaximum, 500); n++;
   XtSetArg(args[n], XmNshowValue, True); n++;
   XtSetArg(args[n], XmNvalue, 100); n++;
   XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
   XtSetArg(args[n], XmNtitleString, tcs); n++;
   LargeCellWidthScale = XmCreateScale(RowColumn4, "LargeCellWidthScale", 
				       args, n);
   XtManageChild(LargeCellWidthScale);
   
   XtAddCallback(LargeCellWidthScale, XmNvalueChangedCallback, 
		 LargeCellWidthCB, NULL);
   
   XmStringFree(tcs);

   tcs = XmStringGenerate("LargeCell Height",
                          XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL);

   n = 0;
   XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen"));  n++;
   XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
   XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
   XtSetArg(args[n], XmNscaleHeight, 20); n++;
   XtSetArg(args[n], XmNscaleWidth, 250); n++;
   XtSetArg(args[n], XmNmaximum, 500); n++;
   XtSetArg(args[n], XmNshowValue, True); n++;
   XtSetArg(args[n], XmNvalue, 100); n++;
   XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
   XtSetArg(args[n], XmNtitleString, tcs); n++;
   LargeCellHeightScale = XmCreateScale(RowColumn4, "LargeCellHeightScale", 
				        args, n);
   XtManageChild(LargeCellHeightScale);
   
   XtAddCallback(LargeCellHeightScale, XmNvalueChangedCallback, 
		 LargeCellHeightCB, NULL);
   
   XmStringFree(tcs);

   tcs = XmStringGenerate("SmallCell Width", 
                          XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL);

   n = 0;
   XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen"));  n++;
   XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
   XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
   XtSetArg(args[n], XmNscaleHeight, 20); n++;
   XtSetArg(args[n], XmNscaleWidth, 250); n++;
   XtSetArg(args[n], XmNmaximum, 500); n++;
   XtSetArg(args[n], XmNshowValue, True); n++;
   XtSetArg(args[n], XmNvalue, 50); n++;
   XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
   XtSetArg(args[n], XmNtitleString, tcs); n++;
   SmallCellWidthScale = XmCreateScale(RowColumn4, "SmallCellWidthScale", 
				       args, n);
   XtManageChild(SmallCellWidthScale);
   
   XtAddCallback(SmallCellWidthScale, XmNvalueChangedCallback, 
		 SmallCellWidthCB, NULL);
   
   XmStringFree(tcs);

   tcs = XmStringGenerate("SmallCell Height", 
                          XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL);

   n = 0;
   XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen"));  n++;
   XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
   XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
   XtSetArg(args[n], XmNscaleHeight, 20); n++;
   XtSetArg(args[n], XmNscaleWidth, 250); n++;
   XtSetArg(args[n], XmNmaximum, 500); n++;
   XtSetArg(args[n], XmNshowValue, True); n++;
   XtSetArg(args[n], XmNvalue, 50); n++;
   XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
   XtSetArg(args[n], XmNtitleString, tcs); n++;
   SmallCellHeightScale = XmCreateScale(RowColumn4, "SmallCellHeightScale", 
				        args, n);
   XtManageChild(SmallCellHeightScale);
   
   XtAddCallback(SmallCellHeightScale, XmNvalueChangedCallback, 
		 SmallCellHeightCB, NULL);
   
   XmStringFree(tcs);

    n = 0;
    XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen"));  n++;
    IncludeModel_Pulldown = XmCreatePulldownMenu(RowColumn4, 
						 "IncludeModel_Pulldown",
						 args, n);
    Append = CreatePushButton(IncludeModel_Pulldown, "Append");
    Closest = CreatePushButton(IncludeModel_Pulldown, "Closest");
    First_Fit = CreatePushButton(IncludeModel_Pulldown, "First_Fit");

    XtAddCallback(Append, XmNactivateCallback,
                  IncludeModelCB, (XtPointer ) &IncludeModel[0]);
    XtAddCallback(Closest, XmNactivateCallback,
                  IncludeModelCB, (XtPointer ) &IncludeModel[1]);
    XtAddCallback(First_Fit, XmNactivateCallback,
                  IncludeModelCB, (XtPointer ) &IncludeModel[2]);

    tcs = XmStringGenerate("Include Model           ",
                           XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs); n++;
    XtSetArg(args[n], XmNmenuHistory, Append); n++;
    XtSetArg(args[n], XmNsubMenuId, IncludeModel_Pulldown); n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
    IncludeModel_Option = XmCreateOptionMenu(RowColumn4, 
					     "IncludeModel_Option",
					     args, n);
    XtManageChild(IncludeModel_Option);

    XmStringFree(tcs);

    n = 0;
    XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen"));  n++;
    PlaceStyle_Pulldown = XmCreatePulldownMenu(RowColumn4, 
					       "PlaceStyle_Pulldown",
                                               args, n);
    PSNone = CreatePushButton(PlaceStyle_Pulldown, "XmNONE");
    PSGrid = CreatePushButton(PlaceStyle_Pulldown, "XmGRID");
    PSCells = CreatePushButton(PlaceStyle_Pulldown, "XmCELLS");

    XtAddCallback(PSNone, XmNactivateCallback,
                  PlaceStyleCB, (XtPointer ) &PlaceStyle[0]);
    XtAddCallback(PSGrid, XmNactivateCallback,
                  PlaceStyleCB, (XtPointer ) &PlaceStyle[1]);
    XtAddCallback(PSCells, XmNactivateCallback,
                  PlaceStyleCB, (XtPointer ) &PlaceStyle[2]);

    tcs = XmStringGenerate("Spatial Style           ",
                          XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs); n++;
    XtSetArg(args[n], XmNmenuHistory, PSNone); n++;
    XtSetArg(args[n], XmNsubMenuId, PlaceStyle_Pulldown); n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
    PlaceStyle_Option = XmCreateOptionMenu(RowColumn4, "PlaceStyle_Option",
                                           args, n);
    XtManageChild(PlaceStyle_Option);

    XmStringFree(tcs);

    n = 0;
    XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen"));  n++;
    SnapModel_Pulldown = XmCreatePulldownMenu(RowColumn4, 
					      "SnapModel_Pulldown",
                                              args, n);
    SMNone = CreatePushButton(SnapModel_Pulldown, "XmNONE");
    SMToGrid = CreatePushButton(SnapModel_Pulldown, "XmSNAP_TO_GRID");
    SMCenter = CreatePushButton(SnapModel_Pulldown, "XmCENTER");

    XtAddCallback(SMNone, XmNactivateCallback,
                  SnapModelCB, (XtPointer ) &SnapModel[0]);
    XtAddCallback(SMToGrid, XmNactivateCallback,
                  SnapModelCB, (XtPointer ) &SnapModel[1]);
    XtAddCallback(SMCenter, XmNactivateCallback,
                  SnapModelCB, (XtPointer ) &SnapModel[2]);

    tcs = XmStringGenerate("Snap Model              ",
                          XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs); n++;
    XtSetArg(args[n], XmNmenuHistory, SMNone); n++;
    XtSetArg(args[n], XmNsubMenuId, SnapModel_Pulldown); n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
    SnapModel_Option = XmCreateOptionMenu(RowColumn4, "SnapModel_Option",
                                          args, n);
    XtManageChild(SnapModel_Option);

    XmStringFree(tcs);

    n = 0;
    XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen"));  n++;
    ResizeModel_Pulldown = XmCreatePulldownMenu(RowColumn4, 
					      "ResizeModel_Pulldown",
                                              args, n);
    RMMinor = CreatePushButton(ResizeModel_Pulldown, "XmGROW_MINOR");
    RMMajor = CreatePushButton(ResizeModel_Pulldown, "XmGROW_MAJOR");
    RMBalanced = CreatePushButton(ResizeModel_Pulldown, "XmGROW_BALANCED");

    XtAddCallback(RMMinor, XmNactivateCallback,
                  ResizeModelCB, (XtPointer ) &ResizeModel[0]);
    XtAddCallback(RMMajor, XmNactivateCallback,
                  ResizeModelCB, (XtPointer ) &ResizeModel[1]);
    XtAddCallback(RMBalanced, XmNactivateCallback,
                  ResizeModelCB, (XtPointer ) &ResizeModel[2]);

    tcs = XmStringGenerate("Resize Model            ",
                          XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs); n++;
    XtSetArg(args[n], XmNmenuHistory, RMMinor); n++;
    XtSetArg(args[n], XmNsubMenuId, ResizeModel_Pulldown); n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
    ResizeModel_Option = XmCreateOptionMenu(RowColumn4, "ResizeModel_Option",
					    args, n);
    XtManageChild(ResizeModel_Option);

    XmStringFree(tcs);

    XtPopup(PopupShell, XtGrabNone);

}


static Widget
CreatePushButton(Widget parent, char *label)

{

    Widget          widget;
    register int    n;
    Arg             args[MAX_ARGS];
    XmString        tcs;

    tcs = XmStringGenerate(label, XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, 
			   NULL);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs); n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
    widget = XmCreatePushButton(parent, label, args, n);
    XtManageChild(widget);

    XmStringFree(tcs);

    return(widget);

}


static void
SetColumnsCB(Widget wid, XtPointer client_data, XtPointer call_data)
{

	Arg		args[MAX_ARGS];
	Cardinal	n;
	Cardinal	num_columns;


	num_columns = *((Cardinal *)client_data);

	n = 0;
	XtSetArg(args[n], XmNdetailOrderCount, num_columns); n++;
	XtSetValues(Container2, args, n);

}


static void
AutoSelectionCB(Widget wid, XtPointer client_data, XtPointer call_data)
{

	Arg		args[MAX_ARGS];
	Cardinal	n;
	unsigned int	AutoSelection;

	AutoSelection = *((unsigned int *)client_data);

	n = 0;
	XtSetArg(args[n], XmNautomaticSelection, AutoSelection); n++;
	XtSetValues(Container2, args, n);

}


static void
EntryViewTypeCB(Widget wid, XtPointer client_data, XtPointer call_data)
{

	Arg		args[MAX_ARGS];
	Cardinal	n;
	int		ViewType;

	ViewType = *((int *)client_data);

	n = 0;
	XtSetArg(args[n], XmNentryViewType, ViewType); n++;
	XtSetValues(Container2, args, n);

}


static void
LayoutTypeCB(Widget wid, XtPointer client_data, XtPointer call_data)
{

	Arg		args[MAX_ARGS];
	Cardinal	n;
	unsigned int	Layout;

	Layout = *((unsigned int *)client_data);

	n = 0;
	XtSetArg(args[n], XmNlayoutType, Layout); n++;
	XtSetValues(Container2, args, n);

	/* If we set the layoutType to Spatial and the spatialStyle is XmNONE,
	   give a position to the Icon gadgets */

	if (Layout == XmSPATIAL) {

	    unsigned char value;

	    n = 0;
	    XtSetArg(args[n], XmNspatialStyle, &value); n++;
	    XtGetValues(Container2, args, n);

	    if (value == XmNONE) {
		
		n = 0;
		XtSetArg(args[n], XmNx, 100); n++;
		XtSetArg(args[n], XmNy, 100); n++;
		XtSetValues(IconGad1, args, n);

		n = 0;
		XtSetArg(args[n], XmNx, 200); n++;
		XtSetArg(args[n], XmNy, 200); n++;
		XtSetValues(IconGad2, args, n);
	
		n = 0;
		XtSetArg(args[n], XmNx, 300); n++;
		XtSetArg(args[n], XmNy, 100); n++;
		XtSetValues(IconGad3, args, n);

		n = 0;
		XtSetArg(args[n], XmNx, 50); n++;
		XtSetArg(args[n], XmNy, 400); n++;
		XtSetValues(IconGad4, args, n);

	    }
	}
}


static void
PrimaryOwnerShipCB(Widget wid, XtPointer client_data, XtPointer call_data)
{

	Arg		args[MAX_ARGS];
	Cardinal	n;
	unsigned int	primary_ownership;

	primary_ownership = *((unsigned int *)client_data);

	n = 0;
	XtSetArg(args[n], XmNprimaryOwnership, primary_ownership); n++;
	XtSetValues(Container2, args, n);

}


static void
SelectionPolicyCB(Widget wid, XtPointer client_data, XtPointer call_data)
{

	Arg		args[MAX_ARGS];
	Cardinal	n;
	unsigned int	selection_policy;

	selection_policy = *((unsigned int *)client_data);

	n = 0;
	XtSetArg(args[n], XmNselectionPolicy, selection_policy); n++;
	XtSetValues(Container2, args, n);

}


static void
ShowColumnHeadingCB(Widget wid, XtPointer client_data, XtPointer call_data)
{

	Arg		args[MAX_ARGS];
	Cardinal	n;
	Boolean		show_heading;

	show_heading = *((Boolean *)client_data);

	n = 0;
	if (show_heading == False) {
		XtSetArg(args[n], XmNdetailColumnHeading, NULL); n++;
	}
	else {
		XtSetArg(args[n], XmNdetailColumnHeading, ColumnHeadings); n++;
	}
	XtSetValues(Container2, args, n);

}


static void
SelectionTechniqueCB(Widget wid, XtPointer client_data, XtPointer call_data)
{

	Arg		args[MAX_ARGS];
	Cardinal	n;
	unsigned int	selection_technique;

	selection_technique = *((unsigned int *)client_data);

	n = 0;
	XtSetArg(args[n], XmNselectionTechnique, selection_technique); n++;
	XtSetValues(Container2, args, n);

}


static void
IncludeModelCB(Widget wid, XtPointer client_data, XtPointer call_data)
{

	Arg		args[MAX_ARGS];
	Cardinal	n;
	unsigned char	include_model;

	include_model = *((unsigned char *)client_data);

	n = 0;
	XtSetArg(args[n], XmNspatialIncludeModel, include_model); n++;
	XtSetValues(Container2, args, n);

}


static void
OutlineLineStyleCB(Widget wid, XtPointer client_data, XtPointer call_data)
{

        Arg             args[MAX_ARGS];
	Cardinal	n;
	unsigned char	outline_linestyle;

	outline_linestyle = *((unsigned char *)client_data);

	n = 0;
	XtSetArg(args[n], XmNoutlineLineStyle, outline_linestyle); n++;
	XtSetValues(Container2, args, n);

}


static void
OutlineButtonPolicyCB(Widget wid, XtPointer client_data, XtPointer call_data)
{

        Arg             args[MAX_ARGS];
	Cardinal	n;
	unsigned char	outline_buttonpolicy;

	outline_buttonpolicy = *((unsigned char *)client_data);

	n = 0;
	XtSetArg(args[n], XmNoutlineButtonPolicy, outline_buttonpolicy); n++;
	XtSetValues(Container2, args, n);

}

static void
IndentationCB(Widget wid, XtPointer client_data, XtPointer call_data)
{

        Arg             args[MAX_ARGS];
	Cardinal	n;
	int             value;

	/* Get the values from the scale */

	XmScaleGetValue(IndentationScale, &value);

	n = 0;
	XtSetArg(args[n], XmNoutlineIndentation, value); n++;
	XtSetValues(Container2, args, n);

}

static void
ColumnWidthCB(Widget wid, XtPointer client_data, XtPointer call_data)
{

        Arg             args[MAX_ARGS];
	Cardinal	n;
	int             value;

	/* Get the values from the scale */

	XmScaleGetValue(ColumnWidthScale, &value);

	n = 0;
	XtSetArg(args[n], XmNoutlineColumnWidth, value); n++;
	XtSetValues(Container2, args, n);

}


static void
PlaceStyleCB(Widget wid, XtPointer client_data, XtPointer call_data)
{

	Arg		args[MAX_ARGS];
	Cardinal	n;
	unsigned char	place_style;

	place_style = *((unsigned char *)client_data);

	n = 0;
	XtSetArg(args[n], XmNspatialStyle, place_style); n++;
	XtSetValues(Container2, args, n);

}


static void
SnapModelCB(Widget wid, XtPointer client_data, XtPointer call_data)
{

	Arg		args[MAX_ARGS];
	Cardinal	n;
	unsigned char	snap_model;

	snap_model = *((unsigned char *)client_data);

	n = 0;
	XtSetArg(args[n], XmNspatialSnapModel, snap_model); n++;
	XtSetValues(Container2, args, n);

}

static void
ResizeModelCB(Widget wid, XtPointer client_data, XtPointer call_data)
{

	Arg		args[MAX_ARGS];
	Cardinal	n;
	unsigned char	resize_model;

	resize_model = *((unsigned char *)client_data);

	n = 0;
	XtSetArg(args[n], XmNspatialResizeModel, resize_model); n++;
	XtSetValues(Container2, args, n);

}


static void
SelectCB(Widget wid, XtPointer client_data, XtPointer call_data)
{

	XmContainerSelectCallbackStruct	*cbs;
	int	i;


	cbs = (XmContainerSelectCallbackStruct *)call_data;

	switch(cbs->reason) {

		case XmCR_SINGLE_SELECT:
			printf("Reason for SelectionCallback: CR_SINGLE_SELECT\n");
			break;
		case XmCR_BROWSE_SELECT:
			printf("Reason for SelectionCallback: CR_BROWSE_SELECT\n");
			break;
		case XmCR_MULTIPLE_SELECT:
			printf("Reason for SelectionCallback: CR_MULTIPLE_SELECT\n");
			break;
		case XmCR_EXTENDED_SELECT:
			printf("Reason for SelectionCallback: CR_EXTENDED_SELECT\n");
			break;
		case XmCR_DEFAULT_ACTION:
			printf("Reason for SelectionCallback: CR_DEFAULT_ACTION\n");
			break;
		default:
			printf("INVALID Selection reason\n");
			break;

	}

	printf("Number of items selected: %d\n",cbs->selected_item_count);

	printf("Names of items selected: ");
	for (i=0; i < cbs->selected_item_count; i++)
		printf("%s ", XtName(cbs->selected_items[i]));
	printf("\n");

}


static void
OutlineCB(Widget wid, XtPointer client_data, XtPointer call_data)
{

	XmContainerOutlineCallbackStruct *cbs;
	
	cbs = (XmContainerOutlineCallbackStruct *)call_data;
	printf("XmNoutlineChangedCallback ");
	if (cbs->reason == XmCR_COLLAPSED)
		printf("XmCR_COLLAPSED\n");
	if (cbs->reason == XmCR_EXPANDED)
		printf("XmCR_EXPANDED\n");

}


static void
LargeCellWidthCB(Widget wid, XtPointer client_data, XtPointer call_data)
{

        Arg             args[MAX_ARGS];
	Cardinal	n;
	int             value;

	/* Get the values from the scale */

	XmScaleGetValue(LargeCellWidthScale, &value);

	n = 0;
	XtSetArg(args[n], XmNlargeCellWidth, value); n++;
	XtSetValues(Container2, args, n);

}


static void
LargeCellHeightCB(Widget wid, XtPointer client_data, XtPointer call_data)
{

        Arg             args[MAX_ARGS];
	Cardinal	n;
	int             value;

	/* Get the values from the scale */

	XmScaleGetValue(LargeCellHeightScale, &value);

	n = 0;
	XtSetArg(args[n], XmNlargeCellHeight, value); n++;
	XtSetValues(Container2, args, n);

}


static void
SmallCellWidthCB(Widget wid, XtPointer client_data, XtPointer call_data)
{

        Arg             args[MAX_ARGS];
	Cardinal	n;
	int             value;

	/* Get the values from the scale */

	XmScaleGetValue(SmallCellWidthScale, &value);

	n = 0;
	XtSetArg(args[n], XmNsmallCellWidth, value); n++;
	XtSetValues(Container2, args, n);

}


static void
SmallCellHeightCB(Widget wid, XtPointer client_data, XtPointer call_data)
{

        Arg             args[MAX_ARGS];
	Cardinal	n;
	int             value;

	/* Get the values from the scale */

	XmScaleGetValue(SmallCellHeightScale, &value);

	n = 0;
	XtSetArg(args[n], XmNsmallCellHeight, value); n++;
	XtSetValues(Container2, args, n);

}


