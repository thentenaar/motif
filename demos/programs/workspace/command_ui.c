/*
 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2.1 of the License, or (at your option)
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
static char rcsid[] = "$TOG: command_ui.c /main/8 1997/05/02 10:08:26 dbl $"
#endif
#endif

#include <stdlib.h>
#include <Xm/Xm.h>
#include <Xm/ToggleB.h>
#include <X11/Xmd.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if HAVE_STDINT_H
#include <stdint.h>
#elif HAVE_INTTYPES_H
#include <inttypes.h>
#endif

#include "Wsm/wsm_proto.h"
#include "Wsm/wsm_funcs.h"
#include "Wsm/utm_send.h"
#include "wsm.h"
#include "wsmStruct.h"
#include "wsmDebug.h"
#include "command_ui.h"
#include "wsm_cb.h"
#include "wsm_ui.h"

extern AtomStruct atoms;
extern Display *dsp;
extern Widget shell;
extern Widget shell_ui;
extern Space* space_list;
extern Space* all_space;
extern Space *current_space;
extern WSM_UI* wsm_ui;

extern Boolean wsm_shown;

int debug_index = 0;

#define GET_COMMAND_ID(dpy) ((CARD32) XAllocID(dpy))

CARD32 switch_id;
CARD32 show_id;
CARD32 occupy_id;
CARD32 copy_all_id;
CARD32 link_all_id;
CARD32 remove_id;

Boolean exiting = False;
CommandSpace *command_space_list;
typedef enum {ROOT, ALL, ICON, WIN, DEFINE, INCLUDE, REMOVE,
	      ENABLE, DISABLE, RENAME, INLINE, MAXTOGGLES } RType;

Atom WM_S0,
     _MOTIF_WM_DEFINE_COMMAND,
     _MOTIF_WM_INCLUDE_COMMAND,
     _MOTIF_WM_DISABLE_COMMAND,
     _MOTIF_WM_ENABLE_COMMAND,
     _MOTIF_WM_INVOKE_COMMAND,
     _MOTIF_WM_REMOVE_COMMAND,
     _MOTIF_WM_RENAME_COMMAND;

Time
GetTimestamp (Display *dpy)
{
  XEvent   event;
  XWindowAttributes attr;
  Window   rwin = RootWindowOfScreen(XtScreen(utmShell));
  Atom     time_atom = XInternAtom(dsp, "_MOTIF_CURRENT_TIME", False);

  XGetWindowAttributes(dsp, rwin, &attr);
  if (! (attr.your_event_mask & PropertyChangeMask))
    XSelectInput(dsp, rwin, attr.your_event_mask | PropertyChangeMask);

  XChangeProperty(dsp, rwin, time_atom, time_atom, 8, PropModeAppend, NULL, 0);
  XWindowEvent(dsp, rwin, PropertyChangeMask, &event);

  if (! (attr.your_event_mask & PropertyChangeMask))
    XSelectInput(dsp, rwin, attr.your_event_mask);

  return(event.xproperty.time);
}


/*----------------------------------------------------------------------*
 |                              SelectionProc                           |
 | This is invoked when a message is finished and the reply is received.|
 *----------------------------------------------------------------------*/
void
SelectionProc (Widget w, XtPointer clientData, XtPointer callData)
{

}




/*----------------------------------------------------------------------*
 |                              LastSelectionProc                       |
 | This is invoked when the last message is finished and the reply is   |
 | received.                                                            |
 *----------------------------------------------------------------------*/
void
LastSelectionProc (Widget w, XtPointer clientData, XtPointer callData)
{
    Display *d = XtDisplay(w);

    XSync(d, False);
    XCloseDisplay(d);
    exit(0);
}



/*----------------------------------------------------------------------*
 |                             SendMessage                              |
 *----------------------------------------------------------------------*/
