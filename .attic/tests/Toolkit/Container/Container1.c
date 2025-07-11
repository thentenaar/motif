/* $XConsortium: Container1.c /main/5 1995/07/15 21:03:23 drk $ */
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
#define	NUM_OBJ		10
#define	NUM_COL		4

void	DefaultActionCB(Widget wid, XtPointer client_data, XtPointer call_data);
void	SelectCB(Widget wid, XtPointer client_data, XtPointer call_data);
void	OutlineCB(Widget wid, XtPointer client_data, XtPointer call_data);

Widget	IconGad1,IconGad2,IconGad3,IconGad4;
Widget	IconGad31,IconGad32,IconGad321,IconGad3211,IconGad33;
Widget	IconGad5;
Widget  Container1;

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


void
main(int argc, char **argv)
{

	XmStringTable	        ColumnHeadings;
	int			i,j;
	Cardinal		n;
	Arg			args[MAX_ARGS];
	int			test_num;
	char			test_char;
	char			ContainerName[NAME_LEN + 1];
	Pixmap			CollapsedStatePixmap, ExpandedStatePixmap;
	WidgetList		SelectedObjects;
	unsigned int	        SelectedObjectCount;

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
		else if (strcmp(UserData, "e") == 0)
			test_num = 5;
		else if (strcmp(UserData, "f") == 0)
			test_num = 6;
		else if (strcmp(UserData, "g") == 0)
			test_num = 7;
		else if (strcmp(UserData, "h") == 0)
			test_num = 8;
		else if (strcmp(UserData, "j") == 0)
			test_num = 9;
		else if (strcmp(UserData, "k") == 0)
			test_num = 10;
		else if (strcmp(UserData, "l") == 0)
			test_num = 11;
		else if (strcmp(UserData, "m") == 0)
			test_num = 12;
		else if (strcmp(UserData, "n") == 0)
			test_num = 13;
		else if (strcmp(UserData, "z") == 0)
			test_num = 26;

		test_char = *UserData;

		free(UserData);

	}
	sprintf(ContainerName, "Container1%c", test_char);

	CollapsedStatePixmap = XCreatePixmapFromBitmapData(display, rootWindow,
				  collapsedState_bits, collapsedState_width, 
				  collapsedState_height, CommonGetColor("black"),
				  CommonGetColor("white"),
				  XDefaultDepth(display, XDefaultScreen(display)));

	ExpandedStatePixmap = XCreatePixmapFromBitmapData(display, rootWindow,
				  expandedState_bits, expandedState_width, 
				  expandedState_height, CommonGetColor("white"),
				  CommonGetColor("black"),
				  XDefaultDepth(display, XDefaultScreen(display)));


	ColumnHeadings = (XmStringTable) XtMalloc(NUM_COL * sizeof(XmString));

	for (i = 0; i < NUM_COL; i++)
	     ColumnHeadings[i] = XmStringGenerate(ColumnHeadingText[i],
						  NULL,
						  XmCHARSET_TEXT,
						  NULL);

	n = 0;

	/* Don't want to set the following resources for Container1l test */

	if (test_num != 11) {
	    XtSetArg(args[n], XmNdetailColumnHeading, ColumnHeadings); n++;
	    XtSetArg(args[n], XmNdetailColumnHeadingCount, NUM_COL); n++;
	}

	XtSetArg(args[n], XmNcollapsedStatePixmap, CollapsedStatePixmap); n++;
	XtSetArg(args[n], XmNexpandedStatePixmap, ExpandedStatePixmap); n++;
	Container1 = XmCreateContainer(Shell1, ContainerName, args, n);
	XtManageChild(Container1);

	XtAddCallback(Container1, XmNdefaultActionCallback, DefaultActionCB, NULL);
	XtAddCallback(Container1, XmNselectionCallback, SelectCB, NULL);
	XtAddCallback(Container1, XmNoutlineChangedCallback, OutlineCB, NULL);

	for (i = 0; i < NUM_COL; i++)
		XmStringFree(ColumnHeadings[i]);
	XtFree((XtPointer)ColumnHeadings);

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

	/* Don't want to set the following resource for Container1m test*/
	if (test_num != 12)
	    XtSetArg(args[n], XmNvisualEmphasis, XmSELECTED); n++;

	XtSetArg(args[n], XmNx, 100); n++;
	XtSetArg(args[n], XmNy, 100); n++;
	XtSetArg(args[n], XmNviewType, XmSMALL_ICON); n++;
	XtSetArg(args[n], XmNdetailCount, NUM_COL-1); n++;
	XtSetArg(args[n], XmNdetail, EntryDetails[0]); n++;
	IconGad1 = XmCreateIconGadget(Container1, "IconGad1", args, n);
	XtManageChild(IconGad1);

	n = 0;
	XtSetArg(args[n], XmNx, 200); n++;
	XtSetArg(args[n], XmNy, 200); n++;
	XtSetArg(args[n], XmNwidth, 50); n++;
	XtSetArg(args[n], XmNviewType, XmSMALL_ICON); n++;
	XtSetArg(args[n], XmNdetailCount, NUM_COL-1); n++;
	XtSetArg(args[n], XmNdetail, EntryDetails[1]); n++;
	IconGad2 = XmCreateIconGadget(Container1, "IconGad2", args, n);
	XtManageChild(IconGad2);

	n = 0;
	XtSetArg(args[n], XmNx, 300); n++;
	XtSetArg(args[n], XmNy, 100); n++;
	XtSetArg(args[n], XmNviewType, XmLARGE_ICON); n++;
	XtSetArg(args[n], XmNdetailCount, NUM_COL-1); n++;
	XtSetArg(args[n], XmNdetail, EntryDetails[2]); n++;
	IconGad3 = XmCreateIconGadget(Container1, "IconGad3", args, n);
	XtManageChild(IconGad3);

	n = 0;
	XtSetArg(args[n], XmNx, 50); n++;
	XtSetArg(args[n], XmNy, 400); n++;
	XtSetArg(args[n], XmNdetailCount, NUM_COL-1); n++;
	XtSetArg(args[n], XmNdetail, EntryDetails[3]); n++;
	XtSetArg(args[n], XmNviewType, XmSMALL_ICON); n++;
	IconGad4 = XmCreateIconGadget(Container1, "IconGad4", args, n);
	XtManageChild(IconGad4);

	n = 0;
	XtSetArg(args[n], XmNentryParent, IconGad3); n++;
	XtSetArg(args[n], XmNdetailCount, NUM_COL-1); n++;
	XtSetArg(args[n], XmNdetail, EntryDetails[4]); n++;
	XtSetArg(args[n], XmNviewType, XmSMALL_ICON); n++;
	IconGad31 = XmCreateIconGadget(Container1, "IconGad31", args, n);
	XtManageChild(IconGad31);

	n = 0;
	XtSetArg(args[n], XmNentryParent, IconGad3); n++;
	XtSetArg(args[n], XmNdetailCount, NUM_COL-1); n++;
	XtSetArg(args[n], XmNdetail, EntryDetails[5]); n++;
	XtSetArg(args[n], XmNviewType, XmSMALL_ICON); n++;
	IconGad32 = XmCreateIconGadget(Container1, "IconGad32", args, n);
	XtManageChild(IconGad32);

	n = 0;
	XtSetArg(args[n], XmNentryParent, IconGad32); n++;
	XtSetArg(args[n], XmNdetailCount, NUM_COL-1); n++;
	XtSetArg(args[n], XmNdetail, EntryDetails[6]); n++;
	XtSetArg(args[n], XmNviewType, XmLARGE_ICON); n++;
	IconGad321 = XmCreateIconGadget(Container1, "IconGad321", args, n);
	XtManageChild(IconGad321);

	n = 0;
	XtSetArg(args[n], XmNentryParent, IconGad321); n++;
	XtSetArg(args[n], XmNdetailCount, NUM_COL-1); n++;
	XtSetArg(args[n], XmNdetail, EntryDetails[7]); n++;
	XtSetArg(args[n], XmNviewType, XmSMALL_ICON); n++;
	IconGad3211 = XmCreateIconGadget(Container1, "IconGad3211", args, n);
	XtManageChild(IconGad3211);

	n = 0;
	XtSetArg(args[n], XmNentryParent, IconGad3); n++;
	XtSetArg(args[n], XmNviewType, XmSMALL_ICON); n++;
	XtSetArg(args[n], XmNdetailCount, NUM_COL-1); n++;
	XtSetArg(args[n], XmNdetail, EntryDetails[8]); n++;
	IconGad33 = XmCreateIconGadget(Container1, "IconGad33", args, n);
	XtManageChild(IconGad33);

