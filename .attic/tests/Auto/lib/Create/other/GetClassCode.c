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
static char rcsid[] = "$XConsortium: GetClassCode.c /main/9 1995/07/14 11:16:01 drk $"
#endif
#endif
/***********************************************************************
  	@(#)GetClassCode.c	1.6.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/21/90 SJS add to SCCS
            06/05/90 SJS fix #define usage of DialogShell class
            06/19/90 SJS add in ToggleB & ToggleBG classes
            08/15/90 PSN added missing class & fixed for checking end condition
	Calls:

	Summary:
            Returns an integer code representation of the class that
            corresponds to all the mvs class codes defined in mvslib.h.

            INPUTS
                widget - the widget to return the class code of

            OUTPUTS
                none
  
            RETURNS
                class code
************************************************************************/

#include "mvslib.h"

#include <X11/Shell.h>
#include <X11/Vendor.h>
#include <X11/Xproto.h>
#include <Xm/ArrowB.h>
#include <Xm/ArrowBG.h>
#include <Xm/BulletinB.h>
#include <Xm/CascadeB.h>
#include <Xm/CascadeBG.h>
#include <Xm/ComboBox.h>
#include <Xm/Command.h>
#include <Xm/Container.h>
#include <Xm/CutPaste.h>
#include <Xm/DialogS.h>
#include <Xm/DrawingA.h>
#include <Xm/DrawnB.h>
#include <Xm/FileSB.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/IconG.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/List.h>
#include <Xm/MainW.h>
#include <Xm/MenuShell.h>
#include <Xm/MessageB.h>
#include <Xm/Notebook.h>
#include <Xm/PanedW.h>
#include <Xm/ProtocolsP.h>    /* Possible non-portability */
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/SashP.h>         /* Possible non-portability */
#include <Xm/Scale.h>
#include <Xm/ScrollBar.h>
#include <Xm/ScrolledW.h>
#include <Xm/SelectioB.h>
#include <Xm/SeparatoG.h>
#include <Xm/Separator.h>
#include <Xm/SpinB.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>

#ifndef MOTIF1_1
#include <Xm/DesktopP.h> 	
#include <Xm/Screen.h>
#include <Xm/ShellEP.h>
#include <Xm/VendorSEP.h>
#include <Xm/TearOffBP.h>
#else
#include <Xm/VendorE.h>
#endif

/* NOTE: The following list must correspond EXACTLY with the mvs class codes
         in mvslib.h                                                         */

static WidgetClass *mvsWidgetClass[] = {
      /*  0 */ &vendorShellWidgetClass,
      /*  1 */ &xmArrowButtonWidgetClass,
      /*  2 */ &xmArrowButtonGadgetClass, 
      /*  3 */ &xmBulletinBoardWidgetClass,
      /*  4 */ &xmCascadeButtonWidgetClass,
      /*  5 */ &xmCascadeButtonGadgetClass, 
      /*  6 */ &xmCommandWidgetClass,
#ifdef MOTIF1_1
      /*  7 */ &xmDesktopObjectClass,
#else
      /*  7 */ &xmDesktopClass,
#endif /* MOTIF1_1 */
      /*  8 */ &xmDrawingAreaWidgetClass,
      /*  9 */ &xmDrawnButtonWidgetClass,
      /* 10 */ &xmDialogShellWidgetClass,
      /* 11 */ &xmExtObjectClass,
      /* 12 */ &xmFileSelectionBoxWidgetClass,
      /* 13 */ &xmFormWidgetClass,
      /* 14 */ &xmFrameWidgetClass,
      /* 15 */ &xmGadgetClass, 
      /* 16 */ &xmLabelWidgetClass,
      /* 17 */ &xmLabelGadgetClass,
      /* 18 */ &xmListWidgetClass,
      /* 19 */ &xmMainWindowWidgetClass,
      /* 20 */ &xmManagerWidgetClass,
      /* 21 */ &xmMenuShellWidgetClass,
      /* 22 */ &xmMessageBoxWidgetClass,
      /* 23 */ &xmPanedWindowWidgetClass,
      /* 24 */ &xmPrimitiveWidgetClass,
      /* 25 */ &xmProtocolObjectClass,
      /* 26 */ &xmPushButtonGadgetClass, 
      /* 27 */ &xmRowColumnWidgetClass, 
      /* 28 */ &xmSashWidgetClass,
      /* 29 */ &xmScaleWidgetClass,
#ifdef MOTIF1_1
      /* 30 */ &xmScreenObjectClass,
#else
      /* 30 */ &xmScreenClass,
#endif
      /* 31 */ &xmScrollBarWidgetClass,
      /* 32 */ &xmScrolledWindowWidgetClass,
      /* 33 */ &xmSelectionBoxWidgetClass,
      /* 34 */ &xmSeparatorGadgetClass, 
      /* 35 */ &xmSeparatorWidgetClass,
      /* 36 */ &xmShellExtObjectClass,
      /* 37 */ &xmTextWidgetClass,
      /* 38 */ &xmTextFieldWidgetClass,
      /* 39 */ &xmToggleButtonWidgetClass,
      /* 40 */ &xmToggleButtonGadgetClass,
      /* 41 */ &xmVendorShellExtObjectClass,
      /* 42 */ &applicationShellWidgetClass,
      /* 43 */ &compositeWidgetClass,
      /* 44 */ &coreWidgetClass,
      /* 45 */ &objectClass,
      /* 46 */ &overrideShellWidgetClass,
      /* 47 */ &rectObjClass,
      /* 48 */ &shellWidgetClass,
      /* 49 */ &topLevelShellWidgetClass,
      /* 50 */ &transientShellWidgetClass,
      /* 51 */ &wmShellWidgetClass,
      /* 52 */ &xmPushButtonWidgetClass, 
#ifndef MOTIF1_1
      /* 53 */ &xmTearOffButtonWidgetClass,
#else
      /* 53 */ &xmPushButtonWidgetClass,  /* Placeholder for 1.1 */
#endif /* MOTIF1_1 */
      /* 54 */ &xmComboBoxWidgetClass,
      /* 55 */ &xmContainerWidgetClass,
      /* 56 */ &xmIconGadgetClass,
      /* 57 */ &xmNotebookWidgetClass,
      /* 58 */ &xmSpinBoxWidgetClass,
      /* 58 */ (WidgetClass *)NULL
  };

int mvsGetClassCode(widget)
Widget widget;
{
    int i;
    WidgetClass widget_class = XtClass(widget);

    for (i=0; i<MAX_WIDGET_CLASS && mvsWidgetClass[i] != NULL; i++)
        if (widget_class == *(mvsWidgetClass[i]))
            break;

    return(i);

} /* End mvsGetClassCode() */
