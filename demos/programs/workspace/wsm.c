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
static char rcsid[] = "$TOG: wsm.c /main/8 1997/05/02 10:08:35 dbl $"
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <Xm/Xm.h>

#include "wsmSend.h"
#include "wsm.h"
#include "wsm_ui.h"
#include "wsmStruct.h"
#include "xrmLib.h"
#include "wsmDebug.h"
#include "wsmData.h"
#include "wsm_cb.h"
#include "wsm_create.h"
#include "command_ui.h"
#include "Wsm/utm_send.h"

#include <Xm/DrawingA.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/Text.h>
#include <Xm/PanedW.h>
#include <Xm/TransferP.h>

extern Atom WM_S0;
extern Atom _MOTIF_WM_INVOKE_COMMAND;

extern Time GetTimestamp (Display *dpy);


/* globals */

Widget utmShell;
AtomStruct atoms;
WSM_UI* wsm_ui;
Boolean mwm_gone =False;
Boolean mwm_reconnect = False;
Boolean diffs_allowed = False;
Widget shell;
Widget shell_ui;
Display *dsp;
Space * current_space = NULL;
Space *all_space;
Space *space_list = NULL;
WSMConfigFormatReply *config_format_reply = NULL;
XrmQuark *space_qlist;
WorkWindow *work_windows;
Window mwm_window;
IndexStruct wsm_index;
int num_window_attrib;
int num_icon_attrib;
int num_global_attrib;
XrmQuark *window_attribq;
XrmQuark *icon_attribq;
XrmQuark *global_attribq;
WSMAttribute *window_attrib_list;
WSMAttribute *icon_attrib_list;
WSMAttribute *global_attrib_list;
Boolean connected = False;

char *file_name;
Space *from_space;
Space *to_space;
Boolean wsm_shown;


typedef struct {
  String file_name;
  String *workspace_list;
  String *background_list;
  String *label_pixmap_list;
  String current_workspace;
  Boolean show_dialog;
  Boolean show_menu;
} AppData;


typedef struct {
  Window window;
  Space *from_space;
  Space *to_space;
} CopyStruct;

AppData app_data;

static String fallbacks[] = {
    "*title: WSM",
    "*nameShell.title:			Workspace Name",
    "*backgroundShell.title:		Workspace Background",
    "*deleteShell.title:		Delete Workspace",
    "*configureShell.title:		Configure Workspace",
    "*saveAsShell.title:		Save As",
    "*occupyShell.title:		Occupy",

    "*newButton.labelString:		New Workspace",
    "*nameButton.labelString:		Rename Workspaces...",
    "*configureButton.labelString:	Configure Workspaces...",
    "*deleteButton.labelString:		Delete Workspaces...",
    "*hideButton.labelString:		Hide Workspace Manager",
    "*saveButton.labelString:		Save",
    "*saveAsButton.labelString:		Save As...",
    "*exitButton.labelString:		Exit",
    "*backgroundButton.labelString:	Set Background...",
    "*fileCascade.labelString:		File",
    "*fileCascade.mnemonic:		F",
    "*workspaceCascade.labelString:	Workspace",
    "*workspaceCascade.mnemonic:	W",
    "*viewCascade.labelString:		View",
    "*viewCascade.mnemonic:		V",
    "*saveButton.mnemonic:		S",
    "*saveAsButton.mnemonic:		A",
    "*exitButton.mnemonic:		x",
    "*backgroundButton.mnemonic:	B",
    "*newButton.mnemonic:		N",
    "*nameButton.mnemonic:		R",
    "*configureButton.mnemonic:		C",
    "*deleteButton.mnemonic:		D",
    "*hideButton.mnemonic:		H",

    "*applyButton.labelString:		Apply",
    "*dismissButton.labelString:	Dismiss",
    "*cancelButton.labelString:		Cancel",
    "*okButton.labelString:	Ok",
    "*cancelButton.labelString:	Cancel",

    "*configureShell*titleLabel.labelString:	Configure Workspace",
    "*configureShell*copyButton.labelString:	Copy",
    "*configureShell*linkButton.labelString:	Link",
    "*configureShell*occupyButton.labelString:  >> Occupy >>",
    "*configureShell*moveButton.labelString:	>> Move >>",
    "*configureShell*deleteButton.labelString:	Remove",
    "*configureShell*clientButton.labelString:	Window",
    "*configureShell*windowButton.labelString:	Client",
    "*configureShell.height:			400",
    "*configureShell.width:			550",
    "*configureShell*fromForm.leftOffset:	10",
    "*configureShell*fromForm.rightOffset:	10",
    "*configureShell*toForm.leftOffset:		10",
    "*configureShell*toForm.rightOffset:	10",
    "*configureShell*buttonForm.leftOffset:	-75",
    "*configureShell*toggleRow.topOffset:	50",
    "*configureShell*separator2.bottomOffset:	5",
    "*configureShell*dismissButton.bottomOffset:5",

    "*nameShell*nameTitleLabel.labelString:	Workspace Name",
    "*nameShell*nameLabel.labelString:		Workspace Name",
    "*nameShell*pixmapLabel.labelString:	Label Pixmap",
    "*nameShell*nameList.height:		200",
    "*nameShell*applyButton.leftPosition:		25",
    "*nameShell*dismissButton.leftPosition:		55",
    "*nameShell*nameTitleLabel.topOffset:			5",
    "*nameShell*nameWindow.leftOffset:			5",
    "*nameShell*nameWindow.rightOffset:		5",
    "*nameShell*nameWindow.topOffset:			5",
    "*nameShell*nameWindow.bottomOffset:		5",
    "*nameShell*wnameForm.leftOffset:		5",
    "*nameShell*wnameForm.rightOffset:		5",
    "*nameShell*pixForm.leftOffset:		5",
    "*nameShell*pixForm.rightOffset:		5",

    "*backgroundShell*backgroundTitleLabel.labelString:	Workspace Background",
    "*backgroundShell*backgroundLabel.labelString:	Workspace Background",
    "*backgroundShell*backgroundList.height:		200",
    "*backgroundShell*applyButton.leftPosition:		25",
    "*backgroundShell*dismissButton.leftPosition:		55",
    "*backgroundShell*backgroundTitleLabel.topOffset:			5",
    "*backgroundShell*backgroundWindow.leftOffset:			5",
    "*backgroundShell*backgroundWindow.rightOffset:		5",
    "*backgroundShell*backgroundWindow.topOffset:			5",
    "*backgroundShell*backgroundWindow.bottomOffset:		5",
    "*backgroundShell*backgroundForm.leftOffset:		5",
    "*backgroundShell*backgroundForm.rightOffset:		5",

    "*deleteShell*deleteTitleLabel.labelString:	Delete Workspace",
    "*deleteShell*deleteList.height:		200",
    "*deleteShell*applyButton.leftPosition:		0",
    "*deleteShell*dismissButton.leftPosition:		45",
    "*deleteShell*deleteTitleLabel.topOffset:			5",
    "*deleteShell*deleteWindow.leftOffset:			5",
    "*deleteShell*deleteWindow.rightOffset:		5",
    "*deleteShell*deleteWindow.topOffset:			5",
    "*deleteShell*deleteWindow.bottomOffset:		5",

    "*saveAsShell*saveAsTitleLabel.labelString:	Save As",
    "*saveAsShell*saveAsLabel.labelString:	Database Name",
    "*saveAsShell*okButton.leftPosition:		20",
    "*saveAsShell*cancelButton.leftPosition:		50",
    "*saveAsShell*saveAsTitleLabel.topOffset:			5",
    "*saveAsShell*dbForm.leftOffset:			5",
    "*saveAsShell*dbForm.rightOffset:		5",
    "*saveAsShell*dbForm.topOffset:			5",
    "*saveAsShell*dbForm.bottomOffset:		5",

    "*occupyShell*occupyTitleLabel.labelString:	Occupy Workspace",
    "*occupyShell*copyButton.labelString:	Copy",
    "*occupyShell*linkButton.labelString:	Link",
    "*occupyShell*okButton.leftPosition:		20",
    "*occupyShell*cancelButton.leftPosition:		50",
    "*occupyShell*occupyList.width:			175",
    "*occupyShell*occupyList.height:			200",
    "*occupyShell*occupyTitleLabel.topOffset:	5",
    "*occupyShell*occupyTitleLabel.leftOffset:	5",
    "*occupyShell*occupyTitleLabel.rightOffset:	5",
    "*occupyShell*occupyWindow.topOffset:		5",
    "*occupyShell*occupyWindow.leftOffset:		5",
    "*occupyShell*occupyWindow.rightOffset:		5",
    "*occupyShell*toggleRow.topOffset:		5",
    "*occupyShell*toggleRow.leftOffset:		30",

    NULL,
};