void
SendMessage(RType rtype, XtPointer msg, int len, Time time)
{
  XtCallbackProc s_proc;
  Atom target = 0;

  if (!exiting) s_proc = SelectionProc;
  else s_proc = LastSelectionProc;
  if (!XtIsRealized(shell))
    {
      PRINT("must be realized\n");
      return;
    }
  switch (rtype)
    {
    case DEFINE:
      target = _MOTIF_WM_DEFINE_COMMAND;
      break;
    case INCLUDE:
      target = _MOTIF_WM_INCLUDE_COMMAND;
      break;
    case REMOVE:
      target = _MOTIF_WM_REMOVE_COMMAND;
      break;
    case ENABLE:
      target = _MOTIF_WM_ENABLE_COMMAND;
      break;
    case DISABLE:
      target = _MOTIF_WM_DISABLE_COMMAND;
      break;
    case RENAME:
      target = _MOTIF_WM_RENAME_COMMAND;
      break;
    case ROOT:
    case ALL:
    case ICON:
    case WIN:
    case INLINE:
    case MAXTOGGLES:
      /* Not handled cases */
      break;
    }

  UTMSendMessage(utmShell, WM_S0, target,
		 (XtPointer) msg, (unsigned long)len, WSM_PROTO_FMT,
		 s_proc, (XtPointer)(intptr_t)++debug_index, time);

  if (msg != NULL)
    XtFree(msg);
}

/*----------------------------------------------------------------------*
 |                                SendEnable                            |
 *----------------------------------------------------------------------*/
void
SendEnableCommand (Window window, CARD32 commandID)
{
#ifndef _NO_DISABLE
  Time    time;
  XtPointer msg,save;
  int     size=0;
  CARD32  count = 1L;
  CARD32  windowBit = 1<<30;
/*  CARD32  iconBit = 1<<31;*/
  char   str[128];

  time = GetTimestamp(dsp);

  size += sizeof(CARD32);  /* command id */
  size += sizeof(CARD32);  /* count */
  size += sizeof(CARD32);  /* window */

  save = msg = (XtPointer) XtMalloc(size);

  window |= windowBit;

  msg = PackCARD32(msg,(CARD32)commandID);
  msg = PackCARD32(msg,(CARD32)count);
  msg = PackCARD32(msg,(CARD32)window);

  PRINT("EnableCommand #%d on window %x (%u).\n",
	  commandID, window, window);

  SendMessage(ENABLE, save, size, time);

#endif
}



/*----------------------------------------------------------------------*
 |                                 SendDisable                            |
 *----------------------------------------------------------------------*/
void
SendDisableCommand (Window window, CARD32 commandID)
{
#ifndef _NO_DISABLE
 Time    time;
  XtPointer msg,save;
  int     size=0;
  CARD32  count = 1L;
  CARD32  windowBit = 1<<30;
/*  CARD32  iconBit = 1<<31;*/
  char   str[128];

  time = GetTimestamp(dsp);

  size += sizeof(CARD32);  /* command id */
  size += sizeof(CARD32);  /* count */
  size += sizeof(CARD32);  /* window */

  save = msg = (XtPointer) XtMalloc(size);

  window |= windowBit;

  msg = PackCARD32(msg,(CARD32)commandID);
  msg = PackCARD32(msg,(CARD32)count);
  msg = PackCARD32(msg,(CARD32)window);

  PRINT("DisableCommand #%d on window %x (%u).\n",
	  commandID, window, window);

  SendMessage(DISABLE, save, size, time);
#endif
}



/*----------------------------------------------------------------------*
 |                                 SendInclude                           |
 *----------------------------------------------------------------------*/
