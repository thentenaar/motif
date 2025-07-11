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
/*   $XConsortium: testlib.h /main/11 1995/07/14 12:00:02 drk $ */


/*  Standard C headers  */

#include <stdio.h>
#include <ctype.h>
#include <sys/signal.h>

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
#include <Xm/MwmUtil.h>
#include <Xm/Notebook.h>
#include <Xm/PanedW.h>
#include <Xm/Protocols.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/Scale.h>
#include <Xm/Screen.h>
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
extern void             CommonExtraResources(Arg args[], Cardinal n);

/* stubs for when NOT linking with malloc library */

extern void             MallocInit(); 
extern void             MallocExit();


/* global variables - command line options */

extern FILE 		*output_file;
extern Boolean 		instructions;
extern char		*UserData;
extern char		*TestName;
extern char             *instruct_file_name;
extern char             *instruct_file_name_orig;
extern int		pause_len;
extern char		test_font[MAX_FONT_LEN];
extern char		default_font[MAX_FONT_LEN];
