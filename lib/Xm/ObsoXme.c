/* $XConsortium: ObsoXme.c /main/5 1995/07/15 20:54:09 drk $ */
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

/*
** This file contains the _Xm routines replaced by Xme functions
** in 2.0. We do not want to mix them with other _Xm in Obso2_0.c
** since there is a higher probability that these have been used
** by programs (that's why they are Xme now)
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define NO_XM_1_2_BC
#include <Xm/DrawP.h>
#include <Xm/XmP.h>
#include <Xm/XmosP.h>
#include <Xm/DropSMgr.h>

/********************************************************************/

void
_XmVirtualToActualKeysym(
        Display *dpy,
        KeySym virtKeysym,
        KeySym *actualKeysymRtn,
        Modifiers *modifiersRtn )
{
  int num_keys;
  XmKeyBinding keys;

  /* Initialize the return parameters. */
  *actualKeysymRtn = NoSymbol;
  *modifiersRtn = 0;

  /* Arbitrarily return the first keysym in the list. */
  num_keys = XmeVirtualToActualKeysyms(dpy, virtKeysym, &keys);
  if (num_keys > 0)
    {
      *actualKeysymRtn = keys[0].keysym;
      *modifiersRtn = keys[0].modifiers;
    }
  XtFree((char*) keys);
}

/************************************************************************
 *
 *  _XmResizeObject
 *	Change the width or height of a widget or gadget.
 *
 ************************************************************************/
void
_XmResizeObject(
        Widget wid,
        Dimension width,
        Dimension height,
        Dimension border_width )
{
    RectObj g = (RectObj) wid ;
    XmDropSiteStartUpdate(wid);
    if (XtIsWidget (g))
	XtResizeWidget ((Widget) g, width, height, border_width);
    else
	XmeConfigureObject((Widget) g, g->rectangle.x, g->rectangle.y,
			   width, height, 0);
    XmDropSiteEndUpdate(wid);
}

/************************************************************************
 *
 *  _XmMoveObject
 *	Change the origin of a widget or gadget.
 *
 ************************************************************************/
void
_XmMoveObject(
        Widget wid,
        Position x,
        Position y )
{
    RectObj g = (RectObj) wid ;

    XmDropSiteStartUpdate(wid);
    if (XtIsWidget (g))
	XtMoveWidget ((Widget) g, x, y);
    else
	XmeConfigureObject((Widget) g, x, y,
			   g->rectangle.width, g->rectangle.height, 0);
    XmDropSiteEndUpdate(wid);
}

