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
/*   $XConsortium: mvslib.h /main/10 1996/10/30 11:34:29 drk $ */
/***********************************************************************
  	@(#)mvslib.h	1.68	Date:1/11/91
  	Author: TAT, SJS, PSN
	History:
            04/30/90 SJS Add to SCCS
            05/25/90 PSN File relocated and changed, resubmit
            05/25/90 PSN Add changes for reason and some callbacks & new func.
            07/02/90 SJS change to _NO_PROTO & play games with Traversal
	    11/26/90 PSN Define global to save Frame test's current permutation
	Calls:

	Summary:
            This file contains the declarations for all globally needed
        include files, types, structures, and variables.
************************************************************************/

#ifndef MVSLIB

#define MVSLIB

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/XmAll.h>

#include "mvs_defines.h"
#include "mvs_types.h"
#include "xislib.h"
#include "vislib.h"

/* Define mvsGLOBAL to cause this file to conditionally generate global code */

#ifdef DECLARE_MVSLIB_GLOBALS_HERE
#    define mvsGLOBAL
#    define mvsINIT(x) = x
#else
#    define mvsGLOBAL extern
#    define mvsINIT(x)
#endif


/**************************************************************************
*                       GENERAL DECLARATIONS
**************************************************************************/


/* Flag values in MvsCallbackInfoRecord */
#define CheckExistance		1
#define CheckCount		2
#define NO_CALLBACK_STRUCT (-1)

/* List specific defines that can be moved elsewhere at a later time */
#define Current 0
#define Previous 0
#define Next 0
#define All 0

/************ Resource Types Codes *****************/    

#define    T_XtTranslations             0
#define    T_Boolean                    1
#define    T_Pixel                      2 
#define    T_Pixmap                     3
#define    T_Dimension                  4
#define    T_Thickness                  5
#define    T_Colormap                   6
#define    T_VDepth                     7
#define    T_XtCallbackList             8
#define    T_XtEnum			9
/*** T_Pointer used to be T_ScreenP ****/
#define    T_Pointer                    10   
#define    T_Position                   11
#define    T_Integer                    12
#define    T_caddr_t                    13
#define    T_unsigned_char              14
#define    T_short                      15
#define    T_String                     16
#define    T_Widget_ref                 17
#define    T_XmFontList                 18
#define    T_XmStringTable              19
#define    T_XmString                   20
#define    T_XmString_temp              20
#define    T_char                       21
#define    T_XmNavigationType           22
#define    T_KeySym                     23
#define    T_XmStringDirection          24
#define    T_WidgetClass_ref            25
#define    T_Cardinal                   26
#define    T_WidgetList                 27
#define    T_XmFunction                 28
#define    T_XtProc                     29
#define    T_XmTextSource               30
#define    T_XmTextPosition             31
#define    T_XtAccelerators             32
#define	   T_XmDirection		33
#define	   T_XtOrderProc		34
#define    T_XmRenderTable		35 
#define    T_XmTabList			36
#define	   T_XmViewType			37
#define    T_XmVisualEmphasis		38
#define	   T_unsigned_int		39


/**************** Resource reference types ************/

#define    RefValue         0
#define    RefBoolean       1
#define    RefString        2
#define    RefCallbackList  3
#define    RefGeneric       4
#define    RefXmString      5
#define    RefXmStringTable 6

/************* Resource Access codes ******************/

#define CREATE (1<<0)
#define SET    (1<<1)
#define GET    (1<<2)

/* This is a list of values that need converting at run time */
#define MVSdynamic              (1)
#define MVSBlack                (2)
#define MVSXtCopyFromParent     (3)
#define MVSXtCopyScreen         (4)
#define MVSWhite                (5)

mvsGLOBAL Display *mvsDisplay mvsINIT(NULL);     /* Display connected to    */
mvsGLOBAL int mvsScreen;                         /* Default screen number   */

mvsGLOBAL int mvsDetailLevel;   /* Set by cmd line for Trace detail level     */

/* Set by cmd line */
mvsGLOBAL int mvsBehaviorOnly mvsINIT(CREATE_SECT | SET_SECT | BEHAVIOR_SECT);
mvsGLOBAL int mvsInBehavior mvsINIT(0); /* The validation is in behavior sect */

mvsGLOBAL int mvsMaxFails mvsINIT(0);/* Maximum allowed failures */
mvsGLOBAL int mvsFailCnt mvsINIT(0); /* The number of failures in behavior
                                        testing */
mvsGLOBAL int mvsCreateNum mvsINIT(0);/* Test combination to begin create */
mvsGLOBAL int mvsSetNum mvsINIT(0);   /* Test combination to begin set */
mvsGLOBAL int mvsCombNum mvsINIT(0);  /* Test combination to begin behavior */

