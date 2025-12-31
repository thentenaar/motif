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
static char rcsid[] = "$XConsortium: xrmLib.c /main/6 1995/07/14 10:01:41 drk $"
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <Xm/Xm.h>
#include "wsm.h"
#include "wsmDebug.h"
#include "wsmData.h"


XrmDatabase wsm_dbase = NULL;
static XrmQuark linkedq = NULLQUARK;
static XrmQuark space_listq = NULLQUARK;
static XrmQuark all_workspacesq = NULLQUARK;
static XrmQuark current_spaceq = NULLQUARK;
static XrmQuark show_dialogq = NULLQUARK;
static XrmQuark workspace_listq = NULLQUARK;
static XrmQuark background_listq = NULLQUARK;
static XrmQuark pixmap_listq = NULLQUARK;
XrmQuark hideq = NULLQUARK;
XrmQuark allspaceq = NULLQUARK;


/*------------------------------------------------------------------
                         InitializeXrm

  ------------------------------------------------------------------*/
Boolean
InitializeXrm(char *dbase_name)
{
  Boolean retval = True;
  /* initialize dbase */
  XrmInitialize();
  if (wsm_dbase == NULL)
    {
      wsm_dbase = XrmGetFileDatabase(dbase_name);
      if (wsm_dbase == NULL)
	{
	  printf("cannot open database %s\n", dbase_name);
	  wsm_dbase = XrmGetStringDatabase("wsm_WSM.WSM.0.linked:True\nwsm_WSM.WSM.0.allWorkspaces:True\nwsm_WSM.WSM.0.linkedRoom.hidden:0\nsaveAsShell_WSM*allWorkspaces:True\nconfigureShell_WSM*allWorkspaces:True\nnameShell_WSM*allWorkspaces: True\nbackgroundShell_WSM*allWorkspaces:True\ndeleteShell_WSM*allWorkspaces:True\nsaveAsShell_WSM*allWorksapces:True\noccupyShell_WSM*allWorkspaces:True");
	  if (wsm_dbase == NULL)
	    {
	      printf("cannot open databse \n");
	      retval =  False;
	    }
	}
    }

  /* initialize workspace resource quarks */
  if (linkedq == NULLQUARK)
    {
      current_spaceq = XrmStringToQuark("currentWorkspace");
      workspace_listq = XrmStringToQuark("workspaceList");
      background_listq = XrmStringToQuark("backgroundList");
      pixmap_listq = XrmStringToQuark("labelPixmapList");
      space_listq = XrmStringToQuark("spaceList");
      show_dialogq = XrmStringToQuark("showWSDialog");
      linkedq = XrmStringToQuark("linked");
      all_workspacesq   = XrmStringToQuark("allWorkspaces");
      hideq = XrmStringToQuark("hidden");
      allspaceq = XrmStringToQuark("all");
    }
  return retval;
}

/*------------------------------------------------------------------
                        GetSpaceListResources

  ------------------------------------------------------------------*/
