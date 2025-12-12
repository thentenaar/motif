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
/*   $XConsortium: CxxXmString.h /main/4 1995/07/13 18:31:09 drk $ */

// Wrapper for XmString objects

class XmCxxString {
private:
  XmString	internal;
public:
      // Constructors
  XmCxxString(const char* str) { 
    internal = XmStringCreateLocalized((String) str); 
  };
  XmCxxString(XmCxxString &str) { internal = XmStringCopy(str.internal); }
  XmCxxString(XmString str) { internal = str; }
  XmCxxString() { internal = XmStringCreateLocalized((String) ""); }
  static XmCxxString& Separator() {
    XmCxxString *n;
    n = new XmCxxString(XmStringSeparatorCreate());
    return *n;
  }
      // Destructor
  ~XmCxxString() { XmStringFree(internal); }
      // Operators + (concat), = and ==
  XmCxxString& operator=(const char *s) {
    XmStringFree(internal);
    internal = XmStringCreateLocalized((String) s);
    return *this;
  }
  XmCxxString& operator=(const XmString s) {
    XmStringFree(internal);
    internal = s;
    return *this;
  }
  XmCxxString& operator=(const XmCxxString &s) {
    XmStringFree(internal);
    internal = XmStringCopy(s.internal);
    return *this;
  }
  friend Boolean operator== (XmCxxString &x, XmCxxString &y) {
    return XmStringCompare(x.internal, y.internal);
  }
  friend XmCxxString& operator+(XmCxxString &x, XmCxxString &y);
  friend XmCxxString& operator+(XmCxxString &x, char *y);
  operator XmString() { return internal; }
      // Accessors and Functions
  Dimension 	baseline(XmRenderTable tab);
  Boolean 	empty() { return XmStringEmpty(internal); }
  void 		draw(Widget w, XmRenderTable r, GC gc, 
		     Position x, Position y, Dimension width, 
		     unsigned char alignment, unsigned char layout_dir,
		     XRectangle *clip);
  void 		draw(Display *d, Window w, XmRenderTable r, GC gc, 
		     Position x, Position y, Dimension width, 
		     unsigned char alignment, unsigned char layout_dir,
		     XRectangle *clip);
  void		extents(XmRenderTable r, Dimension *w, Dimension *h);
  Boolean	hasSubstring(XmCxxString& ss) {
    return XmStringHasSubstring(internal, ss.internal);
  }
  int		length() { return XmStringLength(internal); }
  int		lineCount() { return XmStringLineCount(internal); }
};



