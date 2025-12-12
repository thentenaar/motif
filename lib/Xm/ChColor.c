/* $XConsortium: ChColor.c /main/6 1995/10/25 19:55:45 cde-sun $ */
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
#include <Xm/ManagerP.h>
#include <Xm/PrimitiveP.h>
#include <Xm/GadgetP.h>
#include "XmI.h"

/*********************************************************************
 *
 *  XmChangeColor - change set of colors for existing widget, given 
 *                  background color
 *
 *********************************************************************/
void 
XmChangeColor(
	      Widget w,
	      Pixel background )

{
  Widget windowed_ancestor = w;
  Pixel foreground_ret;
  Pixel topshadow_ret;
  Pixel bottomshadow_ret;
  Pixel select_ret; 
  Pixel use_background = background;
  Pixel gadget_background;
  Arg args[5];

  /*
   * If the gadget is in BC mode (for colors), it must follow its parent's
   * color scheme.  We therefore ignore the background pixel that was
   * passed in.
   */

  _XmWidgetToAppContext(w);
  _XmAppLock(app);

  if (XmIsGadget(w))
    {
      Widget parent = XtParent(w);

      windowed_ancestor = parent;
      XtSetArg(args[0], XmNbackground, &gadget_background);
      XtGetValues(w,args,1);
      if (gadget_background == parent->core.background_pixel)
	use_background = parent->core.background_pixel;
    }

  XmGetColors(windowed_ancestor->core.screen, windowed_ancestor->core.colormap,
	      use_background, &foreground_ret, &topshadow_ret,
	      &bottomshadow_ret, NULL );

  if ( (XmIsManager(w)) ||  (XmIsPrimitive(w)) || (XmIsGadget(w)) )
    { 
      XtSetArg (args[0], XmNbackground, (XtArgVal) use_background);
      XtSetArg (args[1], XmNforeground, (XtArgVal) foreground_ret);
      XtSetArg (args[2], XmNtopShadowColor, (XtArgVal) topshadow_ret);
      XtSetArg (args[3], XmNbottomShadowColor, (XtArgVal) bottomshadow_ret);
      XtSetArg (args[4], XmNhighlightColor, (XtArgVal) foreground_ret);
      
      XtSetValues (w, args, 5);
      
      if (XmIsPrimitive(w) || XmIsGadget(w))
	{
	  if ( (XmIsScrollBar(w)) ||
	       (XmIsPushButton(w)) || (XmIsPushButtonGadget(w)) ||
	       (XmIsToggleButton(w)) || (XmIsToggleButtonGadget(w)) )
	    { 
	      XmGetColors( windowed_ancestor->core.screen,
			   windowed_ancestor->core.colormap,
			   background, NULL, NULL, NULL, &select_ret);

	      if (XmIsToggleButton(w) || XmIsToggleButtonGadget(w))
	        {
		  XtSetArg (args[0], XmNselectColor, (XtArgVal) select_ret);
		  XtSetArg (args[1], XmNunselectColor, (XtArgVal) use_background);
	          XtSetValues (w, args, 2);
	        }
		else
		{
	          if (XmIsScrollBar(w))
		    XtSetArg (args[0], XmNtroughColor, (XtArgVal) select_ret);
	          else if (XmIsPushButton(w) || XmIsPushButtonGadget(w))
		    XtSetArg (args[0], XmNarmColor, (XtArgVal) select_ret);
	          XtSetValues (w, args, 1);
	        }
	    }
	}
    }
  _XmAppUnlock(app);
}



