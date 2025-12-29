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
static char rcsid[] = "$TOG: wsmStruct.c /main/7 1997/05/02 10:04:46 dbl $"
#endif
#endif
#include <stdio.h>
#include <Xm/Xm.h>

#include "wsm.h"
#include "wsmData.h"
#include "wsmStruct.h"
#include "wsm_cb.h"
#include "wsmDebug.h"
#include "xrmLib.h"
#include "command_ui.h"


extern AtomStruct atoms;
extern Space *space_list;
extern Space *all_space;
extern Space *current_space;
extern WorkWindow *work_windows;
extern XrmQuark *space_qlist;
extern XrmQuark *window_attribq;
extern XrmQuark *icon_attribq;
extern XrmQuark *global_attribq;
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

static char* GetWMName(
Window, Boolean
);
static char* GetWMClient(
Window, Boolean
);
static char* GetWMRole(
Window, Boolean
);
static char* GetWMCount(
XrmQuark,XrmQuark
);
XrmQuarkList GetSpecifierQList(
Window, Boolean
);

void AddWindowToSpace(
Space*,WorkWindow*
);

char *Get_SM_CLIENT_ID(
Display*, Window
);

char *Get_WM_WINDOW_ROLE(
Display*, Window
);

Colormap default_cmap = 0;


/**********************************************************************/
/*                        INTERNAL STRUCTURE CODE                     */
/**********************************************************************/



/*------------------------------------------------------------------
                            CreateSpace

  ------------------------------------------------------------------*/
Space*
CreateSpace(XrmQuark name, char *str)
{
  WorkWindowList *w_list;
  Space *s, *s2;
  s = (Space*)XtMalloc(sizeof(Space));
  s->nameq = name;
  s->w_list = NULL;
  if (all_space->w_list != NULL)
    {
      w_list = all_space->w_list;
      while (w_list != NULL)
	{
	  AddWindowToSpace(s,w_list->work_win);
	  w_list = w_list->next;
	}
      }
  s->next = NULL;
  strcpy(s->name, str);
  strcpy(s->background,"None");
  strcpy(s->pixmap_name,"None");
  s->pixel = 0;
  s->pixmap = None;
  s->pixmap_label = None;
  if (space_list == NULL)
    {
      current_space = s;
      space_list = s;
    }
  else
    {
      s2 = space_list;
      while (s2->next != NULL)
	s2 = s2->next;
      s2->next = s;
    }
  return s;
}

/*------------------------------------------------------------------
                            SetSpacePixel

  ------------------------------------------------------------------*/
Boolean
SetSpacePixel(Space *s, char *name)
{
  Boolean retval = True;
  XColor a_color;
  XColor def_color;

  if (default_cmap == 0)
    default_cmap = DefaultColormap(dsp,0);
  if (XAllocNamedColor(dsp,default_cmap,name,&a_color, &def_color)==0)
    return False;
  s->pixel = a_color.pixel;
  strcpy(s->background,name);
  s->pixmap = None;
  return retval;
}





/*------------------------------------------------------------------
                            SpacePixelSet

  ------------------------------------------------------------------*/
Boolean
SpacePixelSet(Space *s)
{
  if (strcmp("None",s->background) == 0)
    return False;
  if (s->pixmap != None)
    return False;
  else return True;
}



/*------------------------------------------------------------------
                            SetSpacePixmap

  ------------------------------------------------------------------*/
Boolean
SetSpacePixmap(Space *s, char *name)
{
  unsigned int width, height;
  Window root = XRootWindowOfScreen(XtScreen(shell));

  s -> pixmap = XmGetPixmapByDepth(XtScreen(shell), name,
			      BlackPixel(XtDisplay(shell), 0),
			      WhitePixel(XtDisplay(shell), 0),
			      DefaultDepth(dsp, 0));
  if (s -> pixmap == XmUNSPECIFIED_PIXMAP)
    {
      fprintf(stderr, "can't get pixmap for %s\n", name);
      return False;
    }

  s->background[0] = '"';
  s->background[1] = '\0';
  strcat(s->background,name);
  PRINT("s->background is %s\n",name);
  return True;
}



/*------------------------------------------------------------------
                            SpacePixmapSet

  ------------------------------------------------------------------*/
Boolean
SpacePixmapSet(Space *s)
{
  if (strcmp("None",s->background) == 0)
    return False;
  if (s->pixmap == None)
    return False;
  else return True;
}




