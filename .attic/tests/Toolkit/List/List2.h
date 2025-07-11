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
/*   $XConsortium: List2.h /main/9 1995/07/13 18:17:08 drk $ */


#define NITEMS 28

char *Istrings[] = {
    "1) DrawnButton",           "2) Label",
    "3) List",                  "4) PushButton",
    "5) Separator",             "6) ArrowButton",
    "7) ScrollBar",             "8) Text",
    "9) ScrolledText",          "10) DrawnButton",
    "11) DrawingArea",          "12) MainWindow",
    "13) ScrolledWindow",       "14) Scale",
    "15) Command",              "16) FileSelectionBox",
    "17) MessageBox",           "18) QuestionDialog",
    "19) BulletinBoard",        "20) Form",
    "21) RowColumn",            "22) ScrolledList",
    "23) ToggleButton",         "24) ToggleButtonGadget",
    "25) PushButtonGadget",     "26) ArrowButtonGadget",
    "27) TextField",            "28) PanedWindow"
};

XmString   ItemList[NITEMS];

/*  Global Variables  */

Widget    	List1;
Widget    	Label_List1, Label_Selected_List;
Widget	  	Form;
Widget	  	Dirs_Option, Fonts_Option, SPolicy_Option;
Widget	  	LSPolicy_Option, SBDPolicy_Option;
Widget    	VIC_Scale, MH_Scale, MW_Scale;
Widget	  	Space_Scale, TIP_Scale;
Widget 	  	Browse, Single, Multiple, Extended;
Widget	  	Constant, Variable, VariableP, Possible;
Widget	  	Static, AsNeeded;
Widget	  	Fixed, Variable, NineByFiveteen, helvetica;
Widget		LtoR, RtoL;
Widget	  	Dirs_Pulldown, Fonts_Pulldown, SPolicy_Pulldown, LSPolicy_Pulldown;
Widget   	SBDPolicy_Pulldown;
Widget	  	Selected_List;
Widget	  	Label_Selected_Count, Label_Select_Type, Label_Item_Pos;
Widget	  	Label_Item, Label_Reason, Label_Item_Count, Label_RowCol;
Widget	  	Label_Selected_Items, Label_Resources, Label_Text;
Widget	  	RowCol1, RowCol2, Destroy_Push;
Widget		Frame1, Frame2;
Widget	  	ToggleB, ToggleB1;
Widget		Text1;

int	  	scroll_display_policy = XmAS_NEEDED;
int       	list_policy = XmVARIABLE;
int      	selection_policy = XmBROWSE_SELECT;
int 	  	visible_item_count = NITEMS - 10;
int       	margin_width = 2;
int       	margin_height = 2;
int 	  	top_item_pos = 1;
int       	spacing = 5;

XmFontList 	font_list;

Boolean		auto_selection;
Boolean		sensitive;

/*  Private Functions  */


static void  	BrSiDeSelCB(Widget w, XtPointer client_data,
					XtPointer call_data);
static void  	ExtSelCB(Widget w, XtPointer client_data,
				 	XtPointer call_data);
static void  	MulSelCB(Widget w, XtPointer client_data,
				 	XtPointer call_data);
static void  	ChangeFontCB(Widget w, XtPointer client_data,
				     	XtPointer call_data);
static void  	ChangeDirCB(Widget w, XtPointer client_data,
				     	XtPointer call_data);
static void  	CHSPolicyCB(Widget w, XtPointer client_data,
				    	XtPointer call_data);
static void  	CHLSPolicyCB(Widget w, XtPointer client_data,
				     	XtPointer call_data);
static void  	CHSBDPolicyCB(Widget w, XtPointer client_data,
				      	XtPointer call_data);
static void  	ChangeVICCB(Widget w, XtPointer client_data,
				    	XtPointer call_data);
static void  	ChangeMHCB(Widget w, XtPointer client_data,
				   	XtPointer call_data);
static void  	ChangeMWCB(Widget w, XtPointer client_data,
				   	XtPointer call_data);
static void  	ChangeSpaceCB(Widget w, XtPointer client_data,
				      	XtPointer call_data);
static void  	ChangeTIPCB(Widget w, XtPointer client_data,
				    	XtPointer call_data);
static void  	Destroy_and_Create(Widget w, XtPointer client_data,
					XtPointer call_data);
static void  	ToggleCB(Widget w, XtPointer client_data,
				 	XtPointer call_data);
static void  	ToggleB1CB(Widget w, XtPointer client_data,
				 	XtPointer call_data);


static void	       	Update_selected_items(XmString *item_list,
					int item_count,
					Boolean single_or_browse,
					int *selected_pos);
static void 	       	CreateOptions();
static void 	       	CreateCallbackInfo();
static void	  	Init_Resources();

static void 	       	ChangeLabel(Widget w, char string[50]);

static Widget          	CreatePushButton(Widget parent, char *label);

