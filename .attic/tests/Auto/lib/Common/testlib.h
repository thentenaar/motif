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
/*   $XConsortium: testlib.h /main/11 1995/07/14 10:57:53 drk $ */


/*  Standard C headers  */

#include <stdio.h>
#include <ctype.h>
#include <sys/signal.h>

#ifndef X_NOT_STDC_ENV
#include <stdlib.h>
#include <unistd.h>
#endif

/*  Xt headers  */

#include <X11/Intrinsic.h>
#include <X11/Shell.h>

/*  Xm headers  */

#include <Xm/Xm.h>
#include <Xm/ArrowB.h>
#include <Xm/ArrowBG.h>
#include <Xm/AtomMgr.h>
#include <Xm/BulletinB.h>
#include <Xm/CascadeB.h>
#include <Xm/CascadeBG.h>
#include <Xm/Command.h>
#include <Xm/ComboBox.h>
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
#include <Xm/MwmUtil.h>
#include <Xm/Notebook.h>
#include <Xm/PanedW.h>
#include <Xm/Protocols.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
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

/* definitions used in all test modules */

#define MAX_ARGS  	100
#define MAX_FILE        256
#define MAX_FONT_LEN    64

/*  Global Variables  */

extern XtAppContext  	app_context;
extern Display  	*display;
extern Window	  	rootWindow;
extern Screen		* screen;
extern Widget   	Shell1;
extern Widget		InstructionBox;
extern Widget   	UTMShell;
extern Widget   	UTMDrawingArea;


/* Private Functions */


extern void		CommonUsage(char *test_name);
extern void		CommonGenericCB(Widget w, XtPointer client_data,
			        	XtPointer call_data);
extern void		CommonGetOptions(int *argc, char **argv);
extern void		CommonPause();
extern void		CommonTestInit(int argc, char **argv);
extern void		CommonTestI18NInit(int argc, char **argv, 
					   XtLanguageProc proc,
					   XtPointer client_data);
extern void 		CommonDumpHierarchy(Widget w, FILE *file);
extern char		*CommonCsToRs(XmString cs);
extern Pixel		CommonGetColor(char *colorstr);
extern XmFontList	CommonGetFontList(char *fontstr);
extern void		CommonExtraResources(Arg args[], Cardinal n);

/* stubs for when NOT linking with malloc library */

extern void             MallocInit(); 
extern void             MallocExit();


/* global variables - command line options */

extern Boolean 		instructions;
extern char		*UserData;
extern char             *instruct_file_name;
extern char             *instruct_file_name_orig;
extern int		pause_len;
extern char		test_font[MAX_FONT_LEN];
extern char		default_font[MAX_FONT_LEN];

/* Global Variables for Automation */

extern Boolean		Automation;
extern Boolean 		AutoRecord;
extern Boolean		AutoBatch;
extern Boolean		AutoManual;
extern Boolean 		AutoNoCheck;
extern Boolean 		AutoTrace;
extern Boolean		AutoDelay;
extern Boolean		AutoWindowDumps;
extern Boolean          Command_In_Progress;
extern Boolean          MonitorOn;
extern Boolean          SyncWidgetCreated;
extern int		AutoDelayCycles;
extern Atom             _MOTIF_WM_AUTOMATION;
extern Atom             WM_S0;
enum             WHICH_TIME { START_TIME, END_TIME };

/* redefinition of Motif Creation routines */