static XtResource resources[] = {
  {
    "fileName",
    "FileName",
    XtRString,
    sizeof(String),
    XtOffsetOf(AppData, file_name),
    XtRString,
    (XtPointer) NULL,
    },
  {
    "workspaceList",
    "WorkspaceList",
    XtRStringTable,
    sizeof(XtRStringTable),
    XtOffsetOf(AppData, workspace_list),
    XtRString,
    (XtPointer) NULL,
    },
  {
    "backgroundList",
    "BackgroundList",
    XtRStringTable,
    sizeof(XtRStringTable),
    XtOffsetOf(AppData, background_list),
    XtRString,
    (XtPointer) NULL,
    },
  {
    "labelPixmapList",
    "LabelPixmapList",
    XtRStringTable,
    sizeof(XtRStringTable),
    XtOffsetOf(AppData, label_pixmap_list),
    XtRString,
    (XtPointer) NULL,
    },
  {
    "currentWorkspace",
    "CurrentWorkspace",
    XtRString,
    sizeof(XtRString),
    XtOffsetOf(AppData, current_workspace),
    XtRString,
    (XtPointer) NULL,
    },
  {
    "showWSDialog",
    "ShowWSDialog",
    XtRBoolean,
    sizeof(Boolean),
    XtOffsetOf(AppData, show_dialog),
    XtRImmediate,
    (XtPointer) True,
    },
  {
    "useMenuBar",
    "UseMenuBar",
    XtRBoolean,
    sizeof(Boolean),
    XtOffsetOf(AppData, show_menu),
    XtRImmediate,
    (XtPointer) True,
    },
};

static XrmOptionDescRec options[] = {
  {"-fileName",	"*fileName", XrmoptionSepArg, NULL},
  {"-file",	"*fileName", XrmoptionSepArg, NULL},
  {"-f",	"*fileName", XrmoptionSepArg, NULL},
  {"-workspaceList", "*workspaceList", XrmoptionSepArg, NULL},
  {"-wL", "*workspaceList", XrmoptionSepArg, NULL},
  {"-backgroundList", "*backgroundList", XrmoptionSepArg, NULL},
  {"-bL", "*backgroundList", XrmoptionSepArg, NULL},
 {"-labelPixmapList", "*labelPixmapList", XrmoptionSepArg, NULL},
  {"-labelPL", "*labelPixmapList", XrmoptionSepArg, NULL},
  {"-currentWorkspace", "*currentWorkspace", XrmoptionSepArg, NULL},
  {"-cW", "*currentWorkspace", XrmoptionSepArg, NULL},
  {"-showWSDialog", "showWSDialog", XrmoptionSepArg, NULL},
  {"-s", "*showWSDialog", XrmoptionSepArg, NULL},
};

Widget text;

static void UTMConvertProc(
Widget w, XtPointer clientData, XtPointer callData
);

static Boolean WsmConvertProc(
Widget, Atom *, Atom *, XtPointer, unsigned long,
int, Atom *, XtPointer *, unsigned long *, int *
);

static void SetupWsmSignalHandlers(
int
);

static void QuitWsmSignalHandler(
int
);