#ifndef DECLARE_MVSLIB_GLOBALS_HERE
mvsGLOBAL char mvsTestName[]; /* Test name */
#else
mvsGLOBAL char mvsTestName[MAX_TEST_NAME] = { 0 };
#endif

#ifndef DECLARE_MVSLIB_GLOBALS_HERE
  mvsGLOBAL MvsTypeInfoRecord mvsTypeInfo[];
#else
  mvsGLOBAL MvsTypeInfoRecord mvsTypeInfo[] = {

/* int ref_type;    int size;            MvsArgVal (*enum_func)(); int nbits
   -------------    --------------------    -----------------  ----------     */
{  RefGeneric,      sizeof(XtTranslations),  NULL,                 0 },
{  RefBoolean,      sizeof(Boolean),         NULL,                 1 },
{  RefValue,        sizeof(Pixel),           NULL,                 7 },
{  RefValue,        sizeof(Pixmap),          NULL,                 7 },
{  RefValue,        sizeof(Dimension),       NULL,       	  10 },
{  RefValue,        sizeof(short),/*Thick*/  NULL,       	  10 },
{  RefValue,        sizeof(Colormap),        NULL,                 0 },
{  RefValue,        sizeof(short),           NULL,                10 },
{  RefCallbackList, sizeof(XtCallbackList),  NULL,   		   0 },
{  RefValue,        sizeof(XtEnum),   	     NULL,       	   8 },
{  RefGeneric,      sizeof(Pointer),         NULL,                 0 },
{  RefValue,        sizeof(Position),        NULL,        	  10 },
{  RefValue,        sizeof(Integer),         NULL,         	  10 },
{  RefGeneric,      sizeof(caddr_t),         NULL,                 0 },
{  RefValue,        sizeof(unsigned_char),   NULL,       	   8 },
{  RefValue,        sizeof(short),           NULL,           	  10 },
{  RefString,       sizeof(String),          NULL,                 0 },
{  RefGeneric,      sizeof(Widget_ref),      NULL,       	   0 },
{  RefGeneric,      sizeof(XmFontList),      NULL,       	   0 },
{  RefXmStringTable,sizeof(XmStringTable),   NULL,    		   1 },
{  RefXmString,     sizeof(XmString),        NULL,         	   1 },
{  RefValue,        sizeof(char),            NULL,                 8 },
{  RefValue,        sizeof(XmNavigationType),NULL,                 0 },
{  RefValue,        sizeof(KeySym),          NULL,                 0 },
{  RefValue,        sizeof(XmStringDirection),NULL,                1 },
{  RefGeneric,      sizeof(WidgetClass),     NULL,                 0 },
{  RefValue,        sizeof(Cardinal),        NULL,                10 },
{  RefGeneric,      sizeof(WidgetList),      NULL,                 0 },
{  RefGeneric,      sizeof(XtOrderProc),     NULL,                 0 },
/**** Check these for validity !!!!! Some ref-types are wrong ****/
/****             the sizeof is WRONG for the next 2, AES doesn't say ****/
      {  RefGeneric, sizeof(XtOrderProc), NULL,   0 },
      {  RefGeneric, sizeof(XmString),    NULL,   0 },
{  RefGeneric,      sizeof(XmTextPosition),  NULL,                 0 },
{  RefGeneric,      sizeof(XtAccelerators),  NULL,                 0 },
{  RefGeneric,      sizeof(XmDirection),     NULL,                 0 },
{  RefGeneric, 	    sizeof(XtOrderProc),     NULL,   		   0 },
{  RefGeneric,      sizeof(XmRenderTable),   NULL,       	   0 },
{  RefGeneric,      sizeof(XmTabList),       NULL,                 0 },
{  RefValue,        sizeof(unsigned_char),   NULL,                 8 },
{  RefValue,        sizeof(unsigned_char),   NULL,                 8 },
{  RefValue,        sizeof(unsigned_int),    NULL,         	  10 }
};
#endif

/* NOTE: mvsResPixmapRef is 1-to-1 with mvsResColorRef. */

/* POSSIBLE WIDGET CLASS CODES */

