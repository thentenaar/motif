/* $XConsortium: CxxXmString.cxx /main/4 1995/07/17 11:45:59 drk $ */
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
*/
/* 
 * HISTORY
*/

#include <Xm/Xm.h>
#include <XmCxx/CxxXmString.h>

XmCxxString& operator+(XmCxxString &x, XmCxxString &y) {
  XmCxxString *n;

  n = new XmCxxString(XmStringConcat(x.internal, y.internal));

  return *n;
}

XmCxxString& operator+(XmCxxString &x, char *y) {
  XmCxxString tmp(y);
  XmCxxString *n;

  n = new XmCxxString(XmStringConcat(x.internal, tmp.internal));

  return *n;
}

Dimension XmCxxString::baseline(XmRenderTable tab) {
    return XmStringBaseline(tab, internal); 
}

void XmCxxString::draw(Widget w, XmRenderTable r, GC gc, 
		       Position x, Position y, Dimension width, 
		       unsigned char alignment, unsigned char layout_dir,
		       XRectangle *clip) {
  XmStringDraw(XtDisplay(w), XtWindow(w), r, internal, gc, 
	       x, y, width, alignment, layout_dir, clip);
}

void XmCxxString::draw(Display *d, Window w, XmRenderTable r, GC gc, 
		       Position x, Position y, Dimension width, 
		       unsigned char alignment, unsigned char layout_dir,
		       XRectangle *clip) {
  XmStringDraw(d, w, r, internal, gc, 
	       x, y, width, alignment, layout_dir, clip);
}

void XmCxxString::extents(XmRenderTable r, Dimension *w, Dimension *h) {
  XmStringExtent(r, internal, w, h);
}
