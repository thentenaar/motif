/* $XConsortium: ObsoXme.c /main/5 1995/07/15 20:54:09 drk $ */
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
#include "RegionI.h"
#include <Xm/DropSMgr.h>

/****************************_XmDrawShadows****************************/
void _XmDrawShadows(Display *display, Drawable d,
		    GC top_gc, GC bottom_gc,
		    Position x, Position y,
		    Dimension width, Dimension height, Dimension shad_thick,
		    unsigned int shad_type)
{
    XmeDrawShadows(display, d, top_gc, bottom_gc, x, y, width, height,
		   shad_thick, shad_type);
}


/*****************************_XmClearBorder*********************************/
void _XmClearBorder (Display *display, Window w,
                     Position x, Position y,
                     Dimension width, Dimension height,
                     Dimension shadow_thick)
{
    XmeClearBorder (display, w, x, y, width, height, shadow_thick);
}

/******************************_XmDrawSeparator**********************/
void _XmDrawSeparator(Display *display, Drawable d,
		      GC top_gc, GC bottom_gc, GC separator_gc,
                      Position x, Position y,
                      Dimension width, Dimension height,
                      Dimension shadow_thick,
                      Dimension margin, unsigned char orientation,
                      unsigned char separator_type)
{
    XmeDrawSeparator(display, d, top_gc, bottom_gc, separator_gc,
		     x, y, width, height, shadow_thick, margin,
		     orientation, separator_type);
}


/***********************_XmDrawDiamond**********************************/
void _XmDrawDiamond(Display *display, Drawable d,
                    GC top_gc, GC bottom_gc, GC center_gc,
                    Position x, Position y,
                    Dimension width, Dimension height,
                    Dimension shadow_thick, Dimension fill)
{
    XmeDrawDiamond (display, d, top_gc, bottom_gc, center_gc,
		    x, y, width, height, shadow_thick, fill);
}


/****************************_XmDrawSimpleHighlight************************/

void _XmDrawSimpleHighlight(Display *display, Drawable d,
		      GC gc,
		      Position x, Position y,
		      Dimension width, Dimension height,
		      Dimension highlight_thickness)
{
    XmeDrawHighlight(display, d, gc, x, y, width, height,
		     highlight_thickness);
}

/****************************_XmDrawArrow**********************************/
void _XmDrawArrow(Display *display, Drawable d,
                  GC top_gc, GC bot_gc, GC cent_gc,
                  Position x, Position y,
                  Dimension width, Dimension height, Dimension shadow_thick,
                  unsigned char direction)
{
    XmeDrawArrow(display, d, top_gc, bot_gc, cent_gc,
		 x, y, width, height, shadow_thick, direction);
}



/**********************************************************************/

void
_XmFromHorizontalPixels(
        Widget widget,
        int offset,
        XtArgVal *value )
{
    XmeFromHorizontalPixels(widget, offset, value);
}

/**********************************************************************/

void
_XmFromVerticalPixels(
        Widget widget,
        int offset,
        XtArgVal *value )
{
    XmeFromVerticalPixels(widget, offset, value);
}

/**********************************************************************/

XmImportOperator
_XmToHorizontalPixels(
        Widget widget,
        int offset,
        XtArgVal *value )
{
    return XmeToHorizontalPixels( widget, offset, value );
}

/********************************************************************/

XmImportOperator
_XmToVerticalPixels(
        Widget widget,
        int offset,
        XtArgVal *value )
{
    return XmeToVerticalPixels( widget, offset, value );
}

/********************************************************************/

void
_XmNavigChangeManaged(
        Widget wid )
{
    XmeNavigChangeManaged(wid);
}

/********************************************************************/

void
_XmRedisplayGadgets(
        Widget w,
        register XEvent *event,
        Region region )
{
    XmeRedisplayGadgets( w, event, region );
}

/********************************************************************/

Widget
_XmGetTextualDragIcon(
        Widget w )
{
    return XmeGetTextualDragIcon(w);
}


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

/********************************************************************/

Cursor
_XmGetNullCursor(
        Widget w )
{
    return XmeGetNullCursor(w);
}

/********************************************************************/

void
_XmWarning(
        Widget w,
        char *message )
{
    XmeWarning( w, message );
}


/********************************************************************/

XmFontList
_XmGetDefaultFontList(
        Widget w,
        unsigned char fontListType )
{
    return XmeGetDefaultRenderTable( w, fontListType );
}

/********************************************************************/

Boolean
_XmFontListGetDefaultFont(
        XmFontList fontlist,
        XFontStruct **font_struct )
{
    return XmeRenderTableGetDefaultFont(fontlist, font_struct);
}

/********************************************************************/

XtGeometryResult
_XmGMReplyToQueryGeometry(Widget widget,
			  XtWidgetGeometry * intended,
			  XtWidgetGeometry * desired)
{
    return XmeReplyToQueryGeometry(widget, intended, desired);
}

/********************************************************************/

Boolean
_XmStringsAreEqual(
        register char *in_str,
        register char *test_str )
{
    return XmeNamesAreEqual( in_str, test_str );
}

/********************************************************************/

void
_XmGetMaxCursorSize(
        Widget w,
        Dimension *width,
        Dimension *height )
{
    XmeQueryBestCursorSize(w, width, height);
}

/********************************************************************/

Boolean
_XmStringIsXmString(
        XmString string )
{
    return XmeStringIsValid(string);
}

/********************************************************************/

XmGadget
_XmInputInGadget(
        Widget wid,
        register int x,
        register int y )
{
    return (XmGadget) XmObjectAtPoint( wid, x, y );
}

/********************************************************************/

XmString
_XmOSGetLocalizedString(
        char *reserved,
        Widget widget,
        char *resource,
        String string)
{
    return XmeGetLocalizedString( reserved, widget, resource, string);
}

/********************************************************************/

int
_XmMicroSleep(
        long    usecs)
{
    return XmeMicroSleep( usecs );
}

/********************************************************************/

String
_XmOSGetHomeDirName()
{
    return XmeGetHomeDirName();
}


/********************************************************************/

void
_XmStringUpdateWMShellTitle(
	XmString xmstr,
	Widget shell)
{
    XmeSetWMShellTitle(xmstr, shell);
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


/************************************************************************
 *
 *  _XmConfigureObject
 *	Change the dimensional aspects of a widget or gadget.
 *
 ************************************************************************/
void
_XmConfigureObject(
        Widget wid,
        Position x,
        Position y,
        Dimension width,
        Dimension height,
        Dimension border_width )
{
    XmeConfigureObject( wid, x, y, width, height, border_width );
}