void
SendIncludeCommand(Window window, int commID, Boolean is_in_line)
{
  Time time;
  XtPointer msg, save;
  CARD32  commandID = 0L;
  CARD32  in_line = 0L;
  CARD32  selection = 0L;
  CARD32  count = 1L;
  CARD32  windowBit = 1<<30;
/*  CARD32  iconBit = 1<<31;*/
  char   str[128];
  int     size=0;

  commandID += commID;
  selection += atoms.own_selection;
  if (is_in_line)
    {
      in_line = 1L;
    }
  time = GetTimestamp(dsp);
  /*
   *  Fixup the window id receiving the client command.
   *  Top 2 bits:
   *  	10 - Icon only
   *	01 - Window only
   *	11 - Both Icon and Window
   *	00 - Reserved (Don't Use!)
   *
   *  Bottom 30 bits:
   *	0x3fffffff - All windows and/or icons
   *	0x00000000 - Root window menu
   */

  size += sizeof(CARD32);  /* in-line*/
  size += sizeof(CARD32);  /* command id */
  size += sizeof(CARD32);  /* selection */
  size += sizeof(CARD32);  /* count */
  size += sizeof(CARD32);  /* window */

  save = msg = (XtPointer) XtMalloc(size);

  window |= windowBit;

  msg = PackCARD32(msg, (CARD32)in_line);
  msg = PackCARD32(msg, (CARD32)commandID);
  msg = PackCARD32(msg, (CARD32)selection);
  msg = PackCARD32(msg, (CARD32)count);
  msg = PackCARD32(msg, (CARD32)window);

  PRINT("IncludeCommand #%d on window %x (%u).\n", commandID, window, window);

  SendMessage(INCLUDE, save, size, time);
}



/*----------------------------------------------------------------------*
 |                                 SendRemove                         |
 *----------------------------------------------------------------------*/
void
SendRemoveCommand(Window window, CARD32 commandID)
{
  Time time;
  XtPointer msg,save;
  CARD32  selection = 0L;
  CARD32  count = 1L;
  CARD32  windowBit = 1<<30;
/*  CARD32  iconBit = 1<<31;*/
  char   str[128];
  int     size=0;

  time = GetTimestamp(dsp);

  selection += atoms.own_selection;
  /*
   *  Fixup the window id receiving the client command.
   *  Top 2 bits:
   *  	10 - Icon only
   *	01 - Window only
   *	11 - Both Icon and Window
   *	00 - Reserved (Don't Use!)
   *
   *  Bottom 30 bits:
   *	0x3fffffff - All windows and/or icons
   *	0x00000000 - Root window menu
   */

  window |= windowBit;

  size += sizeof(CARD32);  /* command id */
  size += sizeof(CARD32);  /* count */
  size += sizeof(CARD32);  /* window id */


  save = msg = (XtPointer) XtMalloc(size);

  msg = PackCARD32(msg, (CARD32) commandID);
  msg = PackCARD32(msg, (CARD32) count);
  msg = PackCARD32(msg, (CARD32) window);

  PRINT("RemoveCommand #%d on window %x (%u).\n", commandID, window, window);

  SendMessage(REMOVE, save, size, time);
}


/*----------------------------------------------------------------------*
  |                                 SendDefineCommand                     |
 *----------------------------------------------------------------------*/
void
SendDefineCommand (int parID, int commID, char *cmdName, char *cmdDefault)
{
  Time    time;
  XtPointer msg,save;
  int     size=0;
  CARD32  selection = 0L;
  CARD32 commandID = 0L;
  CARD32 parentID = 0L;

  commandID += commID;
  parentID += parID;
  time = GetTimestamp(dsp);

  selection += atoms.own_selection;
  size += sizeof(CARD32);  /* command id */
  size += sizeof(CARD32); /* selection */
  size += sizeof(CARD32);  /* parent id */
  size += sizeof(CARD16) + (strlen(cmdName) * sizeof(CARD8)); /* name */
  size += sizeof(CARD16) + (strlen(cmdDefault) * sizeof(CARD8)); /* defalault label */

  save = msg = (XtPointer) XtMalloc(size);


  msg = PackCARD32(msg, (CARD32) commandID);
  msg = PackCARD32(msg, (CARD32) selection);
  msg = PackCARD32(msg, (CARD32) parentID);
  msg = PackString(msg, (String) cmdName);
  msg = PackString(msg, (String) cmdDefault);

  PRINT("DefineCommand: %d, %d, %d, '%s', '%s'\n",
	 commandID, selection, parentID, cmdName, cmdDefault);
  SendMessage(DEFINE, save, size, time);


}


