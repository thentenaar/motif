/**
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

#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: wsmData.c /main/6 1995/07/14 09:48:38 drk $"
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <Xm/Xm.h>
#include "wsm.h"
#include "wsmDebug.h"
#include "xrmLib.h"

void
LongListDestructor(
     XtAppContext ,
     XrmValuePtr,
     XtPointer,
     XrmValuePtr ,
     Cardinal *
);

Boolean CvtStringToLongList(
Display *,
XrmValuePtr,
Cardinal *,
XrmValuePtr,
XrmValuePtr,
XtPointer *
);


extern XrmQuark hideq;
extern IndexStruct wsm_index;
extern Widget shell;
extern Space *current_space;
extern Space *space_list;


/*------------------------------------------------------------------
                             UpdateXrm

   Save the current workspaceList, backgroundList, and labelPixmapList
  ------------------------------------------------------------------*/
void
UpdateXrm()
{
  Space *s;
  char *s_list;
  char *b_list;
  char *p_list;
  char *c_space;
  int curr_size = 0;
  int max_size =200;

  s_list = (char*) XtMalloc(max_size *sizeof(char));
  s_list[0] = '\0';
  for (s = space_list; s != NULL; s = s->next)
    {
      curr_size = strlen(s->name) + 2;
      if (curr_size > max_size)
        {
          max_size +=100;
          s_list = (char*)XtRealloc(s_list,max_size*sizeof(char));
        }
      strcat(s_list,s->name);
      if (s->next != NULL)
        strcat(s_list,",");
    }

  max_size = 200;
  b_list = (char*) XtMalloc(max_size *sizeof(char));
  b_list[0] = '\0';
  for (s = space_list; s != NULL; s = s->next)
    {
      curr_size = strlen(s->background) + 2;
      if (curr_size > max_size)
        {
          max_size +=100;
          b_list = (char*)XtRealloc(b_list,max_size*sizeof(char));
        }
      strcat(b_list,s->background);
      if (s->next != NULL)
        strcat(b_list,",");
    }

  max_size = 200;
  p_list = (char*) XtMalloc(max_size *sizeof(char));
  p_list[0] = '\0';
  for (s = space_list; s != NULL; s = s->next)
    {
      curr_size = strlen(s->pixmap_name) + 2;
      if (curr_size > max_size)
        {
          max_size +=100;
          p_list = (char*)XtRealloc(p_list,max_size*sizeof(char));
        }
      strcat(p_list,s->pixmap_name);
      if (s->next != NULL)
        strcat(p_list,",");
    }

  max_size = strlen(current_space->name) +1;
  c_space = (char*) XtMalloc( max_size*sizeof(char));
  strcpy(c_space,current_space->name);
  (void )SaveSpaceListResources(s_list,b_list,p_list,c_space);

  XtFree((char*)s_list);
  XtFree((char*)b_list);
  XtFree((char*)p_list);
  XtFree((char*)c_space);
}



/*------------------------------------------------------------------
                             ConvertToWsmData

  Convert an XrmValue into a WSMWinData
  ------------------------------------------------------------------*/
