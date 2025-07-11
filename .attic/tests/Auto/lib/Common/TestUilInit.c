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
static char rcsid[] = "$XConsortium: TestUilInit.c /main/7 1995/07/14 10:57:36 drk $"
#endif
#endif

#include <AutoMessages.h>
#include <signal.h>
#include <X11/IntrinsicP.h>
#include "testlib.h"
  
/* This is used for checking Synchronization 
   used with performance testing */
#ifdef DEBUG
typedef struct {
  Boolean synchronous;
} SynchronRec;

SynchronRec Synch;

#define Offset(field) XtOffsetOf(SynchronRec, field)

XtResource resources[] = {
  {"synchronous", "Synchronous", XtRBoolean, sizeof(Boolean),
   Offset(synchronous), XtRImmediate, (XtPointer) FALSE}
};

#undef Offset

XrmOptionDescRec optionDesc[] = {
  {"-synchronous", "*synchronous", XrmoptionNoArg, (XtPointer) "on"}};

if (synchronous == False)  {
  OverridePerformanceMonitor = True;
}
#endif


/*************************************************************************
 *                                                                       *
 *      Name:           CommonTestUilInit()                              *
 *								 	 *
 *	Purpose:	This routine calls CommonTestInit wich  will     *
 *                      Initialize Xt, create an app context, create     *
 *			a connection to the display and create a         *
 *			toplevel application shell.  The routine also    *
 *                      sets up for Uil.                                 *
 *									 *
 *	Return:		Nothing						 *
 *									 *
 *************************************************************************/

void CommonTestUilInit(int argc, char **argv)
     
