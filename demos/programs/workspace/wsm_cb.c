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
static char rcsid[] = "$TOG: wsm_cb.c /main/8 1997/05/02 10:05:18 dbl $"
#endif
#endif
#include <stdio.h>
#include <stdlib.h>
#include <Xm/Xm.h>
#include <Xm/TextF.h>
#include <Xm/Text.h>
#include <Xm/List.h>
#include <Xm/ToggleB.h>
#include <Xm/PushB.h>

#include "wsm.h"
#include "wsmStruct.h"
#include "wsm_ui.h"
#include "wsmDebug.h"
#include "xrmLib.h"
#include "wsm_cb.h"
#include "command_ui.h"
#include "wsm_create.h"

extern Boolean wsm_shown;
extern Widget shell;
extern WSM_UI *wsm_ui;
extern Boolean connected;
extern Boolean mwm_gone;
extern Widget fromField;
extern Widget toField;
extern Space* all_space;
extern Space* current_space;
extern Space* space_list;
extern char *file_name;

Boolean copy_mode = True;

/**********************************************************************/
/*                        WSM Dialog Interface                       */
/**********************************************************************/
void InitializeInterface(WSM_UI *wui)
{
  int num_rooms = GetNumberSpaces();
  int max_num_rooms = 20;

  wui->space_button = (Widget*) XtMalloc(max_num_rooms*sizeof(Widget));
  wui->num_space_buttons = num_rooms;
  wui->from_option_button = (Widget*) XtMalloc(max_num_rooms*sizeof(Widget));
  wui->to_option_button = (Widget*)XtMalloc((max_num_rooms +1)*sizeof(Widget));
  wui->from_space = NULL;
  wui->to_space = NULL;
}

void HideWsm(void)
{
    wsm_shown = False;
    ShowWsmCommand();
/*  XtPopdown(XtParent(XtParent(wsm_ui->wsm_row_column)));*/
  XtPopdown(XtParent(wsm_ui->configure_form));
  XtPopdown(XtParent(wsm_ui->name_form));
  XtPopdown(XtParent(wsm_ui->delete_form));
  XtPopdown(XtParent(wsm_ui->save_as_form));
  XtUnmapWidget(shell);
}

void ShowWsm(void)
{
    wsm_shown = True;
    ShowWsmCommand();
/*  XtPopup(XtParent(XtParent(wsm_ui->wsm_row_column)),XtGrabNone);*/
  XtMapWidget(shell);
}

/**********************************************************************/
/*                       Workspace Panel                              */
/**********************************************************************/

Widget
CreateWorkspacePanel(Widget w, WSM_UI *wui, Boolean show_menu)
{
  Widget parent;
  XmString xmstr;
  int i;
  Space *space;
  parent = CreateWorkspacePanelBX(w, wui, show_menu);

  space = space_list;
  for (i = 0; space != NULL && i < wui->num_space_buttons; i++)
    {
      xmstr = XmStringLtoRCreate(space->name, XmSTRING_DEFAULT_CHARSET);
      XtVaSetValues(wui->space_button[i],XmNlabelString,xmstr,NULL);
      XmStringFree(xmstr);
      space = space->next;
    }
  return parent;
}

void CreateNewSpaceButton(int i, char *name, WSM_UI *wui)
{
  int argcnt;
  Arg args[15];
  XmString xmstr;
  int user_data = i+1;

  argcnt = 0;
  XtSetArg(args[argcnt], XmNindicatorSize, 20); argcnt++;
  XtSetArg(args[argcnt], XmNspacing, 10); argcnt++;
  XtSetArg(args[argcnt], XmNuserData, user_data); argcnt++;
  XtSetArg(args[argcnt], XmNlabelString,
	   (xmstr=XmStringLtoRCreate(name,XmSTRING_DEFAULT_CHARSET))); argcnt++;
  XtSetArg(args[argcnt], XmNrecomputeSize, True); argcnt++;
  wui->space_button[i] = XtCreateWidget("pushButton",
					   xmToggleButtonWidgetClass,
					   wui->wsm_row_column,
					   args,
					   argcnt);
  XmStringFree(xmstr);

  XtAddCallback(wui->space_button[i], XmNvalueChangedCallback,
		SendLeaveRoomCB, (XtPointer)wui);
  XtManageChild(wui->space_button[i]);
}