/*------------------------------------------------------------------
                            SetSpaceLabelPixmap

  ------------------------------------------------------------------*/
Boolean
SetSpaceLabelPixmap(Space *s, char *name)
{
  unsigned int width, height;
  Pixmap bitmap;
  int status;
  GC gc;
  XGCValues values;
  Window root = XRootWindowOfScreen(XtScreen(shell));

  PRINT("setting space %s label pixmap to %s\n", s->name,name);
  if (strcmp(name,"None") == 0 || strcmp(name,"none")== 0)
    return False;
  status = XReadBitmapFile(dsp,root,name,&width,&height,&bitmap,
			   (int *)NULL, (int *)NULL);

  if (status == BitmapOpenFailed)
    {
      fprintf(stderr, "can't open file %s\n", name);
      return False;
    }
  else if (status == BitmapFileInvalid)
    {
      fprintf(stderr, "bad bitmap format file: %s\n", name);
      return False;
    }
  else if (status == BitmapNoMemory)
    {
      fprintf(stderr, "insufficient memory for bitmap file: %s\n", name);
      return False;
    }
  values.foreground = BlackPixel(XtDisplay(shell),0);
  values.background = WhitePixel(XtDisplay(shell),0);
  gc = XCreateGC(dsp,root,GCForeground|GCBackground,&values);
  if (s->pixmap_label != None) XFreePixmap(dsp,s->pixmap_label);
  s->pixmap_label = XCreatePixmap(dsp,root,width,height,
			    (unsigned int)DefaultDepth(dsp,0));
  XCopyPlane(dsp,bitmap,s->pixmap_label,gc,0,0,width,height,0,0,
	    (unsigned long)1);
/*  s->pixmap = XCreatePixmapFromBitmapData(dsp,root,name,width,height,
					  BlackPixel(XtDisplay(shell),0),
					  WhitePixel(XtDisplay(shell),0),
					  (unsigned int)
					  DefaultDepth(dsp,
						       DefaultScreen(dsp)));*/
  strcpy(s->pixmap_name,name);
  PRINT("s->pixmap_name is %s\n",name);
  XFreeGC(dsp, gc);
  return True;
}



/*------------------------------------------------------------------
                            SpaceLabelPixmapSet

  ------------------------------------------------------------------*/
Boolean
SpaceLabelPixmapSet(Space *s)
{
  if (s->pixmap_label == None)
    return False;
  else return True;
}




/*------------------------------------------------------------------
                            CreateInternalStructure

  ------------------------------------------------------------------*/
Boolean
CreateInternalStructure(WorkWindow *w_window,XrmQuark *rooms_qlist)
{
  Space *space;
  int j;
  Boolean in_current_space = False;
  if (w_window->all_workspaces)
    {
      /* create internal structures */
      for (j=0, space = space_list; space !=NULL; space = space->next, j++)
	{
	  if (space == current_space)
	    in_current_space = True;
	  AddSpaceToWindow(space, w_window);
	  AddWindowToSpace(space, w_window);
	}
    }
  else if (rooms_qlist == NULL)
    {
      AddSpaceToWindow(current_space,w_window);
      AddWindowToSpace(current_space,w_window);
      in_current_space = True;
    }
  else
    {
      /* create internal structures */
      for (j=0; rooms_qlist[j] != NULLQUARK; j++)
	{
	  space = GetSpace(rooms_qlist[j]);
	  if (space == NULL)
	    space = CreateSpace(rooms_qlist[j], XrmQuarkToString(rooms_qlist[j]));
	  if (space == current_space)
	    in_current_space = True;
	  AddSpaceToWindow(space, w_window);
	  AddWindowToSpace(space, w_window);
	}
    }

  if (w_window->all_workspaces)
    AddWindowToSpace(all_space,w_window);
  return in_current_space;
}

/*------------------------------------------------------------------
                            CreateWorkWindow

  ------------------------------------------------------------------*/
