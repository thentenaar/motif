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
static char rcsid[] = "$XConsortium: MapClass.c /main/8 1995/07/14 11:24:46 drk $"
#endif
#endif
/***********************************************************************
  	@(#)MapClass.c	1.6.1.1	Date:1/22/91
  	Author: PSN
	History:
	Calls:

	Summary:	This procedure initializes the array of three 
	different widget class codes by proper application lifetime values
	and facilitates mapping of these values into one-another.
************************************************************************/
#include	"mvslib.h"

externalref WidgetClass	xmArrowButtonWidgetClass;
externalref WidgetClass	xmArrowButtonGadgetClass;
externalref WidgetClass	xmCascadeButtonWidgetClass;
externalref WidgetClass	xmCascadeButtonGadgetClass;
externalref WidgetClass	xmIconGadgetClass;
externalref WidgetClass	xmDrawnButtonWidgetClass;
externalref WidgetClass	xmPushButtonWidgetClass;
externalref WidgetClass	xmPushButtonGadgetClass;
externalref WidgetClass	xmToggleButtonWidgetClass;
externalref WidgetClass	xmToggleButtonGadgetClass;

externalref WidgetClass	xmLabelWidgetClass;
externalref WidgetClass	xmLabelGadgetClass;
externalref WidgetClass	xmListWidgetClass;
externalref WidgetClass	xmScrollBarWidgetClass;
externalref WidgetClass	xmSeparatorGadgetClass;
externalref WidgetClass	xmSeparatorWidgetClass;
externalref WidgetClass	xmTextWidgetClass;

externalref WidgetClass	xmBulletinBoardWidgetClass;
externalref WidgetClass xmComboBoxWidgetClass;
externalref WidgetClass	xmCommandWidgetClass;
externalref WidgetClass	xmContainerWidgetClass;
externalref WidgetClass	xmDrawingAreaWidgetClass;
externalref WidgetClass	xmFileSelectionBoxWidgetClass;
externalref WidgetClass	xmFormWidgetClass;
externalref WidgetClass	xmFrameWidgetClass;
externalref WidgetClass	xmMainWindowWidgetClass;
externalref WidgetClass	xmMessageBoxWidgetClass;
externalref WidgetClass	xmNotebookWidgetClass;
externalref WidgetClass	xmPanedWindowWidgetClass;
externalref WidgetClass	xmScaleWidgetClass;
externalref WidgetClass	xmScrolledWindowWidgetClass;
externalref WidgetClass	xmSelectionBoxWidgetClass;
externalref WidgetClass	xmSpinBoxWidgetClass;
externalref WidgetClass	xmTextFieldWidgetClass;
externalref WidgetClass	xmRowColumnWidgetClass;

/* Defined in mvslib.h 
** 
** #define MAXCLASS	50
** 
** typedef struct _ClassRef {
**     WidgetClass		motif_class;
**     int			mvs_class;
**     MvsWidgetClassInfo	*inst_class; 
** } ClassRef;
** 
** ClassRef	MotifClasses[MAXCLASS];
*/ 