void
SendLeaveRoomCB(Widget w, XtPointer client, XtPointer call)
{
  int wsm_index;
  Space *space;
  XmToggleButtonCallbackStruct *toggle_struct = (XmToggleButtonCallbackStruct*)call;

  XtVaGetValues(w,XmNuserData, &wsm_index,NULL);
  if (connected && !mwm_gone)
    {
      if (toggle_struct->set)
	{
	  space = GetSpaceFromID(wsm_index -1);
	  if (space != NULL)
	    (void) SendLeaveRoom(space);
	}
    }
  if (mwm_gone && current_space != NULL)
      XmToggleButtonSetState(wsm_ui->space_button[GetSpaceID(current_space)],True,True);
}

void
NewActivateCB(Widget w, XtPointer client, XtPointer call)
{
  static int i = 0;
  char str[20];
  WSM_UI *wui = (WSM_UI*)client;
  Space *space;

  if (i == 0)
    {
      for (i = 1, space = space_list; space != NULL; i++, space = space->next);
    }
  sprintf(str,"Room%d",i++);
  space = CreateSpace(XrmStringToQuark(str),str);
  CreateNewSpaceButton(wui->num_space_buttons,str, wui);
  CreateFromOptionButton(wui->num_space_buttons,str);
  CreateToOptionButton(wui->num_space_buttons,str);
#ifndef _NO_CLIENT_COMMAND
  AddSpaceCommand(space);
#endif
  wui->num_space_buttons++;
  UpdateSpaceList(wui->delete_list);
  UpdateSpaceList(wui->name_list);
  UpdateSpaceList(wui->background_list);
#ifndef _NO_OCCUPY_DIALOG
  UpdateSpaceList(wui->occupy_list);
#endif
}

void
HideActivateCB(Widget w, XtPointer client, XtPointer call)
{
  (void)w;
  (void)client;
  (void)call;
  HideWsm();
}

void
SaveActivateCB(Widget w, XtPointer client, XtPointer call)
{
  (void)w;
  (void)client;
  (void)call;
  SendSaveWsm(file_name);
}


void
ExitCB(Widget w, XtPointer client, XtPointer call)
{
  (void)w;
  (void)client;
  (void)call;
  ManageAllWindowsAndExit();
}

/**********************************************************************/
/*                        WSM Configure CBs                           */
/**********************************************************************/


void
CreateFromOptionButton(int i, char *name)
{
  int argcnt;
  Arg args[15];
  XmString xmstr;

  argcnt = 0;
  XtSetArg(args[argcnt], XmNuserData, i+1); argcnt++;
  XtSetArg(args[argcnt], XmNrecomputeSize, True); argcnt++;
  XtSetArg(args[argcnt], XmNlabelString,
	   (xmstr=XmStringLtoRCreate(name, XmSTRING_DEFAULT_CHARSET))); argcnt++;
  wsm_ui->from_option_button[i] = XtCreateWidget("fromWorkspace1Button",
						 xmPushButtonWidgetClass,
						 XtParent(wsm_ui->from_option_button[0]),
						 args,
						 argcnt);
  XmStringFree( xmstr );

  XtAddCallback(wsm_ui->from_option_button[i], XmNactivateCallback,
		FromWorkspaceCB, (XtPointer)wsm_ui);
  XtManageChild(wsm_ui->from_option_button[i]);
}



void
CreateToOptionButton(int i, char *name)
{
  int argcnt;
  Arg args[15];
  XmString xmstr[2];

  xmstr[0] = XmStringCreateLocalized(name);
  XtVaSetValues(wsm_ui->to_option_button[i],
		XmNuserData, i+1,
		XmNlabelString, xmstr[0],
		NULL);

  XmStringFree(xmstr[0]);
  argcnt = 0;
  XtSetArg(args[argcnt], XmNuserData, 0); argcnt++;
  XtSetArg(args[argcnt], XmNlabelString,
	   (xmstr[1]=XmStringLtoRCreate("All Workspaces", XmSTRING_DEFAULT_CHARSET))); argcnt++;
  wsm_ui->to_option_button[i+1] = XtCreateWidget("fromWorkspace1Button",
						 xmPushButtonWidgetClass,
						 XtParent(wsm_ui->to_option_button[0]),
						 args,
						 argcnt);
  XmStringFree( xmstr[1] );

  XtAddCallback(wsm_ui->to_option_button[i+1], XmNactivateCallback,
		ToWorkspaceCB, (XtPointer)wsm_ui);
  XtManageChild(wsm_ui->to_option_button[i+1]);
}