#define XmCreateArrowButton		ArrowButtonCreate
#define XmCreateArrowButtonGadget 	ArrowButtonGadgetCreate
#define XmCreateBulletinBoard		BulletinBoardCreate
#define XmCreateBulletinBoardDialog	BulletinBoardDialogCreate
#define XmCreateCascadeButton		CascadeButtonCreate
#define XmCreateCascadeButtonGadget	CascadeButtonGadgetCreate
#define XmCreateCommand			CommandCreate
#define XmCreateComboBox		ComboBoxCreate
#define XmCreateDropDownComboBox	DropDownComboBoxCreate
#define XmCreateDropDownList		DropDownListComboBoxCreate
#define XmCreateNotebook		NotebookCreate
#define XmCreateContainer		ContainerCreate
#define XmCreateIconGadget		IconGadgetCreate
#define XmCreateDialogShell		DialogShellCreate
#define XmCreateDrawingArea		DrawingAreaCreate
#define XmCreateDrawnButton		DrawnButtonCreate
#define XmCreateErrorDialog		ErrorDialogCreate
#define XmCreateFileSelectionBox	FileSelectionBoxCreate
#define XmCreateFileSelectionDialog	FileSelectionDialogCreate
#define XmCreateForm			FormCreate
#define XmCreateFormDialog		FormDialogCreate
#define XmCreateFrame			FrameCreate
#define XmCreateInformationDialog	InformationDialogCreate
#define XmCreateLabel			LabelCreate
#define XmCreateLabelGadget		LabelGadgetCreate
#define XmCreateList			ListCreate
#define XmCreateMainWindow		MainWindowCreate
#define XmCreateMenuBar			MenuBarCreate
#define XmCreateMenuShell		MenuShellCreate
#define XmCreateMessageBox		MessageBoxCreate
#define XmCreateMessageDialog		MessageDialogCreate
#define XmCreateOptionMenu		OptionMenuCreate
#define XmCreatePanedWindow		PanedWindowCreate
#define XmCreatePopupMenu		PopupMenuCreate
#define XmCreatePromptDialog		PromptDialogCreate
#define XmCreatePulldownMenu		PulldownMenuCreate
#define XmCreatePushButton		PushButtonCreate
#define XmCreatePushButtonGadget	PushButtonGadgetCreate
#define XmCreateQuestionDialog		QuestionDialogCreate
#define XmCreateRadioBox		RadioBoxCreate
#define XmCreateRowColumn		RowColumnCreate
#define XmCreateScale			ScaleCreate
#define XmCreateScrollBar		ScrollBarCreate
#define XmCreateScrolledList		ScrolledListCreate
#define XmCreateScrolledText		ScrolledTextCreate
#define XmCreateScrolledWindow		ScrolledWindowCreate
#define XmCreateSelectionBox		SelectionBoxCreate
#define XmCreateSelectionDialog		SelectionDialogCreate
#define XmCreateSeparator		SeparatorCreate
#define XmCreateSeparatorGadget		SeparatorGadgetCreate
#define XmCreateSpinBox			SpinBoxCreate
#define XmCreateTemplateDialog		TemplateDialogCreate
#define XmCreateText			TextCreate
#define XmCreateTextField		TextFieldCreate
#define XmCreateToggleButton		ToggleButtonCreate
#define XmCreateToggleButtonGadget	ToggleButtonGadgetCreate
#define XmCreateWarningDialog		WarningDialogCreate
#define XmCreateWorkArea		WorkAreaCreate
#define XmCreateWorkingDialog		WorkingDialogCreate 
#define XtAppCreateShell		AppShellCreate
#define XtCreatePopupShell		PopupShellCreate
#define XtAppMainLoop			AppMainLoop 
#define XtSetValues			SetValues
#define XtDestroyWidget			DestroyWidget



extern Widget ArrowButtonCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget ArrowButtonGadgetCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget BulletinBoardCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget BulletinBoardDialogCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget CascadeButtonCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget CascadeButtonGadgetCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget CommandCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget ComboBoxCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget DropDownComboBoxCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget DropDownListComboBoxCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget ContainerCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget DialogShellCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget DrawingAreaCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget DrawnButtonCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget ErrorDialogCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget FileSelectionBoxCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget FileSelectionDialogCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget FormCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget FormDialogCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget FrameCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget InformationDialogCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget IconGadgetCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget LabelCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget LabelGadgetCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget ListCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget MainWindowCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget MenuBarCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget MenuShellCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget MessageBoxCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget MessageDialogCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget NotebookCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget OptionMenuCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget PanedWindowCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget PopupMenuCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget PromptDialogCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget PulldownMenuCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget PushButtonCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget PushButtonGadgetCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget QuestionDialogCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget RadioBoxCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget RowColumnCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget ScaleCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget ScrollBarCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget ScrolledListCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget ScrolledTextCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget ScrolledWindowCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget SelectionBoxCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget SelectionDialogCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget SeparatorCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget SeparatorGadgetCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget SpinBoxCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget TemplateDialogCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget TextCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget TextFieldCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget ToggleButtonCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget ToggleButtonGadgetCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget WarningDialogCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget WorkingDialogCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget WorkAreaCreate( 
                        Widget p,
                        char *name,
                        ArgList args,
                        Cardinal n);
extern Widget PopupShellCreate( 
                        String name,
                        WidgetClass widget_class,
                        Widget parent,
                        ArgList args,
                        Cardinal num_args);
extern Widget AppShellCreate( 
                        String app_name,
                        String app_class,
                        WidgetClass widget_class,
                        Display *display_in,
                        ArgList args,
                        Cardinal num_args);
extern void AppMainLoop( 
                        XtAppContext app);
extern void SetValues( 
                        register Widget w,
                        ArgList args,
                        Cardinal num_args);
extern void DestroyWidget( 
                        Widget widget);


extern void UTMDestinationProc( 
                        Widget widget,
			XtPointer client_data,
			XtPointer call_data);


