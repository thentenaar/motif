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
static char rcsid[] = "$TOG: wsmSend.c /main/7 1997/03/31 14:15:32 dbl $"
#endif
#endif
#include <stdio.h>
#include <Xm/Xm.h>

#include "wsm.h"
#include "wsmSend.h"
#include "xrmLib.h"
#include "wsmData.h"
#include "wsmDebug.h"
#include "wsmStruct.h"
#include "command_ui.h"



/* Globals */
extern AtomStruct atoms;
extern Boolean mwm_gone;
extern Window mwm_window;
extern Space *space_list;
extern Space *all_space;
extern Space *current_space;
extern WorkWindow *work_windows;
extern XrmQuark *space_qlist;
extern XrmQuark *window_attribq;
extern XrmQuark *icon_attribq;
extern XrmQuark *global_attribq;
extern IndexStruct wsm_index;
extern WSMAttribute *window_attrib_list;
extern WSMAttribute *icon_attrib_list;
extern WSMAttribute *global_attrib_list;
extern int num_window_attrib;
extern int num_icon_attrib;
extern int num_global_attrib;
extern Boolean diffs_allowed;
extern Widget shell;
extern Display *dsp;
extern Boolean connected;
extern char *file_name;
extern Boolean mwm_reconnect;
static XrmQuark linkedRoomQuark;

/*------------------------------------------------------------------
                            GetDataListStart

  ------------------------------------------------------------------*/
static
void
GetDataListStart(WorkWindow *w_window,
		 int i,
		 XrmQuark room_q,
		 Boolean diffs,
		 WSMGetStateReply *get_state,
		 WSMWinData **data_list,
		 int *num_data)
{
  XrmValue *attr_values;
  /* get window attributes from database */
  GetWindowConfigurationEntry(w_window->specifier_qlist,
			      w_window->attrib_qlist,
			      room_q,
			      &attr_values);
  /* create set state request */
/*  print_values(attr_values,w_window->attrib_qlist,
	       room_q, w_window->num_attrib_list);*/
  /* use WSM_GET_STATE to initialize data structures */
  /* fill in data_list and num_data for WSM_SET_STATE request */
  if (get_state != NULL)
    CreateStartWSMWinData(attr_values,
			diffs,
			get_state->win_info_list[i].data_list,
			get_state->win_info_list[i].num_data_list,
			w_window,
			data_list,
			num_data);
  else
    CreateStartWSMWinData(attr_values,
			diffs,
			NULL,
			0,
			w_window,
			data_list,
			num_data);

  if (attr_values != NULL) XtFree((XtPointer)attr_values);

}


/*------------------------------------------------------------------
                            GetDataList

  ------------------------------------------------------------------*/
static
void
GetDataList(WorkWindow *w_window,
	    XrmQuark room_q,
	    Boolean diffs,
	    WSMWinData **data_list,
	    int *num_data)
{
  XrmValue *attr_values;
  /* get window configuration from database */
  GetWindowConfigurationEntry(w_window->specifier_qlist,
			      w_window->attrib_qlist,
			      room_q,
			      &attr_values);
  /* create set state request */
/*  print_values(attr_values,w_window->attrib_qlist,
	       room_q, w_window->num_attrib_list);*/
  /* fill in data_list and num_data for WSM_SET_STATE request */
  CreateWSMWinData(attr_values,
		   diffs,
		   w_window,
		   data_list,
		   num_data);
  if (attr_values != NULL)
    XtFree((XtPointer)attr_values);
}



/*------------------------------------------------------------------
                            GetUnhideDataList

  ------------------------------------------------------------------*/
static
void
GetUnhideDataList(WorkWindow *w_window,
		 XrmQuark room_q,
		 WSMWinData **data_list,
		 int *num_data)
{
  /* fill in data_list and num_data for WSM_SET_STATE request */
  CreateUnhideWSMWinData(NULL,
			 w_window,
			 data_list,
			 num_data);

}

/*------------------------------------------------------------------
                            CreateStartStateRequest

  ------------------------------------------------------------------*/
