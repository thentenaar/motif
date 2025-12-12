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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: XmTabList.c /main/9 1999/04/27 17:49:59 samborn $"
#endif
#endif

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <Xm/XmosP.h>		/* For ALLOCATE/DEALLOCATE_LOCAL */
#include "MessagesI.h"
#include "ResIndI.h"
#include "XmI.h"
#include "XmRenderTI.h"
#include "XmStringI.h"
#include "XmTabListI.h"

/* Warning Messages */
#define NEGATIVE_VALUE_MSG		_XmMMsgXmTabList_0000

/********    Static Function Declarations    ********/

static XmTab GetNthTab(XmTabList tl,
		       int pos,
		       XmTab cur_tab,
		       int cur_pos);


/********    End Static Function Declarations    ********/

/*
 * This function returns the tab in tl at position pos.  It starts searching
 * either from the start of the tablist or from cur_tab if cur_pos is closer to
 * pos than zero and cur_tab is not NULL.
 */
static XmTab
GetNthTab(XmTabList tl, int pos, XmTab cur_tab, int cur_pos)
{
  XmTab		prev_tab;
  unsigned int 	count;
  int		i;

  if (pos == 0) return(_XmTabLStart(tl));

  count = _XmTabLCount(tl);

  if (abs(pos) >= count)
  {
    if (pos > 0) return(_XmTabPrev(_XmTabLStart(tl)));
    else return(_XmTabLStart(tl));
 }

  /* Convert pos and cur_pos to positives less than count */
  if (pos < 0) pos += count;
  cur_pos %= count;
  if (cur_pos < 0) cur_pos += count;

  if (pos == cur_pos) return(cur_tab);

  /* Is start or cur_tab closer? */
  if ((cur_tab != NULL) &&
      ((pos > cur_pos/2) || (pos < (count + cur_pos)/2)))
    {
      prev_tab = cur_tab;
      i = pos - cur_pos;
    }
  else
    {
      prev_tab = _XmTabLStart(tl);
      i = (pos < count/2) ? pos : pos - count;
    }

  switch (i/abs(i))
    {
    case 1:
      for (; i > 0; i--)
	prev_tab = _XmTabNext(prev_tab);
      break;

    case -1:
      for (; i < 0; i++)
	prev_tab = _XmTabPrev(prev_tab);
      break;
    }

  return(prev_tab);
}

/*
 * A tablist is a doubly linked ring of tabs.
 */
XmTabList
XmTabListInsertTabs(XmTabList oldlist,
		    XmTab *tabs,
		    Cardinal tab_count,
		    int position)
{
  XmTabList	tl;
  int		i;
  XmTab		prev_tab, tab, next_tab;

  _XmProcessLock();
  if ((tabs == NULL) || (tab_count == 0)) {
	_XmProcessUnlock();
	return(oldlist);
  }

  if (oldlist == NULL)
    {
      tl = (XmTabList)XtMalloc(sizeof(_XmTabListRec));

      _XmTabLCount(tl) = tab_count;

      prev_tab = _XmTabCopy(tabs[0]);

      _XmTabLStart(tl) = prev_tab;

      for (i = 1; i < tab_count; i++)
	{
	  tab = _XmTabCopy(tabs[i]);

	  _XmTabPrev(tab) = prev_tab;
	  _XmTabNext(prev_tab) = tab;
	  prev_tab = tab;
	}

      _XmTabNext(prev_tab) = _XmTabLStart(tl);
      _XmTabPrev(_XmTabLStart(tl)) = prev_tab;
    }
  else
    {
      tl = XmTabListCopy(oldlist, 0, 0);

      /* Hook in first tab */
      tab = _XmTabCopy(tabs[0]);

      prev_tab = GetNthTab(tl, position, NULL, 0);

      if (position == 0) _XmTabLStart(tl) = tab;

      next_tab = _XmTabNext(prev_tab);

      _XmTabNext(prev_tab) = tab;
      _XmTabPrev(tab) = prev_tab;
      prev_tab = tab;

      /* Hook in rest of tabs. */
      for (i = 1; i < tab_count; i++)
	{
	  tab = _XmTabCopy(tabs[i]);
	  _XmTabNext(prev_tab) = tab;
	  _XmTabPrev(tab) = prev_tab;
	  prev_tab = tab;
	}

      /* Complete circle */
      _XmTabNext(prev_tab) = next_tab;
      _XmTabPrev(next_tab) = prev_tab;

      _XmTabLCount(tl) += tab_count;

      XmTabListFree( oldlist );
    }

  _XmProcessUnlock();
  return(tl);
}