void
SetupMotifClass()
{
    static int	done=0;
    register int	i=0;	/* Checkout #define MAXCLASS 55, if you add */
				/* more classes (loc mvs_defines.h) */
    if (done == 0) {
	done++;

	MotifClasses[i].motif_class  =    xmArrowButtonWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmArrowButtonWidgetClass;
	MotifClasses[i].inst_class   =   iXmArrowButtonWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmArrowButtonGadgetClass; 
	MotifClasses[i].mvs_class    = mvsXmArrowButtonGadgetClass;
	MotifClasses[i].inst_class   =   iXmArrowButtonGadgetClass; i++;

	MotifClasses[i].motif_class  =    xmCascadeButtonWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmCascadeButtonWidgetClass;
	MotifClasses[i].inst_class   =   iXmCascadeButtonWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmCascadeButtonGadgetClass; 
	MotifClasses[i].mvs_class    = mvsXmCascadeButtonGadgetClass;
	MotifClasses[i].inst_class   =   iXmCascadeButtonGadgetClass; i++;

	MotifClasses[i].motif_class  =    xmDrawnButtonWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmDrawnButtonWidgetClass;
	MotifClasses[i].inst_class   =   iXmDrawnButtonWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmIconGadgetClass; 
	MotifClasses[i].mvs_class    = mvsXmIconGadgetClass;
	MotifClasses[i].inst_class   =   iXmIconGadgetClass; i++;

	MotifClasses[i].motif_class  =    xmPushButtonWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmPushButtonWidgetClass;
	MotifClasses[i].inst_class   =   iXmPushButtonWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmPushButtonGadgetClass; 
	MotifClasses[i].mvs_class    = mvsXmPushButtonGadgetClass;
	MotifClasses[i].inst_class   =   iXmPushButtonGadgetClass; i++;

	MotifClasses[i].motif_class  =    xmToggleButtonWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmToggleButtonWidgetClass;
	MotifClasses[i].inst_class   =   iXmToggleButtonWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmToggleButtonGadgetClass; 
	MotifClasses[i].mvs_class    = mvsXmToggleButtonGadgetClass;
	MotifClasses[i].inst_class   =   iXmToggleButtonGadgetClass; i++;


	MotifClasses[i].motif_class  =    xmLabelWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmLabelWidgetClass;
	MotifClasses[i].inst_class   =   iXmLabelWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmLabelGadgetClass; 
	MotifClasses[i].mvs_class    = mvsXmLabelGadgetClass;
	MotifClasses[i].inst_class   =   iXmLabelGadgetClass; i++;

	MotifClasses[i].motif_class  =    xmListWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmListWidgetClass;
	MotifClasses[i].inst_class   =   iXmListWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmScrollBarWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmScrollBarWidgetClass;
	MotifClasses[i].inst_class   =   iXmScrollBarWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmSeparatorGadgetClass; 
	MotifClasses[i].mvs_class    = mvsXmSeparatorGadgetClass;
	MotifClasses[i].inst_class   =   iXmSeparatorGadgetClass; i++;

	MotifClasses[i].motif_class  =    xmSeparatorWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmSeparatorWidgetClass;
	MotifClasses[i].inst_class   =   iXmSeparatorWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmTextWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmTextWidgetClass;
	MotifClasses[i].inst_class   =   iXmTextWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmTextFieldWidgetClass;
	MotifClasses[i].mvs_class    = mvsXmTextFieldWidgetClass;
	MotifClasses[i].inst_class   =   iXmTextFieldWidgetClass; i++;


	MotifClasses[i].motif_class  =    xmBulletinBoardWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmBulletinBoardWidgetClass;
	MotifClasses[i].inst_class   =   iXmBulletinBoardWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmComboBoxWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmComboBoxWidgetClass;
	MotifClasses[i].inst_class   =   iXmComboBoxWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmCommandWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmCommandWidgetClass;
	MotifClasses[i].inst_class   =   iXmCommandWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmContainerWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmContainerWidgetClass;
	MotifClasses[i].inst_class   =   iXmContainerWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmDrawingAreaWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmDrawingAreaWidgetClass;
	MotifClasses[i].inst_class   =   iXmDrawingAreaWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmFileSelectionBoxWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmFileSelectionBoxWidgetClass;
	MotifClasses[i].inst_class   =   iXmFileSelectionBoxWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmFormWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmFormWidgetClass;
	MotifClasses[i].inst_class   =   iXmFormWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmFrameWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmFrameWidgetClass;
	MotifClasses[i].inst_class   =   iXmFrameWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmMainWindowWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmMainWindowWidgetClass;
	MotifClasses[i].inst_class   =   iXmMainWindowWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmMessageBoxWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmMessageBoxWidgetClass;
	MotifClasses[i].inst_class   =   iXmMessageBoxWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmNotebookWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmNotebookWidgetClass;
	MotifClasses[i].inst_class   =   iXmNotebookWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmPanedWindowWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmPanedWindowWidgetClass;
	MotifClasses[i].inst_class   =   iXmPanedWindowWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmScaleWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmScaleWidgetClass;
	MotifClasses[i].inst_class   =   iXmScaleWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmScrolledWindowWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmScrolledWindowWidgetClass;
	MotifClasses[i].inst_class   =   iXmScrolledListWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmScrolledWindowWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmScrolledWindowWidgetClass;
	MotifClasses[i].inst_class   =   iXmScrolledTextWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmScrolledWindowWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmScrolledWindowWidgetClass;
	MotifClasses[i].inst_class   =   iXmScrolledWindowWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmSelectionBoxWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmSelectionBoxWidgetClass;
	MotifClasses[i].inst_class   =   iXmSelectionBoxWidgetClass; i++;

 	MotifClasses[i].motif_class  =    xmSpinBoxWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmSpinBoxWidgetClass;
	MotifClasses[i].inst_class   =   iXmSpinBoxWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmMessageBoxWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmMessageBoxWidgetClass;
	MotifClasses[i].inst_class   =   iXmBulletinBoardDialogWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmMessageBoxWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmMessageBoxWidgetClass;
	MotifClasses[i].inst_class   =   iXmErrorDialogWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmFileSelectionBoxWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmFileSelectionBoxWidgetClass;
	MotifClasses[i].inst_class   =   iXmFileSelectionDialogWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmFormWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmFormWidgetClass;
	MotifClasses[i].inst_class   =   iXmFormDialogWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmMessageBoxWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmMessageBoxWidgetClass;
	MotifClasses[i].inst_class   =   iXmInformationDialogWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmMessageBoxWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmMessageBoxWidgetClass;
	MotifClasses[i].inst_class   =   iXmMessageDialogWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmSelectionBoxWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmSelectionBoxWidgetClass;
	MotifClasses[i].inst_class   =   iXmPromptDialogWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmMessageBoxWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmMessageBoxWidgetClass;
	MotifClasses[i].inst_class   =   iXmQuestionDialogWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmMessageBoxWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmMessageBoxWidgetClass;
	MotifClasses[i].inst_class   =   iXmTemplateDialogWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmSelectionBoxWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmSelectionBoxWidgetClass;
	MotifClasses[i].inst_class   =   iXmSelectionDialogWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmMessageBoxWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmMessageBoxWidgetClass;
	MotifClasses[i].inst_class   =   iXmWarningDialogWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmMessageBoxWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmMessageBoxWidgetClass;
	MotifClasses[i].inst_class   =   iXmWorkingDialogWidgetClass; i++;


	MotifClasses[i].motif_class  =    xmRowColumnWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmMenuBarWidgetClass;
	MotifClasses[i].inst_class   =   iXmMenuBarWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmRowColumnWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmOptionMenuWidgetClass;
	MotifClasses[i].inst_class   =   iXmOptionMenuWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmRowColumnWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmPopupMenuWidgetClass;
	MotifClasses[i].inst_class   =   iXmPopupMenuWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmRowColumnWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmPulldownMenuWidgetClass;
	MotifClasses[i].inst_class   =   iXmPulldownMenuWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmRowColumnWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmRowColumnWidgetClass;
	MotifClasses[i].inst_class   =   iXmRowColumnWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmRowColumnWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmRadioBoxWidgetClass;
	MotifClasses[i].inst_class   =   iXmRadioBoxWidgetClass; i++;

	MotifClasses[i].motif_class  =    xmRowColumnWidgetClass; 
	MotifClasses[i].mvs_class    = mvsXmWorkAreaWidgetClass;
	MotifClasses[i].inst_class   =   iXmWorkAreaWidgetClass; i++;

	MotifClasses[i].motif_class  = NULL; 
	MotifClasses[i].mvs_class    = -1;
	MotifClasses[i].inst_class   = NULL;
    }
}