Boolean
ConvertToWSMData(XrmValue *value,
                 WSMWinData *win_data,
                 WSMWinData *start_data,
                 WSMAttribute *attrib)
{
  XrmValue toVal;
  XtPointer fByte;
  int i,j;
  static Boolean register_long_converter;
  toVal.size = sizeof(fByte);
  toVal.addr = (XtPointer)&fByte;
  if (!(attrib->is_list))
    {
      if (XtConvertAndStore(shell,
                            XtRString,
                            value,
                            XtRInt,
                            &toVal))
        {
          if (start_data->type == WSM_NONE ||
              (int) *(int*)toVal.addr !=  (int) start_data->data.value )
            {
              win_data->nameq = attrib->nameq;
              win_data->data.value = *(int*)toVal.addr;
              win_data->type = WSM_VALUE_DATA;
#ifdef DEBUG2
              PRINT(" value %s %s %ld %ld\n",XrmQuarkToString(attrib->nameq),
                    (char*)value->addr,*(int*)toVal.addr,start_data->data.value);
#endif
              return True;
            }
          else return False;
        }
    }
  else if (attrib->size == sizeof(long)*8)
    {
      if (!register_long_converter)
        {
          XtSetTypeConverter(XtRString,"LongList",CvtStringToLongList,NULL,
                             0, XtCacheNone,LongListDestructor);
          register_long_converter = True;
        }
      if (XtConvertAndStore(shell,
                            XtRString,
                            value,
                            "LongList",
                            &toVal))
        {
          win_data->nameq = attrib->nameq;
          win_data->data.long_ptr = *(long**)toVal.addr;
          win_data->data_len = toVal.size;
          win_data->type = WSM_LONG_LIST_DATA;
#ifdef DEBUG2
          PRINT("value stackingOrder %d: ", win_data->data_len);
          for (j = 0; j < toVal.size; j++)
            PRINT(" %ld ",win_data->data.long_ptr[j]);
          PRINT("\n");
#endif
          return True;
        }
    }
  else
    {
      PRINT("not converted to wsm struct\n");
    }
  return False;
}



/*------------------------------------------------------------------
                             WinDataCopy

   Copy a WSMWinData structure
  ------------------------------------------------------------------*/
void
WinDataCopy(WSMWinData *w_data,
            WSMWinData *copy_data)
{
  int i;
  switch(copy_data->type) {
  case WSM_CHAR_LIST_DATA:
    {
      w_data->type = WSM_CHAR_LIST_DATA;
      break;
    }
  case WSM_SHORT_LIST_DATA:
    {
      w_data->type = WSM_SHORT_LIST_DATA;
      break;
    }
  case WSM_LONG_LIST_DATA:
    {
/*      long *long_list;
      if (w_data->data_len != 0)
        XtFree((XtPointer)w_data->data.long_ptr);
      long_list = (long*)XtMalloc(copy_data->data_len*sizeof(long));
      for (i = 0; i < copy_data->data_len; i ++)
        long_list[i] = copy_data->data.long_ptr[i];
      w_data->data.long_ptr =long_list;
*/
      w_data->type = WSM_LONG_LIST_DATA;
      break;
    }
  case WSM_VALUE_DATA:
    {
      w_data->data.value =copy_data->data.value;
      w_data->type = WSM_VALUE_DATA;
      break;
    }
  case WSM_NONE:
    /* Not handled case */
    break;
  }
  w_data->nameq = copy_data->nameq;

}



/*------------------------------------------------------------------
                            CreateWSMWinData

  Create WSMWinData list from an XrmValue list
  ------------------------------------------------------------------*/
Boolean
CreateWSMWinData(XrmValue *value_list,
                 Boolean diffs_allowed,
                 WorkWindow *w_window,  /* return */
                 WSMWinData **win_data_return, /* return */
                 int *num_data_return)         /* return */
{
  int i;
  Boolean retval = False;
  WSMWinData *win_data;
  Boolean in_dbase;
  int num_data = 0;
  *num_data_return = 0;

  win_data = NULL;
#ifdef DEBUG
  PRINT("enter CreateWinData ......\n");
#endif
  /* win_data is returned for use in a set state request */
  win_data = (WSMWinData*)XtMalloc(w_window->num_attrib_list*sizeof(WSMWinData));

  /* loop through attributes and convert value_list to win_data */
  for (i = 0; i < w_window->num_attrib_list; i++)
    {
      in_dbase = False;
      /* if value in database */
      if (value_list != NULL)
        if (value_list[i].size > 0)
          {
            /* convert it and assign */
            if (ConvertToWSMData(&(value_list[i]),
                                 &(win_data[num_data]),
                                 &(w_window->win_data[i]),
                                 &(w_window->attrib_list[i])))
              {
                WinDataCopy(&(w_window->win_data[i]), &(win_data[num_data]));
                num_data++;
                retval = True;
                in_dbase = True;
              }
          }
      /* if not in database and diffs aren't allowed*/
      if (!diffs_allowed  && !in_dbase && w_window->win_data != NULL)
          {
#ifdef DEBUG
            PRINT(" no %s\n",XrmQuarkToString(w_window->attrib_qlist[0]));
#endif
            WinDataCopy(&(win_data[num_data]),&(w_window->win_data[i]));
            num_data++;
          }
    }

  if (num_data !=0)
    {
      win_data = (WSMWinData*) XtRealloc((char *)win_data, num_data *sizeof(WSMWinData));
      *win_data_return = win_data;
    }
  else
    {
      XtFree((char*)win_data);
      *win_data_return = NULL;
    }
  *num_data_return = num_data;
#ifdef DEBUG
  PRINT("return\n");
#endif
  return retval;
}