void
GetFileName(char **name)
{
  /* get dbase file name */

  char *home_name = getenv("HOME");

  if (app_data.file_name != NULL)
    {
      if (app_data.file_name[0] != '/')
	{
	  *name = XtMalloc(sizeof(char)*(strlen(home_name)+ 1
					 + strlen(app_data.file_name)+ 2));
	  strcpy(*name,home_name);
	  strcat(*name, "/");
	  strcat(*name,app_data.file_name);
	}
      else
	{
	  *name = XtMalloc(sizeof(char) *(strlen(app_data.file_name) +1));
	  strcpy(*name,app_data.file_name);
	}
    }
  else
    {
      *name = XtMalloc(sizeof(char) *(strlen(home_name) + 9));
      strcpy(*name,home_name);
      strcat(*name, "/.wsmdb");
    }
}


void
GetWorkspaces(String *str_table)
{
  char *str, strq[20];
  int i;
  /* get workspaces from resources or from dbase file or default to 4 rooms */

  if (app_data.workspace_list != NULL)
    {
      for (i = 0; app_data.workspace_list[i] != NULL; i++)
	{
	  sprintf(strq,"Room%d",i+1);
	  str = (char*) XtMalloc((strlen(app_data.workspace_list[i]) +1)
				 * sizeof(char));
	  strcpy(str, app_data.workspace_list[i]);
	  CreateSpace(XrmStringToQuark(strq),str);
	  XtFree(str);
	}
    }
  else if (str_table != NULL)
    {
      for (i = 0; str_table[i] != NULL; i++)
	{
	  sprintf(strq,"Room%d",i+1);
	  str = (char*) XtMalloc((strlen(str_table[i]) +1) * sizeof(char));
	  strcpy(str, str_table[i]);
	  CreateSpace(XrmStringToQuark(strq),str);
	  XtFree(str);
	}
    }
  else
    {
      str = (char*) XtMalloc( 8* sizeof(char));
      for (i = 1; i < 5; i++)
	{
	  sprintf(str,"Room%d",i);
	  CreateSpace(XrmStringToQuark(str), str);
	}
      XtFree(str);
    }
}



void
GetBackgrounds(String *str_table)
{
  int i;
  Space *space;
  char *cptr;

  /* get backgrounds from resources or from dbase file or default to None */
  if (app_data.background_list != NULL)
    {
      for (i = 0; app_data.background_list[i] != NULL; i++)
	{
	  space = GetSpaceFromID(i);
	  if (space != NULL)
	    {
	      if (app_data.background_list[i][0] == '"')
		{
		  cptr = app_data.background_list[i]+1;
		  SetSpacePixmap(space, cptr);
		}
	      else
		SetSpacePixel(space,app_data.background_list[i]);
	    }
	}
     }
  else if (str_table != NULL)
    {
      for (i = 0; str_table[i] != NULL; i++)
	{
	  space = GetSpaceFromID(i);
	  if (space != NULL)
	    {
	      if (str_table[i][0] == '"')
		{
		  cptr = str_table[i] +1;
		  SetSpacePixmap(space,cptr);
		}
	      else
		SetSpacePixel(space,str_table[i]);
	    }
	  XtFree((XtPointer)str_table[i]);
	}
    }
}


void
GetPixmaps(String *str_table)
{
  int i;
  Space *space;
  /* get pixmap labels from resources or from dbase file or default to None */
  if (app_data.label_pixmap_list != NULL)
    {
      for (i = 0; app_data.label_pixmap_list[i] != NULL; i++)
	{
	  space = GetSpaceFromID(i);
	  if (space != NULL)
	    {
	      SetSpaceLabelPixmap(space,app_data.label_pixmap_list[i]);
	    }
	}
    }
  else if (str_table != NULL)
    {
      for (i = 0; str_table[i] != NULL; i++)
	{
	  space = GetSpaceFromID(i);
	  if (space != NULL)
	    {
	      SetSpaceLabelPixmap(space,str_table[i]);
	    }
	  XtFree((XtPointer)str_table[i]);
	}
      XtFree((XtPointer)str_table);
    }
}




void
GetCurrentSpace(String str)
{
  Space *s;
  PRINT("getting current space\n");
  /* get current space from resources or from dbase file */
  if (app_data.current_workspace != NULL)
    {
      PRINT("from app_data\n");
      s = GetSpaceFromName(app_data.current_workspace);
      if (s == NULL) s = GetSpace(XrmStringToQuark(app_data.current_workspace));
      if (s != NULL) current_space = s;
    }
  else if (str != NULL)
    {
      PRINT("from str\n");
      s = GetSpaceFromName(str);
      if (s == NULL) s = GetSpace(XrmStringToQuark(str));
      if (s != NULL) current_space = s;
    }
  PRINT("Current space %s\n", current_space->name);
}


void
GetShowWSDialog(Boolean *s_dialog)
{
  /* get dbase file name */


  if (app_data.show_dialog == False)
    {
      *s_dialog = False;
    }
}


#ifdef _ERROR_HANDLERS
int
MyXErrorHandler(Display *display, XErrorEvent *event)
{
  PRINT("Called MyXErrorHandler XXXXXXXXXXXXXXXXXXXXXXX\n");
  return(0);
}

void
MyXtErrorHandler(char *err)
{
  PRINT("Called MyXtErrorHandler Xt Xt Xt Xt Xt Xt Xt Xt Xt\n");
  return;
}