void
UpdateList(Widget list,Space *s)
{

 char *str;
 XmString xmstr;
 WorkWindowList *w_list;

 w_list = s->w_list;
 XmListDeleteAllItems(list);
 while (w_list != NULL)
   {
     if (_WSMGetConfigFormatType(w_list->work_win->window) == WSM_WINDOW_FMT)
       {
	 str = (char *)XtMalloc(strlen(w_list->work_win->name) + 15);
	 sprintf(str,"0x%x %s",(unsigned int)w_list->work_win->window, w_list->work_win->name);

	 xmstr = XmStringCreateLocalized(str);
	 XmListAddItemUnselected(list,xmstr,0);
	 XmStringFree(xmstr);
	 XtFree(str);
       }
     w_list = w_list->next;
   }
}

void
UpdateBothList(Space *s)
{
  if(connected)
    {
      if (wsm_ui->from_space == s) UpdateList(wsm_ui->from_list,s);
      if (wsm_ui->to_space == s)   UpdateList(wsm_ui->to_list,s);
    }
}

void
UpdateButtons(WorkWindow *w_window)
{
  if (w_window == wsm_ui->w_window)
    {
      if (w_window->linked)
	XmToggleButtonSetState(wsm_ui->link_toggle,True,True);
      else
	XmToggleButtonSetState(wsm_ui->copy_toggle,True,True);
      if (w_window->s_list->next == NULL || w_window->window == 0 ||
	  _WSMGetConfigFormatType(w_window->window) == WSM_ICON_FMT)
	XtVaSetValues(wsm_ui->delete_button,XmNsensitive,False,NULL);
      else
	XtVaSetValues(wsm_ui->delete_button,XmNsensitive,True,NULL);
      if (w_window->window == 0 ||
	  _WSMGetConfigFormatType(w_window->window) == WSM_ICON_FMT ||
	  wsm_ui->to_space == all_space)
	XtVaSetValues(wsm_ui->move_button,XmNsensitive,False,NULL);
      else
	XtVaSetValues(wsm_ui->move_button,XmNsensitive,True,NULL);
    }
}

void
CreateConfigureCB(Widget w, XtPointer client, XtPointer call)
{
  WSM_UI *wui = (WSM_UI*)client;
  int space_id = -1;
  if (connected )
    {
      if (wui->from_space == NULL && wui->to_space == NULL)
	{
	    wui->from_space = current_space;
	    wui->to_space = current_space;
	    UpdateBothList(current_space);
	    space_id = GetSpaceID(current_space);
	    if (space_id != -1)
	      {
		XtVaSetValues(wui->from_option_menu,
			      XmNmenuHistory, wui->from_option_button[space_id],
			      NULL);
		XtVaSetValues(wui->to_option_menu,
			      XmNmenuHistory, wui->to_option_button[space_id],
			      NULL);
	      }
	}
    }
  XtManageChild(wui->configure_form);
  XtPopup(XtParent(wui->configure_form), XtGrabNone);

}

void MoveCB(Widget w, XtPointer client, XtPointer call)
{
	int pos_count = 0;
	int *pos_list;
	int i;
	WorkWindow *w_window;
	WSM_UI *wui = (WSM_UI *)client;

	XtVaGetValues(wui->from_list, XmNselectedPositionCount, &pos_count,
	              XmNselectedPositions, &pos_list, NULL);

	if (pos_count <= 0)
		return;

	for (i = 0; i < pos_count; i++) {
		if ((w_window = GetWorkWindowID(wui->from_space, pos_list[i] - 1)))
			MoveWindow(w_window, wui->from_space, wui->to_space);
	}

	XtFree((XtPointer)pos_list);
}

void DeleteCB(Widget w, XtPointer client, XtPointer call)
{
	int i, pos_count = 0;
	int *pos_list = NULL;
	WorkWindow *w_window;
	WSM_UI *wui = (WSM_UI *)client;

	XtVaGetValues(wui->from_list, XmNselectedPositionCount, &pos_count,
	              XmNselectedPositions, &pos_list, NULL);

	if (pos_count <= 0)
		return;

	for (i = 0; i < pos_count; i++) {
		if ((w_window = GetWorkWindowID(wui->from_space, pos_list[i] - 1)))
			DeleteWindow(w_window, wui->from_space);
	}

	XtFree((XtPointer)pos_list);
}