void
CreateStartStateRequest(WSMGetStateReply *get_state,
			WSMRequest *request)
{
  Window window;
  int i;
  XrmQuarkList rooms_qlist = NULL;
  WSMWinData *xrm_data_list;
  int num_xrm_data_list;
  WorkWindow *w_window = NULL;
  Boolean linked = False;
  Boolean all_work = False;
  int num_win;
  WSMWinInfo *win_info;
  Boolean in_current_space = False;
  XrmQuark room_name = NULLQUARK;

/*  print_reply_start_state(get_state);*/

  PRINT("\nSTART STATE\n");
  linkedRoomQuark = XrmStringToQuark("linkedRoom");
  request->set_state.num_win_info_list = 0;
  num_win = 0;
  win_info = (WSMWinInfo*)XtMalloc((get_state->num_win_info_list)*sizeof(WSMWinInfo));
  for (i = 0; i < get_state->num_win_info_list; i ++)
    {
      in_current_space = False;
      window = get_state->win_info_list[i].window;
      if (mwm_reconnect && (w_window = GetWorkWindow(window)) != NULL)
      {
	  w_window->used = True;
	  if ( !IsWorkWindowInSpace(w_window,current_space))
	      CreateHideWSMWinData(diffs_allowed,
				   w_window,
				   &xrm_data_list,
				   &num_xrm_data_list);
	  else num_xrm_data_list = 0;
      }
      else
	  {
	      w_window = CreateWorkWindow(window);
	      /* get window's workspace resources from database */
	      if (GetWorkspaceResources(shell,
					w_window->specifier_qlist,
					&rooms_qlist,
					&all_work,
					&linked))
	      {
		  /* set the workspace values received from database */
		  SetWorkWindowValues(w_window,all_work,linked);
		  /* set links in internal structures */
		  in_current_space = CreateInternalStructure(w_window,rooms_qlist);
		  /* if its in current space or no diffs allowed,
		     get config info for WSM_SET_STATE */
		  if (in_current_space || !diffs_allowed)
		  {
		      if (linked) room_name = linkedRoomQuark;
		      else if (!diffs_allowed && !in_current_space)
			  room_name = rooms_qlist[0];
		      else room_name = current_space->nameq;
		      GetDataListStart(w_window,
				       i,
				       room_name,
				       diffs_allowed,
				       get_state,
				       &xrm_data_list,
				       &num_xrm_data_list);
		      /* set last space variable */
		      if (in_current_space) w_window->last_space = current_space;
		      /* if no diffs allowed && not in current space then set hide
			 info to True */
		      else
		      {
			  w_window->win_data[wsm_index.hide].data.value = 1;
			  xrm_data_list[wsm_index.hide].data.value = 1;
			  w_window->last_space = GetSpace(room_name);
		      }
		  }
		  /* else only send hide info in WSM_SET_STATE */
		  else
		  {
		      /* create set state request */
		      CreateStartHideWSMWinData(diffs_allowed,
						get_state->win_info_list[i].data_list,
						get_state->win_info_list[i].num_data_list,
						w_window,
						&xrm_data_list,
						&num_xrm_data_list);
		  }
		  if (rooms_qlist != NULL)
		      XtFree((XtPointer)rooms_qlist);
	      }
	      /* nothing in the database */
	      else
	      {
		  CreateStartWSMWinData(NULL,
					diffs_allowed,
					get_state->win_info_list[i].data_list,
					get_state->win_info_list[i].num_data_list,
					w_window,
					&xrm_data_list,
					&num_xrm_data_list);
		  w_window->last_space = current_space;
		  in_current_space = CreateInternalStructure(w_window,NULL);
		  PRINT("nothing in database\n");
	      }
	  }
      /* create WSM_SET_STATE win_info */
      if (num_xrm_data_list > 0)
      {
	  print_win_data(xrm_data_list,num_xrm_data_list);
	  win_info[num_win].data_list = xrm_data_list;
	  win_info[num_win].num_data_list = num_xrm_data_list;
	  win_info[num_win].window = GetWSMWindow(w_window);
	  num_win++;
      }
    }/* end for loop */

  if (num_win ==0)
    {
      request->set_state.allocated = False;
      request->set_state.num_win_info_list = 0;
      request->any.type = WSM_SET_STATE;
      PRINT("REQUEST: no window data\n");
      XtFree((XtPointer)win_info);
    }
  else
    {
      /* create WSM_SET_STATE */
      if (num_win != get_state->num_win_info_list)
	win_info = (WSMWinInfo*)XtRealloc((char*)win_info,num_win*sizeof(WSMWinInfo));
      request->set_state.allocated = True;
      request->set_state.num_win_info_list = num_win;
      request->set_state.win_info_list = win_info;
      request->any.type = WSM_SET_STATE;
      print_request(request);
    }

  if (mwm_reconnect)
      FinishRestartWSM();
/*  SaveState(get_state);*/

}


/*------------------------------------------------------------------
                           SaveState

  ------------------------------------------------------------------*/
void
SaveState(WSMGetStateReply *get_state, Boolean diffs)
{
  int i;
  XrmValue *value_list;
  WorkWindow *w_window;
  XrmQuark roomq;
  Boolean no_save;
  WSMWinData *hide_data;
  PRINT("\n\nSAVE STATE\n");

  /* If diffs = True then check to see if any of the windows
     were unmapped since the last save state. (This is done
     by setting mapped = False for every window in current space and then
     turning mapped = True for each window sent over from window manager.
     Anything not sent over by window manager must have been unmapped ) */
  if (!diffs) UnmapCurrentSpace();
  /* for each window, save its state in database and in internal
     structures */
  print_reply_start_state(get_state);
  for (i = 0; i < get_state->num_win_info_list; i++)
    {
      w_window = GetWorkWindow(get_state->win_info_list[i].window);
      if (w_window != NULL)
	{
	  no_save = False;
	  print_window(w_window);
	  if (!diffs) MapWorkWindow(w_window);
	  if (_WSMGetConfigFormatType(w_window->window) == WSM_WINDOW_FMT)
	    {
	      hide_data = _WSMGetMatchingWinData(get_state->win_info_list[i].data_list,
						 get_state->win_info_list[i].num_data_list,
						 w_window->attrib_qlist[wsm_index.hide]);
	      if (hide_data)
		{
		  if (hide_data->data.value == 1)
		    {
		      no_save = True;
		    }
		}
	    }
	  if (!no_save)
	    {
	      /* update internal structures */
	      UpdateWinData(get_state->win_info_list[i].data_list,
			    get_state->win_info_list[i].num_data_list,
			    w_window->win_data,
			    w_window->num_attrib_list);

	      /* create XrmValues to store in database */
	      CreateValues(w_window,
			   get_state->win_info_list[i].data_list,
			   get_state->win_info_list[i].num_data_list,
			   &value_list);

	      /* store workspace resources */
	      SaveWorkspaceResources(shell,
				     w_window->specifier_qlist,
				     current_space->nameq,
				     NULLQUARK,
				     w_window->all_workspaces,
				     w_window->linked);
	      if (w_window->linked)
		roomq = linkedRoomQuark;
	      else
		roomq = current_space->nameq;
	      print_values(value_list,w_window->attrib_qlist,roomq, w_window->num_attrib_list);
	      /* store window resources */
	      SaveWindowConfiguration(w_window->specifier_qlist,
				      w_window->attrib_qlist,
				      roomq,
				      value_list);
	      FreeValues(w_window->num_attrib_list,value_list);
	    }
	}
    }
}


/*------------------------------------------------------------------
                           GetChangeSpaceRequest

  ------------------------------------------------------------------*/