/*------------------------------------------------------------------
                           CreateStartWSMWinData

  Create WSMWinData list from an XrmValue list and initialize
  a windows WSMWinData list.
  ------------------------------------------------------------------*/
Boolean
CreateStartWSMWinData(XrmValue *value_list,
                      Boolean diffs_allowed,
                      WSMWinData *start_win_data,
                      int num_start_win_data,
                      WorkWindow *w_window,
                      WSMWinData **win_data_return, /* return */
                      int *num_data_return)         /* return */
{
  int i;
  WSMWinData *start_data;

  /* set internal start_win_data_return */
  if (start_win_data != NULL)
    {
      for (i = 0; i < w_window->num_attrib_list; i++)
        {
          start_data = _WSMGetMatchingWinData(start_win_data,
                                              num_start_win_data,
                                              w_window->attrib_qlist[i]);
          if (start_data != NULL)
            WinDataCopy(&(w_window->win_data[i]),start_data);
        }
    }

  return CreateWSMWinData(value_list,
                          diffs_allowed,
                          w_window,
                          win_data_return,
                          num_data_return);
}





/*------------------------------------------------------------------
                             CreateHideWSMWinData

  Create WSMData list with hidden = True.
  ------------------------------------------------------------------*/
void
CreateHideWSMWinData(Boolean diffs_allowed,
                     WorkWindow *w_window,
                     WSMWinData **win_data_return,
                     int *num_data_return)
{
  int i;
  WSMWinData *win_data = NULL;
  int num_data = 0;

  *num_data_return = 0;

#ifdef DEBUG
  PRINT("enter CreateHideWSMWinData ......");
#endif

  if (diffs_allowed && _WSMGetConfigFormatType(w_window->window) == WSM_WINDOW_FMT)
    {
      win_data = (WSMWinData*)XtMalloc(sizeof(WSMWinData));
      num_data = 1;
      win_data[0].data.value = 1;
      win_data[0].nameq = hideq;
      win_data[0].type = WSM_VALUE_DATA;
      WinDataCopy(&(w_window->win_data[wsm_index.hide]), &(win_data[0]));
    }

  else if (_WSMGetConfigFormatType(w_window->window) == WSM_WINDOW_FMT)
    {
      win_data = (WSMWinData*)XtMalloc(w_window->num_attrib_list *sizeof(WSMWinData));
      for (i = 0; i < w_window->num_attrib_list; i++)
        {
          if (w_window->attrib_qlist[i] == hideq)
            win_data[i].data.value = 1;
          else
            WinDataCopy(&(win_data[num_data]),&(w_window->win_data[i]));
          num_data++;
        }
      w_window->win_data[wsm_index.hide].data.value = 1;
    }

  *win_data_return = win_data;
  *num_data_return = num_data;
#ifdef DEBUG
  PRINT("return\n");
#endif
}




/*------------------------------------------------------------------
                         CreateStartHideWSMWinData

  Create WSMWinData list with hidden = True and initialize window's
  WSMWinData list
  ------------------------------------------------------------------*/