{

  CommonTestInit(argc, argv);

  MrmRegisterClass(0, NULL, "XmCreateArrowButton", 
		   ArrowButtonCreate,
		   (WidgetClass)&xmArrowButtonWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreateArrowButtonGadget", 
		   ArrowButtonGadgetCreate,
		   (WidgetClass)&xmArrowButtonGadgetClass);
  MrmRegisterClass(0, NULL, "XmCreateBulletinBoard", 
		   BulletinBoardCreate,
		   (WidgetClass)&xmBulletinBoardWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreateBulletinBoardDialog", 
		   BulletinBoardDialogCreate,
		   (WidgetClass)&xmBulletinBoardWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreateCascadeButton", 
		   CascadeButtonCreate,
		   (WidgetClass)&xmCascadeButtonWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreateCascadeButtonGadget", 
		   CascadeButtonGadgetCreate,
		   (WidgetClass)&xmCascadeButtonGadgetClass);
  MrmRegisterClass(0, NULL, "XmCreateCommand", 
		   CommandCreate,
		   (WidgetClass)&xmCommandWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreateDialogShell", 
		   DialogShellCreate,
		   (WidgetClass)&xmDialogShellWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreateDrawingArea", 
		   DrawingAreaCreate,
		   (WidgetClass)&xmDrawingAreaWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreateDrawnButton", 
		   DrawnButtonCreate,
		   (WidgetClass)&xmDrawnButtonWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreateErrorDialog", 
		   ErrorDialogCreate,
		   (WidgetClass)&xmMessageBoxWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreateMessageBox",
		   MessageBoxCreate,
		   (WidgetClass)&xmMessageBoxWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreateMessageDialog",
		   MessageDialogCreate,
		   (WidgetClass)&xmMessageBoxWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreateInformationDialog",
		   InformationDialogCreate,
		   (WidgetClass)&xmMessageBoxWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreateTemplateDialog",
		   TemplateDialogCreate,
		   (WidgetClass)&xmMessageBoxWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreateQuestionDialog",
		   QuestionDialogCreate,
		   (WidgetClass)&xmMessageBoxWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreateWarningDialog",
		   WarningDialogCreate,
		   (WidgetClass)&xmMessageBoxWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreateWorkingDialog",
		   WorkingDialogCreate,
		   (WidgetClass)&xmMessageBoxWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreateFileSelectionBox",
		   FileSelectionBoxCreate,
		   (WidgetClass)&xmFileSelectionBoxWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreateFileSelectionDialog",
		   FileSelectionDialogCreate,
		   (WidgetClass)&xmFileSelectionBoxWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreateForm",
		   FormCreate,
		   (WidgetClass)&xmFormWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreateFormDialog",
		   FormDialogCreate,
		   (WidgetClass)&xmFormWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreateFrame",
		   FrameCreate,
		   (WidgetClass)&xmFrameWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreateLabel",
		   LabelCreate,
		   (WidgetClass)&xmLabelWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreateLabelGadget",
		   LabelGadgetCreate,
		   (WidgetClass)&xmLabelGadgetClass);
  MrmRegisterClass(0, NULL, "XmCreateList",
		   ListCreate,
		   (WidgetClass)&xmListWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreateScrolledList",
		   ScrolledListCreate,
		   (WidgetClass)&xmListWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreateMainWindow",
		   MainWindowCreate,
		   (WidgetClass)&xmMainWindowWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreateMenuShell",
		   MenuShellCreate,
		   (WidgetClass)&xmMenuShellWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreateMenuBar",
		   MenuBarCreate,
		   (WidgetClass)&xmRowColumnWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreateOptionMenu",
		   OptionMenuCreate,
		   (WidgetClass)&xmRowColumnWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreatePopupMenu",
		   PopupMenuCreate,
		   (WidgetClass)&xmRowColumnWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreatePulldownMenu",
		   PulldownMenuCreate,
		   (WidgetClass)&xmRowColumnWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreateRowColumn",
		   RowColumnCreate,
		   (WidgetClass)&xmRowColumnWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreateRadioBox",
		   RadioBoxCreate,
		   (WidgetClass)&xmRowColumnWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreateWorkArea",
		   WorkAreaCreate,
		   (WidgetClass)&xmRowColumnWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreatePanedWindow",
		   PanedWindowCreate,
		   (WidgetClass)&xmPanedWindowWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreatePromptDialog",
		   PromptDialogCreate,
		   (WidgetClass)&xmSelectionBoxWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreateSelectionBox",
		   SelectionBoxCreate,
		   (WidgetClass)&xmSelectionBoxWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreateSelectionDialog",
		   SelectionDialogCreate,
		   (WidgetClass)&xmSelectionBoxWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreatePushButton",
		   PushButtonCreate,
		   (WidgetClass)&xmPushButtonWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreatePushButtonGadget",
		   PushButtonGadgetCreate,
		   (WidgetClass)&xmPushButtonGadgetClass);
  MrmRegisterClass(0, NULL, "XmCreateScale",
		   ScaleCreate,
		   (WidgetClass)&xmScaleWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreateScrollBar",
		   ScrollBarCreate,
		   (WidgetClass)&xmScrollBarWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreateScrolledText",
		   ScrolledTextCreate,
		   (WidgetClass)&xmTextWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreateText",
		   TextCreate,
		   (WidgetClass)&xmTextWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreateScrolledWindow",
		   ScrolledWindowCreate,
		   (WidgetClass)&xmScrolledWindowWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreateSeparator",
		   SeparatorCreate,
		   (WidgetClass)&xmSeparatorWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreateSeparatorGadget",
		   SeparatorGadgetCreate,
		   (WidgetClass)&xmSeparatorGadgetClass);
  MrmRegisterClass(0, NULL, "XmCreateTextField",
		   TextFieldCreate,
		   (WidgetClass)&xmTextFieldWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreateToggleButton",
		   ToggleButtonCreate,
		   (WidgetClass)&xmToggleButtonWidgetClass);
  MrmRegisterClass(0, NULL, "XmCreateToggleButtonGadget",
		   ToggleButtonGadgetCreate,
		   (WidgetClass)&xmToggleButtonGadgetClass);  
  MrmRegisterClass(0, NULL, "XmCreateComboBoxWidgetClass",
		   ComboBoxCreate,
		   (WidgetClass)&xmComboBoxWidgetClass);  
  MrmRegisterClass(0, NULL, "XmCreateSpinBoxWidgetClass",
		   SpinBoxCreate,
 		   (WidgetClass)&xmSpinBoxWidgetClass);  
  MrmRegisterClass(0, NULL, "XmCreateNotebookWidgetClass",
		   NotebookCreate,
		   (WidgetClass)&xmNotebookWidgetClass);  
  MrmRegisterClass(0, NULL, "XmCreateContainerWidgetClass",
		   ContainerCreate,
		   (WidgetClass)&xmContainerWidgetClass);  
  MrmRegisterClass(0, NULL, "XmCreateIconGadgetClass",
		   IconGadgetCreate,
		   (WidgetClass)&xmIconGadgetClass);  


/* The may be useful for checking Synchronization
   use with performance testing */
#ifdef DEBUG

     XtGetApplicationResources(Shell1, (XtPointer) &Synch, 
			       resources,
			       XtNumber(resources), (Arg *) NULL, 0 );

     printf("sync = %d\n", Synch.synchronous);

#endif /* DEBUG */

}