void
GetChangeSpaceRequest(Space *to_s, WSMRequest *request)
{
  WorkWindow *w_window;
  WorkWindowList *w_list;
  Boolean in_current_space = False;
  Boolean in_to_space = False;
  Boolean first = True;
  int num_win;
  WSMWinData *xrm_data_list;
  int num_xrm_data_list;
  WSMWinInfo *win_info = NULL;
  XrmQuark room_name;

  PRINT("\n\nCHANGE ROOM from %s to %s\n",
	 XrmQuarkToString(current_space->nameq),
	XrmQuarkToString(to_s->nameq));

  request->set_state.num_win_info_list = 0;
  w_list = current_space->w_list;
  /* loop through each window in current workspace */
  while (w_list != NULL)
    {
      num_xrm_data_list = 0;
      w_window = w_list->work_win;
      /* only send info for mapped windows */
      if (w_window->mapped)
	{
	  print_window(w_window);
	  in_to_space = IsWorkWindowInSpace(w_window,to_s);
	  /* if the window is also in the room being switched to
	     then send new config info if window not linked, or send
	     stacking info if it is linked */
	  if (in_to_space)
	    {
	      if (!(w_window->linked) || !diffs_allowed )
		{
		  GetDataList(w_window,
			      to_s->nameq,
			      diffs_allowed,
			      &xrm_data_list,
			      &num_xrm_data_list);
		  w_window->last_space = to_s;
		}
	      /* if the window is linked, i only need to send stacked info */
	      else
		{
		  GetUnhideDataList(w_window,
				    linkedRoomQuark,
				    &xrm_data_list,
				    &num_xrm_data_list);
		}
	    }
	  /* if its not in the destination workspace, hide it */
	  if (!in_to_space)
	    {
	      CreateHideWSMWinData(diffs_allowed,
				   w_window,
				   &xrm_data_list,
				   &num_xrm_data_list);
	    }
	  /* if there is any info to send the window manager store it*/
	  if (num_xrm_data_list > 0)
	    {
	      print_win_data(xrm_data_list,num_xrm_data_list);
	      num_win = request->set_state.num_win_info_list;
	      request->set_state.num_win_info_list++;
	      if (first)
		{
		  win_info = (WSMWinInfo*)XtMalloc(sizeof(WSMWinInfo));
		  first = False;
		}
	      else win_info = (WSMWinInfo*)XtRealloc((char*)win_info,
						     (num_win+1)* sizeof(WSMWinInfo));
	      win_info[num_win].data_list = xrm_data_list;
	      win_info[num_win].num_data_list = num_xrm_data_list;
	      win_info[num_win].window = GetWSMWindow(w_window);
	    }
	}
      w_list = w_list->next;
    }


  w_list = to_s->w_list;
  while (w_list != NULL)
    {
      num_xrm_data_list = 0;
      w_window = w_list->work_win;
      print_window(w_window);
      if (w_window->mapped)
	{
	  in_current_space = IsWorkWindowInSpace(w_window,current_space);
	  /* if it was in current space, it was taken care of in the above loop */
	  if (!in_current_space)
	    {
	      /* if the last configuration for the window was in the
		 destination workspace, i only need to send hide info */
	      if (diffs_allowed && (w_window->last_space == to_s ||
				    (w_window->linked && w_window->last_space != NULL)))
		{
		  GetUnhideDataList(w_window,
				    to_s->nameq,
				    &xrm_data_list,
				    &num_xrm_data_list);
		}
	      else
		{
		  if (w_window->linked) room_name = linkedRoomQuark;
		  else room_name = to_s->nameq;
		  GetDataList(w_window,
			      room_name,
			      diffs_allowed,
			      &xrm_data_list,
			      &num_xrm_data_list);
		  w_window->last_space = to_s;
		}
	      /* if there is any info to send to window manager */
	      if (num_xrm_data_list >0)
		{
		  print_win_data(xrm_data_list,num_xrm_data_list);
		  num_win = request->set_state.num_win_info_list;
		  request->set_state.num_win_info_list++;
		  if (first)
		    {
		      win_info = (WSMWinInfo*)XtMalloc(sizeof(WSMWinInfo));
		      first = False;
		    }
		  else win_info = (WSMWinInfo*)XtRealloc((char*)win_info,
						     (num_win+1)* sizeof(WSMWinInfo));
		  win_info[num_win].data_list = xrm_data_list;
		  win_info[num_win].num_data_list = num_xrm_data_list;
		  win_info[num_win].window = GetWSMWindow(w_window);
		}
	    }
	}
      w_list = w_list->next;
    }
  if (request->set_state.num_win_info_list > 0)
    {
      request->set_state.allocated = True;
      request->set_state.win_info_list = win_info;
    }
  else
    {
      request->set_state.allocated = False;
      request->set_state.win_info_list = NULL;
    }
  request->any.type = WSM_SET_STATE;
 print_request(request);

  current_space = to_s;
}




/*------------------------------------------------------------------
                         GetRegisterOldWindowReply

  ------------------------------------------------------------------*/
void
GetRegisterOldWindowReply(WorkWindow *w_window,WSMReply *reply)
{
  WSMWinData *xrm_data_list = NULL;
  int num_xrm_data_list = 0;
  Boolean in_current_space = False;

  PRINT("\n\nREGISTER OLD WINDOW .....\n");
  reply->any.type = WSM_REG_WINDOW;
  reply->register_window.num_window_data = 0;
  reply->register_window.window_data = NULL;

  in_current_space = IsWorkWindowInSpace(w_window,current_space);
  /* Display window in current room AND any rooms it was in previously.
     If other behavior wanted, take this if statement out */
  if (!in_current_space)
    {
      AddSpaceToWindow(current_space, w_window);
      AddWindowToSpace(current_space, w_window);
      in_current_space = True;
    }
  if (in_current_space)
    {
      if (!(w_window->linked) || !diffs_allowed)
	{
	  GetDataList(w_window,
		      current_space->nameq,
		      diffs_allowed,
		      &xrm_data_list,
		      &num_xrm_data_list);
	  w_window->last_space = current_space;
	}
      /* if the window is linked (no stacked above info, i could remove this) */
      else if (w_window->linked)
	{
	  GetUnhideDataList(w_window,
			    linkedRoomQuark,
			    &xrm_data_list,
			    &num_xrm_data_list);
	}
    }
  else
    {
      CreateHideWSMWinData(diffs_allowed,
			   w_window,
			   &xrm_data_list,
			   &num_xrm_data_list);
    }
  print_win_data(xrm_data_list,num_xrm_data_list);

  reply->any.type = WSM_REG_WINDOW;
  if (num_xrm_data_list > 0)
    {
      reply->register_window.allocated = True;
      reply->register_window.num_window_data = num_xrm_data_list;
      reply->register_window.window_data = xrm_data_list;
    }
  if (w_window->s_list != NULL)
      if (w_window->s_list->next == NULL)
	  DisableDeleteCommand(w_window->window);
  print_reply(reply);
}