# define mvsVendorShellWidgetClass         	 0
# define mvsXmArrowButtonWidgetClass       	 1
# define mvsXmArrowButtonGadgetClass       	 2
# define mvsXmBulletinBoardWidgetClass     	 3
# define mvsXmCascadeButtonWidgetClass     	 4
# define mvsXmCascadeButtonGadgetClass     	 5
# define mvsXmCommandWidgetClass           	 6
# define mvsXmDesktopObjectClass           	 7
# define mvsXmDrawingAreaWidgetClass       	 8
# define mvsXmDrawnButtonWidgetClass       	 9
# define mvsXmDialogShellWidgetClass       	10
# define mvsXmExtObjectClass               	11
# define mvsXmFileSelectionBoxWidgetClass  	12
# define mvsXmFormWidgetClass              	13
# define mvsXmFrameWidgetClass             	14
# define mvsXmGadgetClass                  	15
# define mvsXmLabelWidgetClass             	16
# define mvsXmLabelGadgetClass             	17
# define mvsXmListWidgetClass              	18
# define mvsXmMainWindowWidgetClass        	19
# define mvsXmManagerWidgetClass           	20
# define mvsXmMenuShellWidgetClass         	21
# define mvsXmMessageBoxWidgetClass        	22
# define mvsXmPanedWindowWidgetClass       	23
# define mvsXmPrimitiveWidgetClass         	24
# define mvsXmProtocolObjectClass          	25
# define mvsXmPushButtonGadgetClass        	26
# define mvsXmRowColumnWidgetClass         	27
# define mvsXmSashWidgetClass              	28
# define mvsXmScaleWidgetClass             	29
# define mvsXmScreenObjectClass            	30
# define mvsXmScrollBarWidgetClass         	31
# define mvsXmScrolledWindowWidgetClass    	32
# define mvsXmSelectionBoxWidgetClass      	33
# define mvsXmSeparatorGadgetClass         	34
# define mvsXmSeparatorWidgetClass         	35
# define mvsXmShellExtObjectClass          	36
# define mvsXmTextWidgetClass              	37
# define mvsXmTextFieldWidgetClass         	38
# define mvsXmToggleButtonWidgetClass      	39
# define mvsXmToggleButtonGadgetClass      	40
# define mvsXmVendorShellExtObjectClass    	41
# define mvsApplicationShellWidgetClass    	42
# define mvsCompositeWidgetClass       	   	43
# define mvsCoreWidgetClass    	   	   	44
# define mvsObjectClass        	   	   	45
# define mvsOverrideShellWidgetClass   	   	46
# define mvsRectObjClass     	   	   	47
# define mvsShellWidgetClass       	   	48
# define mvsTopLevelShellWidgetClass   	   	49
# define mvsTransientShellWidgetClass  	   	50
# define mvsWMShellWidgetClass        	   	51
# define mvsXmPushButtonWidgetClass        	52
# define mvsXmTearOffButtonWidgetClass		53
# define mvsXmComboBoxWidgetClass		54
# define mvsXmContainerWidgetClass		55
# define mvsXmIconGadgetClass			56
# define mvsXmNotebookWidgetClass		57
# define mvsXmSpinBoxWidgetClass		58
# define MAX_WIDGET_CLASS                  	58

/* The class code above this line corresponds 1-to-1 with mvsWidgetClass[]
 * array in mvsGetClassCode(). Make sure they match */

# define mvsXmMenuBarWidgetClass        	60
# define mvsXmPulldownMenuWidgetClass        	61
# define mvsXmPopupMenuWidgetClass        	62
# define mvsXmOptionMenuWidgetClass        	63
# define mvsXmRadioBoxWidgetClass        	64
# define mvsXmWorkAreaWidgetClass        	65

#include "instances.h"

/**************************************************************************
*                     SET EXPECTED ACTION CODES
**************************************************************************/
 
/* POSSIBLE OBJECT CODES */

# define oUserDefined             0
# define oSeparator               1
# define oLabel                   2
# define oListItem                3
# define oTextChar                4
# define oSlider                  5
# define oUpOrLeftArrow           6
# define oDownOrRightArrow        7
# define oUpOrLeftSliderArea      8
# define oDownOrRightSliderArea   9
# define oScrollBar              10
# define oHorizScrollBar         11
# define oVertScrollBar          12
# define oMenuBar                13
# define oSash                   14
# define oCloseButton            15
# define oHelpButton             16
# define oOkButton               17
# define oFilterButton           18
# define oCancelButton           19
# define oApplyButton            20
# define oMessageLabel           21
# define oSymbolLabel            22
# define oSelectionText          23
# define oSelectionLabel         24
# define oSelectionList          25
# define oSelectionListLabel     26
# define oFileList               27 
# define oFileListLabel          28
# define oDirList                29 
# define oDirListLabel           30
/* Make this and oSelectionText the same */
# define oFileSelectionText      23
# define oFileSelectionLabel     24
# define oFileFilterText         33
# define oFileFilterLabel        34
# define oWorkArea               35
# define oCommandWindow          36
# define oMessageWindow          37
# define oWorkWindow             38
# define oScale             	 39
# define oOptionButton         	 40
# define oScrolledWindow       	 41
# define oTearOffButton		 42
# define oComboBoxDropDownButton 44
# define oContainerIconG	 45
# define oPageScroller		 46
# define oMajorTabScrollerNext	 47
# define oMajorTabScrollerPrev	 48
# define oMinorTabScrollerNext	 49
# define oMinorTabScrollerPrev	 50
# define oMinorTabScrollerPrev	 50
# define oIncrementBtn 		 51
# define oDecrementBtn 		 52
# define oOutlineBtn 		 53
# define oDirFilterLabel 	 54
# define oDirFilterText 	 55
# define MAX_OBJECT              56