/*----------------------------------------------------------------------*
 |                                 SendRename                           |
 *----------------------------------------------------------------------*/
void
SendRenameCommand(Window window, int commID, char *new_name)
{
  Time time;
  XtPointer msg, save;
  CARD32  commandID = 0L;
  CARD32  count = 1L;
  CARD32  windowBit = 1<<30;
/*  CARD32  iconBit = 1<<31;*/
  char   str[128];
  int     size=0;

  commandID += commID;

  time = GetTimestamp(dsp);
  /*
   *  Fixup the window id receiving the client command.
   *  Top 2 bits:
   *  	10 - Icon only
   *	01 - Window only
   *	11 - Both Icon and Window
   *	00 - Reserved (Don't Use!)
   *
   *  Bottom 30 bits:
   *	0x3fffffff - All windows and/or icons
   *	0x00000000 - Root window menu
   */

  size += sizeof(CARD32);  /* command id */
  size += sizeof(CARD16) + (strlen(new_name) * sizeof(CARD8));
  size += sizeof(CARD32);  /* count */
  size += sizeof(CARD32);  /* window */

  save = msg = (XtPointer) XtMalloc(size);

  window |= windowBit;

  msg = PackCARD32(msg, (CARD32)commandID);
  msg = PackString(msg, (String) new_name);
  msg = PackCARD32(msg, (CARD32)count);
  msg = PackCARD32(msg, (CARD32)window);

  PRINT("RenameCommand #%d to %s on window %x (%u).\n", commandID,
	new_name, window, window);

  SendMessage(RENAME, save, size, time);
}



/*----------------------------------------------------------------------*
  |                               CS Structure stuff                    |
 *----------------------------------------------------------------------*/
CommandSpace *
GetCSFromCopyID(CARD32 i)
{
  CommandSpace *cs;
  cs = command_space_list;
  while (cs != NULL)
    {
      if (cs->copy_command_id == i)
	{
	  return cs;
	}
      cs = cs->next;
    }
  return NULL;
}





CommandSpace *
GetCSFromLinkID(CARD32 i)
{
  CommandSpace *cs;
  cs = command_space_list;
  while (cs != NULL)
    {
      if (cs->link_command_id == i)
	{
	  return cs;
	}
      cs = cs->next;
    }
  return NULL;
}



CommandSpace *
GetCSFromSwitchID(CARD32 i)
{
  CommandSpace *cs;
  cs = command_space_list;
  while (cs != NULL)
    {
      if (cs->switch_command_id == i)
	return cs;
      cs = cs->next;
    }
  PRINT("GetCSFromSwitch(%d) returning NULL\n",i);
  return NULL;
}



CommandSpace *
GetCSFromSpaceID(int i)
{
  int j;
  CommandSpace *cs;
  cs = command_space_list;
  if (cs == NULL) return NULL;

  for (j = 0; j < i; j++)
    {
      if (cs == NULL)
	return NULL;
      cs = cs->next;
    }
  return cs;
}



CommandSpace *
GetCSFromSpace(Space *space)
{
  CommandSpace *cs;
  cs = command_space_list;
  while (cs != NULL)
    {
      if (cs->space == space)
	return cs;
      cs = cs->next;
    }
  return NULL;
}

Space *
GetSpaceFromLinkID(CARD32 c_id)
{
  CommandSpace *cs = command_space_list;
  while (cs != NULL)
    {
      if (cs->link_command_id == c_id)
	return cs->space;
      cs = cs->next;
    }
  return NULL;
}




Space *
GetSpaceFromCopyID(CARD32 c_id)
{
  CommandSpace *cs = command_space_list;
  while (cs != NULL)
    {
      if (cs->copy_command_id == c_id)
	return cs->space;
      cs = cs->next;
    }
  return NULL;
}



