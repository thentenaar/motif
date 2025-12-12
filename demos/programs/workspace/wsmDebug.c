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
static char rcsid[] = "$XConsortium: wsmDebug.c /main/6 1995/07/14 09:48:58 drk $"
#endif
#endif
#include <stdio.h>
#include <Xm/Xm.h>

#include "wsm.h"
#include "wsmStruct.h"
#include "wsmDebug.h"

extern Space *space_list;
extern Space *current_space;
extern WorkWindow *work_windows;

void print_space_windows(
Space *s
);

/**********************************************************************/
/*                        DEBUG CODE                                  */
/**********************************************************************/


void 
print_window(WorkWindow *w_window)
{
#ifdef DEBUG2
  int p;
  PRINT("\nWINDOW %d = ",w_window->window);
  for (p = 0; w_window->specifier_qlist[p] != NULLQUARK; p++)
    PRINT("%s.", XrmQuarkToString(w_window->specifier_qlist[p]));
  PRINT("\n");
  if (w_window->mapped) PRINT("mapped\n");	
  else PRINT("unmapped\n");

#endif
}

void print_all_windows()
{
#ifdef DEBUG
  WorkWindow *w_window = work_windows;
  while (w_window != NULL)
    {
      PRINT("WINDOW   0x%X %s\n", w_window->window, 
	     XrmQuarkToString(w_window->specifier_qlist[0]));
      w_window = w_window->next;
    }
#endif
}

void 
print_space_windows(Space *s)
{
#ifdef DEBUG
  WorkWindowList *w_list;
  if (s!= NULL)
    {
      w_list = s->w_list;
      while (w_list != NULL)
	{
	  PRINT("SPACE %s WINDOW   0x%X %s\n", XrmQuarkToString(s->nameq),
		 w_list->work_win->window, 
		 XrmQuarkToString(w_list->work_win->specifier_qlist[0]));
	  w_list = w_list->next;
	}
    }
  else PRINT("NULL space\n");
#endif
}


void 
print_all_values(XrmValue **attr_values,XrmQuarkList attrib_qlist,XrmQuarkList rooms_qlist, int num_attrib)
{
#ifdef DEBUG
  int p,q;
 for (p = 0; rooms_qlist[p]  != NULLQUARK; p++)
    {
      for (q = 0; q < num_attrib; q++)		{
	if (attr_values[p][q].size != 0)
	  PRINT("%s %s : %s \n", 
		 XrmQuarkToString(rooms_qlist[p]),
		 XrmQuarkToString(attrib_qlist[q]),
		 (char*)attr_values[p][q].addr);
	else
	  {
	    PRINT("%s %s : NOT FOUND \n", 
		   XrmQuarkToString(rooms_qlist[p]),
		   XrmQuarkToString(attrib_qlist[q]));      
	  }
			
      }
    }
#endif	
}


void 
print_values(XrmValue *attr_values,XrmQuarkList attrib_qlist,XrmQuark room, int num_attrib)
{
#ifdef DEBUG2
  int q;

  for (q = 0; q < num_attrib; q++)		
    {
      if (attr_values[q].size != 0)
	PRINT(" v: %s %s : %s \n", 
	       XrmQuarkToString(room),
	       XrmQuarkToString(attrib_qlist[q]),
	       (char*)attr_values[q].addr);
      else
	    PRINT(" v: %s %s : NOT FOUND \n", 
		   XrmQuarkToString(room),
		   XrmQuarkToString(attrib_qlist[q]));      
    }
#endif	
}


void 
print_win_data(  WSMWinData *xrm_data_list, int num_xrm_data_list)
{
  int p;
#ifdef DEBUG2
  if (num_xrm_data_list == 0)
    PRINT("D:  no data \n");
  else	
    {
      for (p = 0; p < num_xrm_data_list; p++)
	{	
	  PRINT("D:  %s %ld\n",
		 XrmQuarkToString(xrm_data_list[p].nameq),
		 xrm_data_list[p].data.value);
	}
    }
#endif
}






void 
print_internal_win_data(  WSMWinData *xrm_data_list, int num_xrm_data_list)
{
#ifdef DEBUG
  int p;

      for (p = 0; p < num_xrm_data_list; p++)
	{	
	  PRINT("I:  %s %ld\n",
		 XrmQuarkToString(xrm_data_list[p].nameq),
		 xrm_data_list[p].data.value);
	}
#endif
}



