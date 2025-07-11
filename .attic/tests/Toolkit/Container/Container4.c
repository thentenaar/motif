/* $XConsortium: Container4.c /main/5 1995/07/15 21:03:40 drk $ */
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

#define	NUM_REORDER 5

#define	NAME_LEN	20
#define	NUM_OBJ		10
#define	NUM_COL		4

static void TestContainerReorder();
static void CreateAnotherContainer();

/*
 * Callbacks
 */
static void PrintChildrenCB(Widget widget, XtPointer client_data, 
			    XtPointer call_data);
static void ConvertCB(Widget widget, XtPointer client_data, 
		      XtPointer call_data);
static void DestinationCB(Widget widget, XtPointer client_data,
			  XtPointer call_data);
static void CutCB(Widget widget, XtPointer client_data, XtPointer call_data);
static void CopyCB(Widget widget, XtPointer client_data, XtPointer call_data);
static void CopyLinkCB(Widget widget, XtPointer client_data, 
		       XtPointer call_data);
static void PasteCB(Widget widget, XtPointer client_data, XtPointer call_data);
static void PasteLinkCB(Widget widget, XtPointer client_data, 
			XtPointer call_data);

/*
 * Forward routines declaration
 */ 
static int SIF_ErrorHandler(Display *display,XErrorEvent *event);
static char *GetSafeAtom(Display *display,Atom a);
static char *GetStringFrom(XtEnum operation);

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
    "Cat Lives",
    "Last object"
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
    "Lemon",
    "Boysenberry"
    };

char	*AgeText[NUM_OBJ] = { 
    "42",
    "10",
    "4",
    "76",
    "2",
    "None of your business","102","23","16",
    "Ask Willard Scott"
    };



XmStringTable	*EntryDetails;
XmStringTable	ColumnHeadings;

Widget  MainW1, menubar1, pulldown1, menu_btn1;
Widget  CutPB1, CopyPB1, CopyLinkPB1, PastePB1, PasteLinkPB1;
Widget	Container4;
Widget	IconGad1,IconGad2,IconGad3,IconGad4,IconGad5;
Widget	IconGad31,IconGad32,IconGad321,IconGad3211,IconGad33;