Widget
_XmCreateTabList(Widget parent,
		 String name,	/* unused */
		 ArgList arglist, /* unused */
		 Cardinal argcount) /* unused */
{
  XmRendition	rend = (XmRendition)parent;
  XmTabList	tl = NULL;

  if (_XmRendTabs(rend) == NULL)
    {
      tl = (XmTabList)XtMalloc(sizeof(_XmTabListRec));
      bzero((char *)tl, sizeof(_XmTabListRec));
      _XmRendTabs(rend) = tl;
    }

  return((Widget)tl);
}

/*
 * This copying routine also works with the internal marking scheming used
 * by the insert and replace routines.
 */
XmTabList
XmTabListCopy(XmTabList tablist,
	      int offset,
	      Cardinal count)
{
  XmTabList	tl;
  XmTab		old_tab, tab, next_tab;
  unsigned int	i;

  _XmProcessLock();
  if (tablist == NULL) {
	_XmProcessUnlock();
	return(NULL);
  }

  tl = (XmTabList)XtMalloc(sizeof(_XmTabListRec));

  /* Zero count implies copy from offset to end/beginning */
  if (count == 0) count = (_XmTabLCount(tablist) - abs(offset));

  if (count > _XmTabLCount(tablist)) count = _XmTabLCount(tablist);

  old_tab = GetNthTab(tablist, offset, NULL, 0);

  /* If marked, routine called by insert/replace. Don't copy. */
  tab = _XmTabMark(old_tab) ? old_tab : _XmTabCopy(old_tab);

  /* Add first. */
  _XmTabLCount(tl) = count;
  _XmTabLStart(tl) = tab;

  /* Add rest. */
  for (i = 1; i < count; i++)
    {
      old_tab = (offset >= 0) ? _XmTabNext(old_tab) : _XmTabPrev(old_tab);
      /* See above.  Don't copy if marked. */
      next_tab = _XmTabMark(old_tab)? old_tab : _XmTabCopy(old_tab);

      _XmTabNext(tab) = next_tab;

      _XmTabPrev(next_tab) = tab;

      tab = next_tab;
    }

  /* Complete circle. */
  _XmTabNext(tab) = _XmTabLStart(tl);
  _XmTabPrev(_XmTabLStart(tl)) = tab;

  _XmProcessUnlock();
  return(tl);
}

/*
 * Marked tabs have mark cleared but aren't actually freed.
 */
void
XmTabListFree(XmTabList tablist)
{
  int	i;
  XmTab	tab, next;

  _XmProcessLock();
  if (tablist == NULL) {
	_XmProcessUnlock();
	return;
  }

  tab = _XmTabLStart(tablist);

  for (i = 1; i < _XmTabLCount(tablist); i++)
    {
      next = _XmTabNext(tab);

      if (_XmTabMark(tab)) _XmTabMark(tab) = FALSE;
      else XmTabFree(tab);

      tab = next;
    }

  if (_XmTabMark(tab)) _XmTabMark(tab) = FALSE;
  else XmTabFree(tab);

  _XmProcessUnlock();
  XtFree((char *)tablist);
}

Cardinal
XmTabListTabCount(XmTabList tablist)
{
  Cardinal ret_val;
  _XmProcessLock();
  if (tablist == NULL) {
	_XmProcessUnlock();
	return 0;
  }

  ret_val = _XmTabLCount(tablist);
  _XmProcessUnlock();
  return ret_val;
}

XmTab
XmTabListGetTab(XmTabList tablist,
		Cardinal position)
{
   XmTab ret_val;
  _XmProcessLock();
  if (tablist == NULL || position >= _XmTabLCount(tablist)) {
	_XmProcessUnlock();
	return((XmTab)NULL);
  }

  ret_val = _XmTabCopy(GetNthTab(tablist, position, NULL, 0));
  _XmProcessUnlock();
  return ret_val;
}