Space *
GetSpaceFromSwitchID(CARD32 c_id)
{
  CommandSpace *cs = command_space_list;
  while (cs != NULL)
    {
      if (cs->switch_command_id == c_id)
	return cs->space;
      cs = cs->next;
    }
  return NULL;
}

void
RemoveCommandSpace(CommandSpace *cspace)
{
  CommandSpace *cs = command_space_list;
  CommandSpace *pcs = command_space_list;

  while (cs != NULL)
    {
      if (cs == cspace)
	{
	  if (cs == pcs)
	    command_space_list = cs->next;
	  else
	    pcs->next = cs->next;
	}
      pcs = cs;
      cs = cs->next;
    }

  XtFree((XtPointer)cspace);
}




CommandSpace*
CreateCommandSpace(Space *space,CARD32 copy_id, CARD32 link_id,CARD32 sw_id)
{
  CommandSpace *s, *s2;
  s = (CommandSpace*)XtMalloc(sizeof(CommandSpace));
  s->space = space;
  s->copy_command_id = copy_id;
  s->link_command_id = link_id;
  s->switch_command_id = sw_id;
  s->next = NULL;

  if (command_space_list == NULL)
    command_space_list = s;
  else
    {
      for (s2 = command_space_list; s2->next != NULL; s2 = s2->next);
      s2->next = s;
    }
  return s;
}


/*----------------------------------------------------------------------*
 |                                InvokeCommand                      |
 *----------------------------------------------------------------------*/
void
InvokeCommand (
     Widget         w,
     Atom           target,
     MessageData    data,
     unsigned long  len,
     int            fmt)
{
  CARD32      	commandID;
  Window  	windowID;
  WorkWindow *w_window;
  CommandSpace *cs;
  commandID = UnpackCARD32(&data);
  windowID = UnpackCARD32(&data);


  PRINT("commandID = %d\n",commandID);
  PRINT("windowID = %d\n", windowID);

  if (commandID == show_id)
  {
      if (wsm_shown)
      {
	  PRINT("hide workspace manager\n");
	  HideWsm();
      }
      else
      {
	  PRINT("show workspace manager\n");
	  ShowWsm();
      }
    }
  else if (commandID == occupy_id)
    {
      PRINT("copy window\n");
      w_window = GetWorkWindow(windowID);
      if (w_window != NULL)
	CreateOccupy(w_window);
    }
  else if (commandID == copy_all_id)
    {
      PRINT("copy window\n");
      w_window = GetWorkWindow(windowID);
      if (w_window != NULL)
	CopyWindow(w_window,current_space,all_space);
    }
  else if (commandID == link_all_id)
    {
      PRINT("copy window\n");
      w_window = GetWorkWindow(windowID);
      if (w_window != NULL)
	LinkWindow(w_window,current_space,all_space);
    }
  else if (commandID == remove_id)
    {
      PRINT("remove id\n");
      w_window = GetWorkWindow(windowID);
      if (w_window != 0)
	{
	  if (w_window->s_list->next != NULL && w_window->window != None &&
	      _WSMGetConfigFormatType(w_window->window) != WSM_ICON_FMT)
	    DeleteWindow(w_window,current_space);
      }
    }
  else if ((cs = GetCSFromSwitchID(commandID)) != NULL)
    {
      PRINT("SendLeaveRoom(%s)\n",cs->space->name);
      XmToggleButtonSetState(wsm_ui->space_button[GetSpaceID(cs->space)],True,True);
    }
  else if ((cs = GetCSFromCopyID(commandID)) != NULL)
    {
      PRINT("copy space");
      w_window = GetWorkWindow(windowID);
      if (w_window != NULL)
	{
	  PRINT(" %s\n",cs->space->name);
	  CopyWindow(w_window,current_space, cs->space);
	}
    }
  else if ((cs = GetCSFromLinkID(commandID)) != NULL)
    {
      PRINT("link space");
      w_window = GetWorkWindow(windowID);
      if (w_window != NULL)
	{
	  PRINT(" %s\n",cs->space->name);
	  LinkWindow(w_window,current_space, cs->space);
	}
    }


}