void
main(int argc, char **argv)
{
    int    		i,j;
    Cardinal		n;
    XmString            Label;
    Arg			args[MAX_ARGS];
    int			test_num;
    char		test_char;
    char		ContainerName[NAME_LEN + 1];
    
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
    sprintf(ContainerName, "Container4%c", test_char);

    /*
     * Create a main window
     */
    n = 0;
    XtSetArg(args[n], XmNborderWidth, 2); n++;
    MainW1 = XmCreateMainWindow(Shell1, "MainW1", args, n);
    XtManageChild(MainW1);

    /*
     * Create a menu bar with a pulldown menu for interacting with the 
     * clipboard
     */

    n = 0;
    menubar1 = XmCreateMenuBar(MainW1, "menubar1", args, n);
    XtManageChild(menubar1);

    n = 0;
    pulldown1 = XmCreatePulldownMenu(menubar1, "pulldown1", args, n);
    
    Label = XmStringCreate("Edit", XmSTRING_DEFAULT_CHARSET);
    n = 0;
    XtSetArg(args[n], XmNlabelString, Label);  n++;
    XtSetArg(args[n], XmNsubMenuId, pulldown1);  n++;
    menu_btn1 = XmCreateCascadeButton(menubar1, "menu_btn1", args, n);
    XtManageChild(menu_btn1);
    XmStringFree(Label);

    n = 0;
    Label = XmStringCreate("Cut", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label);  n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING);  n++;
    CutPB1 = XmCreatePushButtonGadget(pulldown1, "CutPB1", args, n);
    XtManageChild(CutPB1);
    XmStringFree(Label);

    n = 0;
    Label = XmStringCreate("Copy", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label);  n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING);  n++;
    CopyPB1 = XmCreatePushButtonGadget(pulldown1, "CopyPB1", args, n);
    XtManageChild(CopyPB1);
    XmStringFree(Label);

    n = 0;
    Label = XmStringCreate("Copy Link", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label);  n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING);  n++;
    CopyLinkPB1 = XmCreatePushButtonGadget(pulldown1, "CopyLinkPB1", args, n);
    XtManageChild(CopyLinkPB1);
    XmStringFree(Label);

    n = 0;
    Label = XmStringCreate("Paste", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label);  n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING);  n++;
    PastePB1 = XmCreatePushButtonGadget(pulldown1, "PastePB1", args, n);
    XtManageChild(PastePB1);
    XmStringFree(Label);

    n = 0;
    Label = XmStringCreate("Paste Link", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label);  n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING);  n++;
    PasteLinkPB1= XmCreatePushButtonGadget(pulldown1, "PasteLinkPB1", args, n);
    XtManageChild(PasteLinkPB1);
    XmStringFree(Label);
    
    /*
     * Create the container 
     */

    ColumnHeadings = (XmStringTable) XtMalloc(NUM_COL * sizeof(XmString));

    for (i = 0; i < NUM_COL; i++)
	 ColumnHeadings[i] = XmStringGenerate(ColumnHeadingText[i],
					      NULL,
					      XmCHARSET_TEXT,
					      NULL);

    n = 0;
    XtSetArg(args[n], XmNdetailColumnHeading, ColumnHeadings); n++;
    XtSetArg(args[n], XmNdetailColumnHeadingCount, NUM_COL); n++;
    XtSetArg(args[n], XmNdetailOrderCount, NUM_COL-1); n++;
    Container4 = XmCreateContainer(MainW1, ContainerName, args, n);

    XtManageChild(Container4);
    
    XtAddCallback(Container4, XmNdefaultActionCallback, PrintChildrenCB, NULL);
    XtAddCallback(Container4, XmNconvertCallback, ConvertCB, NULL);
    XtAddCallback(Container4, XmNdestinationCallback, DestinationCB, NULL);

    XtAddCallback(CutPB1, XmNactivateCallback, CutCB, (XtPointer) Container4);
    XtAddCallback(CopyPB1, XmNactivateCallback, CopyCB, (XtPointer)Container4);
    XtAddCallback(CopyLinkPB1, XmNactivateCallback, CopyLinkCB, 
		  (XtPointer) Container4);
    XtAddCallback(PastePB1, XmNactivateCallback, PasteCB, 
		  (XtPointer) Container4);
    XtAddCallback(PasteLinkPB1, XmNactivateCallback, PasteLinkCB, 
		  (XtPointer) Container4);

    for (i=0; i<NUM_COL; i++)
      XmStringFree(ColumnHeadings[i]);
    XtFree((XtPointer) ColumnHeadings);

    EntryDetails = (XmStringTable *) XtMalloc(NUM_OBJ * sizeof(XmStringTable));
    
    for (i = 0; i < NUM_OBJ; i++) {
	ColumnHeadings = (XmStringTable)XtMalloc((NUM_COL-1) * sizeof(XmString));
	ColumnHeadings[0] = XmStringGenerate(FullTitleText[i],
					     NULL,
					     XmCHARSET_TEXT,
					     NULL);
	ColumnHeadings[1] = XmStringGenerate(FlavorText[i],
					     NULL,
					     XmCHARSET_TEXT,
					     NULL);
	ColumnHeadings[2] = XmStringGenerate(AgeText[i],
					     NULL,
					     XmCHARSET_TEXT,
					     NULL);
	EntryDetails[i] = ColumnHeadings;
    }


    n = 0;
    XtSetArg(args[n], XmNx, 100); n++;
    XtSetArg(args[n], XmNy, 100); n++;
    XtSetArg(args[n], XmNviewType, XmSMALL_ICON); n++;
    XtSetArg(args[n], XmNdetailCount, NUM_COL-1); n++; 
    XtSetArg(args[n], XmNdetail, EntryDetails[0]); n++;
    IconGad1 = XmCreateIconGadget(Container4, "IconGad1", args, n);
    XtManageChild(IconGad1);
    
    n = 0;
    XtSetArg(args[n], XmNx, 200); n++;
    XtSetArg(args[n], XmNy, 200); n++;
    XtSetArg(args[n], XmNviewType, XmSMALL_ICON); n++;
    XtSetArg(args[n], XmNdetailCount, NUM_COL-1); n++; 
    XtSetArg(args[n], XmNdetail, EntryDetails[1]); n++;
    IconGad2 = XmCreateIconGadget(Container4, "IconGad2", args, n);
    XtManageChild(IconGad2);
    
    n = 0;
    XtSetArg(args[n], XmNx, 300); n++;
    XtSetArg(args[n], XmNy, 100); n++;
    XtSetArg(args[n], XmNviewType, XmLARGE_ICON); n++;
    XtSetArg(args[n], XmNdetailCount, NUM_COL-1); n++; 
    XtSetArg(args[n], XmNdetail, EntryDetails[2]); n++;
    IconGad3 = XmCreateIconGadget(Container4, "IconGad3", args, n);
    XtManageChild(IconGad3);
    
    n = 0;
    XtSetArg(args[n], XmNx, 50); n++;
    XtSetArg(args[n], XmNy, 400); n++;
    XtSetArg(args[n], XmNviewType, XmSMALL_ICON); n++;
    XtSetArg(args[n], XmNdetailCount, NUM_COL-1); n++; 
    XtSetArg(args[n], XmNdetail, EntryDetails[3]); n++;
    IconGad4 = XmCreateIconGadget(Container4, "IconGad4", args, n);
    XtManageChild(IconGad4);
    
    n = 0;
    XtSetArg(args[n], XmNentryParent, IconGad3); n++;
    XtSetArg(args[n], XmNdetailCount, NUM_COL-1); n++; 
    XtSetArg(args[n], XmNviewType, XmSMALL_ICON); n++;
    XtSetArg(args[n], XmNdetail, EntryDetails[4]); n++;
    IconGad31 = XmCreateIconGadget(Container4, "IconGad31", args, n);
    XtManageChild(IconGad31);
    
    n = 0;
    XtSetArg(args[n], XmNentryParent, IconGad3); n++;
    XtSetArg(args[n], XmNviewType, XmSMALL_ICON); n++;
    XtSetArg(args[n], XmNdetailCount, NUM_COL-1); n++; 
    XtSetArg(args[n], XmNdetail, EntryDetails[5]); n++;
    IconGad32 = XmCreateIconGadget(Container4, "IconGad32", args, n);
    XtManageChild(IconGad32);
    
    n = 0;
    XtSetArg(args[n], XmNentryParent, IconGad32); n++;
    XtSetArg(args[n], XmNviewType, XmLARGE_ICON); n++;
    XtSetArg(args[n], XmNdetailCount, NUM_COL-1); n++; 
    XtSetArg(args[n], XmNdetail, EntryDetails[6]); n++;
    IconGad321 = XmCreateIconGadget(Container4, "IconGad321", args, n);
    XtManageChild(IconGad321);
    
    n = 0;
    XtSetArg(args[n], XmNentryParent, IconGad321); n++;
    XtSetArg(args[n], XmNviewType, XmLARGE_ICON); n++;
    XtSetArg(args[n], XmNdetailCount, NUM_COL-1); n++; 
    XtSetArg(args[n], XmNdetail, EntryDetails[7]); n++;
    IconGad3211 = XmCreateIconGadget(Container4, "IconGad3211", args, n);
    XtManageChild(IconGad3211);
    
    n = 0;
    XtSetArg(args[n], XmNentryParent, IconGad3); n++;
    XtSetArg(args[n], XmNviewType, XmSMALL_ICON); n++;
    XtSetArg(args[n], XmNdetailCount, NUM_COL-1); n++; 
    XtSetArg(args[n], XmNdetail, EntryDetails[8]); n++;
    IconGad33 = XmCreateIconGadget(Container4, "IconGad33", args, n);
    XtManageChild(IconGad33);
    
    n = 0;
    XtSetArg(args[n], XmNx, 70); n++;
    XtSetArg(args[n], XmNy, 420); n++;
    XtSetArg(args[n], XmNviewType, XmLARGE_ICON); n++;
    XtSetArg(args[n], XmNdetailCount, NUM_COL-1); n++; 
    XtSetArg(args[n], XmNdetail, EntryDetails[9]); n++;
    IconGad5 = XmCreateIconGadget(Container4, "IconGad5", args, n);
    XtManageChild(IconGad5);
    
    XmMainWindowSetAreas(MainW1, menubar1, NULL, NULL, NULL, Container4);

    for (i = 0; i < NUM_OBJ; i++) {
	
	ColumnHeadings = EntryDetails[i];
	for (j = 0; j < NUM_COL-1; j++)
	    XmStringFree(ColumnHeadings[j]);
	XtFree((XtPointer)ColumnHeadings);
	
    }
    XtFree((XtPointer)EntryDetails);
    
    XtRealizeWidget(Shell1);
    
    CommonPause();

    CommonPause();

    XmContainerRelayout(Container4);
    CommonPause();

    if ((test_num == 1) || (test_num == 2)) {
	TestContainerReorder();
	CommonPause();
    }

    /*
     * Create another Container for UTM testing
     */
    CreateAnotherContainer();
    CommonPause();
    CommonPause();
    CommonPause();
    CommonPause();
    CommonPause();
    CommonPause();
    CommonPause();

    CommonPause();
    
    XtAppMainLoop(app_context);
    
}