void
ToWorkspaceCB(Widget w, XtPointer client, XtPointer call)
{
  WSM_UI *wui = (WSM_UI*)client;
  int room_num = 0;
  Space *space;

  XtVaGetValues(w,XmNuserData,&room_num, NULL);
  if (room_num != 0)
      space = GetSpaceFromID(room_num-1);
  else
      space = all_space;
  if (space != NULL)
    {
      wui->to_space = space;
      UpdateList(wui->to_list,space);
      if (space == all_space)
	XtVaSetValues(wui->move_button,XmNsensitive,False,NULL);
      else
	XtVaSetValues(wui->move_button,XmNsensitive,True,NULL);
    }
  else PRINT("not found %d\n", room_num);
}

void
FromWorkspaceCB(Widget w, XtPointer client, XtPointer call)
{

  WSM_UI *wui = (WSM_UI*)client;
  int room_num;
  Space *space;

  XtVaGetValues(w,XmNuserData,&room_num, NULL);
  space = GetSpaceFromID(room_num-1);
  if (space != NULL)
    {
      wui->from_space = space;
      UpdateList(wui->from_list,space);
    }
  else PRINT("Not Found %d\n", room_num);
}

void Copy(WSM_UI *wui)
{
	int pos_count = 0;
	int *pos_list;
	int i;
	WorkWindow *w_window;

	XtVaGetValues(wui->from_list, XmNselectedPositionCount, &pos_count,
	              XmNselectedPositions, &pos_list, NULL);

	if (pos_count <= 0)
		return;

	for (i = 0; i < pos_count; i++) {
		if ((w_window = GetWorkWindowID(wui->from_space, pos_list[i] - 1)))
			CopyWindow(w_window, wui->from_space, wui->to_space);
	}

	XtFree((XtPointer)pos_list);
}

void Link(WSM_UI *wui)
{
	int pos_count = 0;
	int *pos_list;
	int i;
	WorkWindow *w_window;

	XtVaGetValues(wui->from_list, XmNselectedPositionCount, &pos_count,
	              XmNselectedPositions, &pos_list, NULL);

	if (pos_count <= 0)
		return;

	for (i = 0; i < pos_count; i++) {
		if ((w_window = GetWorkWindowID(wui->from_space, pos_list[i] - 1)))
			LinkWindow(w_window, wui->from_space, wui->to_space);
	}

	XtFree((XtPointer)pos_list);
}

void OccupyCB(Widget w, XtPointer client,XtPointer  call)
{
	WSM_UI *wui = (WSM_UI *)client;
	Boolean copy_set;

	XtVaGetValues(wui->copy_toggle, XmNset,&copy_set,NULL);
	if (copy_set) Copy(wui);
	else Link(wui);
}

void WindowModeCB(Widget w,XtPointer client,XtPointer call)
{
	WSM_UI *wui = (WSM_UI*)client;
	XtVaSetValues(wui->from_list,XmNselectionPolicy,XmSINGLE_SELECT,NULL);
}

void ClientModeCB(Widget w,XtPointer client,XtPointer call)
{
	WSM_UI *wui = (WSM_UI*)client;
	XtVaSetValues(wui->from_list,XmNselectionPolicy,XmMULTIPLE_SELECT,NULL);
}

void SelectFromListCB(Widget w,XtPointer client,XtPointer call)
{
	int pos_count = 0;
	int *pos_list;
	int i;
	WorkWindow *w_window;
	WSM_UI *wui = (WSM_UI *)client;

	XtVaGetValues(wui->from_list, XmNselectedPositionCount, &pos_count,
	              XmNselectedPositions, &pos_list, NULL);

	if (pos_count <= 0)
		return;

	if ((w_window = GetWorkWindowID(wui->from_space, pos_list[0] - 1))) {
		wui->w_window = w_window;
		UpdateButtons(w_window);
	}

	XtFree((XtPointer)pos_list);
}