/*
 * This routine uses the mark bit of tabs so that replaced tabs can
 * be copied upon replacement and not copied when the rest of the tabs
 * are copied or freed when the old tabs are freed.
 */
XmTabList
XmTabListReplacePositions(XmTabList oldlist,
			  Cardinal *position_list,
			  XmTab *tabs,
			  Cardinal tab_count)
{
  unsigned int	i;
  unsigned int	cur_pos;
  XmTab		cur_tab, tab, prev, next;
  XmTabList	tl;

  _XmProcessLock();
  if ((oldlist == NULL) ||
      (position_list == NULL) ||
      (tabs == NULL) || (tab_count == 0)) {
    _XmProcessUnlock();
    return(oldlist);
  }

  tl = (XmTabList)XtMalloc(sizeof(_XmTabListRec));
  _XmTabLCount(tl) = _XmTabLCount(oldlist);
  cur_tab = _XmTabLStart(tl) = _XmTabLStart(oldlist);
  cur_pos = 0;

  /* Make the replacement in the old list, then copy and free. */
  for (i = 0; i < tab_count; i++)
    {
      cur_tab = GetNthTab(tl, position_list[i],
			  cur_tab, cur_pos);
      cur_pos = position_list[i];

      prev = _XmTabPrev(cur_tab);
      next = _XmTabNext(cur_tab);

      /* replace tab copying */
      tab = _XmTabCopy(tabs[i]);

      if (prev == cur_tab) { /* only one tab in list */
	_XmTabPrev(tab) = _XmTabNext(tab) = tab;
      } else {
	_XmTabNext(prev) = tab;
	_XmTabPrev(tab) = prev;
	_XmTabNext(tab) = next;
	_XmTabPrev(next) = tab;
      }
      if (cur_tab == _XmTabLStart(tl))
	_XmTabLStart(tl) = tab;

      XmTabFree(cur_tab);
      cur_tab = tab;
    }

  XtFree((char *)oldlist);

  _XmProcessUnlock();
  return(tl);
}

/*
 * This routine uses a mark/sweep algorithm.
 * A pass over position_list is made to mark tabs for removal without
 * disturbing their positions.
 * Then a pass is made over the oldlist removing marked tabs.
 * A final pass is made to copy the remaining tabs.
 */
XmTabList
XmTabListRemoveTabs(XmTabList oldlist,
		    Cardinal *position_list,
		    Cardinal position_count)
{
  XmTab		cur_tab, tab, prev, next;
  int		cur_pos, i;
  XmTabList	tl;

  _XmProcessLock();
  if ((oldlist == NULL) ||
      (position_list == NULL) ||
      (position_count == 0)) {
    _XmProcessUnlock();
    return(oldlist);
  }

  cur_tab = _XmTabLStart(oldlist);
  cur_pos = 0;

  /* Get position, set mark */
  for (i = 0; i < position_count; i++)
    {
      cur_tab = GetNthTab(oldlist, position_list[i],
			  cur_tab, cur_pos);
      cur_pos = position_list[i];
      _XmTabMark(cur_tab) = TRUE;
    }

  /* Free marked tabs */
  for (tab = _XmTabNext(_XmTabLStart(oldlist));
       tab != _XmTabLStart(oldlist);
       tab = next)
    {
      if (_XmTabMark(tab))
	{
	  prev = _XmTabPrev(tab);
	  next = _XmTabNext(tab);

	  _XmTabNext(prev) = next;
	  _XmTabPrev(next) = prev;

	  XmTabFree(tab);
	  _XmTabLCount(oldlist) --;
	}
      else
	  next = _XmTabNext(tab);
    }

  /* tab is now at start. */
  if (_XmTabMark(tab))
    {
      if (tab == _XmTabNext(tab))
	/* We've deleted all the tabs. */
	{
	  _XmTabLCount(oldlist) = 1;
	  _XmTabMark(tab) = FALSE;
	  XmTabListFree(oldlist);
	  _XmProcessUnlock();
	  return((XmTabList)NULL);
	}

      _XmTabLStart(oldlist) = _XmTabNext(tab);

      prev = _XmTabPrev(tab);
      next = _XmTabNext(tab);

      _XmTabNext(prev) = next;
      _XmTabPrev(next) = prev;

      XmTabFree(tab);
      _XmTabLCount(oldlist) --;
    }

  tl = XmTabListCopy(oldlist, 0, 0);
  XmTabListFree(oldlist);

  _XmProcessUnlock();
  return(tl);
}