#endif
int main(argc, argv)
    int argc;
    char **argv;
{
  XtAppContext app_con;
  Widget rc;
  Arg args[20];
  Atom wsm_0_atom;
  Cardinal num_args = 0;
  XEvent event;
  Widget Shell001;
  Widget Shell001a;
  Widget Shell002;
  Widget Shell003;
  Widget Shell004;
  Widget Shell005;
  Time time;
  int argcnt;
  String *str_table,*b_str_table,*lp_str_table;
  String current_space_str;
  Boolean show_dialog = True;

  shell = XtAppInitialize(&app_con, "WSM", options, XtNumber(options),
			  &argc, argv, fallbacks, args, num_args);
  XtRealizeWidget(shell);

  utmShell = XtVaCreateManagedWidget("utmShell", xmDrawingAreaWidgetClass,
				     shell, NULL);
  XtAddCallback(utmShell, XmNconvertCallback, UTMConvertProc, NULL);

#ifdef NO_TIMERS
  XtSetSelectionTimeout(5000000);
#endif


#ifdef _ERROR_HANDLERS
  XSetErrorHandler(MyXErrorHandler);
  XtAppSetErrorHandler(app_con,MyXtErrorHandler);
#endif
  dsp = XtDisplay(shell);

  XtVaSetValues(shell,XmNallowShellResize,True,NULL);

  app_data.show_dialog = show_dialog;
  XtVaGetApplicationResources(shell,
			      &app_data,
			      resources,
			      XtNumber(resources),
			      NULL);


  InitializeWsm();

  GetFileName(&file_name);
  PRINT("FILE NAME %s\n", file_name);
  InitializeXrm(file_name);

  GetSpaceListResources(shell,
			&str_table,
			&b_str_table,
			&lp_str_table,
			&current_space_str,
			&show_dialog);

  GetWorkspaces(str_table);
  GetBackgrounds(b_str_table);
  GetPixmaps(lp_str_table);
  GetCurrentSpace(current_space_str);
  GetShowWSDialog(&show_dialog);
  PrintAllSpaces();

  wsm_shown = show_dialog;
  wsm_ui = (WSM_UI*)XtMalloc(sizeof(WSM_UI));
  wsm_0_atom = XInternAtom(XtDisplay(shell), "WSM_S0", False);
  InitializeInterface(wsm_ui);

  if (XGetSelectionOwner(XtDisplay(shell), wsm_0_atom) != None) {
    fprintf(stderr, "Someone owms the WSM selection on screen 0.\n");
    exit(1);
  }

  rc = CreateWorkspacePanel(shell, wsm_ui, app_data.show_menu);
  XtAddCallback(shell, XmNpopdownCallback, QuitCB, (XtPointer)NULL);


  /* This arglist is used for all of the dialogs created below */
  argcnt = 0;
  XtSetArg(args[argcnt], XmNdeleteResponse, XmUNMAP); argcnt++;

  Shell001 = XtCreatePopupShell("nameShell", topLevelShellWidgetClass,
				shell, args, argcnt);
  CreateNameWorkspace(Shell001, wsm_ui);


  Shell001a = XtCreatePopupShell("backgroundShell", topLevelShellWidgetClass,
				 shell, args, argcnt);
  CreateBackgroundWorkspace(Shell001a, wsm_ui);


  Shell002 = XtCreatePopupShell("deleteShell", topLevelShellWidgetClass,
				shell, args, argcnt);
  CreateDeleteWorkspace(Shell002, wsm_ui);


  Shell003 = XtCreatePopupShell("configureShell", topLevelShellWidgetClass,
				shell, args, argcnt);
  CreateConfigureWorkspace(Shell003, wsm_ui);


  Shell004 = XtCreatePopupShell("saveAsShell", topLevelShellWidgetClass,
				shell, args, argcnt);
  CreateSaveAs(Shell004, wsm_ui);

#ifndef _NO_OCCUPY_DIALOG

  Shell005 = XtCreatePopupShell("occupyShell", topLevelShellWidgetClass,
				shell, args, argcnt);
  CreateOccupyWorkspace(Shell005, wsm_ui);
#endif

  XtManageChild(rc);

  SetSpaceBackground(current_space);

  InternStuff(dsp);

  atoms.own_selection = _WSMGetSelectionAtom
         (dsp, XScreenNumberOfScreen(XtScreen(shell)), WSM_WORKSPACE_MANAGER);

  PRINT("own selection: %s\n",XGetAtomName(dsp,atoms.own_selection));
  if (XGetSelectionOwner(dsp, atoms.own_selection) != None)
    {
      fprintf(stderr,
	      "Error - Someone out there already owns this selection.\n");
      exit(1);
    }

  WSMRegisterRequestCallback(dsp,
			     XScreenNumberOfScreen(XtScreen(shell)),
			     RequestReceived,NULL);

  /*
   * Setup destination proc - use UTMDestinationProc from  utm_send.c module.
   * Any requests made MUST then use UTMSendMesage.
   * Also setup own convert proc.
   *
   * Note - convertProc was setup when utmShell was created.
   */
  XtAddCallback(utmShell, XmNdestinationCallback, UTMDestinationProc, NULL);

  time = GetTimestamp(dsp);
  XmeNamedSource(utmShell, atoms.own_selection, time);

  SendConnect(utmShell);

  if (!show_dialog)
    XtUnmapWidget(shell);

  SetupWsmSignalHandlers(0);

  for (;;)
    {
      XtAppNextEvent(app_con, &event);
      if (event.type == ClientMessage)
	{
	  DealWithClientMessage(rc,(XClientMessageEvent *)&event);
	}
      if(event.type == DestroyNotify)
	{
	  DealWithDestroyNotify(rc,(XDestroyWindowEvent *) &event);
	}
      if(event.type == MapNotify)
	{
	  DealWithMapNotify(rc,(XMapEvent *) &event);
	}
      if(event.type == UnmapNotify)
	{
	  DealWithUnmapNotify(rc,(XUnmapEvent *) &event);
	}
      if(XtDispatchEvent(&event))
	continue;
    }
  return 0;
}


/*	Function Name: UTMConvertProc
 *	Description: This is the UTM conversion proc called when a request
 *                   comes in that we need to process.
 *	Arguments: w - The widget making the request.
 *                 clientData - not used.
 *                 callData - the UTM convert callback structure.
 */

static void
UTMConvertProc(Widget w, XtPointer clientData, XtPointer callData)
{
  int scr = XScreenNumberOfScreen(XtScreen(w));
  Display *d = XtDisplay(w);
  XmConvertCallbackStruct *ccs = (XmConvertCallbackStruct *)callData;
  Atom lose_sel = XInternAtom(d, "_MOTIF_LOSE_SELECTION", False);
  Atom wm_sel = _WSMGetSelectionAtom(d, scr, WSM_WINDOW_MANAGER);
  Atom wsm_sel = _WSMGetSelectionAtom(d, scr, WSM_WORKSPACE_MANAGER);

  /*
   * Check if the callback was invoked for the right reason.
   * The reason field is not set to anything interresting by the transfer
   * code, so check the selection.
   */
  if ((!ccs) || ((ccs->selection != wm_sel) && (ccs->selection != wsm_sel)))
    return;

  if (ccs->target == lose_sel)
    {
      /* Done with the conversion - free any data used. */
    }

  /*
   * Handle a conversion request with parameter data.
   */
  else
    {
      WsmConvertProc (w, &(ccs->selection), &(ccs->target),
		     ccs->parm, ccs->parm_length, ccs->parm_format,
		     &(ccs->type), &(ccs->value), &(ccs->length),
		     &(ccs->format));
    }
}