/****
     Create only widgets when needed, because automation is realizing and
     managing all created widgets.
     When we use R6, try that again.

	n = 0;
	XtSetArg(args[n], XmNx, 70); n++;
	XtSetArg(args[n], XmNy, 420); n++;
	XtSetArg(args[n], XmNviewType, XmLARGE_ICON); n++;
	XtSetArg(args[n], XmNdetailCount, NUM_COL-1); n++;
	XtSetArg(args[n], XmNdetail, EntryDetails[9]); n++;
	IconGad5 = XmCreateIconGadget(Container1, "IconGad5", args, n);

	for (i = 0; i < NUM_OBJ; i++) {
****/

	IconGad5 = (Widget) 0;
	

	for (i = 0; i < NUM_OBJ-1; i++) {

		ColumnHeadings = EntryDetails[i];
		for (j = 0; j < NUM_COL-1; j++)
			XmStringFree(ColumnHeadings[j]);
		XtFree((XtPointer)ColumnHeadings);

	}
/****
	XtFree((XtPointer)EntryDetails);
****/
	XtRealizeWidget(Shell1);

	CommonPause();
	CommonPause();

	if (test_num == 1) {

		CommonPause();

	}

	if (test_num == 2) {

	   CommonPause();
	   CommonPause();
	   CommonPause();
	   CommonPause();
	   CommonPause();
	   CommonPause();
	   CommonPause();

	   n = 0;
	   XtSetArg(args[n], XmNselectedObjectCount, &SelectedObjectCount); 
	   n++;
	   XtSetArg(args[n], XmNselectedObjects, &SelectedObjects); n++;
	   XtGetValues(Container1, args, n);
	   printf("SelectedObjectCount from GetValues: %d\n", 
		  SelectedObjectCount);
	   printf("SelectedObjects from GetValues: ");
	   for (i = 0; i < SelectedObjectCount; i++)
	     printf("%s ", XtName(SelectedObjects[i]));
	   printf("\n");

	   CommonPause();

	}

	if (test_num == 3) {

	    CommonPause();
	    CommonPause();
	    CommonPause();
	    CommonPause();
	}

	if (test_num == 4) {

	    CommonPause();

	}
	if (test_num == 5) {

	    CommonPause();
	    CommonPause();
	    CommonPause();
	    CommonPause();

	}
	if (test_num == 6) {

	    CommonPause();
	    CommonPause();
	    CommonPause();
	    CommonPause();

	}
	if (test_num == 7) {

	    CommonPause();
	    CommonPause();
	    CommonPause();

	}
	if (test_num == 8) {

	    CommonPause();
	    CommonPause();

	}
	if (test_num == 9) {
	   
	    CommonPause();
	    CommonPause();

	}
	if (test_num == 10) {

	    CommonPause();
	    CommonPause();

	}
	if (test_num == 11) {

	   CommonPause();
	   CommonPause();
	   CommonPause();

	}

	if (test_num == 12) {

	   CommonPause();
	   CommonPause();
	   CommonPause();

	}

	if (test_num == 26) {

		n = 0;
		XtSetArg(args[n], XmNentryViewType, XmLARGE_ICON); n++;
		XtSetArg(args[n], XmNlayoutType, XmOUTLINE); n++;
		XtSetValues(Container1, args, n);

		CommonPause();
		CommonPause();
		CommonPause();

	}

	CommonPause();

	XtAppMainLoop(app_context);

}