Boolean
GetSpaceListResources(Widget w,
		      char ***s_list,
		      char ***b_list,
		      char ***p_list,
		      char **c_space,
		      Boolean *s_dialog)
{
  XrmValue value;
  XrmRepresentation type;
  XrmValue toVal;
  XrmValue boolVal;
  Boolean retval = False;
  XrmQuark specifier[2];


  toVal.addr = NULL;

  specifier[0] = workspace_listq;
  specifier[1] = NULLQUARK;

  if (XrmQGetResource(wsm_dbase,
		      specifier,
		      specifier,
		      &type,
		      &value))
    {
      if (XtConvertAndStore(w,
			    XtRString,
			    &value,
			    XtRStringTable,
			    &toVal))
	{
	  *s_list = *(String**)toVal.addr;
	  retval = True;
	}
    }
  else
      *s_list = NULL;

  specifier[0] = background_listq;
  specifier[1] = NULLQUARK;

  if (XrmQGetResource(wsm_dbase,
		      specifier,
		      specifier,
		      &type,
		      &value))
    {
      if (XtConvertAndStore(w,
			    XtRString,
			    &value,
			    XtRStringTable,
			    &toVal))
	{
	  *b_list = *(String**)toVal.addr;
	  retval = True;
	}
    }
  else
      *b_list = NULL;



  specifier[0] = pixmap_listq;
  specifier[1] = NULLQUARK;

  if (XrmQGetResource(wsm_dbase,
		      specifier,
		      specifier,
		      &type,
		      &value))
    {
      if (XtConvertAndStore(w,
			    XtRString,
			    &value,
			    XtRStringTable,
			    &toVal))
	{
	  *p_list = *(String**)toVal.addr;
	  retval = True;
	}
    }
  else
      *p_list = NULL;

  specifier[0] = current_spaceq;
  if (XrmQGetResource(wsm_dbase,
		      specifier,
		      specifier,
		      &type,
		      &value))
    {
      *c_space = value.addr;
      retval = True;
    }
  else *c_space = NULL;

  specifier[0] = show_dialogq;
  if (XrmQGetResource(wsm_dbase,
		      specifier,
		      specifier,
		      &type,
		      &value))
    {
      if (XtConvertAndStore(w,
			    XtRString,
			    &value,
			    XtRBoolean,
			    &boolVal))
	{
	  *s_dialog = *(Boolean*)toVal.addr;
	  retval = True;
	}
    }
  else
      *s_dialog = True;

  return retval;
}





/*------------------------------------------------------------------
                        GetWorkspaceResources

  ------------------------------------------------------------------*/
Boolean
GetWorkspaceResources(Widget w,
		      XrmQuarkList specifier_list,
		      XrmQuarkList *room_list, /*return*/
		      Boolean *all_workspaces, /*return*/
		      Boolean *linked)         /*return*/
{
  XrmHashTable *xrm_workspace_list;
  XrmValue boolVal;
  XrmValue value;
  XrmRepresentation type;
  Boolean a_bool;
  XrmValue toVal;
  String *room_str_list;
  XrmQuark *room;
  int i;
  Boolean retval = False;
  int list_length = 100;
  int max_length = 5;
  int curr_length = 1;
  xrm_workspace_list =  (XrmHashTable*)XtMalloc(list_length *sizeof(XrmHashTable));

  toVal.addr = NULL;

#ifdef DEBUG
 PRINT("GetWorkSpaceResources for %s\n",XrmQuarkToString(specifier_list[0]));
#endif

  /* fetch a list of database levels that matches the specifier */
  if (XrmQGetSearchList(wsm_dbase,
			specifier_list,
			specifier_list,
			xrm_workspace_list,
			list_length)    == False)
    {
#ifdef DEBUG
      PRINT("XrmQGetSearchList is false\n");
#endif
      return False;
    }

  /* get list of worspaces from database */
  room = (XrmQuark*)XtMalloc(max_length*sizeof(XrmQuark));
  if (XrmQGetSearchResource(xrm_workspace_list,
			    space_listq,
			    space_listq,
			    &type,
			    &value))
    {
      if (XtConvertAndStore(w,
			    XtRString,
			    &value,
			    XtRStringTable,
			    &toVal))
	{
	  room_str_list = *(String **)toVal.addr;
#ifdef DEBUG
	  PRINT("rooms list found: ");
#endif
	  for (i = 0; room_str_list[i] != NULL; i++)
	    {
	      curr_length++;
	      if (curr_length > max_length)
		{
		  max_length +=5;
		  room = (XrmQuark*)XtRealloc((char*)room,(max_length)*sizeof(XrmQuark));
		}
	      room[i] = XrmStringToQuark(room_str_list[i]);
#ifdef DEBUG
	      PRINT(" %s ",room_str_list[i]);
#endif
	    }
#ifdef DEBUG
	  PRINT("\n");
#endif
	  room[i] = NULLQUARK;
	  *room_list = room;
	  retval = True;
	}
    }
  else
    {
#ifdef DEBUG
      PRINT("%s is null for %s\n", XrmQuarkToString(space_listq),
	     XrmQuarkToString(specifier_list[0]));
#endif
      XtFree((XtPointer)room);
      *room_list = NULL;
    }

  /* get linked resource from database */
  boolVal.size = sizeof(Boolean);
  boolVal.addr = (XtPointer)&a_bool;
  if (XrmQGetSearchResource(xrm_workspace_list,
			    linkedq,
			    linkedq,
			    &type,
			    &value))
    {
      if (XtConvertAndStore(w,
			    XtRString,
			    &value,
			    XtRBoolean,
			    &boolVal))
	{
	  *linked = *(Boolean*)boolVal.addr;
#ifdef DEBUG
	  if (*linked) PRINT("linked: True\n");
	  else PRINT("linked: False\n");
#endif
	  retval = True;
	}
    }
  else
    {
#ifdef DEBUG
      PRINT("linked not found\n");
#endif
      *linked = False;
    }

  /* get allWorkspace resource from database */
  if (XrmQGetSearchResource(xrm_workspace_list,
			    all_workspacesq,
			    all_workspacesq,
			    &type,
			    &value))
    {
      if (XtConvertAndStore(w,
			    XtRString,
			    &value,
			    XtRBoolean,
			    &boolVal))
	{
	  *all_workspaces = *(Boolean*)boolVal.addr;
#ifdef DEBUG
	  if (*all_workspaces) PRINT("all_workspaces: True %d\n",*all_workspaces);
	  else PRINT("all_workspaces: False\n");
#endif
	  retval = True;
	}
    }
  else
    {
#ifdef DEBUG
      PRINT("all_workspaces not found\n");
#endif
      *all_workspaces = False;
    }

  XtFree((XtPointer)xrm_workspace_list);
  return retval;
}