/*------------------------------------------------------------------
                         GetRegisterWindowReply

  ------------------------------------------------------------------*/
void
GetRegisterWindowReply(Window window,WSMReply *reply)
{
  WSMWinData *xrm_data_list;
  int num_xrm_data_list = 0;
  WorkWindow *w_window;
  XrmQuarkList rooms_qlist;
  Boolean all_workspaces, linked;
  Boolean in_current_space = False;
  Boolean new_window = True;
  XrmQuark room_name = NULLQUARK;

  PRINT("\n\nREGISTER WINDOW .....\n");
  reply->any.type = WSM_REG_WINDOW;
  reply->register_window.num_window_data = 0;

  w_window = GetWorkWindow(window);
  if (w_window == NULL)
    w_window = CreateWorkWindow(window);
  else new_window = False;
  w_window->mapped = True;

  print_window(w_window);

  if (_WSMGetConfigFormatType(window) == WSM_WINDOW_FMT)
      AddWindow(window);

  if (!new_window)
    {
      GetRegisterOldWindowReply(w_window,reply);
      return;
    }


  /* get window's workspace resources from database */
  if (GetWorkspaceResources(shell,
			    w_window->specifier_qlist,
			    &rooms_qlist,
			    &all_workspaces,
			    &linked))
    {
      /* set the workspace values received from database */
      SetWorkWindowValues(w_window,all_workspaces,linked);
      /* create internal structures */
      in_current_space = CreateInternalStructure(w_window,rooms_qlist);
      if (!in_current_space)
	{
	  AddSpaceToWindow(current_space, w_window);
	  AddWindowToSpace(current_space, w_window);
	  in_current_space = True;
	}
      /* if its in current space or no diffs allowed,
	 get config info for WSM_SET_STATE */
      if (in_current_space || !diffs_allowed)
	{
	  if (linked) room_name = linkedRoomQuark;
	  else if (!diffs_allowed && !in_current_space)
	    room_name = rooms_qlist[0];
	  else room_name = current_space->nameq;
	  GetDataListStart(w_window,
			   0,
			   room_name,
			   diffs_allowed,
			   NULL,
			   &xrm_data_list,
			   &num_xrm_data_list);
	  if (in_current_space)
	    {
	      w_window->win_data[wsm_index.hide].data.value = 0;
	      w_window->last_space = current_space;
	    }
	  else if (!diffs_allowed)
	    {
	      w_window->win_data[wsm_index.hide].data.value = 1;
	      xrm_data_list[wsm_index.hide].data.value = 1;
	      w_window->last_space = GetSpace(room_name);
	    }
	}
      /* else only send hide info in WSM_SET_STATE */
      else
	{
	  CreateStartHideWSMWinData(True,
				    NULL,
				    0,
				    w_window,
				    &xrm_data_list,
				    &num_xrm_data_list);
	  w_window->win_data[wsm_index.hide].type = WSM_VALUE_DATA;
	  w_window->win_data[wsm_index.hide].data.value = 1;
	}
      if (rooms_qlist != NULL)
	XtFree((XtPointer)rooms_qlist);
      print_win_data(xrm_data_list,num_xrm_data_list);

      reply->any.type = WSM_REG_WINDOW;
      reply->register_window.num_window_data = num_xrm_data_list;
      reply->register_window.window_data = xrm_data_list;
      print_reply(reply);
    }
  /* nothing in the database */
  else
    {
      CreateStartWSMWinData(NULL,
			    True,
			    NULL,
			    0,
			    w_window,
			    &xrm_data_list,
			    &num_xrm_data_list);
      CreateInternalStructure(w_window,NULL);
      reply->any.type = WSM_REG_WINDOW;
      reply->register_window.num_window_data = 0;
      reply->register_window.window_data = NULL;
      print_reply(reply);
      w_window->last_space = current_space;
      PRINT("nothing in database\n");
    }
  if (w_window->s_list != NULL)
      if (w_window->s_list->next == NULL)
	  DisableDeleteCommand(w_window->window);
}


/*------------------------------------------------------------------
                          DealWithClientMessage

  ------------------------------------------------------------------*/
void
DealWithClientMessage(Widget widget,XClientMessageEvent *ev)
{
  char **argv;
  int argc = 3;

  if (ev->type != ClientMessage)
    return;
  if (ev->message_type != atoms.protocols_property)
    return;
  if (ev->data.l[0] != atoms.save_property)
    return;

  SaveWsmToFile(file_name);
  argv = (char**)XtMalloc(3*sizeof(char*));
  argv[0] = "wsm";
  argv[1] = "-f";
  argv[2] = file_name;
  XSetCommand(dsp,XtWindow(widget),argv,argc);
  XtFree((char*)argv);
}


/*------------------------------------------------------------------
                          DealWithDestroyNotify

  ------------------------------------------------------------------*/