void 
print_request(WSMRequest *request)
{
#ifdef DEBUG2   
  int p,q,i;
  Window window;
  WorkWindow *w_window;
  PRINT("\nREQUEST:\n");
  if (request->any.type == WSM_SET_STATE)
    {
      PRINT("num_win_info_list = %d\n",request->set_state.num_win_info_list);
      for (p = 0; p < request->set_state.num_win_info_list; p++)
	{
	    window = request->set_state.win_info_list[p].window;
	    w_window = GetWorkWindow(window);
	    PRINT(" %d ",window);
	    if ( _WSMGetConfigFormatType(window) == WSM_ICON_FMT)
		PRINT(" ICON ");
	    else if ( _WSMGetConfigFormatType(window) == WSM_WINDOW_FMT)
		PRINT(" WINDOW ");
	    else if ( _WSMGetConfigFormatType(window) == WSM_GLOBAL_FMT)
		PRINT(" GLOBAL ");
	    if (w_window != NULL)
	    {
		for (i = 0; w_window->specifier_qlist[i] != NULLQUARK; i++)
		    PRINT("%s.", XrmQuarkToString(w_window->specifier_qlist[i]));
	    }
	    PRINT("\n");
	    for (q = 0; q < request->set_state.win_info_list[p].num_data_list; q++)
	    {
		PRINT("     %s %d\n",
		      XrmQuarkToString(request->set_state.win_info_list[p].data_list[q].nameq),	
		      request->set_state.win_info_list[p].data_list[q].data.value);
	    }
	}	
  }
  PRINT("\n");
#endif
}	


void 
print_reply(WSMReply *reply)
{
#ifdef DEBUG2
  int q;
  PRINT("\n");
 if (reply->any.type == WSM_REG_WINDOW)
   {
     PRINT("REPLY    WSM_REG_WINDOW\n");
     for (q = 0; q < reply->register_window.num_window_data; q++)
       {
	 PRINT("   %s %d\n",
		XrmQuarkToString(reply->register_window.window_data[q].nameq),		
		reply->register_window.window_data[q].data.value);
       }
   }
#endif
}

void 
print_reply_start_state(WSMGetStateReply *get_state)
{
#ifdef DEBUG2
  int i,j,p;
  Window window;
  WorkWindow *w_window;
  PRINT("REPLY   WSM_GET_STATE\n");
  for (i = 0; i < get_state->num_win_info_list; i ++)
    {
	window = get_state->win_info_list[i].window;
	w_window = GetWorkWindow(window);
/*      PRINT(" 0x%X \n",window);*/
	PRINT(" %d ",window);
	if ( _WSMGetConfigFormatType(window) == WSM_ICON_FMT)
	    PRINT(" ICON ");
	else if ( _WSMGetConfigFormatType(window) == WSM_WINDOW_FMT)
	    PRINT(" WINDOW ");
	else if ( _WSMGetConfigFormatType(window) == WSM_GLOBAL_FMT)
	    PRINT(" GLOBAL ");
	if (w_window != NULL)
	    {
		for (p = 0; w_window->specifier_qlist[p] != NULLQUARK; p++)
		    PRINT("%s.", XrmQuarkToString(w_window->specifier_qlist[p]));
	    }
	PRINT("\n");
	for (j = 0; j < get_state->win_info_list[i].num_data_list; j++)
	  PRINT("     %s %ld\n",
		 XrmQuarkToString(get_state->win_info_list[i].data_list[j].nameq),	
		 get_state->win_info_list[i].data_list[j].data.value);
    }
#endif
}


void 
PrintSpace(char *name)
{
#ifdef DEBUG
  Space *s;
  s = GetSpace(XrmStringToQuark(name));
  if (s != NULL)
    print_space_windows(s);
#endif
}


void PrintAllSpaces()
{
#ifdef DEBUG
  Space *s = space_list;
  
  while (s != NULL)
    {
      PRINT("SPACE: %s\n", s->name);
      PRINT("        pixmap_name: %s\n", s->pixmap_name);
      PRINT("        background:  %s\n", s->background);
      print_space_windows(s);
      s = s->next;
    }
#endif
}
