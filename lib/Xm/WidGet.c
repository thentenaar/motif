/* $XConsortium: WidGet.c /main/6 1995/10/25 20:27:58 cde-sun $ */
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
#include <Xm/PrimitiveP.h>
#include <Xm/ManagerP.h>
#include <Xm/GadgetP.h>



/************************************************************************
 *
 *   XmWidgetGetBaselines
 *
 ************************************************************************/

Boolean
XmWidgetGetBaselines(
        Widget wid,
        Dimension **baselines,
        int *line_count)
{
  _XmWidgetToAppContext(wid);
  _XmAppLock(app);

  if (XmIsPrimitive(wid))
      {
	  XmPrimitiveClassExt              *wcePtr;
	  WidgetClass   wc = XtClass(wid);
	  
	  wcePtr = _XmGetPrimitiveClassExtPtr(wc, NULLQUARK);
	  
	  if (*wcePtr && (*wcePtr)->widget_baseline)
	      {
		_XmAppUnlock(app);
		return( (*((*wcePtr)->widget_baseline)) 
			 (wid, baselines, line_count)) ;
	      } 
      }
  else if (XmIsGadget(wid))
      {
	  XmGadgetClassExt              *wcePtr;
	  WidgetClass   wc = XtClass(wid);
	  
	  wcePtr = _XmGetGadgetClassExtPtr(wc, NULLQUARK);
	  
	  if (*wcePtr && (*wcePtr)->widget_baseline)
	      {
		_XmAppUnlock(app);
		return( (*((*wcePtr)->widget_baseline)) 
			 (wid, baselines, line_count)) ;
	      }
      }
  _XmAppUnlock(app);
  return (False);
}


/************************************************************************
 *
 *   XmWidgetDisplayRect
 *
 ************************************************************************/

Boolean
XmWidgetGetDisplayRect(
        Widget wid,
        XRectangle *displayrect)
{
    _XmWidgetToAppContext(wid);
    _XmAppLock(app);

    if (XmIsPrimitive(wid))
	{
	    XmPrimitiveClassExt              *wcePtr;
	    WidgetClass   wc = XtClass(wid);
	    
	    wcePtr = _XmGetPrimitiveClassExtPtr(wc, NULLQUARK);
	    
	    if (*wcePtr && (*wcePtr)->widget_display_rect)
		(*((*wcePtr)->widget_display_rect)) (wid, displayrect);
	    _XmAppUnlock(app);
	    return (True);
	}
    else if (XmIsGadget(wid))
	{
	    XmGadgetClassExt              *wcePtr;
	    WidgetClass   wc = XtClass(wid);
	    
	    wcePtr = _XmGetGadgetClassExtPtr(wc, NULLQUARK);
	    
	    if (*wcePtr && (*wcePtr)->widget_display_rect)
		(*((*wcePtr)->widget_display_rect)) (wid, displayrect);
	    _XmAppUnlock(app);
	    return (True);
	}
    else {
	_XmAppUnlock(app);
	return (False);
    }
}
