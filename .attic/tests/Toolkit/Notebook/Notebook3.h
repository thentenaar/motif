/* $XConsortium: Notebook3.h /main/5 1995/07/15 21:07:33 drk $ */
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



/* This is the header file for the test Notebook1.c */



#define TRUE 1
#define FALSE 0
#define NUM_PAGE 30
#define NUM_MAJOR_TAB 60
#define NUM_MINOR_TAB 60
#define NUM_STATUS_AREA 60
#define NUM_PAGE_SCROLLER 60



/* This is an enumeration of values for the "client_data" parameter to the
   routine SetNotebookValues. */

enum             { CURRENT_PAGE_NUMBER = 0,
		   FIRST_PAGE_NUMBER,
		   LAST_PAGE_NUMBER,
		   BACK_PAGE_NUMBER,
		   BACK_PAGE_SIZE,
		   BINDING_WIDTH,
		   INNER_MARGIN_WIDTH,
		   INNER_MARGIN_HEIGHT,
		   MAJOR_TAB_SPACING,
		   MINOR_TAB_SPACING,
		   ORIENTATION,
		   BACK_PAGE_PLACEMENT,
		   BINDING_TYPE,
		   BACK_PAGE_FOREGROUND,
		   BACK_PAGE_BACKGROUND,
		   FRAME_BACKGROUND,
		   FRAME_SHADOW_THICKNESS,
		   SET_VALUES_NOTEBOOK } ClientDataNotebook;


/* This is an enumeration of values for the "client_data" parameter to
   the function "CreateAndSetChildValues" */
enum               {
                     CHILD_TYPE=0,
		     PAGE_NUMBER,
		     RESIZABLE,
		     CREATE_CHILD,
		     SET_VALUES_CHILD
		     
		     } ClientDataChild;


/* This is an enumenration of the possible values for the resource 
   XmNorientation. */

enum               { HORIZONTAL = 0,
		     VERTICAL } Orientation_Types;


/* This is an enumenration of the possible values for the resource 
   XmNbackPagePlacement. */

enum                { BOTTOM_RIGHT = 0,
		      BOTTOM_LEFT,
		      TOP_RIGHT,
		      TOP_LEFT } Back_Page_Placement_Types;


/* This is an enumenration of the possible values for the resource 
   XmNbindingType. */

enum                 { NONE = 0,
		       SOLID,
		       SPIRAL,
		       PIXMAP,
		       PIXMAP_OVERLAP_ONLY } Binding_Types;


/* This is an enumeration of possible values for the resource XmNchildType */

enum                  {  PAGE = 0,
			 MAJOR_TAB,
			 MINOR_TAB,
			 STATUS_AREA,
			 PAGE_SCROLLER
		       } ChildTypes;


/* This is an enumeration of offsets for the array of scales */
enum                   { CurrentPageNumber = 0,
			 FirstPageNumber,
			 LastPageNumber,
			 BackPageNumber,
			 BackPageSize,
			 BindingWidth,
			 InnerMarginWidth,
			 InnerMarginHeight,
			 MajorTabSpacing,
			 MinorTabSpacing,
			 FrameShadowThickness
			} Scale_Offsets;


/* Mechanism used to get the creation function associated with a widget. */
typedef struct {
                 char *name;
		 Widget (*CreateFunction) (Widget, char *, ArgList, Cardinal);
	       } WidgetItem;



WidgetItem WidgetStruct[] =  {
                   "XmArrowButton", XmCreateArrowButton,   
                   "XmArrowButtonGadget", XmCreateArrowButtonGadget,  
		   "XmDrawnButton", XmCreateDrawnButton,   
		   "XmDrawingArea", XmCreateDrawingArea,
                   "XmFileSelectionBox",XmCreateFileSelectionBox,
                   "XmForm", XmCreateForm,
		   "XmFrame", XmCreateFrame,
		   "XmLabel", XmCreateLabel,  
		   "XmLabelGadget", XmCreateLabelGadget,   
                   "XmList", XmCreateList  ,
		   "XmMainWindow", XmCreateMainWindow,
                   "XmMessageBox", XmCreateMessageBox,   
		   "XmPanedWindow",XmCreatePanedWindow,
                   "XmPushButton",   XmCreatePushButton,
		   "XmPushButtonGadget", XmCreatePushButtonGadget,  
		   "XmRowColumn", XmCreateRowColumn,
		   "XmScale", XmCreateScale,
                   "XmScrollBar", XmCreateScrollBar,  
		   "XmScrolledText", XmCreateScrolledText,
                   "XmSelectionBox", XmCreateSelectionBox,  
		   "XmSeparator", XmCreateSeparator,                     
                   "XmSeparatorGadget",XmCreateSeparatorGadget, 
		   "XmText", XmCreateText,
                   "XmTextField", XmCreateTextField,   
		   "XmToggleButton", XmCreateToggleButton,   
                   "XmToggleButtonGadget", XmCreateToggleButtonGadget,  
		   "XmScrolledWindow", XmCreateScrolledWindow
		   };




char *WidgetNames[] = {
                   "XmArrowButton",   
                   "XmArrowButtonGadget",   
		   "XmDrawnButton",   
		   "XmDrawingArea",
                   "XmFileSelectionBox",
                   "XmForm",
		   "XmFrame",
		   "XmLabel",   
		   "XmLabelGadget",   
                   "XmList",   
		   "XmMainWindow",
                   "XmMessageBox",   
		   "XmPanedWindow",
                   "XmPushButton",   
		   "XmPushButtonGadget",   
		   "XmRowColumn",
		   "XmScale",
                   "XmScrollBar",   
		   "XmScrolledText",
                   "XmSelectionBox",   
		   "XmSeparator",                      
                   "XmSeparatorGadget",   
		   "XmText",
                   "XmTextField",   
		   "XmToggleButton",   
                   "XmToggleButtonGadget",  
		   "XmScrolledWindow",

		   };


int num_widgets = XtNumber(WidgetNames);



