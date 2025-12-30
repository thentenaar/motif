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

/* $XConsortium: ResIndI.h /main/5 1995/07/13 17:50:09 drk $ */
#ifndef _XmResIndI_h
#define _XmResIndI_h

#include <Xm/XmP.h>

#ifdef __cplusplus
extern "C" {
#endif

/********    Private Function Declarations for ResInd.c    ********/

extern int _XmConvertUnits(
                        Screen *screen,
                        int dimension,
                        int from_type,
                        int from_val,
                        int to_type) ;
extern void _XmUnitTypeDefault(
                        Widget widget,
                        int offset,
                        XrmValue *value) ;
extern unsigned char _XmGetUnitType(
                        Widget widget) ;
extern int _XmConvertFloatUnitsToIntUnits(
  			int unitType,
  			float unitValue,
  			int *intUnitType,
			float *intUnitValue,
  			int default_from_type) ;
extern int _XmConvertStringToUnits(
			Screen *screen,
		        String spec,
		        int default_from_type,
		        int orientation,
		        int to_type,
		        XtEnum *parse_error);
extern void _XmStringDirectionDefault(Widget widget,
				      int offset,
				      XrmValue *value );

/********    End Private Function Declarations    ********/

#ifdef __cplusplus
}
#endif
#endif /* _XmResIndI_h */