void MultSelectFromListCB(Widget w,XtPointer client,XtPointer call)
{
	WSM_UI *wui = (WSM_UI *)client;
	XmListCallbackStruct *list_call = (XmListCallbackStruct *)call;
	int pos_count = 0;
	int *pos_list = NULL;
	int *select_ids = NULL;
	int num_select = 0;
	int i;
	int item_pos = list_call->item_position;
	Boolean doit = False;
	WorkWindow *w_window;

	if (!(w_window = GetWorkWindowID(wui->from_space, item_pos - 1)))
		return;

	wui->w_window = w_window;
	UpdateButtons(w_window);
	GetWorkWindowClientIDs(item_pos - 1, wui->from_space, &select_ids, &num_select);

	XtVaGetValues(wui->from_list, XmNselectedPositionCount, &pos_count,
	              XmNselectedPositions, &pos_list, NULL);

	for (i = 0; i < pos_count; i++) {
		if (pos_list[i] != item_pos)
			XmListDeselectPos(wui->from_list, pos_list[i]);
	}

	for (i = 0; i < num_select; i++) {
		if (select_ids[i] != item_pos - 1)
			XmListSelectPos(w, select_ids[i] + 1, False);
	}

	XtFree((XtPointer)pos_list);
}

void SelectToListCB(Widget w, XtPointer client,XtPointer call)
{
	WSM_UI *wui = (WSM_UI *)client;
	int pos_count = 0;
	int *pos_list;

	(void)w;
	(void)call;
	/*  XmListDeselectAllItems(wui->to_list);*/
	XtVaGetValues(wui->from_list, XmNselectedPositionCount, &pos_count,
	              XmNselectedPositions, &pos_list, NULL);

	if (!pos_count)
		return;

	XmListDeselectPos(wui->to_list, pos_list[0]);
	XtFree((XtPointer)pos_list);
}

void DismissConfigureCB(Widget w, XtPointer client, XtPointer call)
{
	WSM_UI *wui = (WSM_UI *)client;

	(void)w;
	(void)client;
	(void)call;
	XtPopdown(XtParent(wui->configure_form));
}

/**********************************************************************/
/*                        WSM Name Workspace CBs                      */
/**********************************************************************/

void
CreateNameCB(Widget w, XtPointer client, XtPointer call)
{

  WSM_UI *wui = (WSM_UI*)client;

  if (connected )
    {
      UpdateSpaceList(wui->name_list);
    }

  XtManageChild(wui->name_form);
  XmListSelectPos(wui->name_list,1,True);
  XtPopup(XtParent(wui->name_form), XtGrabNone);
}

void
UpdateSpaceList(Widget list)
{
  XmString xmstr;
  Space *s = space_list;

  XmListDeleteAllItems(list);
  while (s != NULL)
   {
     xmstr = XmStringLtoRCreate(s->name, XmSTRING_DEFAULT_CHARSET);
     XmListAddItem(list,xmstr,0);
     XmStringFree(xmstr);
     s = s->next;
   }
}

void SelectNameSpaceCB(Widget w, XtPointer client, XtPointer call)
{
	WSM_UI *wui = (WSM_UI*)client;
	int pos_count = 0;
	int *pos_list;
	Space *space;

	(void)w;
	(void)call;
	XtVaGetValues(wui->from_list, XmNselectedPositionCount, &pos_count,
	              XmNselectedPositions, &pos_list, NULL);
	if (pos_count <= 0)
		return;

	if ((space = GetSpaceFromID(pos_list[0] - 1))) {
		XmTextSetString(wui->name_text, space->name);
		XmTextSetString(wui->pixmap_text, space->pixmap_name);
	}

	XtFree((XtPointer)pos_list);
}

void
ChangeSpaceName(WSM_UI* wui,Space *space,int wsm_index)
{
  XmString xmstr;

  xmstr = XmStringLtoRCreate(space->name, XmSTRING_DEFAULT_CHARSET);
  XtVaSetValues(wui->space_button[wsm_index],XmNlabelString,xmstr,NULL);
  XtVaSetValues(wui->from_option_button[wsm_index],XmNlabelString,xmstr,NULL);
  XtVaSetValues(wui->to_option_button[wsm_index],XmNlabelString,xmstr,NULL);
  XmStringFree(xmstr);
  UpdateSpaceList(wui->name_list);
  UpdateSpaceList(wui->delete_list);
  UpdateSpaceList(wui->background_list);
#ifndef _NO_OCCUPY_DIALOG
  UpdateSpaceList(wui->occupy_list);
#endif
}