/* Additional definition required for naming purpose only */
# define oScrollBarSlider	 	  ScrollBarSlider(0)
# define oScrollBarUpOrLeftArrow 	  ScrollBarUpOrLeftArrow(0)
# define oScrollBarDownOrRightArrow	  ScrollBarDownOrRightArrow(0)
# define oScrollBarUpOrLeftSliderArea	  ScrollBarUpOrLeftSliderArea(0)
# define oScrollBarDownOrRightSliderArea  ScrollBarDownOrRightSliderArea(0)

#ifndef DECLARE_MVSLIB_GLOBALS_HERE
  mvsGLOBAL char *mvsObjectName[];
#else
  mvsGLOBAL char *mvsObjectName[] = {
         "UserDefined",
         "Separator",
         "Label",
         "ListItem",
         "TextChar",
         "Slider",
         "UpOrLeftArrow",
         "DownOrRightArrow",
         "UpOrLeftSliderArea",
         "DownOrRightSliderArea",
         "ScrollBar",
         "HorizScrollBar",
         "VertScrollBar",
         "ScrollBar",
/*         "HorizScrollBar",    WHY IS THIS REPEATED FROM 2 LINES UP HERE?
         "VertScrollBar",
 */
         "MenuBar",
         "Sash",
         "CloseButton",
         "HelpButton",
         "OkButton" ,
         "FilterButton",
         "CancelButton",
         "ApplyButton",
         "MessageLabel",
         "SymbolLabel" ,
         "SelectionText",
         "SelectionLabel",
         "SelectionList",
         "SelectionListLabel",
         "FileList" ,
         "FileListLabel",
         "DirList",
         "DirListLabel",
         "FileSelectionText",
         "FileSelectionLabel",
         "FileFilterText" ,
         "FileFilterLabel",
         "WorkArea",
         "CommandWindow" ,
         "MessageWindow",
         "WorkWindow",
         "Scale",
         "OptionButton",
         "ScrolledWindow",
	 "TearOffButton",
	 "ComboBoxDropDownButton",
         "ContainerIconG",
         "PageScroller",
         "MajorTabScrollerNext",
         "MajorTabScrollerPrev",
         "MinorTabScrollerNext",
         "MinorTabScrollerPrev",
	 "IncrementBtn",
	 "DecrementBtn",
	 "OutlineBtn",
	 "DirFilterLabel",
	 "DirFilterText",
         "MAX_OBJECT"
  };
#endif

# define UserDefined(x)          	xisConcat(x,oUserDefined)
# define TearOffButton(x)		xisConcat(x,oTearOffButton)
# define Separator(x)            	xisConcat(x,oSeparator)
# define Label(x)                	xisConcat(x,oLabel)
# define ListItem(x)             	xisConcat(x,oListItem)
# define TextChar(x)             	xisConcat(x,oTextChar)
# define Slider(x)               	xisConcat(x,oSlider)
# define UpOrLeftArrow(x)             	xisConcat(x,oUpOrLeftArrow)
# define DownOrRightArrow(x)            xisConcat(x,oDownOrRightArrow)
# define UpOrLeftSliderArea(x)        	xisConcat(x,oUpOrLeftSliderArea)
# define DownOrRightSliderArea(x)       xisConcat(x,oDownOrRightSliderArea)
# define ScrollBar(x)            	xisConcat(x,oScrollBar)

/* Automation added */
# define ComboBoxDropDownButton(x)	xisConcat(x,oComboBoxDropDownButton)
# define OutlineBtn(x)			xisConcat(x,oOutlineBtn)
# define ContainerIconG(x)            	xisConcat(x,oContainerIconG)
# define PageScroller(x)		xisConcat(x,oPageScroller)
# define MajorTabScrollerNext(x)        xisConcat(x,oMajorTabScrollerNext)
# define MajorTabScrollerPrev(x)        xisConcat(x,oMajorTabScrollerPrev)
# define MinorTabScrollerNext(x)        xisConcat(x,oMinorTabScrollerNext)
# define MinorTabScrollerPrev(x)        xisConcat(x,oMinorTabScrollerPrev)
# define IncrementBtn(x)		xisConcat(x,oIncrementBtn)
# define DecrementBtn(x)		xisConcat(x,oDecrementBtn)
# define DirFilterLabel(x)		xisConcat(x,oDirFilterLabel)
# define DirFilterText(x)		xisConcat(x,oDirFilterText)
# define DirFilterTextChar(x)    	TextChar(DirFilterText(x))
# define oDirFilterTextChar  		DirFilterTextChar(0)