/*------------------------------------------------------------------
                        GetWindowConfigurationEntry

  ------------------------------------------------------------------*/
Boolean
GetWindowConfigurationEntry(XrmQuarkList specifier_list,
			    XrmQuarkList attribute_list,
			    XrmQuark room_name,
			    XrmValue **attribute_values) /* return */

{
  int list_length  = 100;
  XrmHashTable *xrm_workspace_list;
  XrmRepresentation type;
  XrmQuark specifier[10];
  XrmValue *values_array = NULL;
  Boolean retval = False;
  int i,j;
  int num_attrib;
/*  PRINT("enter GetWindowConfig ......");*/

  xrm_workspace_list =  (XrmHashTable*)XtMalloc(list_length *sizeof(XrmHashTable));

  for (i = 0; specifier_list[i] != NULLQUARK; i++)
    {
      specifier[i] = specifier_list[i];
    }

  specifier[i] = room_name;
  specifier[i+1] = NULLQUARK;

  if (wsm_dbase == NULL)
    {
      return False;
    }

  for (num_attrib = 0; attribute_list[num_attrib] != NULLQUARK; num_attrib++);


  if (XrmQGetSearchList(wsm_dbase,
		    specifier,
		    specifier,
		    xrm_workspace_list,
		    list_length))
    {
      values_array = (XrmValue*)XtMalloc(num_attrib*sizeof(XrmValue));
      for (j = 0; attribute_list[j] != NULLQUARK ;j++)
	{
	  if (!XrmQGetSearchResource(xrm_workspace_list,
				     attribute_list[j],
				     attribute_list[j],
				     &type,
				     &(values_array[j])))
	    {
	      values_array[j].size = 0;
	      values_array[j].addr = NULL;
	    }
	  else retval = True;
	}
    }

  *attribute_values = values_array;
/*  PRINT("return\n");*/
  XtFree((XtPointer)xrm_workspace_list);
  return retval;
}




/*------------------------------------------------------------------
                      GetAllWindowConfigurationEntry

  ------------------------------------------------------------------*/