/*----------------------------------------------------------------------*
  |                              GenerateSwitchMenu
  |
 *----------------------------------------------------------------------*/
void
GenerateSwitchMenu(void)
{
  CommandSpace *cspace;

  switch_id = GET_COMMAND_ID(dsp);

  SendDefineCommand(0,switch_id, "_WSM_SWITCH_WORKSPACE", "Switch Workspace");

  for (cspace = command_space_list; cspace != NULL;
       cspace = cspace->next)
  {
      SendDefineCommand(switch_id, cspace->switch_command_id,
			cspace->space->name, cspace->space->name);
  }
}

/*----------------------------------------------------------------------*
  |                              SetInitialCommands
  |
 *----------------------------------------------------------------------*/
void
SetInitialCommands(void)
{
  Space *space;
/* all window stuff  */

  PRINT("set initial commands \n");
  occupy_id = GET_COMMAND_ID(dsp);
  copy_all_id = GET_COMMAND_ID(dsp);
  link_all_id = GET_COMMAND_ID(dsp);
  remove_id = GET_COMMAND_ID(dsp);
  show_id = GET_COMMAND_ID(dsp);

  for (space = space_list; space != NULL; space = space->next)
  {
      (void)CreateCommandSpace(space, GET_COMMAND_ID(dsp),
			       GET_COMMAND_ID(dsp), GET_COMMAND_ID(dsp));
  }

  SendDefineCommand(0,occupy_id, "_WSM_OCCUPY", "Occupy...");
  SendDefineCommand(0,copy_all_id, "_WSM_COPY_ALL", "Copy To All");
  SendDefineCommand(0,link_all_id, "_WSM_LINK_ALL", "Link To All");
  SendDefineCommand(0,remove_id, "_WSM_REMOVE_FROM_WS",
		    "Remove From Workspace");
  GenerateSwitchMenu();

  if (wsm_shown)
      SendDefineCommand(0,show_id, "_WSM_HIDE_WSM", "Hide Workspace Manager");
  else
      SendDefineCommand(0,show_id, "_WSM_SHOW_WSM", "Show Workspace Manager");

  SendIncludeCommand(0x3fffffff,occupy_id,   True);
  SendIncludeCommand(0x3fffffff,copy_all_id,   True);
  SendIncludeCommand(0x3fffffff,link_all_id,   True);
  SendIncludeCommand(0x3fffffff,remove_id, True);
  SendIncludeCommand(0x00000000,switch_id, False);
  SendIncludeCommand(0x00000000,show_id,   True);

}


/*----------------------------------------------------------------------*
 |                               AddWindow
 | This will add commands to a new window
 *----------------------------------------------------------------------*/
void
AddWindow(Window win)
{
    SendIncludeCommand(win,occupy_id,   True);
    SendIncludeCommand(win,copy_all_id,   True);
    SendIncludeCommand(win,link_all_id,   True);
    SendIncludeCommand(win,remove_id, True);
}

/*----------------------------------------------------------------------*
 |                               ShowWsmCommand
 | This will add a Show or Hide Workspace Manager
 *----------------------------------------------------------------------*/
void
ShowWsmCommand(void)
{
    if (wsm_shown)
	SendRenameCommand(0x0, show_id, "Hide Workspace Manager");
    else
	SendRenameCommand(0x0, show_id, "Show Workspace Manager");
}

/*----------------------------------------------------------------------*
 |                               AddSpaceCommand                        |
 | This will add a new workspace name to the "Switch" root menu command.
 *----------------------------------------------------------------------*/