void NameActivateCB(Widget w, XtPointer client, XtPointer call)
{
	XmProcessTraversal(wsm_ui->pixmap_text, XmTRAVERSE_CURRENT);
}

void NameOkActivateCB(Widget w, XtPointer client, XtPointer call)
{
	WSM_UI *wui = (WSM_UI *)client;
	Space *space;
	int pos_count = 0;
	int *pos_list;
	char *old_name;
	char *str;

	(void)w;
	(void)call;
	XtVaGetValues(wui->from_list, XmNselectedPositionCount, &pos_count,
	              XmNselectedPositions, &pos_list, NULL);
	if (pos_count <= 0)
		return;

	if (!(space = GetSpaceFromID(pos_list[0] - 1)))
		goto out;

	if (*space->name) {
		if (!(old_name = (char *)XtMalloc(strlen(space->name) + 1)))
			goto out;
	} else {
		if (!(old_name = (char *)XtMalloc(1)))
			goto out;
		*old_name = '\0';
	}

	if (!(str = XmTextGetString(wui->name_text))) {
		XtFree(old_name);
		goto out;
	}

	memcpy(old_name, space->name, strlen(space->name) + 1);
	if (strlen(str) < MAX_LENGTH) {
		strcpy(space->name, str);
		if (strcmp(old_name, str)) {
			ChangeSpaceName(wui, space, pos_list[0] - 1);
#ifndef _NO_CLIENT_COMMAND
			ChangeSpaceCommandName(space);
#endif
		}
	}

	XtFree(str);
	XtFree(old_name);

#ifndef _NO_PIXMAP
	str = XmTextGetString(wui->pixmap_text);
	if (str && strcmp(str, "None") && strcmp(str, "none") && strcmp(str, space->pixmap_name)) {
		if (SetSpaceLabelPixmap(space, str)) {
			XtVaSetValues(wui->space_button[GetSpaceID(space)],
			              XmNlabelPixmap, space->pixmap_label,
			              XmNlabelType,   XmPIXMAP, NULL);
		}
		XtFree(str);
	}

	XmListSelectPos(wui->name_list, pos_list[0], True);
	/* XtPopdown(XtParent(wui->name_form); */
#endif

out:
	XtFree((XtPointer)pos_list);
}

void DismissNameCB(Widget w, XtPointer client, XtPointer call)
{
	WSM_UI *wui = (WSM_UI *)client;
	XtPopdown(XtParent(wui->name_form));
}

/**********************************************************************/
/*                        WSM Background Workspace CBs                      */
/**********************************************************************/

void
CreateBackgroundCB(Widget w, XtPointer client, XtPointer call)
{

  WSM_UI *wui = (WSM_UI*)client;

  if (connected )
    {
      UpdateSpaceList(wui->background_list);
    }
   XtManageChild(wui->background_form);
  XmListSelectPos(wui->background_list,1,True);
  XtPopup(XtParent(wui->background_form), XtGrabNone);

}

void SelectBackgroundSpaceCB(Widget w, XtPointer client, XtPointer call)
{
	WSM_UI *wui = (WSM_UI *)client;
	int pos_count = 0;
	int *pos_list;
	Space *space;

	(void)w;
	(void)call;
	XtVaGetValues(wui->background_list, XmNselectedPositionCount, &pos_count,
	              XmNselectedPositions, &pos_list, NULL);
	if (pos_count <= 0)
		return;

	if ((space = GetSpaceFromID(pos_list[0] - 1)))
		XmTextSetString(wui->background_text, space->background);
	XtFree((XtPointer)pos_list);
}

void
BackgroundActivateCB(Widget w, XtPointer client, XtPointer call)
{
  Space *space;
  int pos_count = 0;
  int *pos_list = NULL;
  char *str;

	(void)w;
	(void)call;
	(void)client;
	XtVaGetValues(wsm_ui->background_list, XmNselectedPositionCount, &pos_count,
	              XmNselectedPositions, &pos_list, NULL);
	if (pos_count <= 0 || !(space = GetSpaceFromID(pos_list[0] - 1)))
		goto out;

	str = XmTextGetString(wsm_ui->background_text);
	if (!*str) {
		XtFree(str);
		goto out;
	}

	/* SendChangeSpaceBackground(space); */
	if (SetSpacePixel(space, *str == '"' ? str + 1 : str)) {
		XtVaSetValues(wsm_ui->space_button[GetSpaceID(space)],
		              XmNbackground, space->pixel,
		              XmNunselectColor, space->pixel, NULL);
		if (space == current_space)
			SetSpaceBackground(space);
	}

	XtFree(str);
	XmListSelectPos(wsm_ui->background_list, pos_list[0], True);
	/* XtPopdown(XtParent(wsm_ui->background_form)); */

out:
	if (pos_list)
		XtFree((XtPointer)pos_list);
}