Boolean
GetAllWindowConfigurationEntry(XrmQuarkList specifier_list,
			       XrmQuarkList attribute_list,
			       XrmQuarkList room_list,
			       XrmValue ***attribute_values)/*return*/
{
  Boolean retval = False;
  int list_length  = 100;
  XrmHashTable *xrm_workspace_list;
  XrmRepresentation type;
  XrmQuark specifier[10];
  XrmValue *values_array;
  int i,j;
  int num_rooms, num_attributes;
  int spec_index;
  XrmValue **values;
#ifdef DEBUG
/*  PRINT("enter GetAllWindowConfig ......");*/
#endif
  xrm_workspace_list =  (XrmHashTable*)XtMalloc(list_length *sizeof(XrmHashTable));

  /* create a QuarkList for use in database query */
  for (i = 0; specifier_list[i] != NULLQUARK; i++)
    {
      specifier[i] = specifier_list[i];
    }

  spec_index = i;
  specifier[spec_index+1] = NULLQUARK;

  if (wsm_dbase == NULL)
    {
      return retval;
    }

  for (num_attributes = 0; attribute_list[num_attributes] != NULLQUARK; num_attributes++);

  for (num_rooms = 0; room_list[num_rooms] != NULLQUARK; num_rooms++);

  values = (XrmValue **)XtMalloc(num_rooms*sizeof(XrmValue*));
  for (i = 0; i < num_rooms; i ++)
    {
      values_array = (XrmValue*)XtMalloc(num_attributes*sizeof(XrmValue));

      specifier[spec_index] = room_list[i];

      if (XrmQGetSearchList(wsm_dbase,
			specifier,
			specifier,
			xrm_workspace_list,
			list_length))
	{
	  for (j = 0;attribute_list[j] != NULLQUARK; j++)
	    {
	      if (!XrmQGetSearchResource(xrm_workspace_list,
					 attribute_list[j],
					 attribute_list[j],
					 &type,
					 &(values_array[j])))
		{
		  values_array[j].size = 0;
		  values_array[j].addr = NULL;
		}
	      else retval = True;
	    }
	}
      values[i] = values_array;
    }

  *attribute_values = values;
/* PRINT("return\n");*/
  XtFree((XtPointer)xrm_workspace_list);
  return retval;
}






/*------------------------------------------------------------------
                       SaveSpaceListResources

  ------------------------------------------------------------------*/
Boolean
SaveSpaceListResources(char *space_list,
		       char *b_list,
		       char *p_list,
		       char *c_space)
{
  XrmBinding bind[2];
  XrmQuark specifier[2];
  XrmValue value;

  PRINT("enter SaveSpaceListResources .....");

  if (wsm_dbase == NULL)
    {
      return False;
    }


  specifier[0] = workspace_listq;
  specifier[1] = NULLQUARK;
  bind[0] = XrmBindLoosely;
  bind[1] = NULLQUARK;


  value.addr = space_list;
  value.size = strlen(space_list) +1;

  XrmQPutResource(&wsm_dbase,
		  bind,
		  specifier,
		  XrmStringToRepresentation("String"),
		  &value);


  specifier[0] = background_listq;
  specifier[1] = NULLQUARK;
  bind[0] = XrmBindLoosely;
  bind[1] = NULLQUARK;


  value.addr = b_list;
  value.size = strlen(b_list) +1;

  XrmQPutResource(&wsm_dbase,
		  bind,
		  specifier,
		  XrmStringToRepresentation("String"),
		  &value);



  specifier[0] = pixmap_listq;
  specifier[1] = NULLQUARK;
  bind[0] = XrmBindLoosely;
  bind[1] = NULLQUARK;


  value.addr = p_list;
  value.size = strlen(p_list) +1;

  XrmQPutResource(&wsm_dbase,
		  bind,
		  specifier,
		  XrmStringToRepresentation("String"),
		  &value);

  specifier[0] = current_spaceq;
  value.addr = c_space;
  value.size = strlen(c_space) +1;

  XrmQPutResource(&wsm_dbase,
		  bind,
		  specifier,
		  XrmStringToRepresentation("String"),
		  &value);


/*  XrmPutFileDatabase(wsm_dbase, ".test2");*/
  PRINT("return\n");
  return True;
}