/*
** #define MOTIF_CLASS     0x1
** #define MVS_CLASS       0x2
** #define INST_CLASS      0x4
**
** typedef union _ClassRefEle {
**     WidgetClass         motif_class;
**     int                 mvs_class;
**     MvsWidgetClassInfo  *inst_class;
** } ClassRefEle;
*/

ClassRef *
ConvertClass (ref_val, ref_typ)
ClassRefEle   ref_val;
int ref_typ;
{
    static int	first_time = 0;
    ClassRef	*result = MotifClasses;
    register int i=0, int_val;

    if (first_time == 0) {
	SetupMotifClass();
	first_time++;
    }

    switch (ref_typ) {
	case MOTIF_CLASS:
	   while (result[i].motif_class != NULL && 
		  result[i].motif_class != ref_val.motif_class)
	   	i++;
	   break;
	case MVS_CLASS:
	   while (result[i].mvs_class != -1 && 
		  result[i].mvs_class != ref_val.mvs_class)
	   	i++;
	   break;
	case INST_CLASS:
	   while (result[i].inst_class != NULL && 
		  result[i].inst_class != ref_val.inst_class)
	   	i++;
	   break;
	default:
	   result = (ClassRef *)NULL;
	   break;
    }

    if (result == (ClassRef *)NULL || result[i].mvs_class == -1) {
	/* msg_trace("ConvertClass: Invalid option\n"); */
	return ((ClassRef *)NULL);
    }
    return (&result[i]);
}