void DismissBackgroundCB(Widget w, XtPointer client, XtPointer call)
{
	WSM_UI *wui = (WSM_UI *)client;
	XtPopdown(XtParent(wui->background_form));
}

/**********************************************************************/
/*                        WSM Delete Workspace CBs                    */
/**********************************************************************/
void
CreateDeleteCB(Widget w, XtPointer client, XtPointer call)
{
 WSM_UI *wui = (WSM_UI*)client;

  if (connected )
    {
      UpdateSpaceList(wui->delete_list);
    }

  XtManageChild(wui->delete_form);
  XtPopup(XtParent(wui->delete_form), XtGrabNone);
}

void DismissDeleteCB(Widget w, XtPointer client, XtPointer call)
{
	WSM_UI *wui = (WSM_UI *)client;
	XtPopdown(XtParent(wui->delete_form));
}

void DeleteActivateCB(Widget w, XtPointer client, XtPointer call)
{
	WSM_UI *wui = (WSM_UI *)client;
	Space *space;
	int pos_count = 0;
	int *pos_list = NULL;
	int i;

	(void)w;
	(void)call;
	XtVaGetValues(wui->background_list, XmNselectedPositionCount, &pos_count,
	              XmNselectedPositions, &pos_list, NULL);
	if (pos_count <= 0 || !(space = GetSpaceFromID(pos_list[0] - 1)))
		goto out;

#ifndef _NO_CLIENT_COMMAND
	RemoveSpaceCommand(pos_list[0] - 1);
#endif
	RemoveSpace(space);
	XtDestroyWidget(wui->space_button[pos_list[0] - 1]);
	XtDestroyWidget(wui->from_option_button[pos_list[0] - 1]);
	XtDestroyWidget(wui->to_option_button[pos_list[0] - 1]);

	for (i = pos_list[0] - 1; i < wui->num_space_buttons - 1; i++) {
		wui->space_button[i] = wui->space_button[i+1];
		XtVaSetValues(wui->space_button[i], XmNuserData, i+1, NULL);
		wui->from_option_button[i] = wui->from_option_button[i+1];
		XtVaSetValues(wui->from_option_button[i], XmNuserData, i+1, NULL);
		wui->to_option_button[i] = wui->to_option_button[i+1];
		XtVaSetValues(wui->to_option_button[i], XmNuserData, i+1, NULL);
	}

	wui->to_option_button[i] = wui->to_option_button[i+1];
	wui->num_space_buttons--;
	UpdateSpaceList(wui->delete_list);
	UpdateSpaceList(wui->name_list);
	UpdateSpaceList(wui->background_list);
#ifndef _NO_OCCUPY_DIALOG
	UpdateSpaceList(wui->occupy_list);
#endif

out:
	if (pos_list)
		XtFree((XtPointer)pos_list);
}

void SelectDeleteCB(Widget w, XtPointer client, XtPointer call)
{
	WSM_UI *wui = (WSM_UI *)client;
	int pos_count = 0;
	int *pos_list = NULL;
	Space *space;
	WorkWindowList *w_list;

	XtVaGetValues(wui->background_list, XmNselectedPositionCount, &pos_count,
	              XmNselectedPositions, &pos_list, NULL);
	if (pos_count <= 0 || !(space = GetSpaceFromID(pos_list[0] - 1)))
		goto out;

	w_list = space->w_list;
	while (w_list) {
		if (!w_list->work_win->s_list->next) {
			XtVaSetValues(wui->ok_button, XmNsensitive, False, NULL);
			goto out;
		}

		w_list = w_list->next;
	}

	XtVaSetValues(wui->ok_button, XmNsensitive, True, NULL);

out:
	if (pos_list)
		XtFree((XtPointer)pos_list);
}