static void TestContainerReorder()
{
   int        num_widgets;
   WidgetList widgets;

   widgets = (WidgetList) XtMalloc(NUM_REORDER * sizeof(Widget));

   num_widgets = 0;
   widgets[num_widgets] = IconGad5;

   num_widgets++;
   widgets[num_widgets] = IconGad3;

   num_widgets++;
   widgets[num_widgets] = IconGad1;

   num_widgets++;
   widgets[num_widgets] = IconGad2;

   num_widgets++;
   widgets[num_widgets] = IconGad4;

   XmContainerReorder(Container4, widgets, num_widgets + 1);
   XmContainerRelayout(Container4);

   XtFree((XtPointer) widgets);
}

static void CreateAnotherContainer()
{
    Widget        PopupShell;
    Widget        MainW2, menubar2, pulldown2, menu_btn2;
    Widget        CutPB2, CopyPB2, CopyLinkPB2, PastePB2, PasteLinkPB2;
    Widget        OtherContainer4;
    Widget        IconGad6, IconGad7, IconGad8, IconGad9, IconGad10;
    Cardinal      n;
    XmString      Label;
    Arg	          args[MAX_ARGS];

    /*
     * Create a Dialog Shell
     */
    n = 0;
    XtSetArg(args[n], XtNgeometry, "=375x375+400+300"); n++;
    XtSetArg(args[n], XtNallowShellResize, True); n++;
    PopupShell = XtCreatePopupShell("OtherContainer", 
				    topLevelShellWidgetClass, Shell1,
				    args, n);

    /*
     * Create a main window
     */
    n = 0;
    XtSetArg(args[n], XmNborderWidth, 2); n++;
    MainW2 = XmCreateMainWindow(PopupShell, "MainW2", args, n);
    XtManageChild(MainW2);

    /*
     * Create a menu bar with a pulldown menu for interacting with the 
     * clipboard
     */

    n = 0;
    menubar2 = XmCreateMenuBar(MainW2, "menubar2", args, n);
    XtManageChild(menubar2);

    n = 0;
    pulldown2 = XmCreatePulldownMenu(menubar2, "pulldown2", args, n);
    
    Label = XmStringCreate("Edit", XmSTRING_DEFAULT_CHARSET);
    n = 0;
    XtSetArg(args[n], XmNlabelString, Label);  n++;
    XtSetArg(args[n], XmNsubMenuId, pulldown2);  n++;
    menu_btn2 = XmCreateCascadeButton(menubar2, "menu_btn2", args, n);
    XtManageChild(menu_btn2);
    XmStringFree(Label);

    n = 0;
    Label = XmStringCreate("Cut", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label);  n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING);  n++;
    CutPB2 = XmCreatePushButtonGadget(pulldown2, "CutPB2", args, n);
    XtManageChild(CutPB2);
    XmStringFree(Label);

    n = 0;
    Label = XmStringCreate("Copy", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label);  n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING);  n++;
    CopyPB2 = XmCreatePushButtonGadget(pulldown2, "CopyPB2", args, n);
    XtManageChild(CopyPB2);
    XmStringFree(Label);

    n = 0;
    Label = XmStringCreate("Copy Link", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label);  n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING);  n++;
    CopyLinkPB2 = XmCreatePushButtonGadget(pulldown2, "CopyLinkPB2", args, n);
    XtManageChild(CopyLinkPB2);
    XmStringFree(Label);

    n = 0;
    Label = XmStringCreate("Paste", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label);  n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING);  n++;
    PastePB2 = XmCreatePushButtonGadget(pulldown2, "PastePB2", args, n);
    XtManageChild(PastePB2);
    XmStringFree(Label);

    n = 0;
    Label = XmStringCreate("Paste Link", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label);  n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING);  n++;
    PasteLinkPB2= XmCreatePushButtonGadget(pulldown2, "PasteLinkPB2", args, n);
    XtManageChild(PasteLinkPB2);
    XmStringFree(Label);
    
    /*
     * Create the container 
     */
    n = 0;
    XtSetArg(args[n], XmNwidth, 450); n++;
    XtSetArg(args[n], XmNheight, 450); n++;
    XtSetArg(args[n], XmNentryViewType, XmANY_ICON); n++;
    XtSetArg(args[n], XmNlayoutType, XmSPATIAL); n++;
    XtSetArg(args[n], XmNspatialStyle, XmNONE); n++;
    XtSetArg(args[n], XmNautomaticSelection, XmNO_AUTO_SELECT); n++;
    OtherContainer4 = XmCreateContainer(MainW2, "OtherContainer", args, n);
    XtManageChild(OtherContainer4);
    
    XtAddCallback(OtherContainer4, XmNconvertCallback, ConvertCB, NULL);
    XtAddCallback(OtherContainer4, XmNdestinationCallback, DestinationCB, 
		  NULL);

    XtAddCallback(CutPB2, XmNactivateCallback, CutCB, 
		  (XtPointer) OtherContainer4);
    XtAddCallback(CopyPB2, XmNactivateCallback, CopyCB, 
		  (XtPointer) OtherContainer4);
    XtAddCallback(CopyLinkPB2, XmNactivateCallback, CopyLinkCB,
		  (XtPointer) OtherContainer4);
    XtAddCallback(PastePB2, XmNactivateCallback, PasteCB, 
		  (XtPointer) OtherContainer4);
    XtAddCallback(PasteLinkPB2, XmNactivateCallback, PasteLinkCB, 
		  (XtPointer) OtherContainer4);
    n = 0;
    XtSetArg(args[n], XmNx, 50); n++;
    XtSetArg(args[n], XmNy, 50); n++;
    XtSetArg(args[n], XmNviewType, XmSMALL_ICON); n++;
    IconGad6 = XmCreateIconGadget(OtherContainer4, "IconGad6", args, n);
    XtManageChild(IconGad6);
    
    n = 0;
    XtSetArg(args[n], XmNx, 150); n++;
    XtSetArg(args[n], XmNy, 150); n++;
    XtSetArg(args[n], XmNviewType, XmLARGE_ICON); n++;
    IconGad7 = XmCreateIconGadget(OtherContainer4, "IconGad7", args, n);
    XtManageChild(IconGad7);
    
    n = 0;
    XtSetArg(args[n], XmNx, 50); n++;
    XtSetArg(args[n], XmNy, 300); n++;
    XtSetArg(args[n], XmNviewType, XmSMALL_ICON); n++;
    IconGad8 = XmCreateIconGadget(OtherContainer4, "IconGad8", args, n);
    XtManageChild(IconGad8);
    
    n = 0;
    XtSetArg(args[n], XmNx, 300); n++;
    XtSetArg(args[n], XmNy, 300); n++;
    XtSetArg(args[n], XmNviewType, XmSMALL_ICON); n++;
    IconGad9 = XmCreateIconGadget(OtherContainer4, "IconGad9", args, n);
    XtManageChild(IconGad9);
    
    n = 0;
    XtSetArg(args[n], XmNx, 200); n++;
    XtSetArg(args[n], XmNy, 50); n++;
    XtSetArg(args[n], XmNviewType, XmLARGE_ICON); n++;
    IconGad10 = XmCreateIconGadget(OtherContainer4, "IconGad10", args, n);
    XtManageChild(IconGad10);
    
    XmMainWindowSetAreas(MainW2, menubar2, NULL, NULL, NULL, OtherContainer4);

    XtPopup(PopupShell, XtGrabNone);
}