XmTab
XmTabCreate(float value,
	    unsigned char units,
	    XmOffsetModel offset_model,
	    unsigned char alignment,
	    char *decimal)
{
  XmTab 	tab;

  _XmProcessLock();
  tab = (XmTab)XtMalloc(sizeof(_XmTabRec));

  _XmTabMark(tab) = FALSE;
  if (value >= 0)
    {
      _XmTabValue(tab) = value;
    }
  else
    {
      _XmTabValue(tab) = 0.0;
      XmeWarning(NULL, NEGATIVE_VALUE_MSG);
    }
  _XmTabUnits(tab) = units;
  _XmTabModel(tab) = offset_model;
  _XmTabAlign(tab) = alignment;
  _XmTabDecimal(tab) = XtNewString(decimal);

  _XmProcessUnlock();
  return(tab);
}

Widget
_XmCreateTab(Widget parent,
	     String name,	/* unused */
	     ArgList arglist,
	     Cardinal argcount)
{
  static XrmQuark quarks[] = {0, 0, 0, 0, 0};

  XmTabList	tl = (XmTabList)parent;
  XrmQuark	qarg;
  float	        value = 0.0;
  unsigned char	units = XmPIXELS;
  XmOffsetModel	model = XmABSOLUTE;
  unsigned char	alignment = XmALIGNMENT_BEGINNING;
  char 		*decimal = ".";
  XmTab		tab, start;
  int		i;

  /* Init quark list */
  if (quarks[0] == 0)
    {
      quarks[0] = XrmPermStringToQuark(XmNtabValue);
      quarks[1] = XrmPermStringToQuark(XmNunitType);
      quarks[2] = XrmPermStringToQuark(XmNoffsetModel);
      quarks[3] = XrmPermStringToQuark(XmNalignment);
      quarks[4] = XrmPermStringToQuark(XmNdecimal);
    }

  /* Get arguments from arglist */
  for (i = 0; i < argcount; i++)
    {
      qarg = XrmStringToQuark(arglist[i].name);

      if (qarg == quarks[0])
	value = (float)arglist[i].value;
      else if (qarg == quarks[1])
	units = (unsigned char)arglist[i].value;
      else if (qarg == quarks[2])
	model = (XmOffsetModel)arglist[i].value;
      else if (qarg == quarks[3])
	alignment = (unsigned char)arglist[i].value;
      else if (qarg == quarks[4])
	decimal = (char *)arglist[i].value;
    }

  tab = XmTabCreate(value, units, model, alignment, decimal);

  if (_XmTabLCount(tl) == 0)
    {
      _XmTabLStart(tl) = tab;
      _XmTabPrev(tab) = tab;
      _XmTabNext(tab) = tab;
    }
  else
    {
      start = _XmTabLStart(tl);
      _XmTabNext(tab) = start;
      _XmTabPrev(tab) = _XmTabPrev(start);
      _XmTabNext(_XmTabPrev(start)) = tab;
      _XmTabPrev(start) = tab;
    }

  _XmTabLCount(tl)++;

  return((Widget)NULL);
}

void
XmTabFree(XmTab tab)
{
  if (tab == NULL) return;

  XtFree(_XmTabDecimal(tab));
  XtFree((char *)tab);
}

float
XmTabGetValues(XmTab tab,
	       unsigned char *units,
	       XmOffsetModel *offset,
	       unsigned char *alignment,
	       char **decimal)
{
  float ret_val;
  _XmProcessLock();
  if (units != NULL) *units = _XmTabUnits(tab);
  if (offset != NULL) *offset = _XmTabModel(tab);
  if (alignment != NULL) *alignment = _XmTabAlign(tab);
  if (decimal != NULL) *decimal = _XmTabDecimal(tab);

  ret_val = _XmTabValue(tab);
  _XmProcessUnlock();
  return ret_val;
}

void
XmTabSetValue(XmTab tab,
	      float value)
{
  _XmProcessLock();
  if (value >= 0) _XmTabValue(tab) = value;
  else XmeWarning(NULL, NEGATIVE_VALUE_MSG);
  _XmProcessUnlock();
}