WorkWindow*
CreateWorkWindow(Window window)
{
  WorkWindow *w, *w2;
  int i;

  w = GetWorkWindow(window);

  /* if wsm already knows about this window then just
     set mapped = True and return the window */
  if (w != NULL)
    {
      w->mapped = True;
      return w;
    }

  w = (WorkWindow *)XtMalloc(sizeof(WorkWindow));
  w->window = window;
  w->used = True;

  switch (_WSMGetConfigFormatType(window))
    {
    case WSM_GLOBAL_FMT:
      w->attrib_qlist = global_attribq;
      w->attrib_list = global_attrib_list;
      w->num_attrib_list = num_global_attrib;
      w->all_workspaces = True;
      w->specifier_qlist = GetSpecifierQList(0,False);
      w->name = GetWMName(0,False);
      break;

    case WSM_WINDOW_FMT:
      w->attrib_qlist = window_attribq;
      w->attrib_list = window_attrib_list;
      w->num_attrib_list = num_window_attrib;
      w->all_workspaces = False;
      w->specifier_qlist = GetSpecifierQList(window,False);
      w->name = GetWMName(window,False);
      break;

    case WSM_ICON_FMT:
      w->attrib_qlist = icon_attribq;
      w->attrib_list = icon_attrib_list;
      w->num_attrib_list = num_icon_attrib;
      w->all_workspaces = True;
      w->specifier_qlist = GetSpecifierQList(window, True);
      w->name = GetWMName(window,True);
      break;

    default:
      break;
    }

  w->next = NULL;
  w->prev = NULL;
  w->last_space = NULL;
  w->s_list = NULL;
  w->linked = False;
  w->mapped = True;
  w->win_data = (WSMWinData*)XtMalloc(w->num_attrib_list*sizeof(WSMWinData));
  for (i = 0; i < w->num_attrib_list; i++)
    {
      w->win_data[i].nameq = w->attrib_qlist[i];
      w->win_data[i].type = WSM_NONE;
      w->win_data[i].data.value = -1;
    }
  if (work_windows == NULL)
    work_windows = w;
  else
    {
      w2 = work_windows;
      while (w2->next != NULL)
	w2 = w2->next;
      w2->next = w;
      w->prev = w2;
    }

  if (w->all_workspaces)
    {
      CreateInternalStructure(w,NULL);
      UpdateBothList(all_space);
    }

  if (_WSMGetConfigFormatType(window) == WSM_WINDOW_FMT)
    {
      PRINT("XSelectInput ");
      print_window(w);
      XSelectInput(dsp,window,StructureNotifyMask);
    }

  return w;
}



/*------------------------------------------------------------------
                            ReCreateWorkWindow

  ------------------------------------------------------------------*/
WorkWindow*
ReCreateWorkWindow(Window window)
{
  WorkWindow *w;
  int i;

  w = GetWorkWindow(window);

  if (w == NULL)
    return CreateWorkWindow(window);

  switch (_WSMGetConfigFormatType(window))
    {
    case WSM_GLOBAL_FMT:
      w->attrib_qlist = global_attribq;
      w->attrib_list = global_attrib_list;
      w->num_attrib_list = num_global_attrib;
      break;

    case WSM_WINDOW_FMT:
      w->attrib_qlist = window_attribq;
      w->attrib_list = window_attrib_list;
      w->num_attrib_list = num_window_attrib;
      break;

    case WSM_ICON_FMT:
      w->attrib_qlist = icon_attribq;
      w->attrib_list = icon_attrib_list;
      w->num_attrib_list = num_icon_attrib;
      break;

    default:
      break;
    }

  XtFree((XtPointer)w->win_data);
  w->win_data = (WSMWinData*)XtMalloc(w->num_attrib_list*sizeof(WSMWinData));
  for (i = 0; i < w->num_attrib_list; i++)
    {
      w->win_data[i].nameq = w->attrib_qlist[i];
      w->win_data[i].type = WSM_NONE;
      w->win_data[i].data.value = -1;
    }

  return w;
}



/*------------------------------------------------------------------
                              GetSpace

  ------------------------------------------------------------------*/
Space*
GetSpace(XrmQuark name)
{
  Space *s;
  s = space_list;
  while (s != NULL)
    {
      if (s->nameq == name)
	return s;
      s = s->next;
    }
  PRINT("returning NULL space\n");
  return NULL;
}



/*------------------------------------------------------------------
                              GetSpaceFromName

  ------------------------------------------------------------------*/
Space*
GetSpaceFromName(char *name)
{
  Space *s;
  s = space_list;
  while (s != NULL)
    {
      if (strcmp(s->name,name)== 0)
	return s;
      s = s->next;
    }
  PRINT("returning NULL space\n");
  return NULL;
}