static void
PrintChildrenCB(Widget widget,XtPointer client_data, XtPointer call_data)
{
   XmContainerSelectCallbackStruct *cbs;
   int i, j, num_children;
   WidgetList children;

   cbs = (XmContainerSelectCallbackStruct *) call_data;

   if ((cbs->reason != XmCR_DEFAULT_ACTION) || (cbs->selected_item_count == 0))
     return;

   for (i=0; i < cbs->selected_item_count; i++) {
      num_children = XmContainerGetItemChildren (widget, 
						 cbs->selected_items[i],
						 &children);
      printf("Number of children for %s : %d\n", 
	     XtName(cbs->selected_items[i]), num_children);

      if (num_children != 0) {
	 printf("Names of children: ");
	 for (j=0; j<num_children; j++)
	   printf("%s ", XtName(children[j]));
	 printf("\n");
	 XtFree((XtPointer) children);
      }
   };

   printf ("\n");
}

static void
ConvertCB(Widget widget,XtPointer client_data, XtPointer call_data)
{
   XmConvertCallbackStruct *cbs;
   char *selection_atom_name, *target_atom_name, *atom_name;

   cbs = (XmConvertCallbackStruct *) call_data;

   selection_atom_name = GetSafeAtom(XtDisplay(widget), cbs->selection);
   if (selection_atom_name != NULL)
	atom_name = selection_atom_name;
   else
	atom_name = "Illegal atom";

   printf("Convert Callback called with: \n");
   printf("        selection = %s\n", atom_name);

   target_atom_name = GetSafeAtom(XtDisplay(widget), cbs->target);
   if (target_atom_name != NULL)
	atom_name = target_atom_name;
   else
	atom_name = "Illegal atom";

   printf("        target    = %s\n", atom_name);

   /* Only print the value of parm (which is the operation) when the 
      selection is CLIPBOARD */

   if (selection_atom_name != NULL 
       && strcmp(selection_atom_name, "CLIPBOARD") == 0 
       && cbs->parm != (XtPointer) 0) 
     printf("        parm      = %s\n", 
	    GetStringFrom((XtEnum) ((int) cbs->parm)));

   printf("\n");

   free(selection_atom_name);
   free(target_atom_name);

}