void
DealWithDestroyNotify(Widget widget,XDestroyWindowEvent *ev)
{
  WorkWindow *w_window = GetWorkWindow(ev->window);
  WorkWindow *i_window;

  if (ev->window == mwm_window)
    DealWithDestroyMwmNotify();
  if (connected == False) return;
#ifdef DEBUG
  PRINT("destroy 0x%x\n",ev->window);
#endif
  if (w_window == NULL)
    {
      w_window = GetWorkWindow(ev->event);
      if (w_window != NULL)
	{
	  RemoveWorkWindow(w_window, False);
	  i_window = GetIconWorkWindow(w_window->window);
	  if (i_window != NULL)
	    RemoveWorkWindow(i_window,False);
	}
    }

  else
    {
      RemoveWorkWindow(w_window,False);
      i_window = GetIconWorkWindow(w_window->window);
      if (i_window != NULL)
	RemoveWorkWindow(i_window,False);
    }
}




/*------------------------------------------------------------------
                          DealWithDestroyMwmNotify

  ------------------------------------------------------------------*/
void
DealWithDestroyMwmNotify()
{

  if (connected == False) return;
  RestartWSM();
  mwm_gone = True;
  PRINT("Mwm went away\n");
}




/*------------------------------------------------------------------
                          DealMapNotify

  ------------------------------------------------------------------*/
void
DealWithMapNotify(Widget widget,XMapEvent *ev)
{
  WorkWindow *w_window = GetWorkWindow(ev->window);

  if (connected == False) return;

#ifdef DEBUG
  PRINT("MapNotify\n");
#endif
  if (w_window == NULL)
    {
      w_window = GetWorkWindow(ev->event);
      if (w_window != NULL)
	w_window->mapped = True;
    }
  else w_window->mapped = True;
}






/*------------------------------------------------------------------
                          DealUnmapNotify

  ------------------------------------------------------------------*/
void
DealWithUnmapNotify(Widget widget,XUnmapEvent *ev)
{
  WorkWindow *i_window;
  WorkWindow *w_window = GetWorkWindow(ev->window);
#ifdef DEBUG
  PRINT("UnapNotify\n");
#endif
  if (connected == False) return;

  if (w_window == NULL)
    {
      w_window = GetWorkWindow(ev->event);
      if (w_window != NULL)
      {
	  w_window->mapped = False;
/*	  RemoveWorkWindow(w_window, False);
	  i_window = GetIconWorkWindow(w_window->window);
	  if (i_window != NULL)
	      RemoveWorkWindow(i_window,False);
*/
      }
    }
  else
  {
      w_window->mapped = False;
/*    RemoveWorkWindow(w_window, False);
      i_window = GetIconWorkWindow(w_window->window);
      if (i_window != NULL)
	  RemoveWorkWindow(i_window,False);
*/
  }
}



/*------------------------------------------------------------------
                          CreateSetState

  ------------------------------------------------------------------*/
Boolean
CreateSetState(WSMWinData *data_list, int num_data_list,
	       WSMWinData *data_list_i, int num_data_list_i,
	       WorkWindow *w_window,
	       WorkWindow *i_window,
	       WSMRequest *request)
{
  Boolean retval;
  WSMWinInfo *win_info = NULL;
  int count;
  print_window(w_window);
  print_win_data(data_list,num_data_list);
  if (i_window != NULL)
    {
      print_window(i_window);
      print_win_data(data_list_i,num_data_list_i);
    }
  count = 0;
  retval = False;
  if (num_data_list > 0 && num_data_list_i > 0)
    win_info = (WSMWinInfo*)XtMalloc(2*sizeof(WSMWinInfo));
  else if (num_data_list > 0 || num_data_list_i > 0)
    win_info = (WSMWinInfo*)XtMalloc(sizeof(WSMWinInfo));
  if (num_data_list > 0)
    {
      win_info[count].data_list = data_list;
      win_info[count].num_data_list = num_data_list;
      win_info[count].window = GetWSMWindow(w_window);
      count++;
    }
  if (num_data_list_i > 0)
    {
      win_info[count].data_list = data_list_i;
      win_info[count].num_data_list = num_data_list_i;
      win_info[count].window = GetWSMWindow(i_window);
      count++;
    }
  if (num_data_list > 0 || num_data_list_i > 0)
    {
      request->set_state.allocated = True;
      request->any.type = WSM_SET_STATE;
      request->set_state.win_info_list = win_info;
      request->set_state.num_win_info_list = count;
      retval = True;
    }
  else
    {
      request->set_state.allocated = True;
      request->any.type = WSM_SET_STATE;
      request->set_state.win_info_list = NULL;
      request->set_state.num_win_info_list = 0;
    }
  return retval;
}



/*------------------------------------------------------------------
                          GetCopyWindowRequest

  ------------------------------------------------------------------*/