/*------------------------------------------------------------------
                              GetNumberSpaces

  ------------------------------------------------------------------*/
int
GetNumberSpaces()
{
  Space *s;
  int i;

  for (i = 0, s = space_list; s != NULL; s = s->next, i++);
  return i;
}


/*------------------------------------------------------------------
                           GetWorkWindowID

  ------------------------------------------------------------------*/
WorkWindow*
GetWorkWindowID(Space *s,int wsm_index)
{
  int j =0;
  WorkWindowList *w_list = s->w_list;

  while (w_list != NULL)
    {
      if (_WSMGetConfigFormatType(w_list->work_win->window) == WSM_WINDOW_FMT)
	{
	  if (j == wsm_index)
	    return w_list->work_win;
	  j++;
	}
      w_list = w_list->next;
    }
  return NULL;
}




/*------------------------------------------------------------------
                           GetSpaceFromID

  ------------------------------------------------------------------*/
Space*
GetSpaceFromID(int wsm_index)
{
  int j;
  Space *s;
  s = space_list;
  if (s == NULL) return NULL;

  for (j = 0; j < wsm_index; j++)
    {
      if (s == NULL)
	return NULL;
      s = s->next;
    }
 return s;
}





/*------------------------------------------------------------------
                           GetSpaceID

  ------------------------------------------------------------------*/
int
GetSpaceID(Space *space)
{
  int j;
  Space *s;

  for (j = 0, s = space_list; s != NULL; s = s->next, j++)
    {
      if (s == space)
	return j;
    }
 return -1;
}




/*------------------------------------------------------------------
                           GetWorkWindow

  ------------------------------------------------------------------*/
WorkWindow*
GetWorkWindow(Window window)
{
  WorkWindow *w;
  w = work_windows;
  while (w != NULL)
    {
      if (w->window == window)
	return w;
      w = w->next;
    }
  return NULL;
}






/*------------------------------------------------------------------
                           GetWorkWindowClient

  ------------------------------------------------------------------*/
/*
Boolean
GetWorkWindowClient(Window window, WorkWindow*** w_windows, int *num_w_windows)
{
  WorkWindow *w, *i_window;
  int num_w = 0;
  WorkWindow **w_wins;
  Boolean found = False;

  w = work_windows;
  while (w != NULL)
    {
      if (w->window == window)
	{
	  main_w = w;
	  found = True;
	  break;
	}
      w = w->next;
    }

  if (!found) return False;

  i_window = GetIconWorkWindow(win);

  if (i_window == NULL)
    w_wins = (WorkWindow*) XtMalloc(sizeof(WorkWindow*));
  else
    w_wins = (WorkWindow*) XtMalloc(2*sizeof(WorkWindow*));

  w_wins[num_w++] = main_w;
  if (i_window != NULL)
    w_wins[num_w++] = i_window;

  client_id = main_w->specifier_qlist[0];
  count = main_w->specifier_qlist[2];

  w = work_windows;
  while (w != NULL)
    {
      if (w->specifier_qlist[0] == client_id && w->specifier_qlist[2] == count)
	{
	  w_wins = (WorkWidnow*) XtRealloc(w_wins,(num_w+1) * sizeof(WorkWindow*));
	  w_wins[num_w] = w;
	  num_w++;
	}
    }

  *w_windows = w_wins;
  *num_w_windows = num_w;
  return True;
}
*/





/*------------------------------------------------------------------
                           GetWorkWindowClientIDs

  ------------------------------------------------------------------*/