static void
DestinationCB(Widget widget,XtPointer client_data, XtPointer call_data)
{
   XmDestinationCallbackStruct *cbs;
   char *selection_atom_name, *atom_name;

   cbs = (XmDestinationCallbackStruct *) call_data;

   selection_atom_name = GetSafeAtom(XtDisplay(widget), cbs->selection);
   if (selection_atom_name != NULL)
	atom_name = selection_atom_name;
   else
	atom_name = "Illegal atom";

   cbs = (XmDestinationCallbackStruct *) call_data;

   printf("Destination Callback called with: \n");
   printf("        selection = %s\n", atom_name);
   printf("        operation = %s\n\n", GetStringFrom(cbs->operation));

   free(selection_atom_name);
}


static void
CutCB(Widget widget,XtPointer client_data, XtPointer call_data)
{
   Boolean status;
   Widget  container;

   container = (Widget) client_data;

   status = XmContainerCut(container, 
			   XtLastTimestampProcessed(XtDisplay(container)));

   printf("Return status of XmContainerCut function: %d\n\n", (int) status);
}

static void
CopyCB(Widget widget,XtPointer client_data, XtPointer call_data)
{
   Boolean status;
   Widget  container;

   container = (Widget) client_data;

   status = XmContainerCopy(container, 
			    XtLastTimestampProcessed(XtDisplay(container)));

   printf("Return status of XmContainerCopy function: %d\n\n", (int) status);

}