Boolean
GetCopyWindowRequest(Window win, Space *from_s, Space *to_s, WSMRequest *request)
{
  WSMWinData *xrm_data_list = NULL;
  WSMWinData *xrm_data_list_i = NULL;
  int num_xrm_data_list = 0;
  int num_xrm_data_list_i = 0;
  WorkWindow *w_window;
  WorkWindow *i_window;
  XrmValue *attr_values = NULL;
  XrmValue *attr_values_i = NULL;
  Boolean retval = False;
  XrmQuark room_nameq;

  PRINT("COPY\n");
  w_window = GetWorkWindow(win);
  i_window = GetIconWorkWindow(win);

  if (w_window != NULL && from_s != NULL && to_s != NULL
      && IsWorkWindowInSpace(w_window,from_s))
    {
      if (w_window->linked) room_nameq = linkedRoomQuark;
      else room_nameq = from_s->nameq;
      GetWindowConfigurationEntry(w_window->specifier_qlist,
				  w_window->attrib_qlist,
				  room_nameq,
				  &attr_values);
      if (i_window != NULL)
	GetWindowConfigurationEntry(i_window->specifier_qlist,
				    i_window->attrib_qlist,
				    room_nameq,
				    &attr_values_i);
      if (to_s == all_space)
	{
	  CopyWindowToAllSpaces(w_window,i_window,attr_values, attr_values_i, from_s);
	}
      else
	{
	  CopyWindowToSpace(w_window,i_window,attr_values,attr_values_i,to_s);
	}
      w_window->linked = False;
      SaveWorkspaceResources(shell,
			     w_window->specifier_qlist,
			     to_s->nameq,
			     NULLQUARK,
			     w_window->all_workspaces,
			     w_window->linked);
      if (i_window != NULL)
	{
	  i_window->linked = False;
	  SaveWorkspaceResources(shell,
				 i_window->specifier_qlist,
				 to_s->nameq,
				 NULLQUARK,
				 i_window->all_workspaces,
				 i_window->linked);
	}
      if ((to_s == current_space || to_s == all_space) && from_s != current_space && w_window->mapped)
	{
	  CreateWSMWinData(attr_values,
			   diffs_allowed,
			   w_window,
			   &xrm_data_list,
			   &num_xrm_data_list);
	  w_window->last_space = current_space;
	  if (i_window != NULL)
	    {
	      CreateWSMWinData(attr_values_i,
			       diffs_allowed,
			       i_window,
			       &xrm_data_list_i,
			       &num_xrm_data_list_i);
	      i_window->last_space = current_space;
	    }
	  retval = CreateSetState(xrm_data_list,num_xrm_data_list,
				  xrm_data_list_i,num_xrm_data_list_i,
				  w_window,
				  i_window,
				  request);
	}
      if (attr_values != NULL) XtFree((char*)attr_values);
      if (attr_values_i != NULL) XtFree((char*)attr_values_i);
    }
  if (retval) print_request(request);
  return retval;
}



/*------------------------------------------------------------------
                          CopyWindowToAllSpaces

  ------------------------------------------------------------------*/
void
CopyWindowToAllSpaces(WorkWindow *w_window, WorkWindow *i_window,
		      XrmValue *attr_values, XrmValue *attr_values_i,Space* skip_space)
{
  Space *space;
  space = space_list;
  w_window->linked = False;
  while (space!= NULL)
    {
      if (space != skip_space)
	{
	  SaveWindowConfiguration(w_window->specifier_qlist,
				  w_window->attrib_qlist,
				  space->nameq,
				  attr_values);
	  if (i_window != NULL)
	    SaveWindowConfiguration(i_window->specifier_qlist,
				    i_window->attrib_qlist,
				    space->nameq,
				    attr_values_i);
	  SaveWorkspaceResources(shell,
				 w_window->specifier_qlist,
				 space->nameq,
				 NULLQUARK,
				 w_window->all_workspaces,
				 w_window->linked);
	}
      if (!(IsWorkWindowInSpace(w_window,space)))
	{
	  AddWindowToSpace(space,w_window);
	  AddSpaceToWindow(space,w_window);
	}
      space = space->next;
    }
  if (!(IsWorkWindowInSpace(w_window,all_space)))
    AddWindowToSpace(all_space,w_window);
  w_window->all_workspaces = True;
}


/*------------------------------------------------------------------
                          CopyWindowToSpace

  ------------------------------------------------------------------*/
void
CopyWindowToSpace(WorkWindow *w_window, WorkWindow *i_window,
		  XrmValue *attr_values, XrmValue *attr_values_i,
		  Space *space)
{
  SpaceList *s_list;
  if (w_window->linked)
    {
      /* break links and replace with copies */
      s_list = w_window->s_list;
      while (s_list != NULL)
	{
	  SaveWindowConfiguration(w_window->specifier_qlist,
				  w_window->attrib_qlist,
				  s_list->space->nameq,
				  attr_values);
	  if (i_window != NULL)
	    SaveWindowConfiguration(i_window->specifier_qlist,
				    i_window->attrib_qlist,
				    s_list->space->nameq,
				    attr_values_i);
	  s_list = s_list->next;
	}
    }
  SaveWindowConfiguration(w_window->specifier_qlist,
			  w_window->attrib_qlist,
			  space->nameq,
			  attr_values);
  if (i_window != NULL)
    SaveWindowConfiguration(i_window->specifier_qlist,
			    i_window->attrib_qlist,
			    space->nameq,
			    attr_values_i);
  if (!(IsWorkWindowInSpace(w_window,space)))
    {
      AddWindowToSpace(space,w_window);
      AddSpaceToWindow(space,w_window);
    }
}


/*------------------------------------------------------------------
                           GetMoveWindowRequest

  ------------------------------------------------------------------*/
