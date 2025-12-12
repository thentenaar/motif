/* $TOG: CareVisualT.c /main/7 1999/01/20 18:20:55 mgreess $ */
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
#include <Xm/TraitP.h>
#include <Xm/CareVisualT.h>
#include "CareVisualTI.h"

/************************************************************************
 *
 *  _XmNotifyChildrenVisual
 *	Loop through the child set of new and for any widget/gadget that has
 *	a CareParentVisual trait, call the redraw proc.
 *      The redraw proc will return True if the widget needs to have the
 *	parent redrawn as well.
 *
 ************************************************************************/
Boolean 
_XmNotifyChildrenVisual(
        Widget cur,
        Widget new_w,
        Mask visual_flag)
{
   register int i;
   Widget child;
   Boolean redisplay = False;
   XmCareVisualTrait care_visual ;
   CompositeWidget cw = (CompositeWidget) new_w ;

   for (i = 0; i < cw->composite.num_children; i++) {
       child = cw->composite.children[i];

       if ((care_visual = (XmCareVisualTrait)
	    XmeTraitGet((XtPointer) XtClass(child),
			XmQTcareParentVisual)) != NULL)

	   redisplay |= care_visual->redraw(child, cur, new_w, visual_flag) ;

   }

   return (redisplay);
}