void
CreateStartHideWSMWinData(Boolean diffs_allowed,
                          WSMWinData *start_win_data,
                          int num_start_win_data,
                          WorkWindow *w_window,
                          WSMWinData **win_data_return,
                          int *num_data_return)
{
  int i;
  WSMWinData *start_data;


  for (i = 0; i < w_window->num_attrib_list; i++)
    {
      if (w_window->attrib_list[i].nameq == hideq)
        {
          w_window->win_data[i].data.value = 1;
          w_window->win_data[i].nameq = hideq;
          w_window->win_data[i].type = WSM_VALUE_DATA;
        }
      else
        {
          if (start_win_data != NULL)
            {
              start_data = _WSMGetMatchingWinData(start_win_data,
                                                  num_start_win_data,
                                                  w_window->attrib_qlist[i]);
              if (start_data != NULL)
                    WinDataCopy(&(w_window->win_data[i]),start_data);
            }
        }
    }

  CreateHideWSMWinData(diffs_allowed,
                       w_window,
                       win_data_return,
                       num_data_return);

}


/*------------------------------------------------------------------
                         CreateUnhideWSMWinData

 Create WSMWinData list with hidden = False
  ------------------------------------------------------------------*/
void
CreateUnhideWSMWinData(XrmValue *value,
                       WorkWindow *w_window,
                       WSMWinData **win_data_return,
                       int *num_data_return)
{
  WSMWinData *win_data;
  XrmValue              toVal;
  XtPointer             fByte;
  int num_data = 0;
  *num_data_return = 0;

#ifdef DEBUG
  PRINT("enter CreateUnhideWSMWinData ......\n");
#endif

  win_data = NULL;
  num_data = 0;

  if (_WSMGetConfigFormatType(w_window->window) == WSM_WINDOW_FMT)
    {
      if (win_data == NULL)
        win_data = (WSMWinData*)XtMalloc(sizeof(WSMWinData));
      win_data[num_data].data.value = 0;
      win_data[num_data].nameq = hideq;
      win_data[num_data].type = WSM_VALUE_DATA;
      w_window->win_data[wsm_index.hide].data.value = 0;
      num_data++;
    }
  *win_data_return = win_data;
  *num_data_return = num_data;
#ifdef DEBUG
  PRINT("return\n");
#endif
    }




/*------------------------------------------------------------------
                            FreeValues

  Free values created in CreateValues
  ------------------------------------------------------------------*/
void
FreeValues(int num_values, XrmValue *value_list)
{
  int i;
  if (value_list == NULL) return;
  for (i = 0; i < num_values; i++)
    {
      if (value_list[i].size !=0 && value_list[i].addr != NULL)
        XtFree((XtPointer)value_list[i].addr);
    }
  XtFree((XtPointer)value_list);

}