static Boolean
WsmConvertProc(Widget w, Atom *selection, Atom *target,
	      XtPointer input, unsigned long input_len, int input_fmt,
	      Atom *return_type, XtPointer *output, unsigned long *output_len,
	      int *output_fmt)
{
  WSMDispInfo   *disp_info;

  PRINT("WsmConvertProc target is %s\n",XGetAtomName(dsp,*target));
  disp_info = _WSMGetDispInfo(XtDisplay(w));

  if (*target == disp_info->targets) {
    *return_type = XA_STRING;
    *output = (XtPointer) (long) WSMGetTargetList(w, TRUE, output_len);
    *output_fmt = 32;
    return(True);
  }

  /*
   * The only target this handles not included in the
   * WSM Protocol is INVOKE_COMMAND. Check for this here.
   * If new targets are added later, they may be added here.
   */
  if (*target == _MOTIF_WM_INVOKE_COMMAND)
    {
      /* ||| CARD8 type? */
      InvokeCommand(w, *target, (CARD8*)input, input_len, input_fmt);
      return (TRUE);
    }

  if (!WSMIsKnownTarget(w, *target))
    {
      PRINT("isn't known target\n");
      return (False);
    }
  else
    {
      PRINT ("is known target \n");
      if (WSMProcessProtoTarget(w, *target, input, input_len, input_fmt,
				return_type, output, output_len, output_fmt))
	return(TRUE);
    }
  return(False);
}


void
SendExtensionsProc(XtPointer client_data, XtIntervalId *int_id)
{
  Widget w = (Widget)client_data;
  SendExtensionCB(w,NULL,NULL);

}

/*ARGSUSED*/
void
RequestReceived(Widget w, XtPointer textp,
		WSMRequest *request, WSMReply *reply)
{
  Boolean version_found = False;
  int i;

  PRINT("RequestReceived\n");
  reply->any.allocated = True;
  switch (reply->any.type = request->any.type) {
  case WSM_CONNECT:
    {
      /* send versions in decending order */
      for (i = 0; i < request->connect.num_versions; i++)
	if (request->connect.known_versions[i] == 1 ||
	    request->connect.known_versions[i] == 2 ||
	    request->connect.known_versions[i] == 3)
	  {
	    reply->connect.version = request->connect.known_versions[i];
	    version_found = True;
	    break;
	  }
      if (!version_found) reply->connect.version = 0;
      if (connected)
	{
	  RestartWSM();
	  connected = False;
	}
      mwm_reconnect = True;
      SendExtensionCB(w,NULL,NULL);
    }
    break;

  case WSM_REG_WINDOW:
    {
      PRINT("wsm: get Register reply\n");
      if (connected)
	{
	  GetRegisterWindowReply(request->register_window.window,reply);
	  print_reply(reply);
	}
      else
	{
	  reply->register_window.allocated = False;
	  reply->register_window.num_window_data = 0;
	  reply->register_window.window_data = NULL;
	}
      PRINT("    got it\n");
    }
    break;
  case WSM_EXTENSIONS:
  case WSM_CONFIG_FMT:
  case WSM_GET_STATE:
  case WSM_SET_STATE:
  case WSM_WM_GET_BACKGROUND_WINDOW:
  case WSM_WM_SET_BACKGROUND_WINDOW:
  case WSM_WM_WINDOWS:
  case WSM_WM_FOCUS:
  case WSM_WM_POINTER:
  case WSM_UNKNOWN:
    /* Not handled cases */
    break;
  }

  /*
   * If we had allocated any data here we need to ask the
   * library to free it, see comments in WSMDefaultOwnSelection for
   * details.
   */

}



void
SendConnect(Widget w)
{
  WSMRequest request;
  static short versions[] = { 3,2,1 };
  Display *d = XtDisplay(w);
  int scr = XScreenNumberOfScreen(XtScreen(shell));
  Atom wm_selection = _WSMGetSelectionAtom(d, scr, WSM_WINDOW_MANAGER);


  if (XGetSelectionOwner(d, wm_selection) == None)
    return;

  PRINT("send connect\n");
  if (connected == False)
    {
      request.any.type = WSM_CONNECT;
      request.connect.allocated = False;
      request.connect.known_versions = versions;
      request.connect.num_versions = XtNumber(versions);
      WSMSendMessage(utmShell, WSM_WINDOW_MANAGER, &request,
		     ReceiveConnectRCB, NULL);
    }
}

void
ReceiveConnectRCB(Widget w, XtPointer call,
		  WSMReply *reply, WSMErrorCode error)
{
  PRINT("receive connect\n");
  if (reply != NULL)
    {
      SendExtensionCB(w,call,call);
    }
  else
    PRINT("receive connect == NULL\n");
}


void
SendExtensionCB(Widget w, XtPointer client,XtPointer call)
{

  WSMRequest request;
  static String extensions[] = { "Test",
				   "Extension #1", "Extension #123" };

  request.any.type = WSM_EXTENSIONS;
  request.extensions.extension_suggestions = extensions;
  request.extensions.num_extensions = 3;
  request.extensions.allocated = False;
  WSMSendMessage(utmShell, WSM_WINDOW_MANAGER, &request,
		 ReceiveExtensionsRCB, call);

}

void
ReceiveExtensionsRCB(Widget w, XtPointer client,
		     WSMReply *reply, WSMErrorCode error)
{
  if (reply != NULL )
    {
      SendConfigCB(w,client, client);
    }
  else
    {
      PRINT(" extension reply == NULL\n");
      SendConfigCB(w,client,client);
    }
}


void
SendConfigCB(Widget w, XtPointer client, XtPointer call)
{
  WSMRequest request;
  request.any.type = WSM_CONFIG_FMT;
  WSMSendMessage(utmShell, WSM_WINDOW_MANAGER, &request,
		 ReceiveConfigRCB, call);
}

