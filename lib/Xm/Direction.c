/* $XConsortium: Direction.c /main/6 1996/03/28 15:14:33 daniel $ */
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
 * 
 */
/*
 * HISTORY
 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include "XmI.h"
#include <Xm/ManagerP.h>
#include <Xm/GadgetP.h>
#include <Xm/PrimitiveP.h>
#include <Xm/TraitP.h>
#include <Xm/LayoutT.h>

/*********************************************************************
 *
 * _XmDirectionDefault
 *    This procedure provides the dynamic resource default for
 *    the layout direction resource dependent on the parent's value.
 *
 *********************************************************************/
/*ARGSUSED*/
void 
_XmDirectionDefault(Widget widget,
		    int offset, /* unused */
		    XrmValue *value )
{
  static XmDirection direction;
  
  value->addr = (XPointer) &direction;

  /* This is an ugly hack, but what to do when user sets stringDirection
     in resource file. Dependent on that stringDirection comes before
     layoutDirection in resource list. Part of the reason is that this is
     the same field. Yuck! */
  if (XmIsManager(widget) && 
      (((XmManagerWidget)widget)->manager.string_direction !=
       XmSTRING_DIRECTION_DEFAULT))
    direction =
      XmStringDirectionToDirection(((XmManagerWidget)widget)->manager.
				   string_direction);
  else
    direction = _XmGetLayoutDirection(XtParent(widget));
}

/*ARGSUSED*/
void 
_XmFromLayoutDirection(
        Widget wid,
        int resource_offset,	/* unused */
        XtArgVal *value )
{   
  if (XmIsManager(wid))
    *value = (XtArgVal)XmDirectionToStringDirection((XmDirection)*value);
  else if (XmIsLabel(wid) || XmIsLabelGadget(wid) || XmIsList(wid))
    *value = 
      (XtArgVal)XmDirectionToStringDirection(_XmGetLayoutDirection(wid));
}

/*ARGSUSED*/
XmImportOperator 
_XmToLayoutDirection(
        Widget wid,
        int resource_offset,	/* unused */
        XtArgVal *value )
{   
  if (XmIsManager(wid)) {
    *value = (XtArgVal)XmStringDirectionToDirection((XmStringDirection)*value);
    return XmSYNTHETIC_LOAD;
  } else if (XmIsLabel(wid) || XmIsList(wid)) {
    XmPrim_layout_direction(((XmPrimitiveWidget)wid)) = 
      XmStringDirectionToDirection((XmStringDirection)*value);
    return XmSYNTHETIC_NONE;
  } else if (XmIsLabelGadget(wid)) {
    ((XmGadget)wid)->gadget.layout_direction = 
      XmStringDirectionToDirection((XmStringDirection)*value);
    return XmSYNTHETIC_NONE;
  }
  return XmSYNTHETIC_LOAD;
}





XmStringDirection
XmDirectionToStringDirection(XmDirection dir)
{
  if (XmDirectionMatch(dir, XmLEFT_TO_RIGHT))
    return XmSTRING_DIRECTION_L_TO_R;
  else if (XmDirectionMatch(dir, XmRIGHT_TO_LEFT))
    return XmSTRING_DIRECTION_R_TO_L;
  return XmSTRING_DIRECTION_DEFAULT;
}

XmDirection
XmStringDirectionToDirection(XmStringDirection dir)
{
  switch (dir) {
  case XmSTRING_DIRECTION_L_TO_R:
    return XmLEFT_TO_RIGHT;
  case XmSTRING_DIRECTION_R_TO_L:
    return XmRIGHT_TO_LEFT;
  default:
    return XmDEFAULT_DIRECTION;
  }
}


XmDirection
_XmGetLayoutDirection( Widget w )
{
  XmSpecifyLayoutDirectionTrait layoutT = NULL;

  while (w && !(layoutT = (XmSpecifyLayoutDirectionTrait)
		XmeTraitGet((XtPointer) XtClass(w),
			    XmQTspecifyLayoutDirection)))
    w = XtParent(w);

  if (w && layoutT && layoutT->get_direction)
    return layoutT->get_direction(w);
  else
    return XmLEFT_TO_RIGHT;
} 


/* Handle compatibility with XmStringDirection */
#define Fixdir(d) (((d) <= 1) ? (~((d)+1)) : ((d) | XmDIRECTION_IGNORED))

Boolean 
XmDirectionMatch(XmDirection d1,
		 XmDirection d2)
{
  d1 = Fixdir(d1);
  d2 = Fixdir(d2);
  return (((XmDirection)(d1 & d2) == (XmDirection)d1) ||
	  ((XmDirection)(d1 & d2) == (XmDirection)d2));
}

Boolean 
XmDirectionMatchPartial(XmDirection d1,
		 XmDirection d2,
		 XmDirection dmask)
{
  return XmDirectionMatch(d1 & dmask, d2 & dmask);
}

