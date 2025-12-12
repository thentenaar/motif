/* $XConsortium: ColObjFunc.c /main/7 1995/10/25 19:56:10 cde-sun $ */
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
#include "ColorObjI.h"


/**********************************************************************/
/** XmeUseColorObj()                                                 **/
/**           Return False if color is not working for some reason.  **/
/**                                                                  **/
/**           Could be due to useColorObj resource == False, or any  **/
/**           problem with the color server or color object.         **/
/**                                                                  **/
/**********************************************************************/
Boolean 
XmeUseColorObj( void )
{
    XmColorObj tmpColorObj = _XmDefaultColorObj;

    _XmProcessLock();
    if (!tmpColorObj ||
      !tmpColorObj->color_obj.colorIsRunning ||
      !tmpColorObj->color_obj.useColorObj) {
      _XmProcessUnlock();
      return False;
    }
    else {
      _XmProcessUnlock();
      return True;
    }
}



/**********************************************************************/
/** Following entries kept for bc with CDE (they'll be moved to
 **       obsolete module later                                      **/
/**                                                                  **/
/**********************************************************************/
Boolean 
_XmGetPixelData(
        int screen,
        int *colorUse,
        XmPixelSet *pixelSet,
        short *a,
        short *i,
        short *p,
        short *s )
{
    return XmeGetPixelData( screen, colorUse, pixelSet, a, i, p, s );
}

Boolean 
_XmGetIconControlInfo(
        Screen  *screen,
	Boolean *useMaskRtn,
        Boolean *useMultiColorIconsRtn,
        Boolean *useIconFileCacheRtn)
{
    return XmeGetIconControlInfo(screen, useMaskRtn, 
			  useMultiColorIconsRtn, useIconFileCacheRtn);
}

Boolean 
_XmUseColorObj( void )
{
    return XmeUseColorObj();
}