Boolean
GetWorkWindowClientIDs(int main_pos, Space *s, int **w_ids, int *num_wids)
{
  WorkWindowList  *w_list;
  int num_w = 0;
  int *wids;
  Boolean found = False;
  int pos;
  WorkWindow *main_w;
  XrmQuark client_id, count;

  w_list = s->w_list;
  pos = 0;
  while (w_list != NULL)
    {
      if (_WSMGetConfigFormatType(w_list->work_win->window) == WSM_WINDOW_FMT)
	{
	  if (pos == main_pos)
	    {
	      main_w = w_list->work_win;
	      found = True;
	      break;
	    }
	  pos++;
	}
      w_list = w_list->next;
    }

  if (!found) return False;

  wids = (int*) XtMalloc(sizeof(int));

  wids[num_w++] = main_pos;

  client_id = main_w->specifier_qlist[0];
  count = main_w->specifier_qlist[2];

  w_list = s->w_list;
  pos = 0;
  while (w_list != NULL)
    {
      if (_WSMGetConfigFormatType(w_list->work_win->window) == WSM_WINDOW_FMT)
	{
	  if (w_list->work_win->specifier_qlist[0] == client_id
	      && w_list->work_win->specifier_qlist[2] == count
	      && w_list->work_win != main_w)
	    {
	      wids = (int*) XtRealloc((char*)wids,(num_w+1) * sizeof(int));
	      wids[num_w] = pos;
	      num_w++;
	    }
	  pos++;
	}
      w_list = w_list->next;
    }

  *w_ids = wids;
  *num_wids = num_w;

  return True;
}




/*------------------------------------------------------------------
                           GetIconWorkWindow

  ------------------------------------------------------------------*/
WorkWindow*
GetIconWorkWindow(Window window)
{
  WorkWindow *w;
  w = work_windows;
  while (w != NULL)
    {
      if (w->window == (window | ~WIN_MASK))
	return w;
      w = w->next;
    }
  PRINT("returning NULL icon window\n");
  return NULL;
}


/*------------------------------------------------------------------
                            AddSpaceToWindow

  ------------------------------------------------------------------*/
void
AddSpaceToWindow(Space *s,
		 WorkWindow *w_window)
{
  SpaceList *s_list, *s_list2;

  s_list = (SpaceList*)XtMalloc(sizeof(SpaceList));
  s_list->next = NULL;
  s_list->space = s;
  if (w_window->s_list == NULL)
    {
      w_window->s_list = s_list;
    }
  else
    {
      s_list2 = w_window->s_list;
      if (s_list2->space == s)
	{
	  XtFree((char*)s_list);
	  return;
	}
      while (s_list2->next != NULL)
	{
	  s_list2 = s_list2->next;
	  if (s_list2->space == s)
	    {
	      XtFree((char*)s_list);
	      return;
	    }
	}
      s_list2->next = s_list;
    }

  if ((w_window->s_list != NULL) && (w_window->s_list->next != NULL) &&
      (w_window->mapped) &&
      (_WSMGetConfigFormatType(w_window->window) == WSM_WINDOW_FMT))
  {
      EnableDeleteCommand(w_window->window);
  }

  UpdateButtons(w_window);
}


/*------------------------------------------------------------------
                            AddWindowToSpace

  ------------------------------------------------------------------*/
void
AddWindowToSpace(Space *s,
		 WorkWindow *w_window)
{
  WorkWindowList *w_list,*w_list2;

  w_list = (WorkWindowList*)XtMalloc(sizeof(WorkWindowList));
  w_list->next = NULL;
  w_list->work_win = w_window;
  w_list->sib_win = NULL;
  if (s->w_list == NULL)
    s->w_list = w_list;
  else
    {
      w_list2 = s->w_list;
      if (w_list2->work_win == w_window)
	{
	  XtFree((char*)w_list);
	  return;
	}
      while (w_list2->next != NULL)
	{
	  w_list2 = w_list2->next;
	  if (w_list2->work_win == w_window)
	    {
	      XtFree((char*)w_list);
	      return;
	    }
	}
      w_list2->next = w_list;
    }
  UpdateBothList(s);
}


/*------------------------------------------------------------------
                          RemoveWorkWindowFromSpace

  ------------------------------------------------------------------*/
void
RemoveWorkWindowFromSpace(Space *s,
			  WorkWindow *w_window)
{
  WorkWindowList *w_list,*pw_list;

  w_list = s->w_list;
  pw_list = w_list;
  while (w_list !=NULL)
    {
      if (w_list->work_win == w_window)
	{
	  if (pw_list == w_list)
	    s->w_list = s->w_list->next;
	  else
	    pw_list->next = w_list->next;
	  XtFree((XtPointer)w_list);
	  UpdateBothList(s);
	  return;
	}
      if (w_list->sib_win == w_window)
	w_list->sib_win = NULL;
      pw_list = w_list;
      w_list = w_list->next;
    }
  return;
}




/*------------------------------------------------------------------
                            RemoveSpaceFromWindow

  ------------------------------------------------------------------*/
