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
/*   $XConsortium: List10.h /main/8 1995/07/13 18:15:17 drk $ */


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
XmString   my_selected[5];

/*  Global Variables  */

Widget    	List1;
Widget    	Label_List1, Label_Selected_List;
Widget	  	Form;
Widget		TextF_AddSL, TextF_AddIL;
Widget		Label_AddSL, Label_AddIL;
Widget 	  	Browse, Single, Multiple, Extended;
Widget	  	Constant, Variable, VariableP, Possible;
Widget	  	Selected_List, Selected_ListSV;
Widget	  	Label_Selected_Count, Label_Select_Type, Label_Item_Pos;
Widget	  	Label_Item, Label_Reason, Label_Item_Count, Label_RowCol;
Widget	  	Label_Selected_Items, Label_Selected, Label_Resources;
Widget	  	RowCol1, RowCol2, Destroy_Push;
Widget		Frame1, Frame2;
Widget		Selected_List;
Widget		Toggle_Selected;
Widget	  	PushB_Update, PushB_GetSelected, RadioBox;

XmFontList 	font_list;

Boolean		Add_Unselected = False;

/*  Private Functions  */


static void  	BrSiDeSelCB(Widget w, caddr_t client_data, caddr_t call_data);
static void  	ExtSelCB(Widget w, caddr_t client_data, caddr_t call_data);
static void  	MulSelCB(Widget w, caddr_t client_data, caddr_t call_data);
static void  	ChangeFontCB(Widget w, caddr_t client_data, caddr_t call_data);
static void  	CHSPolicyCB(Widget w, unsigned char client_data, 
			    caddr_t call_data);
static void  	UpdateCB(Widget w, caddr_t client_data, caddr_t call_data);
static void  	SelectedCB(Widget w, caddr_t client_data, caddr_t call_data);
static void  	AddSLCB(Widget w, caddr_t client_data, caddr_t call_data);
static void  	AddILCB(Widget w, caddr_t client_data, caddr_t call_data);
static void  	ToggleCB(Widget w, caddr_t client_data, caddr_t call_data);
static void	Update_selected_items(XmString *item_list, int item_count,
				       Boolean single_or_browse,
				       int *selected_pos);
static void 	CreateOptions();
static void 	CreateCallbackInfo();
static void 	ChangeLabel(Widget w, char string[50]);
static Widget   CreateToggleButton(Widget parent, char *label);
static Widget   CreatePushButton(Widget parent, char *label);