Boolean
GetMoveWindowRequest(Window win, Space *from_s, Space *to_s, WSMRequest *request)
{
  WSMWinData *xrm_data_list, *xrm_data_list_i = NULL;
  int num_xrm_data_list = 0;
  int num_xrm_data_list_i = 0;
  WorkWindow *w_window;
  WorkWindow *i_window;
  XrmValue *attr_values = NULL;
  XrmValue *attr_values_i = NULL;
  Boolean retval = False;

  PRINT("MOVE  0x%X \n", win);
  w_window = GetWorkWindow(win);
  i_window = GetIconWorkWindow(win);

  if (w_window != NULL && from_s != NULL && to_s != NULL
      && IsWorkWindowInSpace(w_window,from_s))
    {
      /* if the window isn't linked, then save the "from" workspace
	 configuration into t	he "to" workspace */
      if (!(w_window->linked))
	{
	  GetWindowConfigurationEntry(w_window->specifier_qlist,
				      w_window->attrib_qlist,
				      from_s->nameq,
				      &attr_values);
	  SaveWindowConfiguration(w_window->specifier_qlist,
				  w_window->attrib_qlist,
				  to_s->nameq,
				      attr_values);
	  if (i_window != NULL)
	    {
	      GetWindowConfigurationEntry(i_window->specifier_qlist,
					  i_window->attrib_qlist,
					  from_s->nameq,
					  &attr_values_i);
	      SaveWindowConfiguration(i_window->specifier_qlist,
				      i_window->attrib_qlist,
				      to_s->nameq,
				      attr_values_i);
	    }
	}
      /* if the window was in all workspaces, remove it from all_space */
      if (w_window->all_workspaces)
	{
	  RemoveWorkWindowFromSpace(all_space,w_window);
	  w_window->all_workspaces = False;
	}
      /* if the window wasn't in the "to" workspace,  add it */
      if (!IsWorkWindowInSpace(w_window,to_s))
	{
	  SaveWorkspaceResources(shell,
				 w_window->specifier_qlist,
				 to_s->nameq,
				 from_s->nameq,
				 w_window->all_workspaces,
				 w_window->linked);
	  AddWindowToSpace(to_s,w_window);
	  AddSpaceToWindow(to_s,w_window);
	}
      /* if its necessary to send info to the window manager */
      if ((to_s == current_space || from_s == current_space) && w_window->mapped)
	{
	  if (to_s == current_space)
	    {
	      /* if its linked then i need to get its configuration, otherwise i already
		 got it above */
	      if (w_window->linked)
		{
		  GetWindowConfigurationEntry(w_window->specifier_qlist,
					      w_window->attrib_qlist,
					      linkedRoomQuark,
					      &attr_values);
		  if (i_window != NULL)
		  GetWindowConfigurationEntry(i_window->specifier_qlist,
					      i_window->attrib_qlist,
					      linkedRoomQuark,
					      &attr_values_i);
		}
	      CreateWSMWinData(attr_values,
			       True,
			       w_window,
			       &xrm_data_list,
			       &num_xrm_data_list);
	      w_window->last_space = current_space;
	      if (i_window != NULL)
		{
		  CreateWSMWinData(attr_values_i,
				   True,
				   i_window,
				   &xrm_data_list_i,
				   &num_xrm_data_list_i);
		  i_window->last_space = current_space;
		}
	    }
	  else
	    {
	      CreateHideWSMWinData(diffs_allowed,
				   w_window,
				   &xrm_data_list,
				   &num_xrm_data_list);
	    }
	  retval = CreateSetState(xrm_data_list,num_xrm_data_list,
				  xrm_data_list_i,num_xrm_data_list_i,
				  w_window,
				  i_window,
				  request);
	}
      /* get rid of the "from" workspace configuration */
      if (!w_window->linked)
	PurgeWindowConfiguration(shell,
				 w_window->specifier_qlist,
				 w_window->attrib_qlist,
				 from_s->nameq);
      RemoveWorkWindowFromSpace(from_s, w_window);
      RemoveSpaceFromWindow(from_s, w_window);
      if (attr_values != NULL) XtFree((char*)attr_values);
      if (attr_values_i != NULL) XtFree((char*)attr_values_i);
    }

  return retval;
}





/*------------------------------------------------------------------
                           GetDeleteWindowRequest

  ------------------------------------------------------------------*/
Boolean
GetDeleteWindowRequest(Window win, Space *from_s,WSMRequest *request)
{

  WSMWinInfo *win_info;
  WSMWinData *xrm_data_list;
  int num_xrm_data_list = 0;
  WorkWindow *w_window;
  Boolean retval = False;


  PRINT("DELETE  0x%X \n", win);
  w_window = GetWorkWindow(win);

  if (w_window != NULL && from_s != NULL )
    {
      if (IsWorkWindowInSpace(w_window,from_s))
	{
	  PurgeWindowConfiguration(shell,
				   w_window->specifier_qlist,
				   w_window->attrib_qlist,
				   from_s->nameq);
	  RemoveWorkWindowFromSpace(from_s, w_window);
	  RemoveSpaceFromWindow(from_s, w_window);
	  if (w_window->all_workspaces)
	    {
	      RemoveWorkWindowFromSpace(all_space,w_window);
	      w_window->all_workspaces = False;
	    }
	  if (from_s == current_space && w_window->mapped)
	    {
	      CreateHideWSMWinData(diffs_allowed,
				   w_window,
				   &xrm_data_list,
				   &num_xrm_data_list);
	      print_win_data(xrm_data_list,num_xrm_data_list);
	      if (num_xrm_data_list > 0)
		{
		  win_info = (WSMWinInfo*)XtMalloc(sizeof(WSMWinInfo));
		  win_info[0].data_list = xrm_data_list;
		  win_info[0].num_data_list = num_xrm_data_list;
		  win_info[0].window = GetWSMWindow(w_window);
		  request->set_state.allocated = True;
		  request->any.type = WSM_SET_STATE;
		  request->set_state.win_info_list = win_info;
		  request->set_state.num_win_info_list = 1;
		  retval = True;
		}
	    }
	}
    }

  return retval;
}



/*------------------------------------------------------------------
                            GetLinkWindowRequest

  ------------------------------------------------------------------*/