/*------------------------------------------------------------------
                       SaveWorkspaceResources

  ------------------------------------------------------------------*/
Boolean
SaveWorkspaceResources(Widget w,
		       XrmQuarkList specifier_list,
		       XrmQuark room_nameq,
		       XrmQuark remove_roomq,
		       Boolean all_workspaces,
		       Boolean linked)
{
  XrmQuarkList room_list = NULL;
  XrmBinding bind[10];
  XrmQuark specifier[10];
  int specifier_count;
  static char linked_string[10];
  static char all_workspace_string[10];
  XrmValue value;
  int i;
  Boolean room_found = False;
  Boolean in_dbase = False;
  Boolean all_work = False;
  Boolean link = False;
  char *rooms_str;
  char *str;
  char *new_str;
  int size;

/*  PRINT("enter SaveWorkspaceResources .....");*/

  if (wsm_dbase == NULL)
    {
      return False;
    }


  for (i = 0; specifier_list[i] != NULLQUARK; i++)
    {
      specifier[i] = specifier_list[i];
      bind[i] = XrmBindTightly;
    }
  bind[i] = XrmBindTightly;
  bind[i+1] = NULLQUARK;
  specifier[i+1] = NULLQUARK;
  specifier_count = i;

  in_dbase = GetWorkspaceResources(w,
				   specifier_list,
				   &room_list,
				   &all_work,
				   &link);

  if (in_dbase)
    {
      if (room_list!= NULL)
	{
	  for (i = 0; room_list[i] != NULLQUARK; i++)
	    {
	      if (room_nameq == room_list[i])
		room_found = True;
	    }
	}
    }

  if (!room_found)
    {
#ifdef DEBUG
      PRINT("room %s not found %s\n",XrmQuarkToString(room_nameq),
	     XrmQuarkToString(specifier[0]));
#endif
      new_str = XrmQuarkToString(room_nameq);
      rooms_str = (char*) XtMalloc(2 * sizeof(char));
      rooms_str[0] = '\0';

      if (room_list != NULL)
	{
	  for (i = 0; room_list[i] != NULLQUARK; i++)
	    {
	      if (room_list[i] != remove_roomq)
		{
		  str = XrmQuarkToString(room_list[i]);
		  size = strlen(rooms_str) +1;
		  rooms_str = (char*) XtRealloc(rooms_str,(size + strlen(str) + 3)*sizeof(char));
		  strcat(rooms_str,str);
		  strcat(rooms_str,",");
		}
	    }
	}
      size = strlen(rooms_str) +1;
      rooms_str = (char*) XtRealloc(rooms_str,(size + strlen(new_str)+2) * sizeof(char));
      strcat(rooms_str,new_str);
      value.addr = rooms_str;
      value.size = strlen(rooms_str) +1;

      specifier[specifier_count] =space_listq;
      XrmQPutResource(&wsm_dbase,
		      bind,
		      specifier,
		      XrmStringToRepresentation("String"),
		      &value);
      XtFree((XtPointer)rooms_str);
    }


  if (room_list != NULL)
    XtFree((XtPointer)room_list);

  if (!in_dbase || link != linked)
    {
      /* add linked and allWorkspaces  to dbase here */
      if (linked) strcpy(linked_string,"True");
      else strcpy(linked_string,"False");

      value.addr = linked_string;
      value.size = strlen(linked_string) +1;

      specifier[specifier_count] =linkedq;

      XrmQPutResource(&wsm_dbase,
		      bind,
		      specifier,
		      XrmStringToRepresentation("String"),
		      &value);


    }

  if (!in_dbase || all_work != all_workspaces)
    {
      if (all_workspaces) strcpy(all_workspace_string,"True");
      else strcpy(all_workspace_string,"False");

      specifier[specifier_count] = all_workspacesq;

      value.addr = all_workspace_string;
      value.size = strlen(all_workspace_string) +1;

      XrmQPutResource(&wsm_dbase,
		      bind,
		      specifier,
		      XrmStringToRepresentation("String"),
		      &value);
    }

/*  XrmPutFileDatabase(wsm_dbase, ".test2");*/
/*  PRINT("return\n");*/
  return True;
}




