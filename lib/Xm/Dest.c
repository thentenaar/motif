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
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: Dest.c /main/12 1995/09/19 23:00:44 cde-sun $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <Xm/XmP.h>
#include <Xm/DisplayP.h>
#include "XmI.h"
#include "DestI.h"

/* 
   This function is used for setting the "last editable widget on which a
   select, edit, insert, or paste operation was performed and is a destination
   for quick paste and certain clipboard functions" (for this display 
   connection) and is not necessarily a text widget.  
   Under current usage by Motif internals:
   This function is for squirreling away the widget that has the destination
   cursor so that it can be retrieved when pasting from a menu.  Called by 
   _XmTextSetDestinationSelection.
*/
void _XmSetDestination (Display *dpy, Widget w)
{
      XmDisplay   dd = (XmDisplay) XmGetXmDisplay(dpy);	/* w may be NULL */
      if ((XmDisplay)NULL != dd)
	((XmDisplayInfo *)(dd->display.displayInfo))->destinationWidget =
		w;	
}

/* This public function retrieves the widget saved by _XmSetDestination. */
Widget XmGetDestination (Display *display)
{
      XmDisplay   dd = (XmDisplay) XmGetXmDisplay(display);
      Widget w = (Widget)NULL;
      _XmDisplayToAppContext(display);

      _XmAppLock(app);
      if ((XmDisplay)NULL != dd)
	 w = ((XmDisplayInfo *)(dd->display.displayInfo))->destinationWidget;
      _XmAppUnlock(app);
      return w;
}