void
RemoveSpaceFromWindow(Space *s,
		      WorkWindow *w_window)
{
  SpaceList *s_list,*ps_list;

  s_list = w_window->s_list;
  ps_list = s_list;
  while (s_list !=NULL)
    {
      if (s_list->space == s)
	{
	  if (ps_list == s_list)
	    w_window->s_list = w_window->s_list->next;
	  else
	    ps_list->next = s_list->next;
	  XtFree((XtPointer)s_list);

	  if (w_window->s_list != NULL)
	    if (w_window->s_list->next == NULL)
	      DisableDeleteCommand(w_window->window);
	  return;
	}
      ps_list = s_list;
      s_list = s_list->next;
    }
  if (w_window->s_list != NULL)
    if (w_window->s_list->next == NULL)
      DisableDeleteCommand(w_window->window);

  UpdateButtons(w_window);

}


/*------------------------------------------------------------------
                          RemoveWorkWindow

  ------------------------------------------------------------------*/
void
RemoveWorkWindow(WorkWindow *w_window, Boolean purge)
{
  SpaceList *s_list;
  WorkWindow *pre_window, *post_window;
  pre_window = w_window->prev;
  post_window = w_window->next;
  s_list = w_window->s_list;
  while (s_list != NULL)
    {
      RemoveWorkWindowFromSpace(s_list->space, w_window);
      s_list = s_list->next;
    }

  if (w_window->all_workspaces)
    RemoveWorkWindowFromSpace(all_space,w_window);

  if (work_windows == w_window)
    work_windows = w_window->next;
  if (pre_window != NULL)
    pre_window->next = post_window;
  if (post_window != NULL)
    post_window->prev = pre_window;

  if (purge)
    (void) PurgeAllWindowConfiguration(shell,
				w_window->specifier_qlist,
				w_window->attrib_qlist);


  XtFree((XtPointer)w_window->win_data);
  XtFree((XtPointer)w_window);
}

/*------------------------------------------------------------------
                            RemoveSpace

  ------------------------------------------------------------------*/
/* not tested */
void
RemoveSpace(Space *space)
{
  WorkWindowList *w_list, *destroy_list;
  Space *s, *p;

  w_list = space->w_list;
  while (w_list != NULL)
    {
      RemoveSpaceFromWindow(space,w_list->work_win);
      destroy_list = w_list;
      w_list = w_list->next;
      XtFree((char*)destroy_list);
    }

  s = space_list;
  p = s;
  while (s != NULL)
    {
      if (s == space)
	{
	  if (p == s)
	    space_list = s->next;
	  else
	    p->next = s->next;
	}
      p = s;
      s = s->next;
    }

  XtFree((XtPointer)space);
}

/*------------------------------------------------------------------
                            IsWorkWindowInSpace

  ------------------------------------------------------------------*/
Boolean
IsWorkWindowInSpace(WorkWindow *w,Space *s)
{
  WorkWindowList *w_list;

  if (w->all_workspaces && s!= all_space) return True;
  w_list = s->w_list;
  while (w_list != NULL)
    {
      if (w_list->work_win == w)
	return True;
      w_list = w_list->next;
    }
  return False;
}


/*------------------------------------------------------------------
                           IsSpaceInWorkWindow

  ------------------------------------------------------------------*/
Boolean
IsSpaceInWorkWindow(Space *s, WorkWindow *w)
{
  SpaceList *s_list;

  if (w->all_workspaces && s!= all_space) return True;
  s_list = w->s_list;
  while (s_list !=NULL)
    {
      if (s_list->space == s)
	return True;
      s_list = s_list->next;
    }
  return False;
}


/*------------------------------------------------------------------
                            InitializeWsm

  ------------------------------------------------------------------*/
void InitializeWsm()
{
  atoms.save_property = XInternAtom(XtDisplay(shell),"WM_SAVE_YOURSELF",False);
  atoms.protocols_property = XInternAtom(XtDisplay(shell),"WM_PROTOCOLS",False);
  atoms.sm_client_id_property = XInternAtom(XtDisplay(shell),"_MOTIF_SM_CLIENT_ID",False);
  atoms.wm_role_property = XInternAtom(XtDisplay(shell),"_MOTIF_WM_WINDOW_ROLE",False);
  atoms.wm_client_leader = XInternAtom(XtDisplay(shell),"WM_CLIENT_LEADER",False);

  all_space = (Space *)XtMalloc(sizeof(Space));
  all_space->nameq = XrmStringToQuark("all");
  all_space->w_list = NULL;
  all_space->next = NULL;

  current_space = NULL;
}



