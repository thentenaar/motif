/* $TOG: ValTime.c /main/6 1997/06/18 17:46:26 samborn $ */
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

/**********************************************************************
 *
 *  _XmValidTimestamp
 *      Returns a valid timestamp by generating an event.  The time
 *      that is returned is not 0 or CurrentTime.
 *      Arguments: w - any widget on the display.
 *
 **********************************************************************/

Time 
_XmValidTimestamp(
     Widget w )
{
  Window win;
  Display *dsp = XtDisplay(w);
  XEvent event;
  EventMask shellMask;
  Atom timeProp = XInternAtom(dsp, XmI_MOTIF_CURRENT_TIME, False);

  while (!XtIsShell(w)) w = XtParent(w);
  win = XtWindow(w);
 
  if (! ((shellMask = XtBuildEventMask(w)) & PropertyChangeMask) )
    XSelectInput(dsp, win, shellMask | PropertyChangeMask);

  XChangeProperty(dsp, win, timeProp, timeProp,
		  8, PropModeAppend, NULL, 0);

  XWindowEvent(dsp, win, PropertyChangeMask, &event);
  if (!(shellMask & PropertyChangeMask))
    XSelectInput(dsp, win, shellMask);

  return (event.xproperty.time);
}