/*------------------------------------------------------------------
                        SaveWindowConfiguration

  ------------------------------------------------------------------*/
Boolean
SaveWindowConfiguration(XrmQuarkList specifier_list,
			XrmQuarkList attribute_qlist,
			XrmQuark room_nameq,
			XrmValue *attr_values)
{
  int j;
  XrmBinding bind[10];
  XrmQuark specifier[10];
  int specifier_index;
  int i;

/*  PRINT("enter SaveWindowConfigure .....");*/
  if (wsm_dbase == NULL)
    {
      return False;
    }


  for (i = 0; specifier_list[i] != NULLQUARK; i++)
    {
      specifier[i] = specifier_list[i];
      bind[i] = XrmBindTightly;
    }

  specifier[i] = room_nameq;
  bind[i] = XrmBindTightly;
  specifier_index = i+1;
  bind[i+1] = XrmBindTightly;
  specifier[i +2] = NULLQUARK;
  bind[i+2] = NULLQUARK;

  for (j = 0; attribute_qlist[j] != NULLQUARK; j++)
    {
      specifier[specifier_index] = attribute_qlist[j];
      if (attr_values[j].size != 0)
	{
	  XrmQPutResource(&wsm_dbase,
			  bind,
			  specifier,
			  XrmStringToRepresentation("String"),
			  &(attr_values[j]));
	}
    }

/*  XrmPutFileDatabase(wsm_dbase, ".test2");*/
/*  PRINT("return\n");*/
  return True;
}

static
Boolean delete_line(char *line)
{
  int i;
  for (i = 0; line[i] != '\0'; i++);

  if (line[i-2] == 'X' &&
      line[i-3] == 'X' &&
      line[i-4] == 'X' )

    {
      return True;
    }
  else return False;
}





/*------------------------------------------------------------------
                      RemoveAllWorkspaceResources

  ------------------------------------------------------------------*/
void
RemoveAllWorkspaceResources(XrmQuarkList specifier_list)
{
  XrmQuark work_attrib_qlist[5];
  int i,j;
  char *str = "XXX";
  XrmBinding bind[10];
  XrmQuark specifier[10];
  int spec_index;
  XrmValue value;

  work_attrib_qlist[0] = space_listq;
  work_attrib_qlist[1] = linkedq;
  work_attrib_qlist[2] = all_workspacesq;
  work_attrib_qlist[3] = NULLQUARK;

  for (i = 0; specifier_list[i] != NULLQUARK; i++)
    {
      specifier[i] = specifier_list[i];
      bind[i] = XrmBindTightly;
    }

  bind[i] = XrmBindTightly;
  specifier[i +1] = NULLQUARK;
  bind[i+1] = NULLQUARK;
  spec_index = i;

  value.addr = str;
  value.size = strlen(str)+1;

  for (j = 0; work_attrib_qlist[j] != NULLQUARK; j++)
    {
      specifier[spec_index] = work_attrib_qlist[j];
	  XrmQPutResource(&wsm_dbase,
			  bind,
			  specifier,
			  XrmStringToRepresentation("String"),
			  &value);
    }

}



/*------------------------------------------------------------------
                        RemoveWorkspaceResource

  ------------------------------------------------------------------*/