/*------------------------------------------------------------------
                            CreateValues

  Create XrmValue list from WSMWinData list
  ------------------------------------------------------------------*/
  Boolean
  CreateValues(WorkWindow *w_window,
             WSMWinData *data_list,
             int num_data_list,
             XrmValue **value_list_return)
{

  int i;
  WSMWinData *win_data;
  XrmValue *values;
  char* str;
  char tempstr[10];
  int num_values;


#ifdef DEBUG
  PRINT("enter CreateValues ...\n");
#endif
  win_data = NULL;
  num_values = 0;
  values = (XrmValue*)XtMalloc(w_window->num_attrib_list*sizeof(XrmValue));
  for (i = 0; i < w_window->num_attrib_list; i++)
    {
      win_data =  _WSMGetMatchingWinData(data_list,
                                         num_data_list,
                                         w_window->attrib_qlist[i]);
      if (win_data != NULL)
        {
          switch(win_data->type) {
          case WSM_CHAR_LIST_DATA:
            {
            }
          case WSM_SHORT_LIST_DATA:
            {
            }
          case WSM_LONG_LIST_DATA:
            {
              str = (char*)XtMalloc(10 * (win_data->data_len) *sizeof(char));
              str[0] = '\0';
              for (i = 0; i < win_data->data_len; i++)
                {
                  sprintf(tempstr,"%ld",win_data->data.long_ptr[i]);
                  strcat(str,tempstr);
                  if (i < win_data->data_len -1) strcat(str,",");
                }
              values[num_values].addr = str;
              values[num_values].size = strlen(str)+1;
              num_values++;
              break;
            }
          case WSM_VALUE_DATA:
            {
              str = (char*)XtMalloc(10 *sizeof(char));
              sprintf(str,"%ld", win_data->data.value);
              values[num_values].addr = str;
              values[num_values].size = strlen(str) +1;
#ifdef DEBUG
              PRINT(" V: %s = %s \n",XrmQuarkToString(w_window->attrib_qlist[i]),str);
#endif
              num_values++;
              break;
            }
          case WSM_NONE:
              /* Not handled case */
              break;
          }
        }
      else
        {
          values[num_values].addr = NULL;
          values[num_values].size = 0;
          num_values++;
        }
    }
  *value_list_return = values;
#ifdef DEBUG
  PRINT("return\n");
#endif
  if (num_values == 0) return False;
  else return True;
}



/*------------------------------------------------------------------
                             UpdateWinData
  ------------------------------------------------------------------*/
void
UpdateWinData(WSMWinData *win_data,
              int win_data_count,
              WSMWinData *start_win_data,
              int start_win_data_count)
{
  int i;
  WSMWinData *start_data;

  for (i = 0; i < win_data_count; i++)
    {
      start_data = _WSMGetMatchingWinData(start_win_data,
                                          start_win_data_count,
                                          win_data[i].nameq);
      if (start_data != NULL)
        WinDataCopy(start_data,&(win_data[i]));
    }

}


Boolean
CvtStringToLongList(Display *dpy, XrmValuePtr args,
                    Cardinal *num_args, XrmValuePtr from, XrmValuePtr to, XtPointer *data)
{

  int i, count = 1;
  char *ch;
  char *start = from->addr;
  int len;
  static long *list;
  Boolean reset;
  char *str;
  XrmValue value, toVal;
  if (*num_args != 0)
      {
        XtAppErrorMsg(XtDisplayToApplicationContext(dpy),
                      "cvtStringToSTringList", "wrongParamters",
                      "XtToolkitError",
                      "String to string list conversion needs no extra args",
                      (String *) NULL, (Cardinal *) NULL);
      }
  if (to->addr != NULL && to->size < sizeof(XtPointer)) {
    to->size = sizeof(XtPointer);
    return FALSE;
  }

  if (start == NULL || *start == '\0') list = NULL;
  else
    {
      count = 1;
      for (ch = start; *ch != '\0'; ch++)
        {
          if (*ch == ',')
            count++;
        }
      list = (long*)XtMalloc(count*sizeof(long));
      str = (char*)XtMalloc(10*sizeof(char));
      for (i = 0; i < count; i++)
        {
          for (ch = start; *ch != ',' && *ch != '\0'; ch++){}
          len = ch - start;
          strncpy(str,start,len);
          str[len] = '\0';
          list[i] = atol(str);
          start = ch+1;
        }
      XtFree((XtPointer) str);
    }
  if (to->addr == NULL) to->addr = (caddr_t)&list;
  else *(long**) to->addr = list;
  to->size = count;
  return TRUE;

}


void
LongListDestructor(XtAppContext app, XrmValuePtr to ,
                     XtPointer converter_data,
                     XrmValuePtr args,
                     Cardinal *num_args)
{
  long *list = (long *) to->addr;
  long *entry;

  if (list == NULL) return;

  for (entry = list; entry != NULL; entry++)
    {
      XtFree((XtPointer)entry);
    }

  XtFree((XtPointer)list);
}