XmTab
_XmTabCopy(XmTab tab)
{
  XmTab	new_tab;

  new_tab = (XmTab)XtMalloc(sizeof(_XmTabRec));

  memcpy((char *)new_tab, (char *)tab, sizeof(_XmTabRec));

  _XmTabMark(new_tab) = FALSE;
  _XmTabDecimal(new_tab) = XtNewString(_XmTabDecimal(tab));

  return(new_tab);
}

/***********
 * _XmTabListGetPosition
 * returns the x pixel coordinate of the specified tab.
 **********/
Position
_XmTabListGetPosition(
	Screen * screen,
        XmTabList tab_list,
        unsigned char unit_type,
        Cardinal tab_position)
{
    XmTab tab ;
    Position xpos = 0 ;
    unsigned char units;
    XmOffsetModel offset;

    tab = XmTabListGetTab(tab_list, tab_position);

    if (tab) {
	xpos = (Position) XmTabGetValues(tab,
					 &units,
					 &offset,
					 NULL,
					 NULL);
	xpos = _XmConvertUnits(screen, XmHORIZONTAL, units, xpos, unit_type);
	/* a little bit of recursivity here */
	if ((offset == XmRELATIVE) && tab_position){
	    xpos += _XmTabListGetPosition(screen, tab_list, unit_type,
					  tab_position-1);
	}
	XmTabFree(tab) ;
    }

    return xpos ;
}

#ifdef _XmDEBUG_XMTABLIST

static char *
units_image(XtEnum units)
{
  static char buf[100];

  switch (units)
    {
    case XmPIXELS:
      return "px";
    case Xm100TH_MILLIMETERS:
      return "100mm";
    case Xm1000TH_INCHES:
      return "1000in";
    case Xm100TH_POINTS:
      return "100pt";
    case Xm100TH_FONT_UNITS:
      return "100fu";
    case XmINCHES:
      return "in";
    case XmCENTIMETERS:
      return "cm";
    case XmMILLIMETERS:
      return "mm";
    case XmPOINTS:
      return "pt";
    case XmFONT_UNITS:
      return "fu";
    default:
      sprintf(buf, "<Unknown units %d>", units);
      return buf;
    }
}

static char *
model_image(XmOffsetModel model)
{
  static char buf[100];

  switch (model)
    {
    case XmABSOLUTE:
      return "abs.";
    case XmRELATIVE:
      return "rel.";
    default:
      sprintf(buf, "<Unknown model %d>", model);
      return buf;
    }
}

static char *
alignment_image(XtEnum alignment)
{
  static char buf[100];

  switch (alignment)
    {
    case XmALIGNMENT_BEGINNING:
      return "beginning";
    case XmALIGNMENT_CENTER:
      return "center";
    case XmALIGNMENT_END:
      return "end";
    default:
      sprintf(buf, "<Unknown alignment %d>", alignment);
      return buf;
    }
}

void
_Xm_dump_tab(XmTab tab)
{
  unsigned int mark = _XmTabMark(tab);
  /* unsigned int ref_count = ((_XmTab)tab)->ref_count; */
  float value = _XmTabValue(tab);
  unsigned char units = _XmTabUnits(tab);
  XmOffsetModel model = _XmTabModel(tab);
  unsigned char alignment = _XmTabAlign(tab);
  char *decimal = _XmTabDecimal(tab);
  XmTab next = _XmTabNext(tab);
  XmTab prev = _XmTabPrev(tab);

  printf ("%p: %f %s, %s from %s '%s', %p %p, %d\n",
	  tab, value, units_image(units),
	  model_image(model), alignment_image(alignment),
	  decimal, next, prev, mark);
}

void
_Xm_dump_tablist(XmTabList list)
{
  int i;
  int count = _XmTabLCount(list);
  XmTab tab = _XmTabLStart(list);

  printf("(XmTabList)%p: count %d, start %p.\n", list, count, tab);
  for (i = 0; i < count; i++)
    {
      printf ("  #%d> ", i);
      _Xm_dump_tab(tab);
      tab = _XmTabNext(tab);
    }
}

#endif /* _XmDEBUG_XMTABLIST */
