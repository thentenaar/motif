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

#ifndef __XmDraw_h__
#define __XmDraw_h__

#include <Xm/Xm.h>
#include <Xm/Ext.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum {XmBEVEL_BOTTOM, XmBEVEL_TOP, XmBEVEL_BOTH} XmBevelOption;

void XmDrawBevel(Display*, Drawable, GC, GC, int, int, unsigned int, XmBevelOption);

#if defined(__cplusplus)
}
#endif

#endif /* __XmDraw_h__ */
