/**
 * Motif
 *
 * Copyright (c) 2026 Tim Hentenaar
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

#ifdef _XmP_h
#ifndef XM_OBSOLETEP_H
#define XM_OBSOLETEP_H

#include <X11/Xlib.h>
#include <Xm/deprecated.h>

/**
 * Obsolete private functions
 */

XM_ALTERNATIVE(Use XmStringCreateLocalized instead)
extern XmString XmeGetLocalizedString(char *reserved, Widget widget,
                                      char *resource, String string);

XM_ALTERNATIVE(Set XmNtitleString / XmNiconNameString on the shell instead)
extern void XmeSetWMShellTitle(XmString xmstr, Widget shell);

XM_ALTERNATIVE(Use XmRenderTableResolve or XmRenderTableGetRendition for XmFONTLIST_DEFAULT_TAG instead)
extern Boolean XmeRenderTableGetDefaultFont(XmRenderTable fontlist, XFontStruct **font_struct);

XM_ALTERNATIVE(Use XmRenderTableGetDefaultExtents / XmNascent etc. on XmRendition instead)
extern void XmRenderTableGetDefaultFontExtents(XmRenderTable rt, int *height,
                                               int *ascent, int *descent);

#endif /* XM_OBSOLETEP_H */
#endif /* _XmP_h */