void
AddSpaceCommand(Space *space)
{
  CommandSpace *cs =
      CreateCommandSpace(space, GET_COMMAND_ID(dsp), GET_COMMAND_ID(dsp),
			 GET_COMMAND_ID(dsp));

  SendRemoveCommand(0x00000000,switch_id);
  SendRemoveCommand(0x00000000,show_id);

  GenerateSwitchMenu();

  SendIncludeCommand(0x00000000,switch_id, False);
  SendIncludeCommand(0x00000000,show_id, True);
}




/*----------------------------------------------------------------------*
 |                               RemoveSpaceCommand                      |
 *----------------------------------------------------------------------*/
void
RemoveSpaceCommand(int i)
{
  CommandSpace *cs = GetCSFromSpaceID(i);

  if (cs != NULL)
    {
      RemoveCommandSpace(cs);
    }

  SendRemoveCommand(0x00000000,switch_id);
  SendRemoveCommand(0x00000000,show_id);

  GenerateSwitchMenu();

  SendIncludeCommand(0x00000000,switch_id, False);
  SendIncludeCommand(0x00000000,show_id, True);
}




/*----------------------------------------------------------------------*
 |                           ChangeSpaceCommandName                     |
 *----------------------------------------------------------------------*/
void
ChangeSpaceCommandName(Space *space)
{
  CommandSpace *cs = GetCSFromSpace(space);
  if (cs != NULL)
    {
	SendRenameCommand(0x0, cs->switch_command_id, space->name);
    }
}



/*----------------------------------------------------------------------*
 |                               DisableDeleteCommand                  |
 *----------------------------------------------------------------------*/
void
DisableDeleteCommand(Window window)
{
  if (remove_id != 0)
    {
      SendDisableCommand(window,remove_id);
    }
}




/*----------------------------------------------------------------------*
 |                             EnableDeleteCommand                      |
 *----------------------------------------------------------------------*/
void
EnableDeleteCommand(Window window)
{
  if (remove_id != 0)
    {
      SendEnableCommand(window,remove_id);
    }
}



/*----------------------------------------------------------------------*
 |                          RemoveAllCommandsAndExit                    |
 *----------------------------------------------------------------------*/
void
RemoveAllCommandsAndExit()
{
  Window rootWindow = XRootWindowOfScreen(XtScreen(shell));
  PRINT("REMOVE ALL COMMANDS\n");
  SendRemoveCommand(0x3fffffff,occupy_id);
  SendRemoveCommand(0x3fffffff,copy_all_id);
  SendRemoveCommand(0x3fffffff,link_all_id);
  SendRemoveCommand(0x3fffffff,remove_id);
  SendRemoveCommand(0x00000000,switch_id);
  exiting = True;
  SendRemoveCommand(0x00000000,show_id);
}



/*----------------------------------------------------------------------*
 |                              InternStuff                             |
 *----------------------------------------------------------------------*/
void InternStuff(Display *d)
{
  WM_S0                     = XInternAtom(d, "WM_S0", False);
  _MOTIF_WM_DEFINE_COMMAND  = XInternAtom(d, "_MOTIF_WM_DEFINE_COMMAND", False);
  _MOTIF_WM_INCLUDE_COMMAND = XInternAtom(d, "_MOTIF_WM_INCLUDE_COMMAND", False);
  _MOTIF_WM_DISABLE_COMMAND = XInternAtom(d, "_MOTIF_WM_DISABLE_COMMAND", False);
  _MOTIF_WM_ENABLE_COMMAND  = XInternAtom(d, "_MOTIF_WM_ENABLE_COMMAND", False);
  _MOTIF_WM_INVOKE_COMMAND  = XInternAtom(d, "_MOTIF_WM_INVOKE_COMMAND", False);
  _MOTIF_WM_REMOVE_COMMAND  = XInternAtom(d, "_MOTIF_WM_REMOVE_COMMAND", False);
  _MOTIF_WM_RENAME_COMMAND  = XInternAtom(d, "_MOTIF_WM_RENAME_COMMAND", False);
}