/**********************************************************************/
/*                        Save As CBs                      */
/**********************************************************************/
void SaveAsCB(Widget w, XtPointer client, XtPointer call)
{
	WSM_UI *wui = (WSM_UI*)client;

	(void)w;
	(void)call;
	XtManageChild(wui->save_as_form);
	XtPopup(XtParent(wui->save_as_form), XtGrabNone);
}

void
SaveAsOkCB(Widget w, XtPointer client, XtPointer call)
{
  WSM_UI *wui = (WSM_UI*)client;
  int new_length;
  char *new_file_name;
  char *home_name;
  new_file_name = XmTextGetString(wui->save_as_text);
  if (strcmp(new_file_name,"")!= 0)
    {
      if (new_file_name[0] != '/')
	{
	  home_name = getenv("HOME");
	  new_length = strlen(home_name) + 1 + strlen(new_file_name) + 2;
	  file_name = XtRealloc(file_name, new_length * sizeof(char));
	  strcpy(file_name,home_name);
	  strcat(file_name, "/");
	  strcat(file_name,new_file_name);
	}
      else
	{
	  new_length = strlen(new_file_name) + 1;
	  file_name = XtRealloc(file_name, new_length * sizeof(char));
	  strcpy(file_name,new_file_name);
	}

      SendSaveWsm(file_name);
      XtPopdown(XtParent(wui->save_as_form));
      XtFree((XtPointer)new_file_name);
    }
}

void DismissSaveAsCB(Widget w, XtPointer client, XtPointer call)
{
	WSM_UI *wui = (WSM_UI *)client;

	(void)w;
	(void)call;
	XtPopdown(XtParent(wui->save_as_form));
}

/**********************************************************************/
/*                        WSM Occupy Workspace CBs                    */
/**********************************************************************/


void
UpdateOccupySpaceList(Widget list)
{
  XmString xmstr;
  Space *s = space_list;

  XmListDeleteAllItems(list);
  while (s != NULL)
   {
     xmstr = XmStringLtoRCreate(s->name, XmSTRING_DEFAULT_CHARSET);
     XmListAddItem(list,xmstr,0);
     XmStringFree(xmstr);
     s = s->next;
   }

}

/* ARGSUSED */

void
CreateOccupy(WorkWindow *w_window)
{
  wsm_ui->occupy_window = w_window;
  if (connected )
    {
      UpdateSpaceList(wsm_ui->occupy_list);
    }

  if (w_window->linked)
    XmToggleButtonSetState(wsm_ui->link_occupy_toggle,True,True);
  else
    XmToggleButtonSetState(wsm_ui->copy_occupy_toggle,True,True);
  XtManageChild(wsm_ui->occupy_form);
  XtPopup(XtParent(wsm_ui->occupy_form), XtGrabNone);
}

void DismissOccupyCB(Widget w, XtPointer client, XtPointer call)
{
	WSM_UI *wui = (WSM_UI *)client;

	(void)w;
	(void)call;
	XtPopdown(XtParent(wui->occupy_form));
}

void OccupyActivateCB(Widget w, XtPointer client, XtPointer call)
{
	Space *space;
	int pos_count = 0;
	int *pos_list = NULL;
	int i;
	WSM_UI *wui = (WSM_UI *)client;

	(void)w;
	(void)call;
	XtVaGetValues(wui->background_list, XmNselectedPositionCount, &pos_count,
	              XmNselectedPositions, &pos_list, NULL);
	if (pos_count <= 0) // || !(space = GetSpaceFromID(pos_list[0] - 1)))
		goto out;

	for (i = 0; i < pos_count; i++) {
		if (!(space = GetSpaceFromID(pos_list[i] - 1)))
			continue;

		if (copy_mode)
			CopyWindow(wui->occupy_window, current_space, space);
		else LinkWindow(wui->occupy_window, current_space, space);
	}

out:
	if (pos_list)
		XtFree((XtPointer)pos_list);
	XtPopdown(XtParent(wui->occupy_form));
}

void CopyModeCB(Widget w, XtPointer client, XtPointer call)
{
	WSM_UI *wui = (WSM_UI *)client;

	(void)call;
	copy_mode = XmToggleButtonGetState(w);
}

void LinkModeCB(Widget w, XtPointer client, XtPointer call)
{
	WSM_UI *wui = (WSM_UI *)client;

	(void)call;
	copy_mode = XmToggleButtonGetState(w);
}