/* End Automation added */

# define ScrollBarUpOrLeftArrow(x)      UpOrLeftArrow(ScrollBar(x))
# define ScrollBarDownOrRightArrow(x)   DownOrRightArrow(ScrollBar(x))
# define ScrollBarUpOrLeftSliderArea(x) UpOrLeftSliderArea(ScrollBar(x))
# define ScrollBarDownOrRightSliderArea(x) DownOrRightSliderArea(ScrollBar(x))
# define ScrollBarSlider(x)        	Slider(ScrollBar(x))
# define Scale(x)            		xisConcat(x,oScale)
# define ScaleUpOrLeftSliderArea(x) 	ScrollBarUpOrLeftSliderArea(Scale(x))
# define ScaleDownOrRightSliderArea(x) 	ScrollBarDownOrRightSliderArea(Scale(x))
# define ScaleSlider(x)        		ScrollBarSlider(Scale(x))
# define ScaleLabel(x)                  Label(Scale(x))
# define oScaleLabel			ScaleLabel(0)

# define MenuBar(x)              xisConcat(x,oMenuBar)
# define Sash(x)                 xisConcat(x,oSash)
# define CloseButton(x)          xisConcat(x,oCloseButton)
# define HelpButton(x)           xisConcat(x,oHelpButton)
# define OkButton(x)             xisConcat(x,oOkButton)
# define FilterButton(x)         xisConcat(x,oFilterButton)
# define CancelButton(x)         xisConcat(x,oCancelButton)
# define ApplyButton(x)          xisConcat(x,oApplyButton)
# define MessageLabel(x)         xisConcat(x,oMessageLabel)
# define ScrolledWindow(x)       xisConcat(x,oScrolledWindow)
# define SymbolLabel(x)          xisConcat(x,oSymbolLabel)
# define SelectionText(x)        xisConcat(x,oSelectionText)
# define SelectionTextChar(x)    TextChar(SelectionText(x))
# define oSelectionTextChar	 SelectionTextChar(0)
# define SelectionLabel(x)       xisConcat(x,oSelectionLabel)
# define SelectionList(x)        xisConcat(x,oSelectionList)
# define oSelectionList1	 SelectionList(ScrolledWindow(0))
# define SelectionListItem(x)    ListItem(SelectionList(ScrolledWindow(x)))
# define oSelectionListItem      SelectionListItem(0)
# define SelectionListLabel(x)   xisConcat(x,oSelectionListLabel)
# define FileList(x)             xisConcat(x,oFileList)
# define oFileList1		 FileList(ScrolledWindow(0))
# define DirList(x)              xisConcat(x,oDirList)
# define oDirList1		 DirList(ScrolledWindow(0))
# define FileListItem(x)         ListItem(FileList(ScrolledWindow(x)))
# define oFileListItem           FileListItem(0)
# define DirListItem(x)          ListItem(DirList(ScrolledWindow(x)))
# define oDirListItem            DirListItem(0)

/* Automation modified */

# define HorizScrollBar(x) \
	xisConcat(x,oHorizScrollBar)
# define HorizScrollBarUpOrLeftArrow(x) \
	UpOrLeftArrow(HorizScrollBar(x))
# define oHorizScrollBarUpOrLeftArrow \
	HorizScrollBarUpOrLeftArrow(0)
# define HorizScrollBarDownOrRightArrow(x) \
	DownOrRightArrow(HorizScrollBar(x))
# define oHorizScrollBarDownOrRightArrow \
	HorizScrollBarDownOrRightArrow(0)
# define HorizScrollBarUpOrLeftSliderArea(x) \
	UpOrLeftSliderArea(HorizScrollBar(x))
# define oHorizScrollBarUpOrLeftSliderArea \
	HorizScrollBarUpOrLeftSliderArea(0)
# define HorizScrollBarDownOrRightSliderArea(x) \
	DownOrRightSliderArea(HorizScrollBar(x))
# define oHorizScrollBarDownOrRightSliderArea \
	HorizScrollBarDownOrRightSliderArea(0)
# define HorizScrollBarSlider(x) \
	Slider(HorizScrollBar(x))
# define oHorizScrollBarSlider \
	HorizScrollBarSlider(0)
# define VertScrollBar(x) \
	xisConcat(x,oVertScrollBar)
# define VertScrollBarUpOrLeftArrow(x) \
	UpOrLeftArrow(VertScrollBar(x))