static void
CopyLinkCB(Widget widget, XtPointer client_data, XtPointer call_data)
{
   Boolean status;
   Widget  container;

   container = (Widget) client_data;

   status = XmContainerCopyLink(container, 
			XtLastTimestampProcessed(XtDisplay(container)));

   printf("Return status of XmContainerCopyLink function: %d\n\n", 
	  (int) status);

}

static void
PasteCB(Widget widget,XtPointer client_data, XtPointer call_data)
{
   Boolean status;
   Widget  container;

   container = (Widget) client_data;

   status = XmContainerPaste(container); 

   printf("Return status of XmContainerPaste function: %d\n\n", (int) status);

}

static void
PasteLinkCB(Widget widget,XtPointer client_data, XtPointer call_data)
{
   Boolean status;
   Widget  container;

   container = (Widget) client_data;

   status = XmContainerPasteLink(container); 

   printf("Return status of XmContainerPasteLink function: %d\n\n", 
	  (int) status);

}

/* Error handler for XGetAtomName */

static int SIF_ErrorFlag;
 
static int
SIF_ErrorHandler(Display *display,XErrorEvent *event)
{
  SIF_ErrorFlag = event -> type;

  return 0;
}

static char *
GetSafeAtom(Display *display,Atom a)
{
  XErrorHandler old_Handler;
  char *returnvalue;

  /* Setup error proc and reset error flag */
  old_Handler = XSetErrorHandler((XErrorHandler) SIF_ErrorHandler);
  SIF_ErrorFlag = 0;

  returnvalue = XGetAtomName(display, a);

  XSetErrorHandler(old_Handler);

  if (SIF_ErrorFlag == 0)
    return(returnvalue);
  else
    return(NULL);
}

static char *
GetStringFrom(XtEnum operation)
{
  char *returnvalue;

  switch(operation) {
        case XmMOVE:
           returnvalue = "XmMOVE";
	   break;
	case XmCOPY: 
	   returnvalue = "XmCOPY";
	   break;
        case XmLINK:
	   returnvalue = "XmLINK";
	   break;
        case XmOTHER:
	   returnvalue = "XmOTHER";
	   break;
	default:
	   returnvalue = "Bad operation";
	}

  return(returnvalue);
}