/*       */
Boolean
RemoveWorkspacesResource(Widget w,
			 XrmQuarkList specifier_list,
			 XrmQuark room_nameq)
{
  XrmBinding bind[10];
  XrmQuark specifier[10];
  Boolean in_dbase = False;
  XrmQuarkList room_list = NULL;
  int i;
  Boolean all_workspaces = False;
  Boolean linked = False;
  Boolean room_found = False;
  XrmValue value;
  char true_str[10];
  char *rooms_str;
  char *r_str;
  int len;
  int size;

  strcpy(true_str,"True");

  rooms_str = (char*) XtMalloc(2*sizeof(char));
  rooms_str[0] = '\0';
  in_dbase = GetWorkspaceResources(w,
				   specifier_list,
				   &room_list,
				   &all_workspaces,
				   &linked);

  if (in_dbase)
    {
      if (room_list!= NULL)
	for (i = 0; room_list[i] != NULLQUARK; i++)
	  {
	    if (room_nameq == room_list[i] || room_list[i] == allspaceq)
	      {
		room_found = True;
	      }
	    else
	      {
		r_str = XrmQuarkToString(room_list[i]);
		size = strlen(rooms_str) +1;
		rooms_str = (char*) XtRealloc(rooms_str,(size + strlen(r_str) + 3) * sizeof(char));
		strcat(rooms_str,r_str);
		strcat(rooms_str,",");
	      }
	  }
    }

  if (room_list != NULL)
    XtFree((XtPointer)room_list);

  if (in_dbase && room_found && strlen(rooms_str) > 0)
    {
      len = strlen(rooms_str);
      if (len > 0)
	rooms_str[len-1] = '\0';
      for (i = 0; specifier_list[i] != NULLQUARK; i++)
	{
	  specifier[i] = specifier_list[i];
	  bind[i] = XrmBindTightly;
	}
/*      PRINT("New workspace list %s\n",rooms_str);*/
      bind[i] = XrmBindTightly;
      specifier[i] = space_listq;
      bind[i+1] = NULLQUARK;
      specifier[i+1]= NULLQUARK;
      value.addr = rooms_str;
      value.size = strlen(rooms_str) +1;
      XrmQPutResource(&wsm_dbase,
		      bind,
		      specifier,
		      XrmStringToRepresentation("String"),
		      &value);

      /* for now just set room.hidden:True */
/*      specifier[i] = hideq;
      value.addr = true_str;
      value.size = strlen(true_str) + 1;
      XrmQPutResource(&wsm_dbase,
		      bind,
		      specifier,
		      XrmStringToRepresentation("String"),
		      &value);
      */
    }

/*  else if (in_dbase && room_found && strlen(rooms_str) == 0)
    {
      RemoveAllWorkspaceResources(specifier);
    }*/

  else
    {
      XtFree(rooms_str);
      return False;
    }

  XtFree(rooms_str);
  return True;

}





/*------------------------------------------------------------------
                      PurgeAllWindowConfiguration

  ------------------------------------------------------------------*/