# define oVertScrollBarUpOrLeftArrow \
	VertScrollBarUpOrLeftArrow(0)
# define VertScrollBarDownOrRightArrow(x) \
	DownOrRightArrow(VertScrollBar(x))
# define oVertScrollBarDownOrRightArrow \
	VertScrollBarDownOrRightArrow(0)
# define VertScrollBarUpOrLeftSliderArea(x) \
	UpOrLeftSliderArea(VertScrollBar(x))
# define oVertScrollBarUpOrLeftSliderArea \
	VertScrollBarUpOrLeftSliderArea(0)
# define VertScrollBarDownOrRightSliderArea(x) \
	DownOrRightSliderArea(VertScrollBar(x))
# define oVertScrollBarDownOrRightSliderArea \
	VertScrollBarDownOrRightSliderArea(0)
# define VertScrollBarSlider(x) \
	Slider(VertScrollBar(x))
# define oVertScrollBarSlider \
	VertScrollBarSlider(0)

/* End Automation modified */

/* Automation Added */

# define FileListHorizScrollBarUpOrLeftArrow(x) \
             UpOrLeftArrow(HorizScrollBar(FileList(ScrolledWindow(x))))
# define oFileListHorizScrollBarUpOrLeftArrow \
	     FileListHorizScrollBarUpOrLeftArrow(0)
# define FileListHorizScrollBarDownOrRightArrow(x) \
             DownOrRightArrow(HorizScrollBar(FileList(ScrolledWindow(x))))
# define oFileListHorizScrollBarDownOrRightArrow \
	     FileListHorizScrollBarDownOrRightArrow(0)
# define FileListHorizScrollBarUpOrLeftSliderArea(x) \
             UpOrLeftSliderArea(HorizScrollBar(FileList(ScrolledWindow(x))))
# define oFileListHorizScrollBarUpOrLeftSliderArea \
	     FileListHorizScrollBarUpOrLeftSliderArea(0)
# define FileListHorizScrollBarDownOrRightSliderArea(x) \
             DownOrRightSliderArea(HorizScrollBar(FileList(ScrolledWindow(x))))
# define oFileListHorizScrollBarDownOrRightSliderArea \
	     FileListHorizScrollBarDownOrRightSliderArea(0)
# define FileListHorizScrollBarSlider(x) \
             Slider(HorizScrollBar(FileList(ScrolledWindow(x))))
# define oFileListHorizScrollBarSlider \
	     FileListHorizScrollBarSlider(0)
# define FileListVertScrollBarUpOrLeftArrow(x) \
             UpOrLeftArrow(VertScrollBar(FileList(ScrolledWindow(x))))
# define oFileListVertScrollBarUpOrLeftArrow \
	     FileListVertScrollBarUpOrLeftArrow(0)
# define FileListVertScrollBarDownOrRightArrow(x) \
             DownOrRightArrow(VertScrollBar(FileList(ScrolledWindow(x))))
# define oFileListVertScrollBarDownOrRightArrow \
	     FileListVertScrollBarDownOrRightArrow(0)
# define FileListVertScrollBarUpOrLeftSliderArea(x) \
             UpOrLeftSliderArea(VertScrollBar(FileList(ScrolledWindow(x))))
# define oFileListVertScrollBarUpOrLeftSliderArea \
	     FileListVertScrollBarUpOrLeftSliderArea(0)
# define FileListVertScrollBarDownOrRightSliderArea(x) \
             DownOrRightSliderArea(VertScrollBar(FileList(ScrolledWindow(x))))
# define oFileListVertScrollBarDownOrRightSliderArea \
	     FileListVertScrollBarDownOrRightSliderArea(0)
# define FileListVertScrollBarSlider(x) \
             Slider(VertScrollBar(FileList(ScrolledWindow(x))))
# define oFileListVertScrollBarSlider \
	     FileListVertScrollBarSlider(0)

# define SelectionListHorizScrollBarUpOrLeftArrow(x) \
         UpOrLeftArrow(HorizScrollBar(ScrolledWindow(x)))
# define oSelectionListHorizScrollBarUpOrLeftArrow \
	 SelectionListHorizScrollBarUpOrLeftArrow(0)
# define SelectionListHorizScrollBarDownOrRightArrow(x) \
         DownOrRightArrow(HorizScrollBar(ScrolledWindow(x)))
# define oSelectionListHorizScrollBarDownOrRightArrow \
	 SelectionListHorizScrollBarDownOrRightArrow(0)
# define SelectionListHorizScrollBarUpOrLeftSliderArea(x) \
         UpOrLeftSliderArea(HorizScrollBar(ScrolledWindow(x)))
# define oSelectionListHorizScrollBarUpOrLeftSliderArea \
	 SelectionListHorizScrollBarUpOrLeftSliderArea(0)
