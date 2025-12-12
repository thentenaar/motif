/* $XConsortium: IsMwmRun.c /main/7 1996/05/21 12:02:11 pascale $ */
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


#include <Xm/XmP.h>
#include <Xm/MwmUtil.h>
#include "XmI.h"

/************************************************************************
 *
 *  XmIsMotifWMRunning
 *
 ************************************************************************/
Boolean 
XmIsMotifWMRunning(
	 Widget shell )
{
    Atom	motif_wm_info_atom;
    Atom	actual_type;
    int		actual_format;
    unsigned long num_items, bytes_after;
    PropMotifWmInfo	*prop = 0;
    Window	root = RootWindowOfScreen(XtScreen(shell));

    _XmWidgetToAppContext(shell);
 
    _XmAppLock(app);

    motif_wm_info_atom = XInternAtom(XtDisplay(shell),
				       _XA_MOTIF_WM_INFO,
				       FALSE);
    _XmProcessLock();

    XGetWindowProperty (XtDisplay(shell), 
			 root,
			 motif_wm_info_atom,
			 0, (long)PROP_MOTIF_WM_INFO_ELEMENTS,
			 FALSE, motif_wm_info_atom,
			 &actual_type, &actual_format,
			 &num_items, &bytes_after,
			 (unsigned char **) &prop);
    _XmProcessUnlock();

    if ((actual_type != motif_wm_info_atom) ||
	 (actual_format != 32) ||
	 (num_items < PROP_MOTIF_WM_INFO_ELEMENTS))
      {
	   if (prop != 0) XFree((char *)prop);
           _XmAppUnlock(app);
	   return (FALSE);
      }
    else
      {
	   Window	wm_window = (Window) prop->wmWindow;
	   Window	top, parent, *children;
	   unsigned int	num_children;
	   Boolean	returnVal;
	   Cardinal	i;

	   if (XQueryTree(XtDisplay(shell),
			  root, &top, &parent,
			  &children, &num_children))
	     {
		 i = 0; 
		 while ((i < num_children) && (children[i] != wm_window))
		   i++;
		 returnVal =  (i == num_children) ? FALSE : TRUE;
	     }
	   else
	     returnVal = FALSE;

	   if (prop) XFree((char *)prop);
	   if (children) XFree((char *)children);
           _XmAppUnlock(app);
	   return (returnVal);
      }
}