/*------------------------------------------------------------------
                            RestartWsm

  ------------------------------------------------------------------*/

void CompleteRestartWSM()
{
  Space *s;
  WorkWindowList *w_list, *destroy_w_list;
  WorkWindow *w, *destroy_w;
  SpaceList *s_list, *destroy_s_list;

  s = space_list;

  while (s!= NULL)
    {
      w_list = s->w_list;
      while (w_list != NULL)
	{
	  destroy_w_list = w_list;
	  w_list = w_list->next;
	  XtFree((XtPointer)destroy_w_list);
	}
      s->w_list = NULL;
      s = s->next;
    }

  w_list = all_space->w_list;
  while (w_list != NULL)
    {
      destroy_w_list=w_list;
      w_list = w_list->next;
      XtFree((XtPointer)destroy_w_list);
    }

  all_space->w_list = NULL;
  all_space->next = NULL;

  w = work_windows;
  while (w!= NULL)
    {
      destroy_w = w;
      s_list = w->s_list;
      while (s_list != NULL)
	{
	  destroy_s_list = s_list;
	  s_list = s_list->next;
	  XtFree((XtPointer)destroy_s_list);
	}
      w = w->next;
      XtFree((XtPointer)destroy_w);
    }

  work_windows = NULL;

  PRINT("Restarting WSM\n");

}


void RestartWSM()
{
  WorkWindow *w;

  for (w = work_windows; w != NULL; w = w->next)
  {
      w->used = False;
  }
}

void FinishRestartWSM()
{
  WorkWindow *w;

  for (w = work_windows; w != NULL; w = w->next)
  {
      if (w->used == False)
      {
	  RemoveWorkWindow(w,False);
      }
  }
}
/*------------------------------------------------------------------
                            GetWSMWindow

  ------------------------------------------------------------------*/
Window
GetWSMWindow(WorkWindow *w_window)
{

  return (w_window->window);

}



/*------------------------------------------------------------------
                            SetWorkWindowValues

  ------------------------------------------------------------------*/
void
SetWorkWindowValues(WorkWindow *w, Boolean all_workspaces, Boolean linked)
{
  if (all_workspaces && !(w->all_workspaces))
      {
	AddWindowToSpace(all_space,w);
	UpdateBothList(all_space);
	UpdateButtons(w);
      }
  else if (!all_workspaces && w->all_workspaces)
    {
      RemoveWorkWindowFromSpace(all_space,w);
      UpdateBothList(all_space);
      UpdateButtons(w);
    }
  w->all_workspaces = all_workspaces;
  w->linked = linked;


}


/*------------------------------------------------------------------
                            UnmapCurrentSpace

  ------------------------------------------------------------------*/
void
UnmapCurrentSpace()
{
  WorkWindowList *w_list = current_space->w_list;
  while (w_list!= NULL)
    {
      w_list->work_win->mapped = False;
      w_list = w_list->next;
    }
}

/*------------------------------------------------------------------
                            UnmapWorkWindow

  ------------------------------------------------------------------*/
void
UnmapWorkWindow(WorkWindow *w_window)
{
  w_window->mapped = False;
}

/*------------------------------------------------------------------
                            MapWorkWindow

  ------------------------------------------------------------------*/
void
MapWorkWindow(WorkWindow *w_window)
{
  w_window->mapped = True;
}


/**********************************************************************/
/*  This is for testing purposes.				      */
/**********************************************************************/


char*
trimblanks(char *s)
{
  char    *p;

  p = &s[strlen(s)-1];    /* point to last char */
  while((*p <= ' ') && p >= s) p--;
  p++;
  *p = '\0';

  return s;
}


void
translate_for_database(char *s)
{
  int length = strlen(s);
  int i,j;


  for (i = 0; i < length; i++)
    {
      if (s[i] == ':')
	{
	  s[i] = '&';
	}
    }
}

