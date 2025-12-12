/* $XConsortium: GetRes.c /main/5 1995/07/15 20:51:12 drk $ */
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

/********    Static Function Declarations    ********/

static Cardinal GetSecResData(WidgetClass w_class,
			      XmSecondaryResourceData **secResDataRtn);

/********    End Static Function Declarations    ********/


Cardinal 
XmGetSecondaryResourceData(
        WidgetClass w_class,
        XmSecondaryResourceData **secondaryDataRtn )
{
  int num = GetSecResData(w_class, secondaryDataRtn);

  return num;
}

/*
 * GetSecResData()
 *  - Called from : XmGetSecondaryResourceData ().
 */
static Cardinal 
GetSecResData(
        WidgetClass w_class,
        XmSecondaryResourceData **secResDataRtn )
{
  XmBaseClassExt  *bcePtr;	/* bcePtr is really **XmBaseClassExtRec */
  Cardinal count = 0;
  
  bcePtr = _XmGetBaseClassExtPtr( w_class, XmQmotif); 
  if ((bcePtr) && (*bcePtr) && ((*bcePtr)->getSecResData))
    count = ( (*bcePtr)->getSecResData)( w_class, secResDataRtn);

  return count;
}