Boolean
PurgeAllWindowConfiguration(Widget w,
			    XrmQuarkList specifier_list,
			    XrmQuarkList attribute_qlist)
{
  Boolean retval = False;
  int i, j,num_attrib;
  char * str = "XXX";
  FILE *fileD;
  FILE *fileN;
  char line[512];
  XrmBinding bind[10];
  XrmQuark specifier[10];
  XrmValue value;
  Boolean in_dbase = False;
  XrmQuarkList room_list;
  Boolean all_workspaces;
  Boolean linked;
  int room_index, attrib_index;
  char *del_db, *save_db;

  in_dbase = GetWorkspaceResources(w,
				   specifier_list,
				   &room_list,
				   &all_workspaces,
				   &linked);

  if (!in_dbase) return False;

  RemoveAllWorkspaceResources(specifier_list);

  for (num_attrib = 0; attribute_qlist[num_attrib]!= NULLQUARK; num_attrib++)
    {
    }

  value.addr = str;
  value.size = strlen(str)+1;

  for (i = 0; specifier_list[i] != NULLQUARK; i++)
    {
      specifier[i] = specifier_list[i];
      bind[i] = XrmBindTightly;
    }

  bind[i] = XrmBindTightly;
  bind[i+1] = XrmBindTightly;
  specifier[i+2] = NULLQUARK;
  bind[i+2] = NULLQUARK;
  room_index = i;
  attrib_index = i+1;

  for (i = 0; room_list[i] != NULLQUARK; i++)
    {
      specifier[room_index] = room_list[i];
      for (j = 0; attribute_qlist[j] != NULLQUARK; j++)
	{
	  specifier[attrib_index] = attribute_qlist[j];
	      XrmQPutResource(&wsm_dbase,
			      bind,
			      specifier,
			      XrmStringToRepresentation("String"),
			      &value);
	}
    }

  if (room_list != NULL)
    XtFree((XtPointer)room_list);

  del_db = ".wsmDelDB";
  save_db = ".wsmSaveDB";
  XrmPutFileDatabase(wsm_dbase, del_db);

  fileD  = fopen(del_db,"r");
  fileN  = fopen(save_db,"w");
  if (fileD && fileN)
  {
      while (fgets(line,512,fileD))
      {
	  if (!(delete_line(line)))
	  {
	      if (fputs(line,fileN) == EOF)
	      {
		  PRINT("can't puts %s\n",line);
	      }
	  }
      }
      fclose(fileD);
      fclose(fileN);

      XrmDestroyDatabase(wsm_dbase);
      wsm_dbase = XrmGetFileDatabase(save_db);

      remove(del_db);
      remove(save_db);
  }

  return retval;
}




/*------------------------------------------------------------------
                        PurgeWindowConfiguration

  ------------------------------------------------------------------*/
Boolean
PurgeWindowConfiguration(Widget w,
			 XrmQuarkList specifier_list,
			 XrmQuarkList attribute_qlist,
			 XrmQuark room_nameq)
{
  int i,j;
  Boolean retval = False;
  int num_attrib;
  FILE *fileD;
  FILE *fileN;
  char line[512];
  XrmBinding bind[10];
  XrmQuark specifier[10];
  XrmValue value;
  int specifier_index;
  char *str = "XXX";
  char *del_db, *save_db;

  for (num_attrib = 0; attribute_qlist[num_attrib]!= NULLQUARK; num_attrib++)
    {
    }

  value.addr = str;
  value.size = strlen(str)+1;

  RemoveWorkspacesResource(w,
			   specifier_list,
			   room_nameq);


  for (i = 0; specifier_list[i] != NULLQUARK; i++)
    {
      specifier[i] = specifier_list[i];
      bind[i] = XrmBindTightly;
    }

  specifier[i] = room_nameq;
  bind[i] = XrmBindTightly;
  specifier_index = i+1;
  bind[i+1] = XrmBindTightly;
  specifier[i +2] = NULLQUARK;
  bind[i+2] = NULLQUARK;

  for (j = 0; attribute_qlist[j] != NULLQUARK; j++)
    {
      specifier[specifier_index] = attribute_qlist[j];
	  XrmQPutResource(&wsm_dbase,
			  bind,
			  specifier,
			  XrmStringToRepresentation("String"),
			  &value);
    }

  del_db = ".wsmDelDB";
  save_db = ".wsmSaveDB";
  XrmPutFileDatabase(wsm_dbase, del_db);

  fileD  = fopen(del_db,"r");
  fileN  = fopen(save_db,"w");

  if (fileD && fileN)
  {
      while (fgets(line,512,fileD))
      {
	  if (!(delete_line(line)))
	  {
	      if (fputs(line,fileN) == EOF)
	      {
		  PRINT("can't puts %s\n",line);
	      }
	  }
      }
      fclose(fileD);
      fclose(fileN);

      XrmDestroyDatabase(wsm_dbase);
      wsm_dbase = XrmGetFileDatabase(save_db);

      remove(del_db);
      remove(save_db);
  }

  return retval;
}


/*------------------------------------------------------------------
                       SaveWsmToFile

  ------------------------------------------------------------------*/
void
SaveWsmToFile(char *file_name)
{
  UpdateXrm();
  XrmPutFileDatabase(wsm_dbase, file_name);
}