Boolean
find_format(XrmQuark nameq, XrmQuark *nameq_list, int num_list)
{
  int i;
  for (i = 0; i < num_list; i++)
    {
      if (nameq == nameq_list[i])
	return True;
    }
  return False;
}

void
ReceiveConfigRCB(Widget w, XtPointer call,
	     WSMReply *reply, WSMErrorCode error)
{
  int i,j;
  XrmQuark hideq = XrmStringToQuark("hidden");
  XrmQuark iconicq = XrmStringToQuark("iconic");

  if (mwm_reconnect)
  {
      if (num_global_attrib != reply->config_format.num_global_formats)
	  mwm_reconnect = False;
      for (i = 0; i < reply->config_format.num_global_formats && mwm_reconnect; i++)
	{
	  if (global_attribq[i] != reply->config_format.global_formats[i].nameq ||
	      global_attrib_list[i].size !=
	      reply->config_format.global_formats[i].size ||
	      global_attrib_list[i].is_list !=
	      reply->config_format.global_formats[i].is_list)
	      mwm_reconnect = False;
	}
      if (num_window_attrib != reply->config_format.num_window_formats)
	  mwm_reconnect = False;
      for (i = 0; i < reply->config_format.num_window_formats && mwm_reconnect; i++)
	{
	  if (window_attribq[i] != reply->config_format.window_formats[i].nameq ||
	      window_attrib_list[i].size !=
	      reply->config_format.window_formats[i].size ||
	      window_attrib_list[i].is_list !=
	      reply->config_format.window_formats[i].is_list)
	      mwm_reconnect = False;
	}
      for (i = 0; i < reply->config_format.num_icon_formats && mwm_reconnect; i++)
      {
	  if ( icon_attribq[i] != reply->config_format.icon_formats[i].nameq ||
	      icon_attrib_list[i].size != reply->config_format.icon_formats[i].size ||
	      icon_attrib_list[i].is_list !=
	      reply->config_format.icon_formats[i].is_list)
	      mwm_reconnect = False;
      }
      if (!mwm_reconnect)
	  CompleteRestartWSM();
  }
  if (reply != NULL && !mwm_reconnect)
    {
      diffs_allowed =reply->config_format.accepts_diffs;

      global_attrib_list=
	(WSMAttribute*)	XtMalloc(reply->config_format.num_global_formats *
				 sizeof(WSMAttribute));
      global_attribq =
	(XrmQuark*)XtMalloc((1+reply->config_format.num_global_formats) *
			    sizeof(XrmQuark));
      window_attrib_list =
	(WSMAttribute*)XtMalloc(reply->config_format.num_window_formats *
				sizeof(WSMAttribute));
      window_attribq =
	(XrmQuark*)XtMalloc((1+reply->config_format.num_window_formats) *
			    sizeof(XrmQuark));
      icon_attrib_list =
	(WSMAttribute*)XtMalloc(reply->config_format.num_icon_formats *
				sizeof(WSMAttribute));
      icon_attribq =
	(XrmQuark*)XtMalloc((1+reply->config_format.num_icon_formats) *
			    sizeof(XrmQuark));
      wsm_index.hide = -1;
      wsm_index.iconic = -1;
      for (i = 0; i < reply->config_format.num_global_formats; i++)
	{
	  global_attrib_list[i].nameq = global_attribq[i] =
	    reply->config_format.global_formats[i].nameq;
	  global_attrib_list[i].size =
	    reply->config_format.global_formats[i].size;
	  global_attrib_list[i].is_list =
	    reply->config_format.global_formats[i].is_list;
	}
      global_attribq[i] = NULLQUARK;
      num_global_attrib = i;
      for (i = 0; i < reply->config_format.num_window_formats; i++)
	{
	  if (reply->config_format.window_formats[i].nameq == hideq)
	    wsm_index.hide = i;
	  if (reply->config_format.window_formats[i].nameq == iconicq)
	    wsm_index.iconic = i;
	  window_attrib_list[i].nameq = window_attribq[i] =
	    reply->config_format.window_formats[i].nameq;
	  window_attrib_list[i].size =
	    reply->config_format.window_formats[i].size;
	  window_attrib_list[i].is_list =
	    reply->config_format.window_formats[i].is_list;
	}
      window_attribq[i] = NULLQUARK;
      num_window_attrib = i;
      for (i = 0; i < reply->config_format.num_icon_formats; i++)
	{
	  icon_attrib_list[i].nameq = icon_attribq[i] =
	    reply->config_format.icon_formats[i].nameq;
	  icon_attrib_list[i].size = reply->config_format.icon_formats[i].size;
	  icon_attrib_list[i].is_list =
	    reply->config_format.icon_formats[i].is_list;
	}
      icon_attribq[i] = NULLQUARK;
      num_icon_attrib = i;
      config_format_reply = &(reply->config_format);
    }
  if (reply != NULL)
    {
      SendStartGetStateCB(w,call,call);
    }

}


void
SendStartGetStateCB(Widget w, XtPointer call, XtPointer client)
{
  WSMRequest request;
  PRINT("wsm: send start get state\n");
  request.get_state.window = (Window)0;
  request.get_state.diffs_allowed = False;
  request.any.type = WSM_GET_STATE;
  WSMSendMessage(utmShell, WSM_WINDOW_MANAGER, &request, SendStartSetStateRCB,
		 call);
}

void
SendStartSetStateRCB(Widget w, XtPointer call,
		     WSMReply *reply, WSMErrorCode error)
{
  WSMRequest request;
  if (reply != NULL)
  {
    if (reply->any.type == WSM_GET_STATE && config_format_reply != NULL)
      {
	CreateStartStateRequest(&(reply->get_state),
				&request);
	WSMSendMessage(utmShell, WSM_WINDOW_MANAGER, &request,
		       ReceiveSetStateRCB, call);
      }
  }
}