Boolean
GetLinkWindowRequest(Window win, Space *from_s, Space *to_s, WSMRequest *request)
{
  WSMWinData *xrm_data_list;
  int num_xrm_data_list = 0;
  WSMWinData *xrm_data_list_i = NULL;
  int num_xrm_data_list_i = 0;
  WorkWindow *w_window;
  WorkWindow *i_window;
  XrmValue *attr_values = NULL;
  XrmValue *attr_values_i = NULL;
  Boolean retval = False;
  Boolean in_space = False;
  Space *space;

  w_window = GetWorkWindow(win);
  i_window = GetIconWorkWindow(win);

  PRINT("LINK WINDOW ");

  if (w_window != NULL && from_s != NULL && to_s != NULL)
    {
#ifdef DEBUG
      print_window(w_window);
#endif
      if (IsWorkWindowInSpace(w_window,from_s))
	{
	  in_space = IsWorkWindowInSpace(w_window,to_s);
	  if (in_space && w_window->linked) return False;
	  if (!in_space)
	    {
	      /* if the window is being linked into all workspaces,
	       look through all workspaces and also add it to all_space*/
	      if (to_s == all_space)
		{
		  space = space_list;
		  while (space != NULL)
		    {
		      if (!IsWorkWindowInSpace(w_window,space))
			{
			  AddWindowToSpace(space,w_window);
			  AddSpaceToWindow(space,w_window);
			}
		      space = space->next;
		    }
		  AddWindowToSpace(all_space,w_window);
		  w_window->all_workspaces = True;
		}
	      /* else just add it to a single workspace */
	      else
		{
		  AddWindowToSpace(to_s,w_window);
		  AddSpaceToWindow(to_s,w_window);
		}
	    }
	  /* if the window wasn't linked then save the "from" configuration
	     into the database as the linked configuration */
	  if (!(w_window->linked))
	    {
	      GetWindowConfigurationEntry(w_window->specifier_qlist,
					  w_window->attrib_qlist,
					  from_s->nameq,
					  &attr_values);
	      SaveWindowConfiguration(w_window->specifier_qlist,
				      w_window->attrib_qlist,
				      linkedRoomQuark,
				      attr_values);
	      if (i_window != NULL)
		{
		  GetWindowConfigurationEntry(i_window->specifier_qlist,
					      i_window->attrib_qlist,
					      from_s->nameq,
					      &attr_values_i);
		  SaveWindowConfiguration(i_window->specifier_qlist,
					  i_window->attrib_qlist,
					  linkedRoomQuark,
					  attr_values_i);
		}
	      if (attr_values != NULL) XtFree((char*)attr_values);
	      if (attr_values_i != NULL) XtFree((char*)attr_values_i);
	      attr_values = NULL;
	      attr_values_i = NULL;
	    }
	  /* save workspace resources if they have changed */
	  if (!in_space || !(w_window->linked) || to_s == all_space)
	    {
	      SaveWorkspaceResources(shell,
				     w_window->specifier_qlist,
				     to_s->nameq,
				     NULLQUARK,
				     w_window->all_workspaces,
				     True);
	      if (i_window)
		SaveWorkspaceResources(shell,
				       i_window->specifier_qlist,
				       to_s->nameq,
				       NULLQUARK,
				       i_window->all_workspaces,
				       True);
	    }
	  /* send info to window manager if necessary */
	  if ((to_s == current_space || to_s == all_space) && from_s != current_space && w_window->mapped)
	    {
	      if (w_window->linked)
		{
		  GetWindowConfigurationEntry(w_window->specifier_qlist,
					      w_window->attrib_qlist,
					      linkedRoomQuark,
					      &attr_values);
		  if (i_window != NULL)
		    GetWindowConfigurationEntry(i_window->specifier_qlist,
					      i_window->attrib_qlist,
					      linkedRoomQuark,
					      &attr_values_i);
		}
	      CreateWSMWinData(attr_values,
			       diffs_allowed,
			       w_window,
			       &xrm_data_list,
			       &num_xrm_data_list);
	      w_window->last_space = current_space;
	      if (i_window != NULL)
		{
		  CreateWSMWinData(attr_values_i,
				   diffs_allowed,
				   i_window,
				   &xrm_data_list_i,
				   &num_xrm_data_list_i);
		  i_window->last_space = current_space;
		}
	      retval = CreateSetState(xrm_data_list,num_xrm_data_list,
				      xrm_data_list_i,num_xrm_data_list_i,
				      w_window,
				      i_window,
				      request);
	      if (attr_values != NULL) XtFree((char*)attr_values);
	      if (attr_values_i != NULL) XtFree((char*)attr_values_i);
	      attr_values = NULL;
	      attr_values_i = NULL;
	    }
	  w_window->linked = True;
	  if (i_window != NULL) i_window->linked = True;
	}
    }
  PRINT("return\n");
  return retval;
}





/*------------------------------------------------------------------
                            GetManageAllWindowsRequest

  ------------------------------------------------------------------*/
Boolean
GetManageAllWindowsRequest(WSMRequest *request)
{
  WSMWinInfo *win_info;
  WSMWinData *xrm_data_list;
  int num_xrm_data_list = 0;
  WorkWindow *w_window;
  int num_win;

  PRINT("MANAGE WINDOWS ");

  w_window = work_windows;

  win_info = NULL;
  num_win = 0;
  for (w_window = work_windows; w_window!= NULL; w_window = w_window->next)
    {
#ifdef DEBUG
      print_window(w_window);
#endif
      if (_WSMGetConfigFormatType(w_window->window) == WSM_WINDOW_FMT)
	{
	  if (!(IsWorkWindowInSpace(w_window,current_space)))
	    {
		if (w_window->last_space != NULL && w_window->mapped)
		  {
		    GetUnhideDataList(w_window,
				      w_window->last_space->nameq,
				      &xrm_data_list,
				      &num_xrm_data_list);
		    if (num_xrm_data_list > 0)
		      {
			print_win_data(xrm_data_list,num_xrm_data_list);
			if (win_info == NULL)
			    win_info = (WSMWinInfo*)XtMalloc(sizeof(WSMWinInfo));
			else win_info = (WSMWinInfo*)XtRealloc((char*)win_info,
						     (num_win+1)* sizeof(WSMWinInfo));
			win_info[num_win].data_list = xrm_data_list;
			win_info[num_win].num_data_list = num_xrm_data_list;
			win_info[num_win].window = GetWSMWindow(w_window);
			num_win++;
		      }
		  }
	      }
	}
    }
  if (num_win ==0)
    {
      PRINT("MANAGE WINDOWS: no window data\n");
      return False;
    }
  else
    {
      request->set_state.allocated = True;
      request->set_state.num_win_info_list = num_win;
      request->set_state.win_info_list = win_info;
      request->any.type = WSM_SET_STATE;
      print_request(request);
      return True;
    }
}
