/* $XConsortium: CvtUnit.c /main/5 1995/07/15 20:50:25 drk $ */
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
#include "RepTypeI.h"

/************************************************************************
 *
 *  XmCvtStringToUnitType
 *	Convert a string to resolution independent unit type.
 *  This routine is obsolete, the converter is now available
 *  by default. We just call it here.
 *  
 ************************************************************************/
/*ARGSUSED*/
void 
XmCvtStringToUnitType(
        XrmValuePtr args,	/* unused */
        Cardinal *num_args,	/* unused */
        XrmValue *from_val,
        XrmValue *to_val )
{
   Display * dpy = _XmGetDefaultDisplay();

   /* we cannot call XmRUnitType directly, since it would loop
      if a program registers this function with to_type = XmRUnitType (which
      is very likely).
      So we use REAL_UNIT_TYPE_NAME, which has been registered with the
      same semantics as the original XmRUnitType in RepType.c */
   XtConvertAndStore(XmGetXmDisplay(dpy), XmRString, from_val,
		     REAL_UNIT_TYPE_NAME, to_val);

}
