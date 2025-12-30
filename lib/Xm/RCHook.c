/* $XConsortium: RCHook.c /main/10 1996/12/16 18:32:29 drk $ */
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "XmI.h"
#include "ColorObjI.h"
#include "RCHookI.h"

/**********************************************************************/
/** RCHook                                                           **/
/**    With new "per-display" color objects, we need to check to see **/
/**    if this ColorObj is using the color server before we set up   **/
/**    any special colors.                                           **/
/**                                                                  **/
/**********************************************************************/

void _XmRCColorHook(Widget w, ArgList alIn, Cardinal *acPtrIn)
{
    Arg al[10];
    int ac;
    unsigned char rcType;
    static int mono, color, colorPrim, init=0;
    static Screen *screen;
    Pixmap  ditherPix, solidPix;
    XmColorObj tmpColorObj=NULL;
    Pixel  defaultBackground;
    int depth = w->core.depth ;
    Display *ColorObjCacheDisplay;
    XContext ColorObjCache;
    XmColorObj DefaultColorObj;

    (void)alIn;
    (void)acPtrIn;
    _XmProcessLock();
    ColorObjCacheDisplay = _XmColorObjCacheDisplay;
    ColorObjCache = _XmColorObjCache;
    DefaultColorObj = _XmDefaultColorObj;
    _XmProcessUnlock();

    /** get the colorObj for this display connection **/
    if (XFindContext(ColorObjCacheDisplay, (XID)XtDisplay(w),
		     ColorObjCache, (XPointer *)&tmpColorObj) != 0)
    {   /* none found, use default */
	if (DefaultColorObj)
	    tmpColorObj = DefaultColorObj;
	else  /* this should NEVER happen... RowColInitHook won't get called */
	    return;
    }

    /** don't set colors if this display isn't using the color server **/
    if (!tmpColorObj->color_obj.colorIsRunning)	return;

    ac = 0;
    XtSetArg (al[ac], XmNrowColumnType, &rcType);                ac++;
    XtSetArg (al[ac], XmNbackground, &defaultBackground);        ac++;
    XtGetValues (w, al, ac);

    if (rcType == XmMENU_BAR)  /* set to secondary, rather than primary */
    {
	_XmProcessLock();
        if (!init)
        {
            if (tmpColorObj->color_obj.
		colorUse[tmpColorObj->color_obj.myScreen] == XmCO_BLACK_WHITE)
                mono = 1;
            else
                mono = 0;

            color = tmpColorObj->color_obj.secondary;
            colorPrim = tmpColorObj->color_obj.primary;
            screen = XtScreen(tmpColorObj);
            init = 1;
        }
	_XmProcessUnlock();

	/** if background didn't default to ColorObj,
	  don't overwrite colors **/
	if (defaultBackground != tmpColorObj->color_obj.myColors[colorPrim].bg)
	    return;

        ac = 0;
        XtSetArg (al[ac], XmNbackground,
                  tmpColorObj->color_obj.myColors[color].bg);        ac++;
        XtSetArg (al[ac], XmNforeground,
                  tmpColorObj->color_obj.myColors[color].fg);        ac++;
        XtSetArg (al[ac], XmNtopShadowColor,
                  tmpColorObj->color_obj.myColors[color].ts);        ac++;
        XtSetArg (al[ac], XmNbottomShadowColor,
                  tmpColorObj->color_obj.myColors[color].bs);        ac++;

        /** put dithers for top shadows if needed **/
        if (XmCO_DitherTopShadow(tmpColorObj->color_obj.display,
				 tmpColorObj->color_obj.myScreen,
				 &tmpColorObj->color_obj.myColors[color]))
        {
            if (mono)
                ditherPix = XmGetPixmapByDepth (screen, XmS50_foreground,
                                         BlackPixelOfScreen(screen),
                                         WhitePixelOfScreen(screen),
					 depth);
            else
                ditherPix = XmGetPixmapByDepth (screen, XmS50_foreground,
                                    tmpColorObj->color_obj.myColors[color].bg,
                                         WhitePixelOfScreen(screen),
					 depth);

            XtSetArg (al[ac], XmNtopShadowPixmap, ditherPix);         ac++;
        }
        else      /** see if we need to "undo" primary dither **/
        if (XmCO_DitherTopShadow(tmpColorObj->color_obj.display,
				 tmpColorObj->color_obj.myScreen,
				 &tmpColorObj->color_obj.myColors[colorPrim]))
        {   /* simulate solid white (will happen for B_W case only)*/
            solidPix = XmGetPixmapByDepth (screen, "background",
					   WhitePixelOfScreen(screen),
					   WhitePixelOfScreen(screen),
					   depth);
            XtSetArg (al[ac], XmNtopShadowPixmap, solidPix);          ac++;
        }

        /** put dithers for bottom shadows if needed **/
        if (XmCO_DitherBottomShadow(tmpColorObj->color_obj.display,
				    tmpColorObj->color_obj.myScreen,
				    &tmpColorObj->color_obj.myColors[color]))
        {
            if (mono)
                ditherPix = XmGetPixmapByDepth (screen, XmS50_foreground,
						BlackPixelOfScreen(screen),
						WhitePixelOfScreen(screen),
						depth);
            else
                ditherPix = XmGetPixmapByDepth (screen, XmS50_foreground,
                                   tmpColorObj->color_obj.myColors[color].bg,
						BlackPixelOfScreen(screen),
						depth);
            XtSetArg (al[ac], XmNbottomShadowPixmap, ditherPix);      ac++;
        }
        else      /** see if we need to "undo" primary dither **/
        if (XmCO_DitherBottomShadow(tmpColorObj->color_obj.display,
				    tmpColorObj->color_obj.myScreen,
				    &tmpColorObj->color_obj.myColors[colorPrim]))
        {   /* simulate solid black (will happen for B_W case only)*/
            solidPix = XmGetPixmapByDepth (screen, "background",
					   BlackPixelOfScreen(screen),
					   BlackPixelOfScreen(screen),
					   depth);
            XtSetArg (al[ac], XmNbottomShadowPixmap, solidPix);       ac++;
        }

        XtSetValues (w, al, ac);
    }
}