void
ReceiveSetStateRCB(Widget w, XtPointer call,
		     WSMReply *reply, WSMErrorCode error)
{
  Atom mwm_selection;
  WorkWindow *w_window;

  SetInitialCommands();
  w_window = work_windows;
  for (w_window = work_windows; w_window != NULL; w_window = w_window->next)
    if (w_window->s_list != NULL)
      if (w_window->s_list->next == NULL)
	DisableDeleteCommand(w_window->window);

  connected = True;
  mwm_gone = False;
  mwm_reconnect = False;
  mwm_selection = _WSMGetSelectionAtom(dsp,
                  XScreenNumberOfScreen(XtScreen(shell)),WSM_WINDOW_MANAGER);
  mwm_window = XGetSelectionOwner(dsp, mwm_selection);
  if ( mwm_window != None)
    {
      XSelectInput(dsp,mwm_window,StructureNotifyMask);
    }
  PRINT("CONNECTED!\n");
}




void
SendSaveWsm(char *fname)
{
  WSMRequest request;
  request.get_state.window = 0;
  request.get_state.diffs_allowed = False;
  request.any.type = WSM_GET_STATE;
  PRINT("wsm:  WSMSendMessage WSM_GET_STATE %s\n", fname);
  WSMSendMessage(utmShell, WSM_WINDOW_MANAGER, &request, ReceiveSaveWsmRCB,
		 (XtPointer)fname);
}



void
SendLeaveRoom(Space *s)
{
  WSMRequest request;
  request.get_state.window = 0;
  request.get_state.diffs_allowed = False;
  request.any.type = WSM_GET_STATE;
  PRINT("wsm:  WSMSendMessage WSM_GET_STATE\n");
  WSMSendMessage(utmShell, WSM_WINDOW_MANAGER, &request, ReceiveLeaveRoomRCB,
		 (XtPointer)s);
}


void
ReceiveSaveWsmRCB(Widget w, XtPointer client,
	     WSMReply *reply, WSMErrorCode error)
{
  char *fname = (char *)client;
  if (reply != NULL)
    {
      if (reply->any.type == WSM_GET_STATE)
	{
	  PRINT("wsm: save\n");
	  SaveState(&(reply->get_state),False);
	  PRINT("wsm:        done\n\n");
	}
    }
  UpdateXrm();
  SaveWsmToFile(fname);
}



void
ReceiveLeaveRoomRCB(Widget w, XtPointer client,
	     WSMReply *reply, WSMErrorCode error)
{
  if (reply != NULL)
    {
      if (reply->any.type == WSM_GET_STATE)
	{
	  PRINT("wsm: save\n");
	  SaveState(&(reply->get_state),False);
	  PRINT("wsm:        done\n\n");
	    ChangeRoomCB(w,client,NULL);
	}
    }
}

void
ReceiveEnterRoomRCB(Widget w, XtPointer client,
	     WSMReply *reply, WSMErrorCode error)
{
  PRINT("finished entering room\n");
}

void
ChangeRoomCB(Widget w, XtPointer client, XtPointer call)
{
  WSMRequest request;
  Space *space = (Space*)client;

  PRINT("wsm: Get SET_STATE request\n");

  if (space != NULL)
    {
      GetChangeSpaceRequest(space,&request);
      WSMSendMessage(utmShell, WSM_WINDOW_MANAGER, &request,
		     ReceiveEnterRoomRCB, NULL);
      SetSpaceBackground(space);
    }

}

void
ReceiveCopyMoveRCB(Widget w, XtPointer client,
	     WSMReply *reply, WSMErrorCode error)
{
 PRINT("receive copy/move\n");
}

void
CopyWindow(WorkWindow *w_window, Space *from_s, Space* to_s)
{
  WSMRequest request;
  CopyStruct *copy_struct;

  from_space = from_s;
  to_space = to_s;
  PrintAllSpaces();
  if (w_window != NULL)
    {
      print_window(w_window);
      if (from_space == current_space && w_window->mapped)
	{
	  copy_struct = (CopyStruct *)XtMalloc(sizeof(CopyStruct));
	  copy_struct->window = w_window->window;
	  copy_struct->to_space = to_s;
	  copy_struct->from_space = from_s;
	  request.get_state.window = w_window->window;
	  request.get_state.diffs_allowed = True;
	  request.any.type = WSM_GET_STATE;
	  PRINT("Sending Get State \n");
	  WSMSendMessage(utmShell, WSM_WINDOW_MANAGER, &request, CopyRCB,
			 (XtPointer)copy_struct);
	}
      else if (GetCopyWindowRequest(w_window->window, from_space, to_space,
				    &request))
	WSMSendMessage(utmShell, WSM_WINDOW_MANAGER, &request,
		       ReceiveCopyMoveRCB, NULL);
    }
  else PRINT("copy null window\n");
}


void
CopyRCB(Widget w, XtPointer client,
	     WSMReply *reply, WSMErrorCode error)
{
  WSMRequest request;
  CopyStruct *copy_struct = (CopyStruct*)client;
  PRINT("Received get state\n");
  if (reply != NULL)
    {
      if (reply->any.type == WSM_GET_STATE)
	{
	  PRINT("calling SaveState\n");
	  SaveState(&(reply->get_state), True);
	}
      if (GetCopyWindowRequest(copy_struct->window, copy_struct->from_space,
			       copy_struct->to_space, &request))
	WSMSendMessage(utmShell, WSM_WINDOW_MANAGER, &request,
		       ReceiveCopyMoveRCB, NULL);
    }
  XtFree((XtPointer)copy_struct);
}

void
MoveWindow(WorkWindow *w_window, Space *from_s, Space* to_s)
{
  WSMRequest request;

  from_space = from_s;
  to_space = to_s;

  if (w_window != NULL)
    {
      if (from_space == current_space)
	{
	  request.get_state.window = w_window->window;
	  request.get_state.diffs_allowed = True;
	  request.any.type = WSM_GET_STATE;
	  WSMSendMessage(utmShell, WSM_WINDOW_MANAGER, &request, MoveRCB,
			 (XtPointer)w_window->window);
	}
      else if (GetMoveWindowRequest(w_window->window, from_space, to_space,
				    &request))
	WSMSendMessage(utmShell, WSM_WINDOW_MANAGER, &request,
		       ReceiveCopyMoveRCB, NULL);
    }
}