void
DefaultActionCB(Widget wid,XtPointer client_data, XtPointer call_data)
{
/****/
	XmStringTable	        ColumnHeadings;
	Cardinal		n;
	int                     j;
	Arg			args[MAX_ARGS];
/****/	
	XmContainerSelectCallbackStruct	*cbs;
	
	cbs = (XmContainerSelectCallbackStruct *)call_data;
	printf("\nXmNdefaultActionCallback called\n\n");

/****
     Create only widgets when needed, because automation is realizing and
     managing all created widgets.
     When we use R6, remove that again.
****/

	if (IconGad5 == (Widget) 0) {
		
	    n = 0;
	    XtSetArg(args[n], XmNx, 70); n++;
	    XtSetArg(args[n], XmNy, 420); n++;
	    XtSetArg(args[n], XmNviewType, XmLARGE_ICON); n++;
	    XtSetArg(args[n], XmNdetailCount, NUM_COL-1); n++;
	    XtSetArg(args[n], XmNdetail, EntryDetails[9]); n++;
	    IconGad5 = XmCreateIconGadget(Container1, "IconGad5", args, n);

	    ColumnHeadings = EntryDetails[9];
	    for (j = 0; j < NUM_COL-1; j++)
		XmStringFree(ColumnHeadings[j]);
	    XtFree((XtPointer)ColumnHeadings);

	    XtFree((XtPointer)EntryDetails);
	}
	

/****/

	XtManageChild(IconGad5);

}


void
SelectCB(Widget wid,XtPointer client_data, XtPointer call_data)
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


void
OutlineCB(Widget wid,XtPointer client_data, XtPointer call_data)
{

	XmContainerOutlineCallbackStruct *cbs;
	
	cbs = (XmContainerOutlineCallbackStruct *)call_data;
	printf("XmNoutlineChangedCallback ");
	if (cbs->reason == XmCR_COLLAPSED)
		printf("XmCR_COLLAPSED\n");
	if (cbs->reason == XmCR_EXPANDED)
		printf("XmCR_EXPANDED\n");

}