# define SelectionListHorizScrollBarDownOrRightSliderArea(x) \
         DownOrRightSliderArea(HorizScrollBar(ScrolledWindow(x)))
# define oSelectionListHorizScrollBarDownOrRightSliderArea \
	 SelectionListHorizScrollBarDownOrRightSliderArea(0)
# define SelectionListHorizScrollBarSlider(x) \
         Slider(HorizScrollBar(ScrolledWindow(x)))
# define oSelectionListHorizScrollBarSlider \
	 SelectionListHorizScrollBarSlider(0)
# define SelectionListVertScrollBarUpOrLeftArrow(x) \
         UpOrLeftArrow(VertScrollBar(ScrolledWindow(x)))
# define oSelectionListVertScrollBarUpOrLeftArrow \
	 SelectionListVertScrollBarUpOrLeftArrow(0)
# define SelectionListVertScrollBarDownOrRightArrow(x) \
         DownOrRightArrow(VertScrollBar(ScrolledWindow(x)))
# define oSelectionListVertScrollBarDownOrRightArrow \
	 SelectionListVertScrollBarDownOrRightArrow(0)
# define SelectionListVertScrollBarUpOrLeftSliderArea(x) \
         UpOrLeftSliderArea(VertScrollBar(ScrolledWindow(x)))
# define oSelectionListVertScrollBarUpOrLeftSliderArea \
	 SelectionListVertScrollBarUpOrLeftSliderArea(0)
# define SelectionListVertScrollBarDownOrRightSliderArea(x) \
         DownOrRightSliderArea(VertScrollBar(ScrolledWindow(x)))
# define oSelectionListVertScrollBarDownOrRightSliderArea \
	 SelectionListVertScrollBarDownOrRightSliderArea(0)
# define SelectionListVertScrollBarSlider(x) \
         Slider(VertScrollBar(ScrolledWindow(x)))
# define oSelectionListVertScrollBarSlider \
	 SelectionListVertScrollBarSlider(0)

# define FileListLabel(x)        xisConcat(x,oFileListLabel)
# define DirList(x)              xisConcat(x,oDirList)


# define DirListHorizScrollBarUpOrLeftArrow(x) \
             HorizScrollBarUpOrLeftArrow(DirList(ScrolledWindow(x)))
# define oDirListHorizScrollBarUpOrLeftArrow \
	     DirListHorizScrollBarUpOrLeftArrow(0)
# define DirListHorizScrollBarDownOrRightArrow(x) \
             HorizScrollBarDownOrRightArrow(DirList(ScrolledWindow(x)))
# define oDirListHorizScrollBarDownOrRightArrow \
	     DirListHorizScrollBarDownOrRightArrow(0)
# define DirListHorizScrollBarUpOrLeftSliderArea(x) \
             HorizScrollBarUpOrLeftSliderArea(DirList(ScrolledWindow(x)))
# define oDirListHorizScrollBarUpOrLeftSliderArea \
	     DirListHorizScrollBarUpOrLeftSliderArea(0)
# define DirListHorizScrollBarDownOrRightSliderArea(x) \
             HorizScrollBarDownOrRightSliderArea(DirList(ScrolledWindow(x)))
# define oDirListHorizScrollBarDownOrRightSliderArea \
	     DirListHorizScrollBarDownOrRightSliderArea(0)
# define DirListHorizScrollBarSlider(x) \
             HorizScrollBarSlider(DirList(ScrolledWindow(x)))
# define oDirListHorizScrollBarSlider \
	     DirListHorizScrollBarSlider(0)
# define DirListVertScrollBarUpOrLeftArrow(x) \
             UpOrLeftArrow(VertScrollBar(DirList(ScrolledWindow(x))))
# define oDirListVertScrollBarUpOrLeftArrow \
	     DirListVertScrollBarUpOrLeftArrow(0)
# define DirListVertScrollBarDownOrRightArrow(x) \
             DownOrRightArrow(VertScrollBar(DirList(ScrolledWindow(x))))
# define oDirListVertScrollBarDownOrRightArrow \
	     DirListVertScrollBarDownOrRightArrow(0)
# define DirListVertScrollBarUpOrLeftSliderArea(x) \
             UpOrLeftSliderArea(VertScrollBar(DirList(ScrolledWindow(x))))
# define oDirListVertScrollBarUpOrLeftSliderArea \
	     DirListVertScrollBarUpOrLeftSliderArea(0)
# define DirListVertScrollBarDownOrRightSliderArea(x) \
             DownOrRightSliderArea(VertScrollBar(DirList(ScrolledWindow(x))))
# define oDirListVertScrollBarDownOrRightSliderArea \
	     DirListVertScrollBarDownOrRightSliderArea(0)