static char*
GetWMName(Window window, Boolean is_icon)
{
  XTextProperty   textProperty;
  char *str = NULL;

  if (window == 0)
    {
      str = (char*) XtMalloc((strlen("Global") +1) * sizeof(char));
      strcpy(str,"Global");
      return str;
    }
  if(XGetWMName(dsp, (window & WIN_MASK), &textProperty)) {
    if (textProperty.encoding == XA_STRING) {
      if (textProperty.value) {
	str = (String)textProperty.value;
	trimblanks(str);
	if(str[0])
	  {
	    if (is_icon)
	      {
		str = (char*)XtRealloc(str,(strlen(str)+2 )* sizeof(char));
		strcat(str,"I");
	      }
	    return str;
	  }
      }
    }
  }

  strcpy(str, "");
  return str;
}




static char *
GetWMClient(Window window, Boolean is_icon)
{
  int format;
  int status;
  unsigned long nitems, after;
  Atom type = None;
  char *str;

  status = XGetWindowProperty(dsp,
			      (window & WIN_MASK),
			      atoms.sm_client_id_property,
			      0L,(long)1000,
			      False,
			      XA_STRING,
			      &type,
			      &format,
			      &nitems,
			      &after,
			      (unsigned char **)&str);

  if (status == Success)
    {
      if (str == NULL) return (NULL);
      translate_for_database(str);
      if (is_icon)
	{
	  str = (char*) XtRealloc((char*)str,(strlen(str)+2) * sizeof(char));
	  strcat(str,"I");
	}
      return str;
    }


  /*
   * if there's no clientID on this window, then see if it points
   * to a client leader that has a clientID.
   */
  else
    {
      Window leader;

      /* check for CLIENT_LEADER property. */
      status = XGetWindowProperty(dsp,
				  (window & WIN_MASK),
				  atoms.wm_client_leader,
				  0L,(long)1000,
				  False,
				  XA_WINDOW,
				  &type,
				  &format,
				  &nitems,
				  &after,
				  (unsigned char **)&leader);
      if (status == Success)
	{
	  if ((leader == None) || (leader == window))
	    return (NULL);
	  else
	    return (GetWMClient(leader, is_icon));
	}

      else
	{
	  /* Old-style client - no WM_CLIENT_ID property */
	  return (NULL);
	}
    }
}





static
char *
GetWMRole(Window window, Boolean is_icon)
{
  int format;
  int status;
  unsigned long nitems, after;
  Atom type = None;
  char *str;

  status = XGetWindowProperty(dsp,
			      (window & WIN_MASK),
			      atoms.wm_role_property,
			      0L,(long)1000,
			      False,
			      XA_STRING,
			      &type,
			      &format,
			      &nitems,
			      &after,
			      (unsigned char **)&str);

  if (status == Success)
    {
      if (str == NULL) return (NULL);
      translate_for_database(str);
      if (is_icon)
	{
	  str = (char*) XtRealloc((char*)str,(strlen(str)+2) * sizeof(char));
	  strcat(str,"I");
	}
      return str;
    }

  else return (NULL);
}





static
char *
GetWMCount(XrmQuark client, XrmQuark role)
{
  WorkWindow *w;
  char *str;
  int count;

  count = 0;
  for (w = work_windows; w!= NULL; w = w->next)
    {
      if (w->specifier_qlist[0] == client && w->specifier_qlist[1] == role)
	count++;
    }
  str = (String) XtMalloc(4*sizeof(char));
/*  if (count == 0)
    strcpy(str,"*");
  else*/
    sprintf(str,"%d",count);
  return str;
}


XrmQuarkList
GetSpecifierQList(Window window, Boolean is_icon)
{
  XrmQuark *specifier;
  char *client;
  char *role;
  char *count;

  specifier = (XrmQuark*)XtMalloc(4*sizeof(XrmQuark));

  if (window != 0)
    {
      client = GetWMClient(window, is_icon);
      if (client == NULL)
	client = GetWMName(window,is_icon);
      role = GetWMRole(window, is_icon);
      if (role == NULL)
	role = GetWMName(window,is_icon);
      specifier[0] = XrmStringToQuark(client);
      specifier[1] = XrmStringToQuark(role);
      count = GetWMCount(specifier[0],specifier[1]);
      specifier[2] = XrmStringToQuark(count);
      specifier[3] = NULLQUARK;
      XtFree((XtPointer)client);
      XtFree((XtPointer)role);
      XtFree((XtPointer)count);
    }
  else
    {
      specifier[0] = XrmStringToQuark("Global");
      specifier[1] = NULLQUARK;
    }
  return specifier;
}