void
MoveRCB(Widget w, XtPointer client,
	     WSMReply *reply, WSMErrorCode error)
{
    WSMRequest request;

    if (reply != NULL)
    {
	SaveState(&(reply->get_state), True);
	if (GetMoveWindowRequest((Window)client, from_space, to_space,
				 &request))
	{
	    WSMSendMessage(utmShell, WSM_WINDOW_MANAGER, &request,
			   ReceiveCopyMoveRCB, NULL);
	}
    }
}




void
DeleteWindow(WorkWindow *w_window, Space *from_s)
{
  WSMRequest request;

  from_space = from_s;

  if (w_window != NULL)
    {
      if (GetDeleteWindowRequest(w_window->window, from_space,&request))
	WSMSendMessage(utmShell, WSM_WINDOW_MANAGER, &request,
		       ReceiveCopyMoveRCB, NULL);
    }

}

void
LinkWindow(WorkWindow *w_window, Space *from_s, Space *to_s)
{
  WSMRequest request;
  CopyStruct *copy_struct;
  from_space = from_s;
  to_space = to_s;

  if (w_window != NULL)
    {
      if (from_space == current_space)
	{
	  copy_struct = (CopyStruct *)XtMalloc(sizeof(CopyStruct));
	  copy_struct->window = w_window->window;
	  copy_struct->to_space = to_s;
	  copy_struct->from_space = from_s;
	  request.get_state.window = w_window->window;
	  request.get_state.diffs_allowed = True;
	  request.any.type = WSM_GET_STATE;
	  WSMSendMessage(utmShell, WSM_WINDOW_MANAGER, &request, LinkRCB,
			 (XtPointer)copy_struct);
	}
      else if (GetLinkWindowRequest(w_window->window, from_space, to_space,
				    &request))
	WSMSendMessage(utmShell, WSM_WINDOW_MANAGER, &request,
		       ReceiveCopyMoveRCB, NULL);
    }
  else PRINT("copy null window\n");
}


void
LinkRCB(Widget w, XtPointer client,
	     WSMReply *reply, WSMErrorCode error)
{
  WSMRequest request;
  CopyStruct *copy_struct = (CopyStruct*)client;

  if (reply != NULL)
  {
      SaveState(&(reply->get_state),True);
      if (GetLinkWindowRequest(copy_struct->window, copy_struct->from_space,
			       copy_struct->to_space, &request))
	  WSMSendMessage(utmShell, WSM_WINDOW_MANAGER, &request,
			 ReceiveCopyMoveRCB, NULL);
  }
  XtFree((XtPointer)copy_struct);
}

/* This function doesn't appear to be used anywhere */
#ifdef notdef
void
BackgroundRCB(Widget w, XtPointer client,
	     WSMReply *reply, WSMErrorCode error)
{
  Window windowID = reply->get_background.window;
  GC gc;
  unsigned int root_width, root_height;
  XGCValues values;
  Window rootWindow = XRootWindowOfScreen(XtScreen(shell));
  gc = XCreateGC(dsp,rootWindow,0,&values);
  XSetBackground(dsp,gc,WhitePixel(dsp,0));
  PRINT("background reply returned. %d %d\n", windowID, rootWindow);
  XFillRectangle(dsp,rootWindow,gc,0,0,root_width,root_height);
}
#endif


void
SetSpaceBackground(Space *space)
{
  if (SpacePixelSet(space))
    {
      PRINT("setting background color\n");
      XSetWindowBackground(dsp, XRootWindowOfScreen(XtScreen(shell)),
			   space->pixel);
      XClearWindow(dsp,XRootWindowOfScreen(XtScreen(shell)));
    }
  else if (SpacePixmapSet(space))
    {
      PRINT("setting background pixmap\n");
      XSetWindowBackgroundPixmap(dsp,XRootWindowOfScreen(XtScreen(shell)),
				 space->pixmap);
      XClearWindow(dsp,XRootWindowOfScreen(XtScreen(shell)));
    }

}
void
QuitCB(Widget w, XtPointer client, XtPointer call)
{
  PRINT("QUITCB\n");
  ManageAllWindowsAndExit();
}

void
ManageAllWindowsAndExit()
{
  WSMRequest request;
  PRINT("ManageAllWindowsAndExit()\n");
  if (!mwm_gone)
    {
      if (GetManageAllWindowsRequest(&request))
	WSMSendMessage(utmShell, WSM_WINDOW_MANAGER, &request, ExitRCB, NULL);
      else
	CleanUpAndExit();
    }
  else exit(0);
}


void
ExitRCB(Widget w, XtPointer client,
	     WSMReply *reply, WSMErrorCode error)
{
    CleanUpAndExit();
}

void CleanUpAndExit()
{
    if (!mwm_gone)
	RemoveAllCommandsAndExit();
}

/*************************************<->*************************************
 *
 *  SetupWsmSignalHandlers ()
 *
 *
 *  Description:
 *  -----------
 *  This function sets up the signal handlers for the workspace manager.
 *
 *************************************<->***********************************/

/*ARGSUSED*/
static void SetupWsmSignalHandlers (int dummy)
{
    void (*signalHandler) ();


    signalHandler = (void (*)())signal (SIGINT, SIG_IGN);
    if (signalHandler != (void (*)())SIG_IGN)
    {
	signal (SIGINT, QuitWsmSignalHandler);
    }

    signalHandler = (void (*)())signal (SIGHUP, SIG_IGN);
    if (signalHandler != (void (*)())SIG_IGN)
    {
	signal (SIGHUP, QuitWsmSignalHandler);
    }

    signal (SIGQUIT, QuitWsmSignalHandler);

    signal (SIGTERM, QuitWsmSignalHandler);


} /* END OF FUNCTION SetupWsmSignalHandlers */


/*************************************<->*************************************
 *
 *  QuitWsmSignalHandler ()
 *
 *
 *  Description:
 *  -----------
 *  This function is called on receipt of a signal that is to terminate the
 *  workspace manager.
 *
 *************************************<->***********************************/

/*ARGSUSED*/
static void QuitWsmSignalHandler (int dummy)
{
    ManageAllWindowsAndExit();
    XFlush(dsp);
} /* END OF FUNCTION QuitWsmSignalHandler */
