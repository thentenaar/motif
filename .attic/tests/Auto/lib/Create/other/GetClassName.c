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
static char rcsid[] = "$XConsortium: GetClassName.c /main/8 1995/07/14 11:16:17 drk $"
#endif
#endif
/***********************************************************************
  	@(#)GetClassName.c	1.4.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/21/90 SJS add to SCCS
            06/19/90 SJS add in ToggleB & ToggleBG classes
	Calls:

	Summary:
            Returns a string pointer to the widget class name.

            INPUTS
                widget - the widget to return the class name of

            OUTPUTS
                none
  
            RETURNS
                pointer to a statically declared string which names the
                widget class
************************************************************************/

#include "mvslib.h"

static char *mvsWidgetClassName[] = { 
      /*  0 */ "vendorShellWidgetClass",
      /*  1 */ "xmArrowButtonWidgetClass",
      /*  2 */ "xmArrowButtonGadgetClass", 
      /*  3 */ "xmBulletinBoardWidgetClass",
      /*  4 */ "xmCascadeButtonWidgetClass",
      /*  5 */ "xmCascadeButtonGadgetClass", 
      /*  6 */ "xmCommandWidgetClass",
      /*  7 */ "xmDesktopObjectClass",
      /*  8 */ "xmDrawingAreaWidgetClass",
      /*  9 */ "xmDrawnButtonWidgetClass",
      /* 10 */ "xmDialogShellExtObjectClass",
      /* 11 */ "xmExtObjectClass",
      /* 12 */ "xmFileSelectionBoxWidgetClass",
      /* 13 */ "xmFormWidgetClass",
      /* 14 */ "xmFrameWidgetClass",
      /* 15 */ "xmGadgetClass",
      /* 16 */ "xmLabelWidgetClass",
      /* 17 */ "xmLabelGadgetClass",
      /* 18 */ "xmListWidgetClass",
      /* 19 */ "xmMainWindowWidgetClass",
      /* 20 */ "xmManagerWidgetClass",
      /* 21 */ "xmMenuShellWidgetClass",
      /* 22 */ "xmMessageBoxWidgetClass",
      /* 23 */ "xmPanedWindowWidgetClass",
      /* 24 */ "xmPrimitiveWidgetClass",
      /* 25 */ "xmProtocolObjectClass",
      /* 26 */ "xmPushButtonGadgetClass", 
      /* 27 */ "xmRowColumnWidgetClass",
      /* 28 */ "xmSashWidgetClass",
      /* 29 */ "xmScaleWidgetClass",
      /* 30 */ "xmScreenObjectClass",
      /* 31 */ "xmScrollBarWidgetClass",
      /* 32 */ "xmScrolledWindowWidgetClass",
      /* 33 */ "xmSelectionBoxWidgetClass",
      /* 34 */ "xmSeparatorGadgetClass", 
      /* 35 */ "xmSeparatorWidgetClass",
      /* 36 */ "xmShellExtObjectClass",
      /* 37 */ "xmTextWidgetClass",
      /* 38 */ "xmTextFieldWidgetClass",
      /* 39 */ "xmToggleButtonWidgetClass",
      /* 40 */ "xmToggleButtonGadgetClass",
      /* 41 */ "xmVendorShellExtObjectClass",
      /* 42 */ "applicationShellWidgetClass",
      /* 43 */ "compositeWidgetClass",
      /* 44 */ "coreWidgetClass",
      /* 45 */ "objectClass",
      /* 46 */ "overrideShellWidgetClass",
      /* 47 */ "rectObjClass",
      /* 48 */ "shellWidgetClass",
      /* 49 */ "topLevelShellWidgetClass",
      /* 50 */ "transientShellWidgetClass",
      /* 51 */ "wmShellWidgetClass",
      /* 52 */ "xmPushButtonWidgetClass",
#ifndef MOTIF1_1
      /* 53 */ "xmTearOffButtonWidgetClass",
#else
      /* 53 */ "xmPushButtonWidgetClass",  /* Placeholder for 1.1 */
#endif /* MOTIF1_1 */
      /* 54 */ "xmComboBoxWidgetClass",
      /* 55 */ "xmContainerWidgetClass",
      /* 56 */ "xmIconGadgetClass",
      /* 57 */ "xmNotebookWidgetClass",
      /* 58 */ "xmSpinBoxWidgetClass",
      /* 59 */ "unknownWidgetClass"
  };

char *mvsGetClassName(widget)
Widget widget;
{
    return( mvsWidgetClassName[mvsGetClassCode(widget)] );

} /* End mvsGetClassName() */