# define DirListVertScrollBarSlider(x) \
             Slider(VertScrollBar(DirList(ScrolledWindow(x))))
# define oDirListVertScrollBarSlider \
	     DirListVertScrollBarSlider(0)



/* End Automation Added */

# define DirListLabel(x)         xisConcat(x,oDirListLabel)
# define FileSelectionText(x)    xisConcat(x,oFileSelectionText)
# define FileSelectionTextChar(x)    TextChar(FileSelectionText(x))
# define oFileSelectionTextChar  FileSelectionTextChar(0)
# define FileSelectionLabel(x)   xisConcat(x,oFileSelectionLabel)
# define FileFilterText(x)       xisConcat(x,oFileFilterText)
# define FileFilterTextChar(x)       TextChar(FileFilterText(x))
# define oFileFilterTextChar	 FileFilterTextChar(0)
# define FileFilterLabel(x)      xisConcat(x,oFileFilterLabel)
# define WorkArea(x)             xisConcat(x,oWorkArea)
# define CommandWindow(x)        xisConcat(x,oCommandWindow)
# define MessageWindow(x)        xisConcat(x,oMessageWindow)
# define WorkWindow(x)           xisConcat(x,oWorkWindow)
# define OptionButton(x)         xisConcat(x,oOptionButton)


/* POSSIBLE INSTANCE CODES */

  /* For List, 0 = first (top) item in the list, n = (n-1)th list item */
  /* For Text, 0 = first character in list of chars, etc.              */
  /* For PanedWindow, 0 = first sash, n = (n-1)th sash                 */
  /* For Buttons and all else, just use 0                              */


  mvsGLOBAL MvsWidgetInfoRecord mvsWidgetInfos[MAX_NUM_WIDGET_INFOS] 
                                                   mvsINIT({ 0 });
  /* NOTE: mvsWidgetInfos[0] is always the root widget info record */

  mvsGLOBAL int mvsNumWidgetInfos mvsINIT(0);

  mvsGLOBAL MvsWidgetInfoRecord *mvsRootWidgetInfo mvsINIT(NULL);


  /* Constants for use with mvsCreateWidget */

# define DontUseCommonArgs 0
# define UseCommonArgs     1


  /* Constants for use with mvsDestroyWidget */

# define DontCheckCallbacks 0
# define DoCheckCallbacks   1


/**************************************************************************
*                     GLOBAL CALLBACK STRUCTURES
**************************************************************************/

mvsGLOBAL int mvsCallbackEnum;
mvsGLOBAL int mvsCallbacksInitialized mvsINIT(FALSE);

mvsGLOBAL long mvsCallbackSum;

#ifdef DECLARE_MVSLIB_GLOBALS_HERE
mvsGLOBAL long mvsCallbacksCheckSum[] = {2, 2, 543, 6387, 8911389 };
#else
mvsGLOBAL long mvsCallbacksCheckSum[];
#endif


mvsGLOBAL MvsCallbackInfoRecord mvsCallbackInfo[MAX_CALLBACKS] 
          mvsINIT({ 0 });

mvsGLOBAL int mvsExpectedCallback[MAX_CALLBACKS]; 
mvsGLOBAL int mvsExpectedReasons[MAX_CALLBACKS]; 


#ifndef DECLARE_MVSLIB_GLOBALS_HERE
mvsGLOBAL MvsFontInfoRecord mvsFontInfo[];
#else
/* Reduced the size of array to 1, and made the font a more general one */
mvsGLOBAL MvsFontInfoRecord mvsFontInfo[] = {
/*    char_set_name  bits_per_char   mono_spaced  left_to_right iso_latin1 */
{ NULL, "8x13",               8,        True,        True,        True,
        "8x13"                                                          },
};
#endif /* DECLARE_MVSLIB_GLOBALS_HERE */

mvsGLOBAL int mvsNumFonts mvsINIT(sizeof(mvsFontInfo)/sizeof(MvsFontInfoRecord));

mvsGLOBAL int mvsNumLoadedFonts mvsINIT({ 0 });
mvsGLOBAL XmFontList mvsFontList mvsINIT({ 0 });

/* Following defines are used by class converting procedures */

/* Different class code types, can be ORed to combine them */
#define MOTIF_CLASS	0x1
#define MVS_CLASS	0x2
#define INST_CLASS	0x4

mvsGLOBAL ClassRef        MotifClasses[MAXCLASS];

/* Hack for getting unique test case number in XmFrame test */
mvsGLOBAL int mvsFramePerm mvsINIT({ 0 });
#define   MAX_BUTTON_PERM  100

#include "mvs_protos.h"

#endif
/* MVSLIB - don't put anything after this! */
