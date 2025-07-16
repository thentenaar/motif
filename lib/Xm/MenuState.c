/* $XConsortium: MenuState.c /main/5 1995/07/15 20:52:55 drk $ */
/*
 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
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
#include <Xm/RowColumnP.h>
#include <Xm/ScreenP.h>
#include "MenuStateI.h"

/********    Static Function Declarations    ********/

static void ScreenDestroyCallback (
			Widget w,
			XtPointer client_data,
			XtPointer call_data );

/********    End Static Function Declarations    ********/


Widget
_XmGetRC_PopupPosted (
       Widget wid)
{
   if (XmIsRowColumn(wid))
      return (RC_PopupPosted(wid));
   else
      return NULL;
}

/*
 * The following two functions are used by menu and menu-item widgets to keep
 * track of whether we're in drag (button down) or traversal mode.
 */
Boolean
_XmGetInDragMode (
        Widget widget)
{
  return((_XmGetMenuState(widget))->MU_InDragMode);
}

void
_XmSetInDragMode(
        Widget widget,
        Boolean mode )
{
  (_XmGetMenuState(widget))->MU_InDragMode = mode;
}


/************************************************************************
 *
 * _XmGetMenuState(wid)
 *
 ************************************************************************/
XmMenuState
_XmGetMenuState(
        Widget wid)
{
   XmScreen scrn = (XmScreen) XmGetXmScreen(XtScreen(wid));
   XmMenuState menu_state = (XmMenuState)NULL;

   if ((XmScreen)NULL != scrn)
   {
     menu_state  =
	(XmMenuState)((XmScreenInfo *)(scrn->screen.screenInfo))->menu_state;

     if ((XmMenuState)NULL == menu_state)
     {
      menu_state = (XmMenuState)XtMalloc(sizeof(XmMenuStateRec));
      ((XmScreenInfo *)(scrn->screen.screenInfo))->menu_state =
		(XtPointer)menu_state;
      XtAddCallback((Widget)scrn, XtNdestroyCallback,
		    ScreenDestroyCallback, (XtPointer) NULL);

      menu_state->RC_LastSelectToplevel = NULL;
      menu_state->RC_ButtonEventStatus.time = (unsigned) -1;
      menu_state->RC_ButtonEventStatus.verified = FALSE;
      menu_state->RC_ButtonEventStatus.waiting_to_be_managed = TRUE;
      /*menu_state->RC_ButtonEventStatus.event = (XButtonEvent)NULL;*/
      menu_state->RC_ReplayInfo.time = 0;
      menu_state->RC_ReplayInfo.toplevel_menu = NULL;
      menu_state->RC_activeItem = NULL;
      menu_state->RC_allowAcceleratedInsensitiveUnmanagedMenuItems = False;
      menu_state->RC_menuFocus.oldFocus = (Window)NULL;
      menu_state->RC_menuFocus.oldRevert = 0;
      menu_state->RC_menuFocus.oldWidget = NULL;

      menu_state->MS_LastManagedMenuTime = (Time)0L;

      menu_state->MU_InDragMode = False;
      menu_state->MU_CurrentMenuChild = NULL;
      menu_state->MU_InPMMode = False;
     }
   }

   return menu_state;
}

static void
ScreenDestroyCallback
	( Widget w,
        XtPointer client_data,
        XtPointer call_data )	/* unused */
{
   XmScreen scrn = (XmScreen) XmGetXmScreen(XtScreen(w));
   if ((XmScreen)NULL != scrn) {
       XmMenuState menu_state =
	   (XmMenuState)((XmScreenInfo *)
			 (scrn->screen.screenInfo))->menu_state;
	if ((XmMenuState)NULL != menu_state) {
	    XtFree((char*)menu_state);
   	}
   }
}
